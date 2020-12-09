#!/bin/bash
OPENSSL_SRC=../openssl-src/ make hello-query
/home/hvub/vanilla-ndp/fresh-sqlite/hello-query $1 $2 $3 "$4"
