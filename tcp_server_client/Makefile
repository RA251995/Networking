all: tcp_server tcp_client tcp_server_mux http_server

tcp_server:
	gcc -Wall $@.c -o $@.out

tcp_client:
	gcc -Wall $@.c -o $@.out

tcp_server_mux:
	gcc -Wall $@.c -o $@.out

http_server:
	gcc -g -Wall $@.c -o $@.out

clean:
	rm -f *.out
