import signal
import os
import socket
import sys

s = 0

def handler(signum, frame):
	s.close()
	sys.exit(-1)

# very similar to example in docs.python.org/2/library/socket.html
if (len(sys.argv) < 2):
	print 'Please enter the inet address'
	sys.exit(-1)
HOST = sys.argv[1]
PORT = 9880
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
signal.signal(signal.SIGQUIT, handler)
signal.signal(signal.SIGINT, handler)
signal.signal(signal.SIGHUP, handler)
if (len(sys.argv) == 3):
	s.send('3')
else:
	s.send('2')
s.close()
