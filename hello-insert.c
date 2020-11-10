#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/stat.h>
#include<fcntl.h>

#include "sqlite3.h"
#include "mt_serialize.h"
// #include "mt_wrapper.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void init_mt(mt_obj *tree)
{
  tree->mt = mt_create();
  printf("TREE_LEVELS:%d\n", TREE_LEVELS);
  if(tree->mt == NULL)
  {
    printf("Cannot create merkle tree\n");
    exit(1);
  }
  tree->write = 0;
  tree->num_blocks = 0;
}

int main(int argc, char *argv[])
{
  sqlite3* db;
  char *zErrMsg = 0;
  int rc, fd;
  int save_mt = 0;
  char *serial_mt = NULL;
  FILE *mt_file;

  mt_obj *tree = (mt_obj*) malloc(sizeof(mt_obj));

  struct stat stat_record;

  rc = sqlite3_open(argv[1], &db);
  if(rc)
  {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  // if(!stat(argv[2], &stat_record))
  // {
  //   fd = open(argv[2], O_RDWR|O_CREAT);
  //   init_mt(tree);
  //   save_mt = 1;
  // }
  // else
  // {
  //   fd = open(argv[2], O_RDWR);
  //   deserialize_init_mt(fd, tree);
  //   save_mt = 1;
  // }
  init_mt(tree);

  rc = sqlite3_key(db, argv[3], strlen(argv[3]), tree);
  // rc = sqlite3_key(db, argv[3], strlen(argv[3]));
  if(rc != SQLITE_OK)
  {
    fprintf(stderr, "Can't set key for database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  FILE *qfd = fopen(argv[4], "r");
  char *qfd_line;
  ssize_t read;
  size_t qfd_len = 0;

  // rc = sqlite3_exec(db, argv[4], callback, 0, &zErrMsg);

  while ((read = getline(&qfd_line, &qfd_len, qfd)) != -1)
  {
    rc = sqlite3_exec(db, qfd_line, callback, 0, &zErrMsg);
    if(rc)
    {
      fprintf(stderr, "Cannot execute query\n");
      sqlite3_close(db);
      return 1;
    }
  }

  rc = sqlite3_exec(db, argv[5], callback, 0, &zErrMsg);
  if(rc)
  {
    fprintf(stderr, "Cannot execute query\n");
    sqlite3_close(db);
    return 1;
  }

  int num_ele = mt_get_size(tree->mt);
  printf("Number of pages protected by tree:%d\n", num_ele);

  sqlite3_close(db);

  int num_bytes = serialize_mt(&serial_mt, tree->mt);
  printf("Creating merkle tree file...\n");
  while((mt_file = fopen(argv[2], "wb+")) == NULL){}

  printf("Merkle tree file created successfully...\n");
  fwrite(serial_mt, 1, num_bytes, mt_file);
  fclose(mt_file);

  return 0;
}
