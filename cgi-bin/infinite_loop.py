#!/usr/bin/env python3

import time
import cgitb
cgitb.enable()  # Enable debugging

print("Content-Type: text/html\n")  # HTTP header
print("""
<!DOCTYPE html>
<html>
<head>
    <title>Infinite Loop Test</title>
</head>
<body>
    <h1>Infinite Loop Test</h1>
    <p>This script runs an infinite loop. You can terminate it manually if needed.</p>
</body>
</html>
""")

# Infinite loop
while True:
    time.sleep(1)  # Sleep to avoid high CPU usage