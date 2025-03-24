# Webserv - HTTP Server
Webserv is a lightweight HTTP server written in C++98, built as part of the 42 Network curriculum. It supports CGI execution, request handling, and custom configuration parsing.

## Made By
M amine Kariouh && Florian Ghysbrecht && Firdawsse Mazouz


## Features
- Handles HTTP requests (GET, POST, DELETE)
- CGI script execution (Python, PHP, etc.)
- Configurable through a custom configuration file
- Directory listing support
- Minimal dependencies, built using C++98

## Installation & Usage

### 1. Build the Project
```bash
#Compile the program
make
#Execute the code
./bin/webserv defaults/example.config

#Test the server
curl -X GET http://localhost:8080/
curl -X POST http://localhost:8080/upload -d "data=test"
curl -X DELETE http://localhost:8080/file.txt
or
you can serach in webBrowser http://localhost:8080/
