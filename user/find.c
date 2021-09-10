#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define ARGS 2
#define MAX_PATH 512

#define CHECK(expr, fmt, ...) do { if (!(expr)) {fprintf(2, fmt, ##__VA_ARGS__); err = -1; goto cleanup;} } while (0)

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

int is_cur_prev_dir(char* name) {
	return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;

}

int match(char* filename, char* key) {
	int i = 0;
	int key_len = strlen(key);

	for (i = 0; i < DIRSIZ; i++) {
		if (0 == memcmp(filename + i, key, key_len)) {
			return 1;
		}
	}

	return 0;

}


int deepsearch(char* dir_path, char* key) {
  int fd = -1;
  int err = 0;
  int status = 0;
  int dir_path_len = 0;

  struct stat st = { 0 };
  struct dirent de = { 0 };
  char file_path[MAX_PATH] = { 0 };

  dir_path_len = strlen(dir_path);

  fd = open(dir_path, 0);
  CHECK(fd != -1, "find: cannot open %s\n", dir_path);
  CHECK(fstat(fd, &st)!= -1, "find: cannot stat %s\n", dir_path);
  CHECK(st.type == T_DIR, "find: %s is not a directory\n", dir_path);
  CHECK(dir_path_len + DIRSIZ + 2 <= sizeof(file_path), "find: path too long\n");

  strcpy(file_path, dir_path);
  strcpy(file_path + dir_path_len, "/");
  
  do {
  	status = safe_read(fd, &de, sizeof(de));
  	CHECK(status != -1, "find: cannot read from %s\n", dir_path);
  	if (status != sizeof(de)) {
  		break;
  	}

  	if (is_cur_prev_dir(de.name) || de.inum == 0) {
  		continue;
  	} 
  	memmove(file_path + dir_path_len + 1, de.name, DIRSIZ);
  	file_path[dir_path_len + DIRSIZ + 1]  = 0;
  	if (match(de.name, key)) {
  		printf("%s\n", file_path);
  	}
	CHECK(stat(file_path, &st) !=  -1, "find: cannot stat %s\n", file_path);
	if (st.type == T_DIR) {
		CHECK(deepsearch(file_path, key) == 0, "");
	}
  }
  while (1);
cleanup:
  if (fd != -1) {
  	close(fd);
  	fd = -1;
  }
  return err;

}


int usage(char* program) {
	printf("Usage: %s path search-key\n", program);
	return 1;
}


int main(int argc, char* argv[]) {
	if (argc != ARGS + 1) {
		return usage(argv[0]);
	}
	exit(deepsearch(argv[1], argv[2]));
	return 0;
}