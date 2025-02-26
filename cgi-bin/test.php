!/usr/bin/php-cgi
<?php
// Add to your PHP script

// Output raw data with minimal processing
echo "Content-Type: text/plain\r\n\r\n";

// Try multiple ways to read input
echo "READING INPUT METHODS:\n";
echo "1. Raw input: '" . file_get_contents('php://input') . "'\n";

// Raw stdin read
$stdin = fopen('php://stdin', 'r');
$stdin_data = '';
while (!feof($stdin)) {
    $stdin_data .= fread($stdin, 1024);
}
fclose($stdin);
echo "2. Direct stdin read: '" . $stdin_data . "'\n";

// Debug environment
echo "ENVIRONMENT:\n";
echo "CONTENT_LENGTH: " . $_SERVER['CONTENT_LENGTH'] . "\n"; 
echo "REMOTE_ADDR: " . $_SERVER['REMOTE_ADDR'] . "\n";
?>