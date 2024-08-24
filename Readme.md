the basic C web server that dynamically chooses an available port and can serve files from a frontend folder containing index.html, script.js, and style.css. This also includes dynamic memory management for handling requests and responses.


Explanation of the Code


--Dynamic Port Selection:

The find_available_port function attempts to bind the server socket to ports starting from 8080 up to 65535. If a port is busy, it tries the next one until it finds an available port.


--Dynamic Memory Management:

The load_file function reads the contents of a file (like index.html, script.js, or style.css) into a dynamically allocated buffer. This buffer is freed after sending the content.


--Serving Files:

This example only serves the index.html file from the frontend directory, but you can extend it to serve script.js and style.css by checking the requested path in the HTTP request.


--Basic HTML Serving:

The server sends an HTTP response with a dynamically generated header, followed by the contents of the index.html file.
