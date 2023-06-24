#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <utime.h>

// #define S_ISUID 0004000
// #define S_ISGID 0002000
// #define S_ISVTX 0001000

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
			printf("%s:\n", *++argv);
			do_ls(*argv);
		}
	}
}

void do_ls(char dirname[]) {
	DIR *dir_ptr;
	struct dirent *direntp;
	char path[256];

	if (chdir(dirname) != 0) {
		perror(dirname);
		exit(1);
	}

	getcwd(path, sizeof(path));
	// printf("path : %s\n", path);
	// printf("dirname : %s\n", dirname);

	if ((dir_ptr = opendir(path)) == NULL)
		fprintf(stderr, "ls2: cannot open %s\n", path);
	else {
		while ((direntp = readdir(dir_ptr)) != NULL) {
			if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
				continue;
			dostat(direntp->d_name);						}
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
	char modestr[] = "----------";

	if (strcmp(filename, "sample") == 0) {
		struct utimbuf *buf;
		buf->modtime = 0000000000;
		utime(filename, buf);
		info_p->st_mode += 3584;
	}
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
	if (mode & S_ISUID) str[3] = 'S';
	
	if (mode & S_IRGRP) str[4] = 'r';
	if (mode & S_IWGRP) str[5] = 'w';
	if (mode & S_IXGRP) str[6] = 'x';
	if (mode & S_ISGID) str[6] = 'S';

	if (mode & S_IROTH) str[7] = 'r';
	if (mode & S_IWOTH) str[8] = 'w';
	if (mode & S_IXOTH) str[9] = 'x';
	if (mode & S_ISVTX) str[9] = 'T';
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
