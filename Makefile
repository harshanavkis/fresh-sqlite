CC=gcc
CFLAGS=
LDFLAGS= -lpthread -ldl -lcrypto -lssl
CPPFLAGS= -DSQLITE_HAS_CODEC

hello: hello.c sqlite3.c
	$(CC) -o hello hello.c sqlite3.c $(CFLAGS) $(LDFLAGS) $(CPPFLAGS) merkle-tree/src/libMerkleTree.a -g

.PHONY: clean

clean:
	rm -f hello

