import requests
from bs4 import BeautifulSoup

def getVersions(switch):
    versions = []
    # Get new releases
    response = requests.get("https://releases.ubuntu.com/")
    parsed_response = BeautifulSoup(response.text, 'html.parser')
    body = parsed_response.body
    for link in body.findAll('a'):
        if "/" in link.contents[0] and link.contents[0].count(".")==1:
            version = str(link.contents[0])
            version = version.replace("/","")
            if switch:
                versions.append(version+" new")
            else:
                versions.append(version)

    # Get old releases
    response = requests.get("https://old-releases.ubuntu.com/releases")
    parsed_response = BeautifulSoup(response.text, 'html.parser')
    body = parsed_response.body
    for link in body.findAll('a'):
        if len(link.contents) != 0:
            if link.contents[0].count(".")==1 and "/" in link.contents[0]:
                version = str(link.contents[0])
                version = version.replace("/","")
                if version+" new" not in versions:
                    if switch:
                        versions.append(version+" old")
                    else:
                        versions.append(version)
    versions=set(versions)
    versions=list(versions)
    versions.sort(key = lambda x: int(x.split(".")[0]))
    versions.reverse()
    if not switch:
        versions = str(versions)
        versions = versions.replace(" ","")
        versions = versions.replace("[","")
        versions = versions.replace("]","")
        versions = versions.replace("'","")
    return versions

def getFiles(version):
    files = []
    ver = version.split(" ")[-2]
    kind = version.split(" ")[-1]
    if kind == "old":
        url = "https://old-releases.ubuntu.com/releases/"+ver
        response = requests.get(url)
        parsed_response = BeautifulSoup(response.text, 'html.parser')
        table = parsed_response.body.find('table')
        for item in table.findAll('a'):
            if item.contents[0].split(".")[-1] == "iso":
                files.append(item.contents[0])
                files.append(url+"/"+item.contents[0])
    elif kind == "new":
        url = "https://releases.ubuntu.com/"+ver
        response = requests.get(url)
        parsed_response = BeautifulSoup(response.text, 'html.parser')
        table = parsed_response.body.find("table")
        for link in table.findAll('a'):
            if link.contents[0].split(".")[-1] == "iso":
                files.append(link.contents[0])
                files.append(url+"/"+link.contents[0])
    return files

def getLinks(version):
    versions = getVersions(True)
    # Call function to get the link to search
    for i in versions:
        if version == i.split(" ")[0]:
            version=i
            break
    links = str(getFiles(version))
    links = links.replace(" ","")
    links = links.replace("[","")
    links = links.replace("]","")
    links = links.replace("'","")
    return links
