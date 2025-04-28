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
print("<style>@import url('https://fonts.cdnfonts.com/css/jetbrains-mono-2');:root{--font-family:\"JetBrains Mono\",monospace;--line-height:1.20rem;--border-thickness:2px;--text-color:#000;--text-color-alt:#666;--background-color:#fff;--background-color-alt:#eee;--font-weight-normal:500;--font-weight-medium:600;--font-weight-bold:800}body{font-family:var(--font-family);font-weight:var(--font-weight-normal);color:var(--text-color);background-color:var(--background-color);max-width:1000px;margin:0 auto;padding:2rem}.header{border-bottom:var(--border-thickness) solid var(--text-color);margin-bottom:2rem}.section{border:var(--border-thickness) solid var(--text-color);padding:1rem;margin-bottom:1rem}.method-badge{background:var(--background-color-alt);padding:0.2rem 0.5rem;font-size:0.8rem}button,.btn{font-family:var(--font-family);background:var(--text-color);color:var(--background-color);border:none;padding:0.5rem 1rem;cursor:pointer;text-decoration:none;display:inline-block;margin:0.25rem}input,select{font-family:var(--font-family);border:var(--border-thickness) solid var(--text-color);padding:0.5rem;margin:0.5rem 0;width:50%}.form-group{margin-bottom:1rem}</style>")