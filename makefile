CC=gcc
LD=gcc
CFLAGS=-g -std=c99 -Wall
CPPFLAGS=-I. -I./include
SP_LIBRARY_DIR=.

all:  chat_client chat_server

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

chat_client:  $(SP_LIBRARY_DIR)/libspread-core.a chat_client.o
	$(LD) -o $@ chat_client.o $(SP_LIBRARY_DIR)/libspread-core.a -ldl -lm -lrt -lnsl $(SP_LIBRARY_DIR)/libspread-util.a

chat_server: $(SP_LIBRARY_DIR)/libspread-core.a chat_server.o
	$(LD) -o $@ chat_server.o $(SP_LIBRARY_DIR)/libspread-core.a -ldl -lm -lrt -lnsl $(SP_LIBRARY_DIR)/libspread-util.a

clean:
	rm -f *.o chat_client
	rm -f *.o chat_server

