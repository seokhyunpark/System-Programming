#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

char symbol;
int dir;
int row_dir, col_dir;
int start_col, start_row;
int top_row, bot_row;
int left_edge, right_edge;
int delay_x, delay_y;

int set_ticker_x(int n_msecs) {
    struct itimerval timeset_x;
    long n_sec, n_usecs;

    n_sec = n_msecs / 1000;
    n_usecs = (n_msecs % 1000) * 1000L;

    timeset_x.it_interval.tv_sec = n_sec;
    timeset_x.it_interval.tv_usec = n_usecs;
    timeset_x.it_value.tv_sec = n_sec;
    timeset_x.it_value.tv_usec = n_usecs;

    return setitimer(ITIMER_REAL, &timeset_x, NULL);
}

int set_ticker_y(int n_msecs) {
    struct itimerval timeset_y;
    long n_sec, n_usecs;

    n_sec = n_msecs / 1000;
    n_usecs = (n_msecs % 1000) * 1000L;

    timeset_y.it_interval.tv_sec = n_sec;
    timeset_y.it_interval.tv_usec = n_usecs;
    timeset_y.it_value.tv_sec = n_sec;
    timeset_y.it_value.tv_usec = n_usecs;

    return setitimer(ITIMER_REAL, &timeset_y, NULL);
}


void move_msg(int signum) {
    signal(SIGALRM, move_msg);
    move(start_row, start_col);
    addch(' ');
    start_row += row_dir;
    start_col += col_dir;

    if (row_dir == -1 && start_row <= top_row) {
        start_row = top_row + 2;
        row_dir = 1;
        set_ticker_x(delay_x *= 2);
        set_ticker_y(delay_x *= 2);
    }
    else if (row_dir == 1 && start_row >= bot_row) {
        start_row = bot_row - 2;
        row_dir = -1;
        set_ticker_x(delay_x *= 2);
        set_ticker_y(delay_x *= 2);
    }
    if (col_dir == -1 && start_col <= left_edge) {
        start_col = left_edge + 2;
        col_dir = 1;
        set_ticker_x(delay_x /= 2);
        set_ticker_y(delay_x /= 2);
    }
    else if (col_dir == 1 && start_col >= right_edge) {
        start_col = right_edge - 2;
        col_dir = -1;
        set_ticker_x(delay_x /= 2);
        set_ticker_y(delay_x /= 2);
    }
    move(start_row, start_col);
    addch(symbol);
    refresh();
}



int main(int argc, char *argv[]) {
    symbol = argv[1][0];
    start_col = atoi(argv[2]); start_row = atoi(argv[3]);
    top_row = atoi(argv[4]); bot_row = atoi(argv[5]);
    left_edge = atoi(argv[6]); right_edge = atoi(argv[7]);
    int delay, ndelay, c;
    void move_msg(int);

    initscr();
    crmode();
    noecho();
    clear();

    for (int i = 0; i < bot_row + top_row; i++) {
		for (int j = 0; j < left_edge + right_edge; j++) {
            if (i < top_row || i > bot_row || j < left_edge || j > right_edge)
                addch(' ');
            else if (i == top_row || i == bot_row || j == left_edge || j == right_edge)
                addch('*');
            else
                addch(' ');
		}
        addstr("\n");
	}

    row_dir = 1, col_dir = 1;
    delay_x = 500, delay_y = 500;
    move(++start_row, ++start_col);
    addch(symbol);
    signal(SIGALRM, move_msg);
    signal(SIGQUIT, SIG_IGN);
    set_ticker_x(delay_x); set_ticker_y(delay_y);
    while (1) {
        c = getch();
        if (c == 'Q') break;
        if (c == 'q' && delay_x < 200) set_ticker_x(delay_x *= 2);
        if (c == 'w' && delay_y < 200) set_ticker_y(delay_y *= 2);
        if (c == 'e' && delay_x > 2) set_ticker_x(delay_x /= 2);
        if (c == 'r' && delay_y > 2) set_ticker_y(delay_y /= 2);
    }
    endwin();

    return 0;
}
