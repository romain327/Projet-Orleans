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
 * \param table_t_struct structure à afficher
 */
void print_table(table_t * table_t_struct) {
	printf("Name: %s\n", table_t_struct->name);
	printf("Size: %d\n", table_t_struct->size);
	printf("Reserved: %d\n", table_t_struct->reserved);
	printf("Datetime: %s\n", table_t_struct->datetime_str);
	printf("\n");
}

int main() {
	uint32_t struct_nb = 0;
	uint32_t cpt = 0;
	uint8_t c = 0x0;
	uint8_t * temp_c = &c;

	/*
	 * On ouvre le fichier flash10.bin en lecture seule. Cela nous donne un numéro de descripteur de fichier.
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
	table_t table_t_array[struct_nb];
	lseek(fd, 0, SEEK_SET);

	/*
	 * On affiche le nombre de structures table_t qu'on a trouvé.
	 * Techniquement parlant, elles n'ont pas encore été crées, mais on sait déjà qu'on en aura struct_nb.
	 */
	printf("Nombre de structures: %d\n", struct_nb);

	/*
	 * On lit les structures du fichier et on les stocke dans le tableau de structures qu'on a créé.
	 */
	while(cpt<struct_nb) {
		read(fd, &table_t_array[cpt].name, 12);
		read(fd, &table_t_array[cpt].size, 4);
		read(fd, &table_t_array[cpt].reserved, 4);
		read(fd, &table_t_array[cpt].datetime_str, 28);
		cpt++;
	}

	/*
	 * On affiche les structures qu'on a créées.
	 */
	cpt = 0;
	while(cpt<struct_nb) {
		print_table(&table_t_array[cpt]);
		cpt++;
	}

	/*
	 * On veut maintenant créer les fichiers qui correspondent aux structures qu'on a créées.
	 * On va donc parcourir le tableau de structures, et pour chaque structure, on va créer un fichier.
	 * On va ensuite écrire le contenu du fichier, que l'on calcule grâce à la taille renseignée
	 * dans la structure.
	 * A chaque tour de boucle, on va donc allouer un tableau de la taille du contenu à écrire.
	 * On va ensuite lire le contenu du fichier, on n'a pas besoin de repositionner la tête de lecture puisqu'on est déjà au bon endroit.
	 * On va ensuite concaténer le nom de notre structure avec la commande touch, ce qui va nous permettre de créer le fichier.
	 * On va ensuite ouvrir le fichier en écriture seule, et écrire le contenu qu'on a lu précédement.
	 * Enfin, on ferme le fichier, et on passe à la structure suivante.
	 */
	cpt = 0;
	uint8_t cpt2 = 0;
	while(cpt<struct_nb || table_t_array[cpt].size != 0) {
		char content[table_t_array[cpt].size];
		read(fd, &content, table_t_array[cpt].size);

		for(char i = 0; i < 12; i++) {
			while(table_t_array[cpt].name[i] != 0x20 && table_t_array[cpt].name[i] != 0x0) {
				cpt2++;
			}
		}

		char n[cpt2];
		for(char i = 0; i < cpt2; i++) {
			n[i] = table_t_array[cpt].name[i];
		}

		char *cmd = malloc((11 + cpt2)*sizeof(char));
		sprintf(cmd, "touch %s %s", n, ".txt");
		system(cmd);
		free(cmd);

		const ssize_t fd2 = open(table_t_array[cpt].name, O_WRONLY);
		write(fd2, &content, table_t_array[cpt].size);
		close(fd2);
		cpt++;
	}

	close(fd);
}