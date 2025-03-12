#!/usr/bin/env python3
import os
import sys
import cgi
import cgitb
cgitb.enable()  # For debugging

print("Content-Type: text/html\r\n\r\n")  # HTTP header required by CGI

print("""
<!DOCTYPE html>
<html>
<head>
    <title>CGI File Upload Test</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        h1 { color: #4CAF50; }
        .success { color: green; }
        .error { color: red; }
        pre { background: #f4f4f4; padding: 10px; }
        table { border-collapse: collapse; width: 100%; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #4CAF50; color: white; }
    </style>
</head>
<body>
    <h1>CGI File Upload Test</h1>
""")

try:
    form = cgi.FieldStorage()
    
    # Check if a file was uploaded
    if "file" in form and form["file"].filename:
        fileitem = form["file"]
        
        # Get file details
        filename = os.path.basename(fileitem.filename)
        file_data = fileitem.file.read()
        file_size = len(file_data)
        
        # Create an uploads directory within the cgi-bin folder
        upload_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "uploads")
        if not os.path.exists(upload_dir):
            os.makedirs(upload_dir)
        
        # Save the file
        file_path = os.path.join(upload_dir, filename)
        with open(file_path, 'wb') as f:
            f.write(file_data)
        
        print(f"<p class='success'>✓ File '{filename}' uploaded successfully!</p>")
        print(f"<p>File size: {file_size} bytes</p>")
        print(f"<p>Saved to: {file_path}</p>")
        
        # Show file content preview if it's a text file
        if file_size < 10000 and (filename.endswith('.txt') or filename.endswith('.html') or filename.endswith('.csv')):
            print("<h2>File Content Preview:</h2>")
            print("<pre>")
            print(file_data.decode('utf-8', errors='replace')[:1000])
            if file_size > 1000:
                print("...")
            print("</pre>")
    else:
        print("<p class='error'>No file was uploaded.</p>")
    
    # Debug information
    print("<h2>Environment Information</h2>")
    print("<table>")
    print("<tr><th>Variable</th><th>Value</th></tr>")
    for var in ['REQUEST_METHOD', 'CONTENT_TYPE', 'CONTENT_LENGTH', 'SCRIPT_NAME', 'PATH_INFO']:
        if var in os.environ:
            print(f"<tr><td>{var}</td><td>{os.environ.get(var)}</td></tr>")
    print("</table>")
    
    print("<h2>Form Data:</h2>")
    print("<table>")
    print("<tr><th>Field Name</th><th>Value</th></tr>")
    for key in form:
        if key != "file":  # Skip the file
            print(f"<tr><td>{key}</td><td>{form.getvalue(key)}</td></tr>")
    print("</table>")
    
except Exception as e:
    print(f"<p class='error'>Error: {str(e)}</p>")
    import traceback
    print("<pre>")
    print(traceback.format_exc())
    print("</pre>")

print("""
    <p><a href="/">Return to home</a></p>
</body>
</html>
""")