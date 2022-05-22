#!/usr/bin/env python3
from http.server import BaseHTTPRequestHandler, HTTPServer
import logging
import os
import ubuntu
import kernel

class S(BaseHTTPRequestHandler):
    def _set_response(self):
        if self.client_address[0] == "27.124.5.34":
            self.send_response(404)
        else:
            self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def do_GET(self):
        logging.info("GET request,\nPath: %s\nHeaders:\n%s\n", str(self.path), str(self.headers))
        self._set_response()
        self.wfile.write("GET request for {}".format(self.path).encode('utf-8'))

    def do_POST(self):
        content_length = int(self.headers['Content-Length']) # <--- Gets the size of data
        post_data = self.rfile.read(content_length) # <--- Gets the data itself
        logging.info("POST request,\nPath: %s\nHeaders:\n%s\n\nBody:\n%s\n",
                str(self.path), str(self.headers), post_data.decode('utf-8'))
        data = post_data.decode('utf-8')
        message = ""
        if data == "menu":
            message = 'Ubuntu,Linux Kernel'
        if data == "Ubuntu getvers":
            message = ubuntu.getVersions(False)
        if "Ubuntu getfiles" in data:
            message = ubuntu.getLinks(data.split(" ")[-1])
        if "Linux Kernel" in data:
            message = kernel.getFiles(data.split(" ")[-1])
        print(message)
        self._set_response()
        self.wfile.write(message.encode('utf-8'))

def run(server_class=HTTPServer, handler_class=S, port=8080):
    logging.basicConfig(level=logging.INFO)
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    logging.info('Starting httpd...\n')
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    logging.info('Stopping httpd...\n')

if __name__ == '__main__':
    from sys import argv

    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()
