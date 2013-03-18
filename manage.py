import select
import socket

HOST = ''
SERV_COMP_PORT = 9880
SERV_CONT_PORT = 9882
backlog = 2
nclients = 0
BUF_SIZE = 4096
running = 1
inputs = []
outputs = []
contcli = []
start =2
end = 30000
data = []
report = 'Perfect Numbers: '
clients = ''
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
while running: 
	inputready, outputready, exceptready = select.select(inputs, outputs, [])

	for s in inputready:

		if s == server:
			# new client connection
			client, addr = server.accept()
			print 'Connected by', addr
			n = int(client.recv(BUF_SIZE))
			if (n == 1):
				clients += ('Client ' + str(nclients + 1) + ': ')
				clients += (addr[0] + '\n')
				clients += ('start: ' + str(start) + '\n')
				clients += ('end: ' + str(end) + '\n\n')
	
				nclients += 1
				print 'There are now ',nclients,'clients'
				inputs.append(client)
				outputs.append(client)
				client.send(str(start)+ '\0' + str(end) + '\0');
			elif (n == 2):
				print 'Detected Report'
				print report + '\n'
				print clients
			else:
				print 'Detected Report'
				print report + '\n'
				print clients
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
			s.send(str(start)+ '\0' + str(end) + '\0');
server.close()
control.close()
