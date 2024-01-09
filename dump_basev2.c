#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

typedef struct table_s {
	char name[12];
	uint32_t size;
	uint32_t reserved;
	char datetime_str[28];
} __attribute__((packed)) table_t;

/**
 * \brief routine d'impression des structures qui composent la fat
 * \param table structure à afficher
 */
void print_table(table_t * table) {
	printf("Name: %s\n", table->name);
	printf("Size: %d\n", table->size);
	printf("Reserved: %d\n", table->reserved);
	//printf("Datetime: %s\n", table->datetime_str);
	printf("\n");
}

int main() {
	uint32_t struct_nb = 0;
	uint32_t j = 0;
	uint8_t c = 0x0;
	uint8_t * temp_c = &c;

	/*
	 * On ouvre le fichier flash10.bin en lecture seule. Cela nous donne un descripteur de fichier.
	 * Le fait qu'on manipule un descripteur de fichier est important ici dans le sens où on va ouvrir le
	 * fichier d'une autre manière plus tard.
	 */
	const uint32_t fd = open("flash10.bin", O_RDONLY);
	if(fd == -1) {
		perror("Ce fichier n'existe pas.");
		exit(0);
	}

	/*
	 * On positionne la tête de lecture au sur le 16ème octet de la première structure.
	 * L'idée est que, comme l'on remarque que le 16ème octet de toutes les structures vaut 0x00,
	 * mais que ce n'est plus le cas après, tant que ce 16ème octet vaut 0x00, on sait qu'on est dans une structure table_t.
	 * Cela nous permet de récupérer le nombre de structures table_t dans le fichier, qui sera stocké dans l'entier struct_nb.
	 */
	lseek(fd, 15, SEEK_SET);
	while(c == 0x0) {
		read(fd, temp_c, 1);
		struct_nb++;
		lseek(fd, sizeof(table_t)-1, SEEK_CUR);
	}

	/*
	 * On retranche 1 au nombre de structure puisqu'on en a compté une de trop.
	 * On alloue un tableau de structures table_t de taille struct_nb, et on se repositionne au début du fichier.
	 */
	struct_nb--;
	table_t table_t_list[struct_nb];
	lseek(fd, 0, SEEK_SET);

	/*
	 * On affiche le nombre de structures table_t qu'on a trouvé.
	 * Techniquement parlant, elles n'ont pas encore été crées, mais on sait déjà qu'on en aura struct_nb.
	 */
	printf("Nombre de structures: %d\n", struct_nb);

	/*
	 * On ouvre le fichier flash10.bin en lecture seule, mais cette fois-ci avec fopen.
	 * Cela nous donne un pointeur de fichier, on a donc un flux de caractères, et non plus un entier comme précédement.
	 * On peut maintenant lire le fichier et remplir notre tableau de structures.
	 */
	while(j<struct_nb) {
		read(fd, &table_t_list[j].name, 12);
		read(fd, &table_t_list[j].size, 4);
		read(fd, &table_t_list[j].reserved, 4);
		read(fd, &table_t_list[j].datetime_str, 28);
		j++;
	}
	close(fd);

	/*
	 * On affiche les structures qu'on a créées.
	 */
	j = 0;
	while(j<struct_nb) {
		print_table(&table_t_list[j]);
		j++;
	}
}