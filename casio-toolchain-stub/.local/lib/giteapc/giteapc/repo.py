import giteapc.gitea as gitea
from giteapc.repos import LocalRepo, RemoteRepo
from giteapc.util import *
from giteapc.config import REPO_FOLDER, PREFIX_FOLDER
import fnmatch
import shutil
import os
import re

#
# Determine full repo names from short versions
#

def local_match(name):
    """Find all local repositories with the specified name."""
    return [ r for r in LocalRepo.all() if r.name == name ]

def remote_match(name):
    """Find all remote repositories matching the given name."""
    return [ r for r in gitea.all_remote_repos() if r.name == name ]

def resolve(name, local_only=False, remote_only=False):
    assert not (local_only and remote_only)

    # Resolve full names directly
    if "/" in name:
        if not remote_only and LocalRepo.exists(name):
            return LocalRepo(name)
        if local_only:
            raise ResolveMissingException(name, local_only, remote_only)

        r = gitea.repo_get(name)
        if r is None:
            raise ResolveMissingException(name, local_only, remote_only)
        return r

    # Match local names without owners
    if not remote_only:
        r = local_match(name)
        if len(r) == 0 and local_only:
            raise ResolveMissingException(name, local_only, remote_only)
        elif len(r) == 1:
            return r[0]
        elif len(r) > 1:
            raise ResolveAmbiguousException(name, r, "local")

    # Match remote names without owners
    if not local_only:
        r = remote_match(name)
        if len(r) == 0:
            raise ResolveMissingException(name, local_only, remote_only)
        elif len(r) == 1:
            return r[0]
        else:
            raise ResolveAmbiguousException(name, r, "remote")

#
# Utilities
#

def print_repo(r, branches=None, tags=None, has_giteapc=True):
    color = "ARGYBMW"[sum(map(ord,r.owner[:5])) % 7]

    print(colors()[color] +
          "{}{_}/{W}{}{_}".format(r.owner, r.name, **colors()), end="")
    print(" (remote)" if r.remote else " (local)", end="")
    if r.remote and r.parent:
        print(" {A}[{}]{_}".format(r.parent.fullname, **colors()), end="")
    if r.remote and has_giteapc == False:
        print(" {R}(NOT SUPPORTED){_}".format(**colors()), end="")
    print("")

    if r.remote:
        print(("\n" + r.description).replace("\n", "\n  ")[1:])
    else:
        print("  {W}HEAD:{_}".format(**colors()), r.describe(all=True))
        print("  {W}Path:{_}".format(**colors()), r.folder, end="")
        if os.path.islink(r.folder):
            print(" ->", os.readlink(r.folder))
        else:
            print("")
        print("  {W}Configs:{_}".format(**colors()), end="")
        for c in r.configs():
            print(" " + c, end="")
        print("")
        branches = r.branches()
        tags = r.tags()

    if branches:
        print("  {W}Branches:{_}".format(**colors()), end="")
        for b in branches:
            if "local" in b and b["local"] == False:
                print(" {A}{}{_}".format(b["name"], **colors()), end="")
            else:
                print(" " + b["name"], end="")
        print("")

    if tags:
        print("  {W}Tags:{_}".format(**colors()),
              " ".join(t["name"] for t in reversed(tags)))

def pretty_repo(r):
    color = "ARGYBMW"[sum(map(ord,r.owner[:5])) % 7]
    return colors()[color] + "{}{_}/{W}{}{_}".format(r.owner,r.name,**colors())

#
# Repository specifications
#

class Spec:
    # A spec in REPOSITORY[@VERSION][:CONFIGURATION]
    RE_SPEC = re.compile(r'^([^@:]+)(?:@([^@:]+))?(?:([:])([^@:]*))?')

    def __init__(self, string):
        m = re.match(Spec.RE_SPEC, string)
        if m is None:
            raise Error(f"wrong format in specification {string}")

        self.name    = m[1]
        self.version = m[2]
        self.config  = m[4] or (m[3] and "")
        self.repo    = None

    def resolve(self, local_only=False, remote_only=False):
        self.repo = resolve(self.name, local_only, remote_only)
        return self.repo

    def is_blank(self):
        return self.version is None and self.config is None

    def str(self, pretty=False):
        if self.repo and pretty:
            name = pretty_repo(self.repo)
        elif self.repo:
            name = self.repo.fullname
        else:
            name = self.name

        version = f"@{self.version}" if self.version else ""
        config  = f":{self.config}"  if self.config  else ""
        return name + version + config

    def same_version_config_as(self, other):
        return self.version == other.version and self.config == other.config

    def __str__(self):
        return self.str(pretty=False)
    def __repr__(self):
        return self.str(pretty=False)

    def pretty_str(self):
        return self.str(pretty=True)

#
# repo list command
#

def _list(*args, remote=False):
    if len(args) > 1:
        return fatal("repo list: too many arguments")

    if remote:
        # Since there aren't many repositories under [giteapc], just list them
        # and then filter by hand (this avoids some requests and makes search
        # results more consistent with local repositories)
        repos = gitea.all_remote_repos()
    else:
        repos = LocalRepo.all()

    # - Ignore case in pattern
    # - Add '*' at start and end to match substrings only
    pattern = args[0].lower() if args else "*"
    if not pattern.startswith("*"):
        pattern = "*" + pattern
    if not pattern.endswith("*"):
        pattern = pattern + "*"

    # Filter
    repos = [ r for r in repos
              if fnmatch.fnmatch(r.fullname.lower(), pattern)
              or r.remote and fnmatch.fnmatch(r.description.lower(), pattern) ]

    # Print
    if repos == []:
        if args:
            print(f"no repository matching '{args[0]}'")
        else:
            print(f"no repository")
        return 1
    else:
        for r in repos:
            print_repo(r)
        return 0

#
# repo fetch command
#

def fetch(*args, use_ssh=False, use_https=False, force=False, update=False):
    # Use HTTPS by default
    if use_ssh and use_https:
        return fatal("repo fetch: --ssh and --https are mutually exclusive")
    protocol = "ssh" if use_ssh else "https"

    # With no arguments, fetch all local repositories
    if args == ():
        for r in LocalRepo.all():
            msg(f"Fetching {pretty_repo(r)}...")
            r.fetch()
            if update:
                r.pull()
        return 0

    for arg in args:
        s = arg if isinstance(arg, Spec) else Spec(arg)
        r = s.resolve()

        # If this is a local repository, just git fetch
        if not r.remote:
            msg(f"Fetching {pretty_repo(r)}...")
            r.fetch()
        # If this is a remote repository, clone it
        else:
            msg(f"Cloning {pretty_repo(r)}...")
            has_tag = "giteapc" in gitea.repo_topics(r)

            if has_tag or force:
                LocalRepo.clone(r, protocol)
            if not has_tag and force:
                warn(f"{r.fullname} doesn't have the [giteapc] tag")
            if not has_tag and not force:
                raise Error(f"{r.fullname} doesn't have the [giteapc] tag, "+\
                    "use -f to force")
            r = s.resolve()

        # Checkout requested version, if any
        if s.version:
            msg("Checking out {W}{}{_}".format(s.version, **colors()))
            r.checkout(s.version)
        if update:
            r.pull()

#
# repo show command
#

def show(*args, remote=False, path=False):
    if remote and path:
        raise Error("repo show: -r and -p are exclusive")

    if not remote:
        for name in args:
            r = resolve(name, local_only=True)
            if path:
                print(LocalRepo.path(r.fullname))
            else:
                print_repo(r)
        return 0

    repos = []
    for name in args:
        r = resolve(name, remote_only=True)
        branches = gitea.repo_branches(r)
        tags     = gitea.repo_tags(r)
        topics   = gitea.repo_topics(r)
        repos.append((r, branches, tags, "giteapc" in topics))

    for (r, branches, tags, has_giteapc) in repos:
        print_repo(r, branches, tags, has_giteapc=has_giteapc)

#
# repo build command
#

def build(*args, install=False, skip_configure=False):
    if len(args) < 1:
        return fatal("repo build: specify at least one repository")

    specs = []
    for arg in args:
        s = arg if isinstance(arg, Spec) else Spec(arg)
        s.resolve(local_only=True)
        specs.append(s)

    for s in specs:
        r = s.repo
        pretty = pretty_repo(r)
        config_string = f" for {s.config}" if s.config else ""

        if s.version:
            msg("{}: Checking out {W}{}{_}".format(pretty,s.version,**colors()))
            r.checkout(s.version)

        # Check that the project has a Makefile
        if not os.path.exists(r.makefile):
            raise Error(f"{r.fullname} has no giteapc.make")

        env = os.environ.copy()
        if s.config is not None:
            env["GITEAPC_CONFIG"] = s.config
            r.set_config(s.config)
        env["GITEAPC_PREFIX"] = PREFIX_FOLDER

        if not skip_configure:
            msg(f"{pretty}: Configuring{config_string}")
            r.make("configure", env)

        msg(f"{pretty}: Building")
        r.make("build", env)

        if install:
            msg(f"{pretty}: Installing")
            r.make("install", env)

        msg(f"{pretty}: Done! :D")


#
# repo install command
#

def search_dependencies(names, fetched, plan, **kwargs):
    for name in names:
        s = Spec(name)
        r = s.resolve()

        if r.fullname not in fetched:
            fetch(s, **kwargs)
            fetched.add(r.fullname)
            # Re-resolve, as a local repository this time
            if r.remote:
                r = s.resolve(local_only=True)

            # Schedule dependencies before r
            search_dependencies(r.dependencies(), fetched, plan, **kwargs)
            plan.append(s)

def install(*args, use_https=False, use_ssh=False, update=False, yes=False,
    dry_run=False):

    # Update all repositories
    if args == () and not update:
        return fatal(f"repo install: need one argument (unless -u is given)")
    if args == () and update:
        args = [ r.fullname for r in LocalRepo.all() ]

    # Fetch every repository and determine its dependencies to form a basic
    # plan of what repo to build in what order, but without version/config info

    dep_order = []
    search_dependencies(args, set(), dep_order, use_https=use_https,
        use_ssh=use_ssh, update=update)

    # Apply configurations on everyone and make sure they are no contradictions
    # and all configs exist
    spec_by_repo = { spec.repo.fullname: spec for spec in dep_order }
    for arg in args:
        s = Spec(arg)
        if s.is_blank():
            continue
        r = s.resolve(local_only=True)

        name = s.repo.fullname
        if not s.same_version_config_as(spec_by_repo[name]):
            return fatal(f"repo install: multiple specs for {name}: {s}, " +
                f"{spec_by_repo[name]}")

        if s.config not in ["", None] and s.config not in r.configs():
            return fatal(f"repo install: no config {s.config} for {name}" +
                " (configs: " + ", ".join(r.configs()) + ")")

        spec_by_repo[name] = s

    # Final plan
    plan = [ spec_by_repo[s.repo.fullname] for s in dep_order ]

    # Plan review and confirmation
    msg("Will install:", ", ".join(s.pretty_str() for s in plan))

    if dry_run:
        return 0

    if not yes:
        msg("Is that okay (Y/n)? ", end="")
        confirm = input().strip()
        if confirm not in ["Y", "y", ""]:
            return 1

    # Final build
    build(*plan, install=True)

#
# repo uninstall command
#

def uninstall(*args, keep=False):
    if len(args) < 1:
        return fatal("repo uninstall: specify at least one repository")

    for name in args:
        r = resolve(name, local_only=True)
        msg(f"{pretty_repo(r)}: Uninstalling")

        env = os.environ.copy()
        env["GITEAPC_PREFIX"] = PREFIX_FOLDER
        r.make("uninstall", env)

        if not keep:
            msg("{}: {R}Removing files{_}".format(pretty_repo(r), **colors()))

            if os.path.islink(r.folder):
                os.remove(r.folder)
            elif os.path.isdir(r.folder):
                shutil.rmtree(r.folder)
            else:
                raise Error(f"cannot handle {r.folder} (not a folder/symlink)")

            parent = os.path.dirname(r.folder)
            if not os.listdir(parent):
                os.rmdir(parent)
