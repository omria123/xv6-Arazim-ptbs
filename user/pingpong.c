#include "user.h"


#define NET_BYTE 101


int __main(int, char *a[]);

int
main(int argc, char *argv[]) {
	exit(__main(argc, argv));

}
int __main(int argc, char *argv[]) {
	char byte = NET_BYTE;
	int child_pid = 0;
	int down[2] = {}, up[2] = {};
	
	if (-1 == pipe(down))  {
		return -1;
	}
	if (-1 == pipe(up)) {
		return -1;
	}
	
	if (-1 == (child_pid = fork())) {
		return -1;
	}

	if (0 == child_pid) { // child execution
		
		if (1 != read(down[0], &byte, 1)) {
			printf("hellloooo\n");
			return -1;
		}
		
		printf("%d: received ping\n", getpid());
		
		if (1 != write(up[1], &byte, 1)) {
			return -1;
		}
		return 0;	
	}

	//parent execution
	if (1 != write(down[1], &byte, 1)) {
		return -1;
	}
	if (1 != read(up[0], &byte, 1)) {
		return -1;
	}
	
	printf("%d: received pong\n", getpid());
	

	
	return 0;
}
