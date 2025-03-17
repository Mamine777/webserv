#!/usr/bin/php
<?php
print "Status: 200 OK\r\n";
print "Content-Type: text/html\r\n";
print "Connection: close\r\n";
print "\r\n\r\n";
print "<html>";
print "<head><title>CGI Response</title></head>";
print "<body>";
print "<h1>lokakka</h1>";
print "<p>This is a proper HTTP response from a CGI script.</p>";
print "</body>";
print "</html>";
?>
