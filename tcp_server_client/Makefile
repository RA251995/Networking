all: tcp_server tcp_client tcp_server_mux

tcp_server:
	gcc -Wall $@.c -o $@.out

tcp_client:
	gcc -Wall $@.c -o $@.out

tcp_server_mux:
	gcc -Wall $@.c -o $@.out

clean:
	rm -f *.out
