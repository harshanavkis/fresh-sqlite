#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/stat.h>
#include<fcntl.h>

#include "sqlite3.h"
#include "mt_serialize.h"
#include "mt_wrapper.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

int main(int argc, char const *argv[])
{
	char *mt_path = argv[1];
	char *db_path = argv[2];
	char *db_pwd  = argv[3];
	char *query   = argv[4];

	sqlite3* db;
  char *zErrMsg = 0;
  int rc;

	mt_obj *tree = (mt_obj*) malloc(sizeof(mt_obj));
	deserialize_init_mt(mt_path, tree);

	int num_ele = mt_get_size(tree->mt);
  printf("Number of pages protected by tree:%d\n", num_ele);

  rc = sqlite3_open(db_path, &db);
  if(rc)
  {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  rc = sqlite3_key(db, db_pwd, strlen(db_pwd), tree);
  if(rc != SQLITE_OK)
  {
    fprintf(stderr, "Can't set key for database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
  if(rc)
  {
    fprintf(stderr, "Cannot execute query\n");
    sqlite3_close(db);
    return 1;
  }

	return 0;
}