#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

struct flaginfo {
	int fl_value;
	char *fl_name;
};

void showbaud(int thespeed);
void show_some_flags(struct termios *ttyp);
void show_flagset(int thevalue, struct flaginfo thebitnames[]);
void flip_output_flags(struct termios *ttyp);
void flip_input_flags(struct termios *ttyp, char *arg);
void flip_local_flags(struct termios *ttyp, char *arg);
void flip_all_flags(struct termios *ttyp); 

int main(int argc, char *argv[]) {
	struct termios ttyinfo;
	if (tcgetattr(0, &ttyinfo) == -1) {
		perror("cannot get params about stdin");
		exit(1);
	}
	showbaud(cfgetospeed(&ttyinfo));
	printf("erase = ^%c;", ttyinfo.c_cc[VERASE] -1 + 'A');
	printf("kill = ^%c", ttyinfo.c_cc[VKILL] -1 + 'A');
	printf("\n");
	if (argc == 1)
		show_some_flags(&ttyinfo);
	else {
		char *option = argv[1];
		if (strcmp(option, "-FI") == 0) {
			while (--argc) {
				flip_input_flags(&ttyinfo, *++argv);
			}
		}
		else if (strcmp(option, "-FO") == 0) {
			flip_output_flags(&ttyinfo);
		}
		else if (strcmp(option, "-FL") == 0) {
			while (--argc) {
				flip_local_flags(&ttyinfo, *++argv);
			}
		}
		else if (strcmp(option, "-A") == 0) {
			flip_all_flags(&ttyinfo);
		}
		show_some_flags(&ttyinfo);
	}

	
}

void showbaud(int thespeed) {
	printf("speed ");
	switch(thespeed) {
		case B300:	printf("300"); break;
		case B600:	printf("600"); break;
		case B1200:	printf("1200"); break;
		case B1800:	printf("1800"); break;
		case B2400:	printf("2400"); break;
		case B4800:	printf("4800"); break;
		case B9600:	printf("9600"); break;
		default:	printf("Fast"); break;
	}
	printf(" baud;\n");
}

struct flaginfo input_flags[] = {
	IGNBRK	, "ignbrk",
	BRKINT	, "brkint",
	IGNPAR	, "ignpar",
	PARMRK	, "parmrk",
	INPCK	, "inpck",
	ISTRIP	, "istrip",
	// NL: newline, CR: carriage return
	INLCR	, "inlcr",
	IGNCR	, "igncr",
	ICRNL	, "icrnl",
	IXON	, "ixon",
	IXOFF	, "ixoff",
	0	, NULL
};

struct flaginfo output_flags[] = {
	OLCUC	, "olcuc",
	0	, NULL
};

struct flaginfo local_flags[] = {
	ISIG	, "isig",
	ICANON	, "icanon",
	ECHO	, "echo",
	ECHOE	, "echoe",
	ECHOK	, "echok",
	0	, NULL
};

void show_some_flags(struct termios *ttyp) {
	show_flagset(ttyp->c_iflag, input_flags);
	show_flagset(ttyp->c_oflag, output_flags);
	show_flagset(ttyp->c_lflag, local_flags);
}

void flip_output_flags(struct termios *ttyp) {
	if (ttyp->c_oflag & OLCUC)
		ttyp->c_oflag &= ~OLCUC;
	else
		ttyp->c_oflag |= OLCUC;
	tcsetattr(0, TCSANOW, ttyp);
}

void flip_input_flags(struct termios *ttyp, char *arg) {
	for (int i = 0; input_flags[i].fl_value; i++) {
		if (strcmp(arg, input_flags[i].fl_name) == 0) {
			int fl_value = input_flags[i].fl_value;
			if (ttyp->c_iflag & fl_value)
				ttyp->c_iflag &= ~fl_value;
			else
				ttyp->c_iflag |= fl_value;
		}
	}
	tcsetattr(0, TCSANOW, ttyp);
}

void flip_local_flags(struct termios *ttyp, char *arg) {
	for (int i = 0; local_flags[i].fl_value; i++) {
		if (strcmp(arg, local_flags[i].fl_name) == 0) {
			int fl_value = local_flags[i].fl_value;
			if (ttyp->c_lflag & fl_value)
				ttyp->c_lflag &= ~fl_value;
			else
				ttyp->c_lflag |= fl_value;
		}
	}
	tcsetattr(0, TCSANOW, ttyp);
}

void flip_all_flags(struct termios *ttyp) {
	for (int i = 0; input_flags[i].fl_value; i++) {
		int fl_value = input_flags[i].fl_value;
		if (ttyp->c_iflag & fl_value)
			ttyp->c_iflag &= ~fl_value;
		else
			ttyp->c_iflag |= fl_value;
	}

	if (ttyp->c_oflag & OLCUC)
		ttyp->c_oflag &= ~OLCUC;
	else
		ttyp->c_oflag |= OLCUC;

	for (int i = 0; local_flags[i].fl_value; i++) {
		int fl_value = local_flags[i].fl_value;
		if (ttyp->c_lflag & fl_value)
			ttyp->c_lflag &= ~fl_value;
		else
			ttyp->c_lflag |= fl_value;
	}

	tcsetattr(0, TCSANOW, ttyp);
}

void show_flagset(int thevalue, struct flaginfo thebitnames[]) {
	int i;
	for (i = 0; thebitnames[i].fl_value; i++) {
		if (!(thevalue & thebitnames[i].fl_value))
			printf("-");
		printf("%s ", thebitnames[i].fl_name);
	}
	printf("\n");
}
