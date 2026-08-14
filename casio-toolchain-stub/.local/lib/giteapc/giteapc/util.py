import sys
import subprocess
import os.path
import shlex

# Error management; the base Error class is provided for errors to be caught at
# top-level and displayed as a GiteaPC error message.

class Error(Exception):
    pass

class NetworkError(Error):
    def __init__(self, status):
        self.status = status

class ProcessError(Error):
    def __init__(self, process):
        self.process = process
        self.returncode = process.returncode

    def __str__(self):
        p = self.process
        e = f"error {p.returncode} in command: "
        c = " ".join(shlex.quote(arg) for arg in p.args)
        e += "{W}{}{_}\n".format(c, **colors())

        out = p.stdout.decode('utf-8').strip() if p.stdout else ""
        err = p.stderr.decode('utf-8').strip() if p.stderr else ""

        # Show standard output and standard error (omit names if only one has
        # content to show)
        if len(out) > 0 and len(err) > 0:
            e += "{W}Standard output:{_}\n".format(**colors())
        if len(out) > 0:
            e += out + "\n"
        if len(out) > 0 and len(err) > 0:
            e += "{W}Standard error:{_}\n".format(**colors())
        if len(err) > 0:
            e += err + "\n"

        return e.strip()

class ResolveMissingException(Error):
    def __init__(self, name, local_only, remote_only):
        self.name = name
        self.local_only = local_only
        self.remote_only = remote_only

    def __str__(self):
        if self.local_only:
            spec = "local repository"
        elif self.remote_only:
            spec = "remote repository"
        else:
            spec = "local or remote repository"

        return f"no such {spec}: '{self.name}'"

class ResolveAmbiguousException(Error):
    def __init__(self, name, matches, kind):
        self.name = name
        self.matches = [r.fullname for r in matches]
        self.kind = kind

    def __str__(self):
        return f"multiple {self.kind} repositories match '{self.name}': " + \
            ", ".join(self.matches)

def warn(*args):
    print("{Y}warning:{_}".format(**colors()), *args, file=sys.stderr)
def fatal(*args):
    print("{R}error:{_}".format(**colors()), *args, file=sys.stderr)
    return 1

# Color output

def colors():
    colors = {
        # Gray, Red, Green, Yello, Blue, Magenta, Cyan, White
        "A": "30;1", "R": "31;1", "G": "32;1", "Y": "33;1",
        "B": "34;1", "M": "35;1", "C": "36;1", "W": "37;1",
        # Same but without bold
        "a": "30", "r": "31", "g": "32", "y": "33",
        "b": "34", "m": "35", "c": "36", "w": "37",
        # Italic
        "i": "3",
        # Clear formatting
        "_": "0",
    }
    # Disable colors if stdout is not a TTY
    if sys.stdout.isatty():
        return { name: f"\x1b[{code}m" for name, code in colors.items() }
    else:
        return { name: "" for name in colors }

def msg(*args, **kwargs):
    print("{c}<giteapc>{_}".format(**colors()), *args, **kwargs)

# Change directory and guarantee changing back even if an exception occurs

class ChangeDirectory:
    def __init__(self, destination):
        self.destination = destination

    def __enter__(self):
        self.source = os.getcwd()
        os.chdir(self.destination)

    def __exit__(self, type, value, traceback):
        os.chdir(self.source)

# Tool detection

def has_curl():
    proc = subprocess.run(["curl", "--version"], stdout=subprocess.DEVNULL)
    return proc.returncode == 0

def has_git():
    proc = subprocess.run(["git", "--version"], stdout=subprocess.DEVNULL)
    return proc.returncode == 0

# HTTP requests

def requests_get(url, params):
    # Don't import requests until the gitea module confirms it's available
    import requests
    r = requests.get(url, params)
    if r.status_code != 200:
        raise NetworkError(r.status_code)
    return r.text

def curl_get(url, params):
    if params:
        url = url + "?" + "&".join(f"{n}={v}" for (n,v) in params.items())
    proc = subprocess.run(["curl", "-s", url], stdout=subprocess.PIPE)
    if proc.returncode != 0:
        raise NetworkError(-1)
    return proc.stdout.decode("utf-8")

# Create path to folder

def mkdir_p(folder):
    try:
        os.mkdir(folder)
    except FileExistsError:
        return
    except FileNotFoundError:
        mkdir_p(os.path.dirname(folder))
        os.mkdir(folder)

# Run process and throw exception on error

def run(process, *args, **kwargs):
    proc = subprocess.run(process, *args, **kwargs)
    if proc.returncode != 0:
        raise ProcessError(proc)
    return proc
