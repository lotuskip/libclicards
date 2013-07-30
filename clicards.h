#ifndef CLICARDS_H
#define CLICARDS_H

#include <ncurses.h>
#include <stdbool.h>

/* Cards, suites, ranks, colors, and the manipulation of these.
 * **************************************************************/

typedef char card;

/* The suites: */
enum { DIAMONDS, SPADES, HEARTS, CLUBS };
int card_suite(const card c); /* returns the suite of c */

/* The ranks: */
enum { ACE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN,
	KING };
int card_rank(const card c); /* returns the rank of c */

/* The colors: */
enum { RED, BLACK };
int card_color(const card c); /* returns the color of c */

/* Get a card by suite and rank: */
card card_from_suiterank(const int suite, const int rank);


/* Piles of cards (hands, decks, etc.) and the basic manipulation of these.
 * ************************************************************************/

typedef struct Pile Pile;
struct Pile {
	card top; /* top card of the pile */
	Pile *next; /* this pile sans the top card */
};

/* Counts the cards in p. */
int num_cards(Pile *p);

/* Pull out a specific card from a pile. Returns NULL and does nothing
 * if the specified card is not present in the pile. */ 
Pile *card_in_pile(const card c, Pile **p);
/* Removes the nth card from p and returns a pointer to it. */
Pile *nth_card(Pile **p, int n);

/* Returns the last card of the given pile (without modifying the pile). */
card last_card(const Pile *p);

/* Add a card to the top of the pile (mallocs a new Pile). Returns new p. */
void add_card_to_pile(const card c, Pile **p);
/* Concatenates p2 to p. Returns the whole merged pile. */
void add_pile_to_pile(Pile **p, Pile *p2);
/* Creates a pile from a single card (mallocs). */
Pile *pile_from_card(const card c);

/* Clears a pile, frees up allocated memory. */
void free_pile(Pile *p);

/* Draw n top cards from the pile. Return these as a new pile (no mallocing
 * takes place). */
Pile *draw_n_cards(Pile **p, int n);


/* Special manipulations of piles.
 * ********************************/

/* Creates n copies of a standard 52 card deck. */
Pile *standard_deck(int n);

/* Shuffle a pile and return a pointer to the shuffled result. Second argument
 * is a random number generating function; use e.g. random() from stdlib.h:
 *    #include <stdlib.h>
 *    Pile *p;
 *    ...
 *    shuffle(&p, &random);
 * (remember to seed with srandom()! */
void shuffle_pile(Pile **p, long (*rndm)(void));

/* Sort a pile by rank & suite. */
void sort_pile(Pile **p, const _Bool ascending, const _Bool ace_high);


/* Printing functions.
 * Make a call to clicards_init_ncurses() after setting NCurses up along
 * with start_color(). Also do set_locale(LC_ALL, "") to have unicode print
 * correctly. See examples for more insight. */

/* Initialize the two special color pairs that are used. These are numbers 16
 * and 17; don't redefine them! */
void clicards_init_ncurses(void);

/* Print a card (two UTF-8 symbols), in the usual ncurses fashion. Note that
 * these do not call (w)refresh. */
void wprint_card(const card c, WINDOW *w);
void mvprint_card(const card c, WINDOW *w, const int x, const int y);

/* Print a pile. Prints all cards with spaces in between. The spaces are printed
 * in COLOR_PAIR(1). */
void wprint_pile(Pile *p, WINDOW *w);
void mvprint_pile(Pile *p, WINDOW *w, const int x, const int y);


/* Input-related functions.
 * **************************/

#define NO_CARD -1

/* Reads an input string such as "4d" or "d4" and interprets that as defining a
 * card. DHSC give the suite, 1234567890JQK give the rank. 'A' is
 * synonym for '1', 'T' is a synonym for '0'. All letters are case-insensitive.
 * Returns NO_CARD if the input is faulty. */
card card_from_input(const char input_str[3]);

#endif
