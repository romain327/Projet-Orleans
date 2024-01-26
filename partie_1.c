#include "partie_1.h"

// routine d'affichage des structures
void print_table(table_t * table_t_struct, char * datetime_str2) {
	printf("Name: %s\n", table_t_struct->name);
	printf("Size: %d\n", table_t_struct->size);
	printf("Reserved: %d\n", table_t_struct->reserved);
	memcpy(datetime_str2, table_t_struct->datetime_str, sizeof(table_t_struct->datetime_str));
	printf("Datetime: %s\n", datetime_str2);
	printf("\n");
}

void partie_1(char * filename) {
	uint32_t struct_nb = 0;
	uint32_t real_struct_nb = 0;
	uint32_t cpt = 0;
	uint32_t cpt2;
	uint8_t c = 0x0;
	uint8_t rc;
	uint8_t * temp_c = &c;
	uint8_t * temp_rc = &rc;

	// ouverture du fichier
	const uint32_t fd = open(filename, O_RDONLY);
	if(fd == -1) {
		perror("Ce fichier n'existe pas.");
		exit(0);
	}

	// récuperation du nombre de structures dans la variable struct_nb, et du nombre de structures non-vides dans la variable real_struct_nb
	while(c == 0x0) {
		read(fd, temp_rc, 1);
		if(rc != 0x20) {
			real_struct_nb++;
		}
		lseek(fd, 14, SEEK_CUR);
		read(fd, temp_c, 1);
		struct_nb++;
		lseek(fd, 32, SEEK_CUR);
	}
	struct_nb--;
	real_struct_nb--;
	printf("Nombre de structures: %d\n", struct_nb);
	printf("Nombre de structures non vides: %d\n", real_struct_nb);

	table_t table_t_array[real_struct_nb];
	lseek(fd, 0, SEEK_SET);

	// lecture de la fat et remplissage des structures
	while(cpt<real_struct_nb) {
		memset(&table_t_array[cpt], 0, sizeof(table_t_array[cpt]));
		read(fd, &table_t_array[cpt].name, member_size(table_t, name)));
		read(fd, &table_t_array[cpt].size, member_size(table_t, size)));
		read(fd, &table_t_array[cpt].reserved, member_size(table_t, reserved)));
		read(fd, &table_t_array[cpt].datetime_str, member_size(table_t, datetime_str)));
		cpt++;
	}

	// affichage des structures non-vides
	const int sizeof_datetime_str2 = member_size(table_t, datetime_str))+1;
	char datetime_str2[sizeof_datetime_str2];
	cpt = 0;
	while(cpt<real_struct_nb) {
		memset(datetime_str2, 0, sizeof_datetime_str2);
		print_table(&table_t_array[cpt], datetime_str2);
		cpt++;
	}


	cpt = 0;
	lseek(fd, 0, SEEK_SET);
	while(cpt<real_struct_nb) {
		cpt2 = 0;

		// lecture du contenu du fichier à créer
		char content[table_t_array[cpt].size + 1];
		read(fd, &content, table_t_array[cpt].size);

		for(int i = 0; i < member_size(table_t, name)); i++) {
			if(table_t_array[cpt].name[i] != 0x20 && table_t_array[cpt].name[i] != 0x0) {
				cpt2++;
			}
		}
		cpt2--;

		// récupération du nom exact du fichier
		char n[cpt2+1];
		for(int i = 0; i < cpt2; i++) {
			n[i] = table_t_array[cpt].name[i];
		}
		n[cpt2] = '\0';

		// association de l'extension .bin
		char * name = malloc(cpt2 + 4);
		sprintf(name, "%s%s", n, ".bin");

		// Création et écriture du contenu fichier
		const ssize_t fd2 = open(name, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
		if(fd2 == -1) {
			perror("Ce fichier n'existe pas.");
			exit(0);
		}
		write(fd2, &content, table_t_array[cpt].size);
		close(fd2);

		const ssize_t fd3 = open(name, O_RDONLY);
		if(fd3 == -1) {
			perror("Ce fichier n'existe pas.");
			exit(0);
		}

		// vérification du nombre d'octets écrits
		off_t offset = lseek(fd3, 0, SEEK_END);
		if(offset != table_t_array[cpt].size) {
			printf("Erreur lors de l'écriture du fichier %s, taille supposée : %d octets, taille réelle : %ld octets", name, table_t_array[cpt].size, offset);
		}
		close(fd3);

		free(name);
		cpt++;
	}
	close(fd);
}
