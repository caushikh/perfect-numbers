import socket
import sys


# very similar to example in docs.python.org/2/library/socket.html
if (len(sys.argv) < 2):
	print 'Please enter the inet address'
	sys.exit(-1)
HOST = sys.argv[1]
PORT = 9880
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
if (len(sys.argv) == 3):
	s.send('3')
else:
	s.send('2')
s.close()
