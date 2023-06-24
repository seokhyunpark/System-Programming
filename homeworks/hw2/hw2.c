#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

void do_ls(char[]);
void dostat(char *);
void show_file_info(char *, struct stat *);
void mode_to_letters(int, char[]);
char *uid_to_name(uid_t);
char *gid_to_name(gid_t);

int main(int argc, char *argv[]) {
	if (argc == 1)
		do_ls(".");
	else {
		while (--argc) {
			do_ls(*++argv);
		}
	}
}

void do_ls(char dirname[]) {
	DIR *dir_ptr;
	struct dirent *direntp;
	char path[256];

	if (chdir(dirname) != 0) {
		if (strcmp(dirname, "-s") * strcmp(dirname, "-S") != 0) {
			perror(dirname);
			exit(1);
		}
	}

	getcwd(path, sizeof(path));
	if ((dir_ptr = opendir(path)) == NULL)
		fprintf(stderr, "ls2: cannot open %s\n", path);
	else {
		int cnt = 0;
		struct dirent **d_arr = malloc(sizeof(struct stat));
		while ((direntp = readdir(dir_ptr)) != NULL) {
			if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
				continue;
			cnt++;
			d_arr = realloc(d_arr, sizeof(struct stat) * cnt);
			d_arr[cnt] = direntp;
		}
		if (strcmp(dirname, "-S") == 0) {
			for (int i = 1; i <= cnt; i++) {
				for (int j = 1; j <= cnt - i; j++) {
					struct stat info1;
					struct stat info2;
					stat(d_arr[j]->d_name, &info1);
					stat(d_arr[j + 1]->d_name, &info2);
					int arr1 = (long)info1.st_size;
					int arr2 = (long)info2.st_size;
					if (arr2 > arr1) {
					struct dirent *temp = d_arr[j + 1];
					d_arr[j + 1] = d_arr[j];
						d_arr[j] = temp;
					}
				}
			}
		}
		else if (strcmp(dirname, "-s") == 0) {
			for (int i = 1; i <= cnt; i++) {
				for (int j = 1; j <= cnt - i; j++) {
					struct stat info1;
					struct stat info2;
					stat(d_arr[j]->d_name, &info1);
					stat(d_arr[j + 1]->d_name, &info2);
					int arr1 = (long)info1.st_size;
					int arr2 = (long)info2.st_size;
					if (arr2 < arr1) {
					struct dirent *temp = d_arr[j + 1];
					d_arr[j + 1] = d_arr[j];
						d_arr[j] = temp;
					}
				}
			}
		}
		for (int i = 1; i <= cnt; i++) {
			dostat(d_arr[i]->d_name);
		}
	}
	closedir(dir_ptr);
}


void dostat(char *filename) {
	struct stat info;
	if (stat(filename, &info) == -1)
		perror(filename);
	else
		show_file_info(filename, &info);
}

void show_file_info(char *filename, struct stat *info_p) {
	char *uid_to_name(), *ctime(), *gid_to_name(), *filemode();
	void mode_to_letters();
	char modestr[] = "------------";

	mode_to_letters(info_p->st_mode, modestr);
	printf("%s", modestr);
	printf("%4d ", (int)info_p->st_nlink);
	printf("%-8s ", uid_to_name(info_p->st_uid));
	printf("%-8s ", gid_to_name(info_p->st_gid));
	printf("%-8ld ", (long)info_p->st_size);
	printf("%.12s ", 4+ctime(&info_p->st_mtime));
	printf("%s \n", filename);
}

void mode_to_letters(int mode, char str[]) {
	if (S_ISDIR(mode)) str[0] = 'd';
	if (S_ISCHR(mode)) str[0] = 'c';
	if (S_ISBLK(mode)) str[0] = 'b';
	
	if (mode & S_IRUSR) str[1] = 'r';
	if (mode & S_IWUSR) str[2] = 'w';
	if (mode & S_IXUSR) str[3] = 'x';

	if (mode & S_IRGRP) str[4] = 'r';
	if (mode & S_IWGRP) str[5] = 'w';
	if (mode & S_IXGRP) str[6] = 'x';

	if (mode & S_IROTH) str[7] = 'r';
	if (mode & S_IWOTH) str[8] = 'w';
	if (mode & S_IXOTH) str[9] = 'x';
}

char *uid_to_name(uid_t uid) {
	struct passwd *getpwuid(), *pw_ptr;
	static char numstr[10];

	if ((pw_ptr = getpwuid(uid)) == NULL) {
		sprintf(numstr, "%d", uid);
		return numstr;
	}
	else
		return pw_ptr->pw_name;
}

char *gid_to_name(gid_t gid) {
	struct group *getgrid(), *grp_ptr;
	static char numstr[10];

	if ((grp_ptr = getgrgid(gid)) == NULL) {
		sprintf(numstr, "%d", gid);
		return numstr;
	}
	else
		return grp_ptr->gr_name;
}
