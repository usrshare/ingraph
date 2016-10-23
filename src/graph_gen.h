#ifndef GRAPH_GEN_H
#define GRAPH_GEN_H

// generic functions
int init_ui();
void exit_ui(void);

#define OK 0
extern int getnstr(char* string, int n);

// line chart functions

int line_draw(void);

int line_push_value(int column, double value);
int line_push_row(int columns);

int line_getopt(int argc, char** argv);

#endif
