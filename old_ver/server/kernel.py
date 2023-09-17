import requests
from bs4 import BeautifulSoup

def getFiles(dir):
    items = []
    # Get new releases
    response = requests.get("https://mirrors.edge.kernel.org"+dir)
    parsed_response = BeautifulSoup(response.text, 'html.parser')
    body = parsed_response.body
    files = ["xz", "gz"]
    for link in body.findAll('a'):
        print(link.contents[0].split(".")[-1])
        if "Change" not in link.contents[0] and (link.contents[0].split(".")[-1] in files or link.contents[0][-1]=="/"):
            items.append(link.contents[0])
    items = str(items)
    items = items.replace(" ","")
    items = items.replace("[","")
    items = items.replace("]","")
    items = items.replace("'","")
    return items
