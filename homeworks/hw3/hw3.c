#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

void do_mv(char *dirname, char *destination, char *extension);

int main(int argc, char *argv[]) {
	char *f_name = argv[0];
	char *d = argv[1];
	char *destination = argv[2];
	char *e = argv[3];
	char *extension = argv[4];

	if (argc == 1) {
		printf("Destination directory does not exist\n");
		exit(1);
	}
	if (strcmp(d, "-d") != 0) {
		printf("Destination directory does not exist\n");
		exit(1);
	}
	if (strcmp(e, "-e") != 0) {
		printf("Destination directory does not exist\n");
		exit(1);
	}
	if (argc > 5) {
		printf("%s: invalid option -- '%c'\n", f_name, argv[5][1]);
		printf("Usage: %s -d destination_directory -e extension\n", f_name);
		exit(1);
	}
	do_mv(f_name, destination, extension);

	return 0;
}

void do_mv(char *dirname, char *destination, char *extension) {
	DIR *dir_ptr;
	struct dirent *direntp;
	char path[256];

	/*
	if (chdir(dirname) != 0) {
		perror(dirname);
		exit(1);
	}
	*/

	getcwd(path, sizeof(path));
	if ((dir_ptr = opendir(path)) == NULL)
		fprintf(stderr, "hw3_s2020110973: cannot open %s\n", path);
	else {
		int file_found = 0;
		int dest_found = 0;
		while ((direntp = readdir(dir_ptr)) != NULL) {
			if (strcmp(direntp->d_name, destination) == 0) dest_found++;
			if (strstr(direntp->d_name, ".") != NULL) {
				char *ext = strrchr(direntp->d_name, '.');
				char ext2[100]  = ".";
				strcat(ext2, extension);
				if (strcmp(ext2, ext) == 0) {
					char link_to[100];
					strcpy(link_to, destination);
					strcat(link_to, "/");
					strcat(link_to, direntp->d_name);
					rename(direntp->d_name, link_to);
					file_found++;
				}
			}
		}
		if (dest_found == 0) {
			printf("Destination directory does not exist\n");
			exit(1);
		}
		if (file_found == 0)
			printf("No files with extension %s found\n", extension);
	}
	closedir(dir_ptr);

}
