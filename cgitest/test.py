#!/usr/bin/python3

print("Status: 200 OK\r")
print("Content-Type: text/html\r")
print("Connection: close\r")
print("\r\n\r\n")

print("<html>")
print("<head><title>CGI Response</title></head>")
print("<body>")
print("<h1>lokakka</h1>")
print("<p>This is a proper HTTP response from a CGI script.</p>")
print("</body>")
print("</html>")
