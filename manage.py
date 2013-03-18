import signal
import os
import sys
import select
import socket

inputs = []
contcli = []
server = -1
control = -1

def handler(signum, frame):
	for i in contcli:
		i.send('0')
		i.close()
		contcli.remove(i)
	for i in inputs:
		i.close()
		inputs.remove(i)
	server.close()
	control.close()
	sys.exit(-1)


HOST = ''
SERV_COMP_PORT = 9880
SERV_CONT_PORT = 9882
backlog = 2
nclients = 0
BUF_SIZE = 4096
running = 1
outputs = []
start =2
end = 30000
data = []
report = 'Perfect Numbers: '
clistr = ''
clients = []
modspersec = 0

# bind sockets
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, SERV_COMP_PORT))
server.listen(backlog)

control = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
control.bind((HOST, SERV_CONT_PORT))
control.listen(backlog)

# get available sockets
inputs.append(server)
inputs.append(control)

signal.signal(signal.SIGQUIT, handler)
signal.signal(signal.SIGINT, handler)
signal.signal(signal.SIGHUP, handler)
while running: 
	inputready, outputready, exceptready = select.select(inputs, outputs, [])

	for s in inputready:

		if s == server:
			# new client connection
			client, addr = server.accept()
			print 'Connected by', addr
			n = int(client.recv(BUF_SIZE))
			if (n == 1):
				clients.append(addr[0]);
				clients.append(str(start));
				clients.append(str(end));
				nclients += 1
				print 'There are now ',nclients,'clients'
				inputs.append(client)
				outputs.append(client)
				client.send(str(start)+ '\0' + str(end) + '\0');
			elif (n == 2):
				print 'Detected Report'
				print report + '\n'
				i = 0
				clistr = ''
				while (3*i) < len(clients):
					clistr += 'Client ' + str(i+1) + ': ' + clients[3*i] + '\n'
					clistr += 'start: ' + clients[3*i+1] + '\n'
					clistr += 'end: ' + clients[3*i+2] + '\n'
					clistr += '\n'
					i += 1
				print clistr

			else:
				print 'Detected Report'
				print report + '\n'
				i = 0
				clistr = ''
				while (3*i) < len(clients):
					clistr += 'Client ' + str(i+1) + ': ' + clients[3*i] + '\n'
					clistr += 'start: ' + clients[3*i+1] + '\n'
					clistr += 'end: ' + clients[3*i+2] + '\n'
					clistr += '\n'
					i += 1
				print clistr
				print 'Closing...'
				for i in contcli:
					i.send('0')
					i.close()
					contcli.remove(i)
				for i in inputs:
					i.close()
					inputs.remove(i)
					running = 0
					break
		
		elif s == control:
			client, addr = control.accept()
			contcli.append(client)
		else:
			# read from and write to clients
			data = s.recv(BUF_SIZE).split(';')
			report = report + data[1]
			modspersec = float(data[0])
			total = 15 * modspersec
			i = 0
			temp = end
			while i < total:
				temp += 1
				i += (temp/2)
			start = end + 1
			end = temp
			for i in range(len(inputs)):
				if inputs[i] == s:
					clients[3*(i-2) + 1] = str(start)
					clients[3*(i-2) + 2] = str(end)
			s.send(str(start)+ '\0' + str(end) + '\0');
server.close()
control.close()
