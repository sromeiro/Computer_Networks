all: projectServer projectClient

projectServer: projectServer.c
	gcc projectServer.c -lnsl -o projectServer

projectClient: projectClient.c
	gcc projectClient.c -lnsl -o projectClient

clean:
	rm -rf *o projectServer
	rm -rf *o projectClient
	rm -rf *o recvFile.txt
