/*
 * clear
 *
 * Clears the terminal.
 * This is a very dumb version and probably only works
 * within the toaruOS terminal, but it might also work
 * with an xterm or similar.
 */
#include <stdio.h>

int main(int argc, char ** argv) {
	printf("\033[H\033[2J");
	fflush(stdout);
	return 0;
}
