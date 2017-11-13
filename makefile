all: projectServer projectClient

projectServer: projectServer.c
	gcc projectServer.c -lnsl -w -o projectServer

projectClient: projectClient.c
	gcc projectClient.c -lnsl -w -o projectClient

clean:
	rm -rf *o projectServer
	rm -rf *o projectClient
	rm -rf *o recvFile.txt
