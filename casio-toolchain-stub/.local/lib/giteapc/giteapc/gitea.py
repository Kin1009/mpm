from giteapc.util import *
import giteapc.repos
import giteapc.config
import subprocess
import json
import sys

# Provide a mechanism to submit HTTP GET requests to the forge. Try to use the
# requests module if it's available, otherwise default to cURL. If none is
# available, disable network features but keep going.
_http_get = None

try:
    import requests
    _http_get = requests_get
except ImportError:
    pass

if _http_get is None and has_curl():
    _http_get = curl_get

if _http_get is None:
    warn("found neither requests nor curl, network will be disabled")

# Send a GET request to the specified API endpoint
def _get(url, params=None):
    if _http_get is None:
        raise Error("cannot access forge (network is disabled)")
    return _http_get(giteapc.config.GIT_URL + "/api/v1" + url, params)

# Search for repositories
def repo_search(keyword, **kwargs):
    r = _get("/repos/search", { "q": keyword, **kwargs })
    results = json.loads(r)['data']
    results = [ giteapc.repos.RemoteRepo(r) for r in results ]
    return sorted(results, key=lambda r: r.fullname)

# List all remote repositories (with topic "giteapc")
def all_remote_repos():
    return repo_search("giteapc", topic=True)

# Repository info
def repo_get(fullname):
    try:
        r = _get(f"/repos/{fullname}")
        return giteapc.repos.RemoteRepo(json.loads(r))
    except NetworkError as e:
        if e.status == 404:
            return None
        else:
            raise e

def repo_branches(r):
    r = _get(r.url + "/branches")
    return json.loads(r)
def repo_tags(r):
    r = _get(r.url + "/tags")
    return json.loads(r)
def repo_topics(r):
    r = _get(r.url + "/topics")
    return json.loads(r)["topics"]
