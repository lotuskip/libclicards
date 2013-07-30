/* A simple test program to test and illustrate the use of libclicards. */
#include <clicards.h>
#include <stdlib.h> /* For srandom() */
#include <time.h>
#include <locale.h> /* Important for UTF-8 to print properly. */

int main(void)
{
	WINDOW *mainw; /* An NCurses window to draw on. */
	Pile *deck; /* A deck of cards. */

	setlocale(LC_ALL, ""); /* Important for UTF-8 to print properly. */
	srandom(time(NULL)); /* Need to seed RNG for shuffling. */

	deck = standard_deck(1); /* Create a 52 card deck (1 copy). */
	shuffle_pile(&deck, &random); /* Shuffle it using standard random() */

	/* Init NCurses and setup a colour for printing our stuff. */
    if(!initscr() || start_color() == ERR)
		return 1;
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	mainw = newwin(24,80,0,0);
	clicards_init_ncurses(); /* This call to make colours work right. */
	refresh();
	flushinp();

	wattrset(mainw, COLOR_PAIR(1));
	wprintw(mainw, "%i cards:\n", num_cards(deck));
	wprint_pile(deck, mainw); /* Print the shuffled deck. */
	wrefresh(mainw);
	getch(); /* Allow user to see it before closing. */
	delwin(mainw);
	endwin(); /* Close NCurses session. */

	free_pile(deck); /* Free up memory used by deck. */
	return 0;
}
