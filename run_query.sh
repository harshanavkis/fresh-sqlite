#!/bin/bash
OPENSSL_SRC=../openssl-src/ make storage-query
/home/hvub/vanilla-ndp/fresh-sqlite/storage-query $1 $2 $3 "$4"
