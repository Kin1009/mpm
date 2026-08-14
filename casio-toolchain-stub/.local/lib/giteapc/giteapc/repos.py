from giteapc.config import REPO_FOLDER
from giteapc.util import *
import subprocess
from subprocess import PIPE
import os.path
import shutil
import glob
import re

class RemoteRepo:
    # Create a remote repo from the JSON object returned by Gitea
    def __init__(self, j):
        self.j = j
        self.remote = True

    @property
    def name(self):
        return self.j["name"]

    @property
    def owner(self):
        return self.j["owner"]["username"]

    @property
    def fullname(self):
        return self.j["full_name"]

    @property
    def description(self):
        return self.j["description"]

    @property
    def parent(self):
        p = self.j["parent"]
        return RemoteRepo(p) if p is not None else None

    @property
    def url(self):
        return f"/repos/{self.owner}/{self.name}"

    def clone_url(self, protocol):
        if protocol == "ssh":
            return self.j["ssh_url"]
        else:
            return self.j["clone_url"]


class LocalRepo:
    # Create a remote repo from the full name or path
    def __init__(self, fullname):
        if fullname.startswith(REPO_FOLDER + "/"):
            fullname = fullname[len(REPO_FOLDER)+1:]

        assert fullname.count("/") == 1

        self.fullname = fullname
        self.owner, self.name = fullname.split("/")
        self.folder = REPO_FOLDER + "/" + fullname
        self.remote = False

        if os.path.exists(self.folder + "/.giteapc/giteapc.make"):
            self.makefile = self.folder + "/.giteapc/giteapc.make"
        else:
            self.makefile = self.folder + "/giteapc.make"

    @staticmethod
    def path(fullname):
        return REPO_FOLDER + "/" + fullname

    @staticmethod
    def exists(fullname):
        return os.path.exists(LocalRepo.path(fullname))

    @staticmethod
    def all():
        return [ LocalRepo(path) for path in glob.glob(REPO_FOLDER + f"/*/*") ]

    @staticmethod
    def clone(r, method="https"):
        src = r.clone_url(method)
        dst = LocalRepo.path(r.fullname)
        mkdir_p(dst)
        cmd = ["git", "clone", src, dst]

        try:
            run(cmd, stdout=PIPE, stderr=PIPE)
            return LocalRepo(r.fullname)
        except ProcessError as e:
            # On error, delete the failed clone
            shutil.rmtree(dst)
            raise e

    # Git commands

    def _git(self, command, *args, **kwargs):
        return run(["git", "-C", self.folder] + command,
            *args, **kwargs)

    def is_on_branch(self):
        try:
            self._git(["symbolic-ref", "-q", "HEAD"], stdout=PIPE)
            return True
        except ProcessError as e:
            if e.returncode == 1:
                return False
            raise e

    def updateRemotes(self):
        # Automatically switch old gitea URLs to Forgejo
        for r in self._git(["remote"], stdout=PIPE).stdout.split():
            url = self._git(["remote", "get-url", r], stdout=PIPE).stdout.rstrip(b"\n")
            new = url
            if url.startswith(b"gitea@gitea.planet-casio.com:"):
                new = b"forgejo@git" + url[11:]
            if url.startswith(b"https://gitea.planet-casio.com/"):
                new = b"https://git." + url[14:]
            if new != url:
                self._git(["remote", "set-url", r, new])

    def fetch(self):
        self.updateRemotes()
        self._git(["fetch"])

    def pull(self):
        self.updateRemotes()
        if self.is_on_branch():
            self._git(["pull"])

    def describe(self, tags=True, all=False, always=True):
        args = ["--tags"]*tags + ["--all"]*all + ["--always"]*always
        p = self._git(["describe"] + args, stdout=PIPE)
        return p.stdout.decode("utf-8").strip()

    def checkout(self, version):
        self._git(["checkout", version], stdout=PIPE, stderr=PIPE)

    def branches(self):
        proc = self._git(["branch"], stdout=subprocess.PIPE)
        local = proc.stdout.decode("utf-8").split("\n")
        local = [ b[2:] for b in local if b ]

        proc = self._git(["branch", "-r"], stdout=subprocess.PIPE)
        remote = proc.stdout.decode("utf-8").split("\n")
        remote = [ b[9:] for b in remote
                   if b and b.startswith("  origin/") and "/HEAD " not in b ]
        remote = [ b for b in remote if b not in local ]

        return [ {"name": b, "local": True } for b in local ] + \
               [ {"name": b, "local": False} for b in remote ]

    def tags(self):
        proc = self._git(["tag", "--list"], stdout=subprocess.PIPE)
        tags = proc.stdout.decode("utf-8").split("\n")
        return [ {"name": t} for t in tags if t ]

    # Make commands

    def make(self, target, env=None):
        # Use GNU Make even on OpenBSD
        if shutil.which("gmake") is not None:
            command = "gmake"
        else:
            command = "make"

        with ChangeDirectory(self.folder):
            return run([command, "-f", self.makefile, target], env=env)

    def configs(self):
        RE_NAME = re.compile(r'giteapc-config-(.*)\.make')
        files = glob.glob(self.folder + "/giteapc-config-*.make")
        files += glob.glob(self.folder + "/.giteapc/giteapc-config-*.make")
        return sorted(RE_NAME.match(os.path.basename(x))[1] for x in files)

    def set_config(self, config):
        source = os.path.dirname(self.makefile) + f"/giteapc-config.make"
        target = os.path.dirname(self.makefile) + f"/giteapc-config-{config}.make"

        if config == "":
            if os.path.islink(source):
                os.remove(source)
            return

        if not os.path.exists(target):
            raise Error(f"config '{config}' does not exist")

        if os.path.islink(source):
            os.remove(source)
        elif os.path.exists(source):
            raise Error("giteapc-config.make is not a link, was it altered?")

        os.symlink(target, source)

    # Metadata

    def metadata(self):
        RE_METADATA = re.compile(r'^\s*#\s*giteapc\s*:\s*(.*)$')
        metadata = dict()

        with open(self.makefile, "r") as fp:
            makefile = fp.read()

        for line in makefile.split("\n"):
            m = re.match(RE_METADATA, line)
            if m is None:
                break

            for assignment in m[1].split():
                name, value = assignment.split("=", 1)
                if name not in metadata:
                    metadata[name] = value
                else:
                    metadata[name] += "," + value

        return metadata

    def dependencies(self):
        metadata = self.metadata()
        if "depends" in metadata:
            return metadata["depends"].split(",")
        else:
            return []
