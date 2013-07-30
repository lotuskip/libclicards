#define LIBCLICARDS_CURSES
#include "clicards.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/* Internal functions:
 * **********************/
void fatal_error(void)
{
	perror("libclicards");
	exit(1);
}

void sort_split(Pile **p, Pile **p1, Pile **p2)
{
	while(*p)
	{
		add_pile_to_pile(p1, draw_n_cards(p, 1));
		if(*p)
			add_pile_to_pile(p2, draw_n_cards(p, 1));
	}
}

void sort_finish_merge(Pile **p, Pile *p1, Pile *p2)
{
	if(p1)
		add_pile_to_pile(p, p1);
	if(p2)
		add_pile_to_pile(p, p2);
}

/* Return 1 if a comes before b in ascending order. */
int sort_cmp(const card a, const card b, const _Bool ace_high)
{
	if(card_rank(a) == card_rank(b))
		return card_suite(a) < card_suite(b);
	if(card_rank(a) < card_rank(b))
	{
		if(ace_high && card_rank(a) == ACE)
			return 0;
		return 1;
	}
	/* rank(b) < rank(a) */
	return (ace_high && card_rank(b) == ACE);
}

/*************************************************/

int card_suite(const card c) { return c%4; }
int card_rank(const card c) { return (c/4)%13; }
int card_color(const card c) { return c%2; }

card card_from_suiterank(const int suite, const int rank)
	{ return rank*4 + suite; }


int num_cards(Pile *p)
{
	int n = 0;
	Pile *pp = p;
	for(; pp; pp = pp->next)
		++n;
	return n;
}

Pile *card_in_pile(const card c, Pile **p)
{
	Pile *pp, *pp2;
	if(!(pp = *p))
		return NULL;
	if(pp->top == c)
	{
		*p = pp->next;
		pp->next = NULL;
		return pp;
	}
	while(pp->next && pp->next->top != c)
		pp = pp->next;
	if(!pp->next)
		return NULL;
	pp2 = pp->next;
	pp->next = pp2->next;
	pp2->next = NULL;
	return pp2;
}

Pile *nth_card(Pile **p, int n)
{
	Pile *pp, *pp2;
	if(!(pp = *p))
		fatal_error();
	if(!n)
	{
		*p = pp->next;
		pp->next = NULL;
		return pp;
	}
	for(; n > 1; --n)
		if(!(pp = pp->next))
			fatal_error();
	if(!(pp2 = pp->next))
		fatal_error();
	pp->next = pp2->next;
	pp2->next = NULL;
	return pp2;
}

card last_card(const Pile *p)
{
	const Pile *pp = p;
	if(!pp)
		fatal_error();
	while(pp->next)
		pp = pp->next;
	return pp->top;
}

void add_card_to_pile(const card c, Pile **p)
{
	Pile *n;
	if(!(n = malloc(sizeof(Pile))))
		fatal_error();
	n->top = c;
	n->next = *p;
	*p = n;
}

void add_pile_to_pile(Pile **p, Pile *p2)
{
	Pile *pp;

	if(!p2)
		return;
	if(!(pp = *p))
		*p = p2;
	else
	{
		for(; pp->next; pp = pp->next);
		pp->next = p2;
	}
}


Pile *pile_from_card(const card c)
{
	Pile *p;
	if(!(p = malloc(sizeof(Pile))))
		fatal_error();
	p->top = c;
	p->next = NULL;
	return p;
}

void free_pile(Pile *p)
{
	Pile *tmp;
	while(p)
	{
		tmp = p->next;
		free(p);
		p = tmp;
	}
}

Pile *draw_n_cards(Pile **p, int n)
{
	Pile *q, *pp;

	if(n <= 0 || !(*p))
		fatal_error();	
	for(pp = q = *p; n > 1; --n)
		if(!(pp = pp->next))
			fatal_error();
	*p = pp->next;
	pp->next = NULL;
	return q;
}


Pile *standard_deck(int n)
{
	card c;
	Pile *p = NULL;

	if(n < 0)
		fatal_error();
	for(; n > 0; --n)
	{
		for(c = 0; c < 13*4; ++c)
			add_card_to_pile(c, &p);
	}
	return p;
}

void shuffle_pile(Pile **p, long (*rndm)(void))
{
	Pile *p1 = NULL, *p2 = NULL;

	if(num_cards(*p) <= 1)
		return;
	sort_split(p, &p1, &p2);
    shuffle_pile(&p1, rndm);
    shuffle_pile(&p2, rndm);
	while(p1 && p2)
	{
		if(rndm()%2)
			add_pile_to_pile(p, draw_n_cards(&p1, 1));
		else
			add_pile_to_pile(p, draw_n_cards(&p2, 1));
	}
	sort_finish_merge(p, p1, p2);
}

void sort_pile(Pile **p, const _Bool ascending, const _Bool ace_high)
{
	Pile *p1 = NULL, *p2 = NULL;

	if(num_cards(*p) <= 1)
		return;
	sort_split(p, &p1, &p2);
    sort_pile(&p1, ascending, ace_high);
    sort_pile(&p2, ascending, ace_high);
	while(p1 && p2)
	{
		if(sort_cmp(p1->top, p2->top, ace_high) ^ ascending)
			add_pile_to_pile(p, draw_n_cards(&p2, 1));
		else
			add_pile_to_pile(p, draw_n_cards(&p1, 1));
	}
	sort_finish_merge(p, p1, p2);
}


const char suite_symbol[4][4] = {
	  "♦", /* U+2666 Black Diamond Suit */
	  "♠", /* U+2660 Black Spade Suit */
	  "♥", /* U+2665 Black Heart Suit */
	  "♣" /* U+2663 Black Club Suit */
};
const char rankstr[] = "A23456789tJQK";

void clicards_init_ncurses(void)
{
	init_pair(16, COLOR_RED, COLOR_WHITE);
	init_pair(17, COLOR_BLACK, COLOR_WHITE);
}

void wprint_card(const card c, WINDOW *w)
{
	wattrset(w, COLOR_PAIR(16+card_color(c)));
	waddch(w, rankstr[card_rank(c)]);
	waddstr(w, suite_symbol[card_suite(c)]);
}

void mvprint_card(const card c, WINDOW *w, const int x, const int y)
{
	wmove(w, y, x);
	wprint_card(c, w);
}

void wprint_pile(Pile *p, WINDOW *w)
{
	for(; p; p = p->next)
	{
		wprint_card(p->top, w);
		wattrset(w, COLOR_PAIR(1));
		waddch(w, ' ');
	}
}

void mvprint_pile(Pile *p, WINDOW *w, const int x, const int y)
{
	wmove(w, y, x);
	wprint_pile(p, w);
}


card card_from_input(const char input_str[3])
{
	int suite = -1, rank = -1;
	int i;
	for(i = 0; i < 2; ++i)
	{
		if(input_str[i] >= '1' && input_str[i] <= '9')
			rank = input_str[i]-'1';
		else
		{
			switch(tolower(input_str[i]))
			{
			case 'd': suite = DIAMONDS; break;
			case 'h': suite = HEARTS; break;
			case 's': suite = SPADES; break;
			case 'c': suite = CLUBS; break;
			case '0': case 't': rank = TEN; break;
			case 'a': rank = ACE; break;
			case 'j': rank = JACK; break;
			case 'q': rank = QUEEN; break;
			case 'k': rank = KING; break;
			default: return NO_CARD;
			}
		}
	}
	if(suite == -1 || rank == -1)
		return NO_CARD;
	return card_from_suiterank(suite, rank);
}

