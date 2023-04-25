all:
	gcc client/client.c client/client_tools.c utils/msglib.c -o client.exe
	gcc server/server.c server/server_tools.c utils/msglib.c -o server.exe
clean:
	rm client.exe
	rm server.exe
