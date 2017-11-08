all: projectServer projectClient

projectServer:
	gcc projectServer.c -lnsl -w -o projectServer

projectClient:
	gcc projectClient.c -lnsl -w -o projectClient

clean:
	rm -rf *o projectServer
	rm -rf *o projectClient
