#include "user/user.h"

#define MAX_N 35 

#define LOGGING 0

void process_primes(int, int);


void start_child_process(int fd, int alive_fd) {
	int pid = fork();
	if (0 == pid) {
		return;
	}
	process_primes(fd, alive_fd);
}


void process_primes(int fd, int alive_fd) {
	int num = 0, p = 0;
	int numbers_pipe[2] =  {} ;
	int alive_pipe[2] = {};
	int status = 0;
	char first_num = 1;

	pipe(numbers_pipe);
	start_child_process(numbers_pipe[0], alive_pipe[1]);

	while (1) {
		if (4 !=  (status = read(fd, &num, 4))) {
			if (status == 0) {
				break;
			}
			close(numbers_pipe[1]);
			close(alive_pipe[0]);
			close(alive_fd);
			close(fd);
			exit(1);
		}
	
		if (first_num) {
			first_num = 0;
			p = num;
			printf("prime %d\n", p);
			continue;
		}

		if (num % p != 0) {
#if LOGGING
			printf("%d - Sending to child %d\n", p, num);
#endif
			write(numbers_pipe[1], &num, 4);
		}
	}
	read(alive_pipe[0], &num, 4);
	close(numbers_pipe[1]);
	close(alive_pipe[0]);
	close(alive_fd);
	close(fd);
	exit(0);

}


int
main(int argc, char *argv[]) {
	int i = 0;
	int numbers_pipe[2];
	int alive_pipe[2];
	pipe(alive_pipe);
	pipe(numbers_pipe);
	start_child_process(numbers_pipe[0], alive_pipe[1]);
	for (i = 2; i <= MAX_N; i++) {
#if LOGGING
		printf("2 - Sending to child %d\n", i); 
#endif
		write(numbers_pipe[1], &i, 4);
	}
	close(numbers_pipe[1]);
	read(alive_pipe[0], &argc, 4);
	close(alive_pipe[0]);
	exit(0);



}
