#include "user.h"



int main(int argc, char *argv[]) {

	if (2 != argc) {
		printf("usage: %s [n]\n", argv[0]);
		exit(1);
	}
	exit(sleep(atoi(argv[1])));
	


}
