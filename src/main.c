#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "graph_gen.h"

enum graph_types {

	GT_LINE = 0, //draw a line graph
	GT_COUNT //end of list
};

int gtype = 0;

struct graph_functions {

	int (*push_val) (int column, double val);
	int (*push_row) (int columns);

	int (*g_getopt) (int argc, char** argv);
	int (*draw_graph) (void);
};

struct graph_functions graphs[] = {
	{line_push_value,line_push_row,line_getopt,line_draw},
};

int new_string(const char* data) {

	char newdata[255];
	strcpy(newdata,data);

	int i=0;
	char* token = strtok(newdata,", ");

	while (token) {

		double val = strtod(token,NULL);
		graphs[gtype].push_val(i,val);

		i++;
		token = strtok(NULL,", ");
	}
	graphs[gtype].push_row(i);
	graphs[gtype].draw_graph();
}

int main(int argc, char** argv) {

	//getopt

	int opt = -1;
	int gtype = 0;
       	opterr = 0;	

	while ((opt = getopt(argc, argv, "t:")) != -1 ) {
		switch (opt) {
			case 't':
				gtype = atoi(optarg);
			break;
		}
	}

	if ((gtype < 0) || (gtype >= GT_COUNT)) {
		fprintf(stderr, "Invalid graph type.\n");
		return 1;
	}
	optind = 0;

	graphs[gtype].g_getopt(argc,argv);

	//init ncurses

	init_ui();
	atexit(exit_ui);

	char data[255];

	char* newstr = data;

	while ((OK == getnstr(data,255))) {

		new_string(data);
	}
	return 0;
}
