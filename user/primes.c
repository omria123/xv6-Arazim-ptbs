#include "user/user.h"

#define MAX_N 35 

#define LOGGING 0

void process_primes(int);


void start_child_process(int fd) {
	int pid = fork();
	if (0 != pid) {
		return;
	}
	process_primes(fd);
}


void process_primes(int fd) {
	int num = 0, p = 0;
	int numbers_pipe[2] =  {} ;
	int status = 0;
	char first_num = 1;


	while (1) {
		if (4 !=  (status = read(fd, &num, 4))) {
			if (status == 0) {
				break;
			}
			close(numbers_pipe[1]);
			close(fd);
			if (-1 == wait(NULL)) {printf("whattttt???");}


			exit(1);
		}
	
		if (first_num) {
			pipe(numbers_pipe);
			start_child_process(numbers_pipe[0]);
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
	close(fd);
	close(numbers_pipe[1]);
	if (-1 == wait(NULL)) {printf("1111whattttt???");}
	exit(0);

}


int
main(int argc, char *argv[]) {
	int i = 0;
	int numbers_pipe[2];
	
	pipe(numbers_pipe);
	
	start_child_process(numbers_pipe[0]);
	
	for (i = 2; i <= MAX_N; i++) {
#if LOGGING
		printf("2 - Sending to child %d\n", i); 
#endif
		write(numbers_pipe[1], &i, 4);
	}
	close(numbers_pipe[1]);
	if (-1 == wait(NULL)) {printf("2222whattttt???");}
	printf("Everyone finished!!\n");
	exit(0);



}
