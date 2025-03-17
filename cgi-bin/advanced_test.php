#!/usr/bin/php-cgi
<?php
header('Content-Type: text/html');
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Advanced PHP CGI Test</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        h1 { color: #4285f4; }
        h2 { color: #34a853; margin-top: 20px; }
        pre { background: #f5f5f5; padding: 10px; border-radius: 5px; }
        .section { margin-bottom: 30px; border-bottom: 1px solid #ddd; padding-bottom: 20px; }
    </style>
</head>
<body>
    <h1>Advanced PHP CGI Test</h1>
    
    <div class="section">
        <h2>Request Information</h2>
        <p><strong>Request Method:</strong> <?php echo $_SERVER['REQUEST_METHOD']; ?></p>
        <p><strong>Script Name:</strong> <?php echo $_SERVER['SCRIPT_NAME']; ?></p>
        <p><strong>Server Software:</strong> <?php echo $_SERVER['SERVER_SOFTWARE'] ?? 'Unknown'; ?></p>
        <p><strong>Server Time:</strong> <?php echo date('Y-m-d H:i:s'); ?></p>
    </div>
    
    <div class="section">
        <h2>Form Data</h2>
        <?php if (!empty($_GET)): ?>
            <h3>GET Parameters:</h3>
            <pre><?php print_r($_GET); ?></pre>
        <?php endif; ?>
        
        <?php if (!empty($_POST)): ?>
            <h3>POST Parameters:</h3>
            <pre><?php print_r($_POST); ?></pre>
        <?php endif; ?>
        
        <?php if (!empty($_FILES)): ?>
            <h3>Uploaded Files:</h3>
            <pre><?php print_r($_FILES); ?></pre>
        <?php endif; ?>
    </div>
    
    <div class="section">
        <h2>HTTP Headers</h2>
        <?php
        $headers = [];
        foreach ($_SERVER as $key => $value) {
            if (strpos($key, 'HTTP_') === 0) {
                $headers[str_replace('HTTP_', '', $key)] = $value;
            }
        }
        ?>
        <pre><?php print_r($headers); ?></pre>
    </div>
    
    <div class="section">
        <h2>Environment Variables</h2>
        <pre><?php print_r($_ENV); ?></pre>
    </div>
</body>
</html>