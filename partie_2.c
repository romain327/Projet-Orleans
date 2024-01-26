#include "partie_2.h"

// routine d'affichage des structures dico
void print_dico_entry(dico_entry_t * dico_entry_struct) {
	printf("Name: %s\n", dico_entry_struct->name);
	printf("Addr1: %d\n", dico_entry_struct->addr1);
	printf("Addr2: %d\n", dico_entry_struct->addr2);
	printf("Cnt: %d\n", dico_entry_struct->cnt);
	printf("\n");
}

void partie_2(char * filename) {
    char cd = 0x0;
	char * temp_cd = &cd;
	int cpt3 = 0;
	int cpt4 = 0;
	int addr;

	const int fd_dico = open(filename, O_RDONLY);
	if(fd_dico == -1) {
		perror("Ce fichier n'existe pas.");
		exit(0);
	}

	// récupération du nombre de structures dans la variabke cpt3
	while(cd != 0x20) {
		read(fd_dico, temp_cd, 1);
		cpt3++;
		lseek(fd_dico, sizeof(dico_entry_t)-1, SEEK_CUR);
	}
	cpt3--;

	lseek(fd_dico, 0, SEEK_SET);
	dico_entry_t dico_array[cpt3];

	// lecture du fichier et remplissage des structures
	while(cpt4 < cpt3) {
		memset(&dico_array[cpt4], 0, sizeof(dico_array[cpt4]));
		read(fd_dico, &dico_array[cpt4].name,	member_size(dico_entry_t, name)));
		read(fd_dico, &dico_array[cpt4].addr1, member_size(dico_entry_t, addr1)));
		read(fd_dico, &dico_array[cpt4].addr2, member_size(dico_entry_t, addr2)));
		read(fd_dico, &dico_array[cpt4].cnt, member_size(dico_entry_t, cnt)));
		cpt4++;
	}
	close(fd_dico);

	cpt4 = 0;
	while(cpt4 < cpt3) {
		print_dico_entry(&dico_array[cpt4]);
		cpt4++;
	}

	// création du répertoire son s'il n'existe pas
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
		// calcul de la taille du fichier à écrire et lecture du contenu
		addr = dico_array[cpt4].addr2 - dico_array[cpt4].addr1;
		char content[addr + 1];
		lseek(fd_son, dico_array[cpt4].addr1, SEEK_SET);
		read(fd_son, &content, addr);

		// préparation du nom
		char name[member_size(dico_entry_t, name))+1];
		memset(&name, 0, member_size(dico_entry_t, name)+1));
		memcpy(&name, &dico_array[cpt4].name, member_size(dico_entry_t, name)+1));
		name[member_size(dico_entry_t, name))] = '\0';

		// remplacement des '/' qui posent problème lors de la création du fichier par des '_'
		for(int i = 0; i < member_size(dico_entry_t, name))+1; i++) {
			if(name[i] == '/') {
				name[i] = '_';
			}
		}

		char * file = malloc(member_size(dico_entry_t, name) + 9));
		sprintf(file, "son/%s%s", name, ".bin");

		// création et écriture du fichier
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

		// vérification des octets écrits
		off_t offset = lseek(fd_verify, 0, SEEK_END);
		if(offset != addr) {
			printf("Erreur lors de l'écriture du fichier %s, taille supposée : %d octets, taille réelle : %ld octets", file, addr, offset);
		}
		close(fd_verify);

		free(file);
		cpt4++;
	}
	close(fd_son);

}