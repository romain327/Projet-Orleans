#ifndef PARTIE_2_H
#define PARTIE_2_H

// définition d'une macro permettant de récupérer la taille d'un champ d'une structure
#define member_size(type, member) sizeof(((type *)0)->member

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct dico_entry_s {
    char name[8];
    uint32_t addr1;
    uint32_t addr2;
    uint16_t cnt;
} __attribute__((packed)) dico_entry_t;

void print_dico_entry(dico_entry_t * dico_entry_struct);
void partie_2(char * filename);
#endif //PARTIE_2_H
