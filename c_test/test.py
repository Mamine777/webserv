#!/usr/bin/python3
import sys

# Read from stdin (POST data sent by the parent process)
input_data = sys.stdin.read()

# Output header (CGI Response Header)
print("Content-Type: text/plain")
print("\n\r")  # Blank line to separate headers from the body

# Output the body (CGI response message)
print(f"Received POST data: {input_data}")
