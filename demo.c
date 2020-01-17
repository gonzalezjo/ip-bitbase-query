// (c) Jordi Gonzalez, 2020

#include <stdlib.h>
#include <stdio.h>
#include "fastsearch.h"

int main(void) {
#ifdef MAKE_DB
  create_database(RAW_IP_SOURCE, DATABASE_NAME);
#else 
  (void) create_database;
#endif

  database *l_database = read_database(DATABASE_NAME);
  char     *buf        = calloc(sizeof(char), 32);

  if (!l_database) {
    printf("Failed to read database file!\n");
    exit(EXIT_FAILURE);
  }

  while (1) { 
    printf("Enter an IP address: ");

    if (scanf("%s", buf) == EOF) {
      break;
    }

    printf("%s is %s in the database.\n", 
            buf,
            ip_in_database(l_database, buf) ? "present" : "not");
  }

  free(buf);
  free(l_database);

  return EXIT_SUCCESS;
}
