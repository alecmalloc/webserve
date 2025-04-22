#!/usr/bin/perl
use strict;
use warnings;
use CGI;

# Create CGI object
my $cgi = CGI->new;

# Print the HTTP header
print $cgi->header('text/html');

# Get the name parameter from the form
my $name = $cgi->param('name');

# Print the HTML content
print <<EOF;
<html>
<head>
    <title>Hello CGI Test</title>
    <style>
        body { font-family: 'Segoe UI', Arial, sans-serif; background-color: #f5f5f5; color: #333; padding: 20px; }
        h1 { color: #3498db; }
        p { font-size: 1.2rem; }
    </style>
</head>
<body>
    <h1>Hello, $name!</h1>
    <p>Welcome to the WebServ CGI Perl Test Suite. Your name has been received and displayed successfully.</p>
    <p><a href="/" class="btn">Go back to the Test Suite</a></p>
</body>
</html>
EOF

