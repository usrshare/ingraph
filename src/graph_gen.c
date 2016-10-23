#include "graph_gen.h"
#include <ncurses.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXCOL 256 //maximum columns
#define MAXGRF 6 //maximum graphs

#define FIRSTCOLPAIR 3

WINDOW* screen;

bool acs_lines = false;

double minV = 0.0;
double maxV = 1.0;
double lineV = 1.0;
int lineH = 0;

double values[MAXCOL*MAXGRF];

unsigned int ccount = 0; //columns count
unsigned int lastval = 0;
unsigned int nextval = 0;
unsigned int datacols = 0; //number of graphs

int get_acsline(double rowval) {

	double rem = rowval - floor(rowval);
	if (rem < 0.2) return ACS_S1;
	if (rem < 0.4) return ACS_S3;
	if (rem < 0.6) return ACS_HLINE;
	if (rem < 0.8) return ACS_S7;
	return ACS_S9;
}

double getrowval(double val) {
	return ((val - minV) / (maxV - minV)) * (LINES-1);
}

int line_draw(void) {

	unsigned int col_c = (ccount < COLS ? ccount : COLS); //columns to display

	unsigned int curc = COLS-1; //current column
	unsigned int colsleft = col_c; //columns left

	erase();

	if (lineH) {
		int firstcol = (COLS % lineH)-1 - ((ccount-1) % lineH);
		for (int iy = 0; iy < LINES; iy++ ) {
			for (int ix = firstcol; ix < COLS; ix+= lineH) {
				attron(COLOR_PAIR(2));
				mvaddch(iy,ix,acs_lines ? ACS_VLINE : '|');
				attroff(COLOR_PAIR(2));

			}
		}
	}

	if (lineV) {

		double curline = ceil(minV / lineV) * lineV;

		while (curline <= maxV) {

			double rowval = getrowval(curline);
			int row = (LINES-1 - rowval);

			if ( (row != 0) || (row != (LINES-1)) ) 
				mvprintw(row,0,"%.3f",curline);

			int rx = getcurx(screen);

			int cchar = '-';
			if (acs_lines) cchar = get_acsline(rowval);

			for (int ix=rx; ix < COLS; ix++) {
				attron(COLOR_PAIR(2));
				if (lineH && (!acs_lines)) {
				mvaddch(row,ix,( ((ix + (COLS % lineH) + (ccount)) % lineH) == 0) ? '+' : cchar);	
				} else {
				mvaddch(row,ix,cchar);	
				}
				attroff(COLOR_PAIR(2));
			}
			curline += lineV;
		}
	}

	mvprintw(0,0,"%.3f",maxV);

	mvprintw(LINES-1,0,"%.3f",minV);

	while (colsleft > 0) {

		int thiscol = lastval - col_c + colsleft;
		if (thiscol < 0) thiscol += MAXCOL; 

		for (int i=0; i < datacols; i++) {

			double thisval = values[thiscol * MAXGRF + i];

			double rowval = getrowval(thisval);

			int row = (LINES-1 - rowval);

			int cchar = '*';

			if (acs_lines) cchar = get_acsline(rowval);

			if (row > (LINES-1)) { row = LINES-1; cchar = 'v'; }

			if (row < 0) { row = 0; cchar = '^'; }

			attron(COLOR_PAIR(FIRSTCOLPAIR+i));
			mvaddch(row,curc,cchar);
			attroff(COLOR_PAIR(FIRSTCOLPAIR+i));
		}	

		colsleft--;
		curc--;
	}
	refresh();
}

int line_push_value(int column, double value) {

	if (column >= MAXGRF) return 1;
	values[nextval * MAXGRF + column] = value;
	return 0;
}

int line_push_row(int columns) {

	ccount++;
	datacols = columns;
	lastval = nextval;
	nextval++;
	if (nextval >= MAXCOL) nextval -= MAXCOL;
};

int init_ui() {
	
	screen = initscr();
	noecho();
	leaveok(screen, true);
	start_color();

	init_pair(1,COLOR_WHITE,COLOR_BLACK);
	init_pair(2,COLOR_BLUE,COLOR_BLACK);
	init_pair(3,COLOR_GREEN,COLOR_BLACK);
	init_pair(4,COLOR_CYAN,COLOR_BLACK);
	init_pair(5,COLOR_YELLOW,COLOR_BLACK);
	init_pair(6,COLOR_RED,COLOR_BLACK);
	init_pair(7,COLOR_MAGENTA,COLOR_BLACK);
}

void exit_ui(void) {

	endwin();
}

int line_getopt (int argc, char** argv) {
	
	int opt = -1;

	while ((opt = getopt(argc, argv, "m:M:l:h:a")) != -1) {
		switch (opt) {
			case 'm':
				minV = strtod(optarg,NULL);
				break;
			case 'M':
				maxV = strtod(optarg,NULL);
				break;
			case 'h':
				lineH = atoi(optarg);
				break;
			case 'l':
				lineV = strtod(optarg,NULL);
				break;
			case 'a':
				acs_lines = true;
				break;
			case '?':
			default: /* '?' */
				fprintf(stderr, "Usage: %s [-m value] [-M value] [-a]\n"
						"\n"
						" -m : minimum value.\n"
						" -M : maximum value.\n"
						" -a : use ACS scanline characters.\n"
						" -h : interval for lines across the X axis. \n"
						" -l : interval for lines across the Y axis. \n"
						"\n"
						,argv[0]);
				return 1;
		}
	}
	
	if (maxV <= minV) {
		fprintf(stderr,"Invalid minimum and maximum values.\n"); return 1;} 
	
	return 0;
}
