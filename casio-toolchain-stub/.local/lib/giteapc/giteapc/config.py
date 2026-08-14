import os

# URL to the Git forge supplying the resources
GIT_URL = "https://git.planet-casio.com"
# Data folder to store repositores
XDG_DATA_HOME = os.getenv("XDG_DATA_HOME", os.getenv("HOME")+"/.local/share")
REPO_FOLDER = os.getenv("GITEAPC_HOME") or XDG_DATA_HOME + "/giteapc"
# Prefix folder to install files to
PREFIX_FOLDER = os.getenv("GITEAPC_PREFIX") or os.getenv("HOME") + "/.local"
