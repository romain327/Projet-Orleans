#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

typedef struct table_s {
	char name[32];
	uint32_t size;
	uint32_t addr2;
	char datetime_str[28];
} __attribute__((packed)) table_t;

typedef struct autom {
	unsigned char state;
	signed char move;
} AUTOMATE;

char * change_state(char curr_state, char value) {
	char[2] next_state;

	if(curr_state == 1 && 65 <= value && value <= 90) {
		next_state[0] = 1;
		next_state[1] = 1;
	}

	if(curr_state == 1 && (value == 46 || value == 32)) {
		next_state[0] = 2;
		next_state[1] = 1;
	}

	if(curr_state == 2 && (value == 46 || value == 32)) {
		next_state[0] = 2;
		next_state[1] = 1;
	}

	if(curr_state == 2 && 48 <= value && value <= 57) {
		next_state[0] = 3;
		next_state[1] = 1;
	}

	if(curr_state == 3 && 48 <= value && value <= 57) {
		next_state[0] = 3;
		next_state[1] = 1;
	}

	if(curr_state == 3 && value == 47) {
		next_state[0] = 3;
		next_state[1] = 1;
	}

	if(curr_state == 3 && value == 32) {
		next_state[0] = 4;
		next_state[1] = 1;
	}

	if(curr_state == 4 && 48 <= value && value <= 57) {
		next_state[0] = 4;
		next_state[1] = 1;
	}

}

int main() {
	char c1 = 57;
	char *ptr_c1 = &c1;
	ssize_t rd;

	AUTOMATE at;
	at.state = 1;
	at.move = 1;

	int fd = open("flash10.bin", O_RDONLY);
	if(fd == -1) {
		perror("Ce fichier n'existe pas.");
		exit(0);
	}

	while((c1 != EOF) || c2 != EOF) {
		rd = read(fd, ptr_c1, 1);
	}


	/*
	
	unsigned char buffer[4000];
	FILE *ptr;
	ptr = fopen("flash10.bin","rb");
	fread(buffer,sizeof(buffer),1,ptr);
	for(int i = 0; i<4000; i++)
    	printf("%c ", buffer[i]);
    printf("\n");
    */
}