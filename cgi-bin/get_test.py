#!/usr/bin/env python3

import cgi
import cgitb
import os
import sys

# Enable detailed error reporting
cgitb.enable()

# Output HTTP headers
print("Content-type: text/html\n")

# Start HTML output
print("""
<!DOCTYPE html>
<html>
<head>
    <title>Python CGI GET Test</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        h1 { color: #3498db; }
        .data { background: #f8f9fa; border: 1px solid #ddd; padding: 15px; border-radius: 5px; }
        .success { color: green; }
        .debug { color: #666; font-size: 0.9em; margin-top: 20px; }
        table { border-collapse: collapse; width: 100%; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #f2f2f2; }
    </style>
</head>
<body>
    <h1>Python CGI GET Test</h1>
""")

# Process the GET parameters
form = cgi.FieldStorage()

# Display GET data
print("<h2>GET Parameters</h2>")
if form:
    print("<div class='data'>")
    print("<table>")
    print("<tr><th>Parameter</th><th>Value</th></tr>")
    for key in form:
        print(f"<tr><td>{key}</td><td>{form[key].value}</td></tr>")
    print("</table>")
    print("</div>")
    print("<p class='success'>✓ GET parameters received successfully!</p>")
else:
    print("<p>No GET parameters received. Try adding ?param=value to the URL.</p>")

# Debug information
print("<h2>Environment Information</h2>")
print("<div class='data debug'>")
print("<h3>CGI Environment Variables</h3>")
print("<table>")
print("<tr><th>Variable</th><th>Value</th></tr>")
for var in ['REQUEST_METHOD', 'QUERY_STRING', 'CONTENT_LENGTH', 'HTTP_USER_AGENT', 'REMOTE_ADDR']:
    if var in os.environ:
        print(f"<tr><td>{var}</td><td>{os.environ.get(var)}</td></tr>")
print("</table>")
print("</div>")

print("</body></html>")