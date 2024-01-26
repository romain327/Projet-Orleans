#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

// On définit une macro qui va nous permettre de récupérer la taille d'un membre d'une structure.
#define member_size(type, member) sizeof(((type *)0)->member)

typedef struct table_s {
	char name[12];
	uint32_t size;
	uint32_t reserved;
	char datetime_str[28];
} __attribute__((packed)) table_t;

typedef struct dico_entry_s {
	char name[8];
	uint32_t addr1;
	uint32_t addr2;
	uint16_t cnt;
} __attribute__((packed)) dico_entry_t;

/**
 * \brief routine d'impression des structures qui composent la fat
 * \param table_t_struct structure à afficher
 */
void print_table(table_t * table_t_struct, char * datetime_str2) {
	printf("Name: %s\n", table_t_struct->name);
	printf("Size: %d\n", table_t_struct->size);
	printf("Reserved: %d\n", table_t_struct->reserved);
	memcpy(datetime_str2, table_t_struct->datetime_str, sizeof(table_t_struct->datetime_str));
	printf("Datetime: %s\n", datetime_str2);
	printf("\n");
}

void print_dico_entry(dico_entry_t * dico_entry_struct) {
	printf("Name: %s\n", dico_entry_struct->name);
	printf("Addr1: %d\n", dico_entry_struct->addr1);
	printf("Addr2: %d\n", dico_entry_struct->addr2);
	printf("Cnt: %d\n", dico_entry_struct->cnt);
	printf("\n");
}

int main() {
	uint32_t struct_nb = 0;
	uint32_t real_struct_nb = 0;
	uint32_t cpt = 0;
	uint32_t cpt2;
	uint8_t c = 0x0;
	uint8_t rc;
	uint8_t * temp_c = &c;
	uint8_t * temp_rc = &rc;

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

	/*
	 * On retranche 1 au nombre de structure puisqu'on en a compté une de trop, et on affiche le nombre de structures.
	 */
	struct_nb--;
	real_struct_nb--;
	printf("Nombre de structures: %d\n", struct_nb);
	printf("Nombre de structures non vides: %d\n", real_struct_nb);

	table_t table_t_array[real_struct_nb];
	lseek(fd, 0, SEEK_SET);

	/*
	 * On lit les structures du fichier et on les stocke dans le tableau de structures qu'on a créé.
	 */
	while(cpt<real_struct_nb) {
		memset(&table_t_array[cpt], 0, sizeof(table_t_array[cpt]));
		read(fd, &table_t_array[cpt].name, member_size(table_t, name));
		read(fd, &table_t_array[cpt].size, member_size(table_t, size));
		read(fd, &table_t_array[cpt].reserved, member_size(table_t, reserved));
		read(fd, &table_t_array[cpt].datetime_str, member_size(table_t, datetime_str));
		cpt++;
	}

	/*
	 * On cherche à afficher uniquement les structures qui ne sont pas vides
	 */
	const int sizeof_datetime_str2 = member_size(table_t, datetime_str)+1;
	char datetime_str2[sizeof_datetime_str2];
	cpt = 0;
	while(cpt<real_struct_nb) {
		memset(datetime_str2, 0, sizeof_datetime_str2);
		print_table(&table_t_array[cpt], datetime_str2);
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
	lseek(fd, 0, SEEK_SET);
	while(cpt<real_struct_nb) {
		cpt2 = 0;
		char content[table_t_array[cpt].size + 1];
		read(fd, &content, table_t_array[cpt].size);

		for(int i = 0; i < member_size(table_t, name); i++) {
			if(table_t_array[cpt].name[i] != 0x20 && table_t_array[cpt].name[i] != 0x0) {
				cpt2++;
			}
		}
		cpt2--;

		char n[cpt2+1];
		for(int i = 0; i < cpt2; i++) {
			n[i] = table_t_array[cpt].name[i];
		}
		n[cpt2] = '\0';

		char * name = malloc(cpt2 + 4);
		sprintf(name, "%s%s", n, ".bin");

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

		off_t offset = lseek(fd3, 0, SEEK_END);
		if(offset != table_t_array[cpt].size) {
			printf("Erreur lors de l'écriture du fichier %s, taille supposée : %d octets, taille réelle : %ld octets", name, table_t_array[cpt].size, offset);
		}
		close(fd3);

		free(name);
		cpt++;
	}
	close(fd);


	// PARTIE 2
	char cd = 0x0;
	char * temp_cd = &cd;
	int cpt3 = 0;
	int cpt4 = 0;
	int cpt5 = 0;
	int addr;

	const int fd_dico = open("DICO.bin", O_RDONLY);
	if(fd_dico == -1) {
		perror("Ce fichier n'existe pas.");
		exit(0);
	}

	while(cd != 0x20) {
		read(fd_dico, temp_cd, 1);
		cpt3++;
		lseek(fd_dico, sizeof(dico_entry_t)-1, SEEK_CUR);
	}
	cpt3--;

	lseek(fd_dico, 0, SEEK_SET);
	dico_entry_t dico_array[cpt3];

	while(cpt4 < cpt3) {
		memset(&dico_array[cpt4], 0, sizeof(dico_array[cpt4]));
		read(fd_dico, &dico_array[cpt4].name,	member_size(dico_entry_t, name));
		read(fd_dico, &dico_array[cpt4].addr1, member_size(dico_entry_t, addr1));
		read(fd_dico, &dico_array[cpt4].addr2, member_size(dico_entry_t, addr2));
		read(fd_dico, &dico_array[cpt4].cnt, member_size(dico_entry_t, cnt));
		cpt4++;
	}
	close(fd_dico);

	cpt4 = 0;
	while(cpt4 < cpt3) {
		print_dico_entry(&dico_array[cpt4]);
		cpt4++;
	}

	if(system("test -d son") != 0) {
		system("mkdir son/");
	}

	int fd_son = open("SON.bin", O_RDONLY);
	if(fd_son == -1) {
		perror("Ce fichier n'existe pas.");
		exit(0);
	}

	cpt4 = 0;
	while(cpt4 < cpt3) {
		addr = dico_array[cpt4].addr2 - dico_array[cpt4].addr1;
		char content[addr + 1];
		lseek(fd_son, dico_array[cpt4].addr1, SEEK_SET);
		read(fd_son, &content, addr);

		char name[member_size(dico_entry_t, name)+1];
		memset(&name, 0, member_size(dico_entry_t, name)+1);
		memcpy(&name, &dico_array[cpt4].name, member_size(dico_entry_t, name)+1);
		name[member_size(dico_entry_t, name)] = '\0';

		for(int i = 0; i < member_size(dico_entry_t, name)+1; i++) {
			if(name[i] == '/') {
				name[i] = '_';
			}
		}

		char * file = malloc(member_size(dico_entry_t, name) + 9);
		sprintf(file, "son/%s%s", name, ".bin");

		const ssize_t fd_write = open(file, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
		if(fd_write == -1) {
			printf("Ce fichier n'existe pas.\n");
		}
		write(fd_write, &content, addr);
		close(fd_write);

		const ssize_t fd_verify = open(file, O_RDONLY);
		if(fd_verify == -1) {
			perror("Ce fichier n'existe pas.");
			exit(0);
		}

		off_t offset = lseek(fd_verify, 0, SEEK_END);
		if(offset != addr) {
			printf("Erreur lors de l'écriture du fichier %s, taille supposée : %d octets, taille réelle : %ld octets", file, addr, offset);
		}
		close(fd_verify);

		free(file);
		cpt4++;
	}
}