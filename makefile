all: projectServer projectClient

projectServer:
	gcc projectServer.c -lnsl -o projectServer

projectClient:
	gcc projectClient.c -lnsl -o projectClient

clean:
	rm -rf *o projectServer
	rm -rf *o projectClient
