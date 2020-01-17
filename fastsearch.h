// (c) Jordi Gonzalez, 2020

#include <stdint.h>
#include "config.h"

#define NUM_TERMINALS (1<<24)

typedef union ip_address_t { 
  uint32_t bin;
  uint8_t  bytes[4];
} __attribute__((aligned(32))) ip_address;

typedef uint64_t terminal[4];

typedef struct db_header_t
{
  uint64_t max_terminal_address;
  terminal terminals[NUM_TERMINALS];
} database;

void create_database(const char *p_src, const char *p_dst);
database *read_database(const char *p_name);
int ip_in_database(const database *p_database, const char *p_ip_address);
