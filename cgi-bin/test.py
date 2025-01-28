#!/usr/bin/env python3
import os, sys

print("Content-Type: text/plain\n")
print("CGI Script Executed")
print("Request Method:", os.environ.get("REQUEST_METHOD", ""))
print("Query String:", os.environ.get("QUERY_STRING", ""))
print("Content-Type:", os.environ.get("CONTENT_TYPE", ""))
print("Body:", sys.stdin.read())

while True:
    sys.stdin.read();
