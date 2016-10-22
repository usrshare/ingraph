#include <ncurses.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define MAXCOL 256 //maximum columns
#define MAXGRF 6 //maximum graphs

#define FIRSTCOLPAIR 3

WINDOW* screen;

bool acs_lines = false;

double minV = 0.0;
double maxV = 1.0;
double lineV = 1.0;

double values[MAXCOL*MAXGRF];

unsigned int ccount = 0; //columns count
unsigned int lastval = 0;
unsigned int graphs = 0; //number of graphs

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

int draw_columns() {

	unsigned int col_c = (ccount < COLS ? ccount : COLS);

	unsigned int curc = COLS-1;
	unsigned int colsleft = col_c;

	clear();


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
			mvaddch(row,ix,cchar);	
			attroff(COLOR_PAIR(2));
		}
		curline += lineV;
	}
	}

	mvprintw(0,0,"%.3f",maxV);

	mvprintw(LINES-1,0,"%.3f",minV);

	while (colsleft > 0) {

		int thiscol = lastval - col_c + colsleft;

		for (int i=0; i < graphs; i++) {

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

int new_string(const char* data) {

	char newdata[255];
	strcpy(newdata,data);

	int i=0;
	char* token = strtok(newdata,", ");

	while (token && (i < MAXGRF)) {

		double val = strtod(token,NULL);
		values[lastval * MAXGRF + i] = val;

		i++;
		token = strtok(NULL,", ");
	}

	for (int col = i; col < MAXGRF; col++) values[lastval * MAXGRF + i] = NAN;

	ccount++;
	graphs = i;
	draw_columns();
	lastval++; if (lastval >= MAXCOL) lastval = 0;
}



int main(int argc, char** argv) {

	//getopt

	int opt = -1;

	while ((opt = getopt(argc, argv, "m:M:l:a")) != -1) {
		switch (opt) {
			case 'm':
				minV = strtod(optarg,NULL);
				break;
			case 'M':
				maxV = strtod(optarg,NULL);
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
						" -l : interval for horizontal lines. \n"
						"\n"
						,argv[0]);
				exit(0);
		}
	}

	if (maxV <= minV) {
		fprintf(stderr,"Invalid minimum and maximum values.\n"); return 1;} 
	//init ncurses

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

	char data[255];

	char* newstr = data;


	bkgd(' ');

	while ((OK == getnstr(data,255))) {

		new_string(data);
	}
	endwin();
	return 0;
}
