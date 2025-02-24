<!-- filepath: /home/leschenb/Desktop/webserve/cgi-bin/test.php -->
#!/usr/bin/php-cgi
<?php
header('Content-Type: text/html');
echo "<html><body>";
echo "<h1>PHP CGI Test</h1>";

if (isset($_POST['message'])) {
    echo "<p>Your message: " . htmlspecialchars($_POST['message']) . "</p>";
} else {
    echo "<p>No message provided</p>";
}

echo "</body></html>";
?>