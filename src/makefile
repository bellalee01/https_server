.PHONY:clean

default:
	gcc -O server.c protocal.c -o server -lssl -lcrypto -lpthread -levent -levent_openssl -levent_pthreads
	gcc -O http_client.c -o http_client -lssl -lcrypto -lpthread -levent -levent_openssl -levent_pthreads
	gcc -O https_client.c -o https_client -lssl -lcrypto -lpthread -levent -levent_openssl -levent_pthreads

clean:
	rm server *client *log
