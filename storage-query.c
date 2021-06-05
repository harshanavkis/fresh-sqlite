#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/stat.h>
#include<fcntl.h>

#include "sqlite3.h"
#include "mt_serialize.h"
#include "mt_wrapper.h"
#include "perf_counter.h"
#include "common_globals.h"

int tot_rows_processed = 0;

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  //int i;
  //for(i=0; i<argc; i++){
  //  printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  //}
  //printf("\n");
  tot_rows_processed += 1;
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

  make_record = 1;
  make_ssd_records_proc = 0;

  num_codec_enc = 0;
  num_codec_dec = 0;

  total_enc_time = 0;
  total_kdf_time = 0;
  mt_verify_time = 0;

	mt_obj *tree = (mt_obj*) malloc(sizeof(mt_obj));
	deserialize_init_mt(mt_path, tree);

	int num_ele = mt_get_size(tree->mt);
  // printf("Number of pages protected by tree:%d\n", num_ele);

  rc = sqlite3_open(db_path, &db);
  if(rc)
  {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  if(strlen(db_pwd))
  {
    rc = sqlite3_key(db, db_pwd, strlen(db_pwd), tree);
    if(rc != SQLITE_OK)
    {
      fprintf(stderr, "Can't set key for database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return 1;
    }
  }

  //rc = sqlite3_exec(db, "PRAGMA cache_size=0;", NULL, 0, &zErrMsg);
  //if(rc)
  //{
  //  fprintf(stderr, "Unable to reduce page cache size\n");
  //  return 1;
  //}

  struct timeval tv1, tv2;

  gettimeofday(&tv1, NULL);

  rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
  if(rc)
  {
    fprintf(stderr, "Cannot execute query\n");
    sqlite3_close(db);
    return 1;
  }

  gettimeofday(&tv2, NULL);

  double query_exec_time = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));


  // printf ("Total query execution time = %f seconds\n",
  //        (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
  //        (double) (tv2.tv_sec - tv1.tv_sec));

  // printf("Total time spent in codec: %f seconds\n", total_enc_time);
  // printf("Total time for key derivation: %f seconds\n", total_kdf_time);
  // printf("Total time spent in merkle tree verification during decryption: %f seconds\n", mt_verify_time);
  // printf("Total number of encryptions: %u\n", num_codec_enc);
  // printf("Total number of decryptions: %u\n", num_codec_dec);

  printf("{\"num_prot_pages\": %d, \"query_exec_time\": %f, \"codec_time\": %f, \"mt_verify_time\": %f, \"num_encryption\": %u, \"num_decryption\": %u}\n", 
    num_ele, query_exec_time, total_enc_time, mt_verify_time, num_codec_enc, num_codec_dec);
  fprintf(stderr, "ssd_records:%d\n", make_ssd_records_proc);
  fprintf(stderr, "Tot rows proc:%d\n",tot_rows_processed); 

	return 0;
}
