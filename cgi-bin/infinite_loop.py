#!/usr/bin/env python3

import time
import sys

# Print the HTTP header to indicate it's a valid response
print("Content-Type: text/html\n\n")

# HTML structure for the page
print("<html><body>")
print("<h1>Running infinite loop to test CGI timeout</h1>")
print("<p>This page will run indefinitely. You will see a timeout after a while.</p>")
print("<p>If you see this message for a long time, it's because the script is still running...</p>")

sys.stdout.flush()  # Flush stdout buffer to ensure output is written

# Infinite loop to test timeout
while True:
    time.sleep(1)  # Sleep for 1 second to prevent excessive CPU usage

print("</body></html>")

