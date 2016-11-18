import threading
import webbrowser
import BaseHTTPServer
import SimpleHTTPServer
import serial
import time

dicWords = {
    "close window": "1\n",
    "open window": "2\n",
    "ac on": "3\n",
    "ac off": "4\n",
    "shower on": "5\n",
    "shower off": "6\n"
}

FILE = 'webspeechdemo.html'
PORT = 8080
arduino = serial.Serial('COM3', 115200, timeout=.1) # ALTERAR
#arduino2 = serial.Serial('COM4', 115200, timeout=.1) # ALTERAR

class TestHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):
    """The test example handler."""

    def do_POST(self):
        """Handle a post request by returning the square of the number."""
        length = int(self.headers.getheader('content-length'))
        data_string = self.rfile.read(length)
        try:
            print "Recieved from web: %s" % data_string.strip().lower()

            if data_string.strip().lower() in dicWords:
                print "Data extracted from dictionary: %s" % dicWords[data_string.strip().lower()]
                arduino.write(dicWords[data_string.strip().lower()])
                #arduino2.write(dicWords[data_string.strip().lower()])
                '''while True:
					print "Esperando receber dados"
					data = arduino.readline().strip()
					if data:
						print "Data recieved from arduino:", data
						break
					time.sleep(1)'''
            else:
                print "Command not recognized"

            result = data_string
        except:
            result = 'error'
        self.wfile.write(result)

def open_browser():
    """Start a browser after waiting for half a second."""
    def _open_browser():
        webbrowser.open('http://localhost:%s/%s' % (PORT, FILE))
    thread = threading.Timer(0.5, _open_browser)
    thread.start()

def start_server():
    """Start the server."""
    server_address = ("", PORT)
    server = BaseHTTPServer.HTTPServer(server_address, TestHandler)
    server.serve_forever()

if __name__ == "__main__":
    open_browser()
    start_server()
