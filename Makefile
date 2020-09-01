CC=gcc
CFLAGS=-I${OPENSSL_SRC}/include/
LDFLAGS= -lpthread -ldl -lcrypto -lssl
CPPFLAGS= -DSQLITE_HAS_CODEC

hello-insert: hello-insert.c sqlite3.c
	$(CC) -o hello-insert hello-insert.c sqlite3.c mt_serialize.c $(CFLAGS) $(LDFLAGS) $(CPPFLAGS) merkle-tree/src/libMerkleTree.a -g

hello-query: hello-query.c sqlite3.c
	$(CC) -o hello-query hello-query.c sqlite3.c mt_serialize.c $(CFLAGS) $(LDFLAGS) $(CPPFLAGS) -L${OPENSSL_SRC} merkle-tree/src/libMerkleTree.a -g

.PHONY: clean

clean:
	rm -f hello-insert
	rm -f hello-query

