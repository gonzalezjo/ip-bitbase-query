// (c) Jordi Gonzalez, 2020

#include <stdlib.h>
#include <stdio.h>
#include <lz4.h>

#include "config.h"
#include "fastsearch.h"

static inline ip_address decode_ip(const char *p_ip) {
  ip_address l_ip = {0};

  sscanf(p_ip, "%hhu.%hhu.%hhu.%hhu",
        &l_ip.bytes[0],
        &l_ip.bytes[1],
        &l_ip.bytes[2],
        &l_ip.bytes[3]);

  return l_ip;
};

void create_database(const char *p_src, const char *p_dst) {
  char      l_buffer[32] = {0};
  size_t    l_max_term   = 0;
  database *l_database   = calloc(1, sizeof(database));

  if (!l_database) {
    printf("calloc failed!\n");
    exit(EXIT_FAILURE);
  }

  // Generate DB
  {
    FILE *l_ip_list_handle;

    if (!(l_ip_list_handle = fopen(p_src, "r"))) {
      printf("Failed to open dictionary file '%s'.", p_src);
      exit(EXIT_FAILURE);
    }

    while (fgets(l_buffer, sizeof(l_buffer), l_ip_list_handle)) {
      const ip_address l_ip    = decode_ip(l_buffer);
      const uint8_t    l_lsb   = l_ip.bytes[3];
      const size_t     l_index = l_ip.bin & 0xffffff;

      if (l_index >= l_max_term) {
        l_max_term = l_index + 1;
        l_database->max_terminal_address = l_max_term;
      }

      l_database->terminals[l_index][l_lsb >> 6] |= 1ULL << (l_lsb & 0b111111);
    }

    fclose(l_ip_list_handle);
  }

  // Write out compressed DB
  {
    char        *l_buf      = calloc(1, sizeof(database));
    const size_t l_bytes    = l_max_term * sizeof(terminal)
                            + sizeof(l_database->max_terminal_address);
    const size_t l_size     = LZ4_compress_fast((void*) l_database, l_buf, 
                                                l_bytes, sizeof(database), 
                                                LZ4_ACCEL_LVL);
    FILE *l_database_handle = fopen(p_dst, "wb");

    fprintf(l_database_handle, "%zu", l_size);
    fwrite(l_buf, l_size, 1, l_database_handle);
    fclose(l_database_handle);
    free(l_buf);
  }

  free(l_database);
}

database *read_database(const char *p_name) {
  database *l_database = calloc(1, sizeof(database));
  FILE *l_database_handle = fopen(p_name, "r"); 

  if (!l_database) { 
    return NULL;
  } else { 
    size_t l_database_size;
    void *l_compressed_bin;

    fscanf(l_database_handle, "%zu", &l_database_size);
    l_compressed_bin = calloc(1, l_database_size);

    fread(l_compressed_bin, l_database_size, 1, l_database_handle);
    fclose(l_database_handle);

    LZ4_decompress_safe(l_compressed_bin, (void*) l_database, 
                        l_database_size, sizeof(database));
  }

  return l_database;
}

int ip_in_database(const database *p_database, const char *p_ip_address) {
  const ip_address l_ip    = decode_ip(p_ip_address);
  const uint8_t    l_lsb   = l_ip.bytes[3];
  const size_t     l_index = l_ip.bin & 0xffffff;

  if (l_index >= p_database->max_terminal_address) {
    return 0;
  }

  return !!(p_database->terminals[l_index][l_lsb >> 6] & 1ULL << (l_lsb & 0b111111));
}
