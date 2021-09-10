#include "user/user.h"

#define MAX_N 35 

#define LOGGING 0

void process_primes(int);


void start_child_process(int p[]) {
	int pid = fork();

	if (pid == -1) {
		printf("Can't fork process\n");
	}

	if (0 != pid) {
		close(p[0]);
		return;
	}
	close(p[1]);
	process_primes(p[0]);
}

int safe_read(int fd, void* buf, int n) {
	int bytes_read = 0;
	int status = 0; 
	do {
		status = read(fd, buf, n - bytes_read);
		if (status == -1) {
			return -1;
		}
		if (status == 0) {
			break;
		}
		bytes_read += status;
	} while(bytes_read < n);

	return bytes_read;
}


void process_primes(int fd) {
	int num = 0;
	int p = 0;
	int numbers_pipe[2] =  {} ;
	int status = 0;
	char first_num = 1;


	while (1) {
		status = safe_read(fd, &num, 4);
		if  (status < 4) {	
			if (status == -1) {
				printf("BAD READ\n");
				break;
			}
			if (status == 0) {
				break;
			}
			printf("Non complete read\n");
			break;
		}
	
		if (first_num) {
			if (pipe(numbers_pipe) == -1) {
				printf("BAD PIPE\n");
				break;
			}
			start_child_process(numbers_pipe);
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
	if (!first_num) {
		if (-1 == wait(NULL)) {
			printf("1111whattttt???");
		} 
	}
	exit(0);
}


int
main(int argc, char *argv[]) {
	int i = 0;
	int numbers_pipe[2];
	
	pipe(numbers_pipe);
	
	start_child_process(numbers_pipe);
	
	for (i = 2; i <= MAX_N; i++) {
#if LOGGING
		printf("2 - Sending to child %d\n", i); 
#endif
		write(numbers_pipe[1], &i, 4);
	}
	close(numbers_pipe[1]);
	if (-1 == wait(NULL)) {printf("2222whattttt???");}
	exit(0);

}
