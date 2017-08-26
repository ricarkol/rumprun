// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2015 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sqlite/sqlite3.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>

int fdatasync(int name) {
  //printf("fdatasync(%d) called, not supported\n", name);
  //errno = ENOTSUP;
  return 0;
}


long sysconf2(int name) {
  printf("sysconf(%d) called, not supported\n", name);
  errno = ENOTSUP;
  return -1;
}
uid_t geteuid() {
  printf("geteuid() called, not supported\n");
  return 0;
}
int utime(const char *filename, const struct utimbuf *times)
{
  printf("utime(%s, %p) called, not supported\n", filename, times);
  errno = ENOTSUP;
  return -1;
}
int utimes(const char *filename, const struct timeval times[2])
{
  printf("utimes(%s, %p) called, not supported\n", filename, times);
  errno = ENOTSUP;
  return -1;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
static int sqlite_exec(sqlite3* db, const char* message)
{
  char* error_message = NULL;
  int rc = sqlite3_exec(db, message, callback, 0, &error_message);
  if (rc) {
    assert(error_message);
    fprintf(stderr, "SQL error: %s\n", error_message);
    sqlite3_free(error_message);
    return -1;
  }
  return 0;
}

int main()
{
  //
  sqlite3* db;
  char buffer[1024];
  int rc;

  rc = sqlite3_open(":memory:", &db);
  //rc = sqlite3_open_v2("/db/company/my.db", &db, SQLITE_OPEN_READWRITE, "unix-none");
  assert(rc == 0);

  rc = sqlite_exec(db, "PRAGMA journal_mode=OFF;");
  assert(rc == 0);

  rc = sqlite_exec(db,
      "CREATE TABLE company("
      "id INT PRIMARY KEY     NOT NULL,"
      "name           TEXT    NOT NULL,"
      "age            INT     NOT NULL,"
      "address        CHAR(50),"
      "salary         REAL );");
  assert(rc == 0);
  if (rc == 0) {

    int i;
    for (i = 0; i < 100000; i++)
    {
      static int next_id = 0;
      next_id++;
      char buffer[1024];

      snprintf(buffer, sizeof(buffer),
          "INSERT INTO company("
          "id, name, age, address, salary)"
          " VALUES ("
          "%d, 'Fjell Kåre', 12, 'Fjellveien 12', 100.0 );",
          next_id);
      rc = sqlite_exec(db, buffer);
      assert(rc == 0);
    }
    
    rc = sqlite_exec(db,
        "SELECT id, name, age FROM company");
  }

  rc = sqlite3_close(db);

  return 0;
}
