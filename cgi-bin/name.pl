#!/usr/bin/perl
use strict;
use warnings;

# Send HTTP header manually
print "Content-Type: text/html\r\n\r\n";

# Read query string or POST body
my $query = '';

# Handle GET or POST
if ($ENV{'REQUEST_METHOD'} eq 'GET') {
    $query = $ENV{'QUERY_STRING'} || '';
}
elsif ($ENV{'REQUEST_METHOD'} eq 'POST') {
    my $content_length = $ENV{'CONTENT_LENGTH'} || 0;
    read(STDIN, $query, $content_length);
}

# Parse parameters manually
my %params;
foreach my $pair (split /&/, $query) {
    my ($key, $value) = split /=/, $pair, 2;
    $key =~ tr/+/ /;
    $value =~ tr/+/ /;
    $key =~ s/%([0-9A-Fa-f]{2})/chr(hex($1))/eg;
    $value =~ s/%([0-9A-Fa-f]{2})/chr(hex($1))/eg;
    $params{$key} = $value;
}

# Get 'name' parameter
my $name = $params{'name'} || 'Guest';

# Print HTML
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

