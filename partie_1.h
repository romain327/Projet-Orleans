#ifndef PARTIE_1_H
#define PARTIE_1_H

// définition d'une macro permettant de récupérer la taille d'un champ d'une structure
#define member_size(type, member) sizeof(((type *)0)->member

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct table_s {
    char name[12];
    uint32_t size;
    uint32_t reserved;
    char datetime_str[28];
} __attribute__((packed)) table_t;

void print_table(table_t * table_t_struct, char * datetime_str2);
void partie_1(char * filename);

#endif //PARTIE_1_H
