// utmplib.c: functions to buffer reads from utmp file

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <utmp.h>

#define NRECS 16
#define NULLUT ((struct utmp*)NULL)
#define UTSIZE (sizeof(struct utmp))

static char utmpbuf[NRECS * UTSIZE]; // storage
static int num_recs;  // num stored
static int cur_rec;  // next to go
static int fd_utmp = -1;  // read from

// decalre the following functions to be written below
int utmp_reload();
int utmp_open();
struct utmp* utmp_next();
void utmp_close();


// OPEN the UTMP file
// return: a file descriptor of the file
int utmp_open(char *filename) {
	fd_utmp = open(filename, O_RDONLY);
	cur_rec = num_recs = 0;
	return fd_utmp;
}

// Access the next UTMP record if any
// return: the pointer of the next record
struct utmp *utmp_next() {
	struct utmp *recp;
	if (fd_utmp == -1)
		return NULLUT;
	if (cur_rec == num_recs && utmp_reload() == 0)
		return NULLUT;
	recp = (struct utmp*) &utmpbuf[cur_rec * UTSIZE];
	cur_rec++;
	return recp;
}

// Read next bunch of records into buffer
// return: how many bytes read
int utmp_reload() {
	int amt_read;
	amt_read = read(fd_utmp, utmpbuf, NRECS * UTSIZE);
	num_recs = amt_read / UTSIZE;
	cur_rec = 0;
	return num_recs;
}

void utmp_close() {
	if (fd_utmp != -1)
		close(fd_utmp);
}
