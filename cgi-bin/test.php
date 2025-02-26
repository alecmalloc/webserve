#!/usr/bin/php-cgi
<?php

echo "<pre>Environment variables:<br>";
foreach($_SERVER as $key => $value) {
    echo htmlspecialchars("$key: $value") . "<br>";
}
echo "</pre>";

// Send proper headers
header("Content-Type: text/html");

// Process request
$message = isset($_POST['message']) ? $_POST['message'] : '';

// Generate HTML response
echo "<html><body>";
echo "<h1>PHP CGI Test</h1>";

if (!empty($message)) {
    echo "<p>Your message: " . htmlspecialchars($message) . "</p>";
} else {
    echo "<p>No message provided</p>";
    echo "<p>POST data: " . json_encode($_POST) . "</p>";
    echo "<p>REQUEST_METHOD: " . $_SERVER['REQUEST_METHOD'] . "</p>";
}

echo "</body></html>";
?>