#!/usr/bin/env python3
import cgi
import cgitb
cgitb.enable()

print("Content-type: text/html\n")
print("<html><body>")
print("<h1>Python CGI Test</h1>")

form = cgi.FieldStorage()
if "name" in form:
    print(f"<p>Hello, {form['name'].value}!</p>")
else:
    print("<p>No name provided</p>")

print("</body></html>")