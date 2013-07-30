/* An example solitaire (calculation) illustrating the use of libclicards. */

#include <clicards.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <locale.h>

int sym_to_idx(const int sym);
int valid_placement(const card place, const card under, const int slot);

int main(void)
{
	WINDOW *mainw;
	Pile *deck, *next_card = NULL;
	Pile *stacks[4] = { NULL, NULL, NULL, NULL };
	Pile *tmps[4] = { NULL, NULL, NULL, NULL };
	int input, num;
	char draw_new = 1;

	setlocale(LC_ALL, "");
	srandom(time(NULL));

	deck = standard_deck(1);

    if(!initscr() || start_color() == ERR)
		return 1;
	noecho();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	mainw = newwin(8,50,0,0);
	clicards_init_ncurses();
	refresh();
	flushinp();

	/* Pull out ace through 4 of diamonds to start with: */
	for(input = 0; input < 4; ++input)
		add_pile_to_pile(&stacks[input],
			card_in_pile(card_from_suiterank(DIAMONDS, input), &deck));
	shuffle_pile(&deck, &random);
	wclear(mainw);
	wattrset(mainw, COLOR_PAIR(1));
	mvwaddstr(mainw, 1, 0, ">1 >2 >3 >4");
	mvwaddstr(mainw, 3, 0, ">q >w >e >r");

	for(;;)
	{
		if(draw_new)
			next_card = deck ? draw_n_cards(&deck, 1) : NULL;
		for(input = 0; input < 4; ++input)
		{
			mvprint_card(last_card(stacks[input]), mainw, input*3, 0);
			if(tmps[input])
				mvprint_card(last_card(tmps[input]), mainw, input*3, 2);
			else
				mvwaddstr(mainw, 2, input*3, "  ");
		}
		wattrset(mainw, COLOR_PAIR(1));
		mvwaddstr(mainw, 4, 0, "\'m\' to move, '!' to quit");
		if(next_card)
		{
			waddstr(mainw, ", or place next card: ");
			wprint_card(next_card->top, mainw);
		}
		else
			wclrtoeol(mainw);
		wrefresh(mainw);

		draw_new = 1;
		for(;;) /* wait until get proper input */
		{
			input = tolower(getch());
			if(next_card && (input == 'q' || input == 'w' ||
				input == 'e' || input == 'r'))
				{ add_pile_to_pile(&tmps[sym_to_idx(input)], next_card); break; }
			if(input == '!')
				goto quitting;
			if(input == 'm')
			{
				wattrset(mainw, COLOR_PAIR(1));
				mvwaddstr(mainw, 5, 0, "q-r to select card, 1-4 for target,"
					" \'x\' to cancel.");
				wrefresh(mainw);
				do num = tolower(getch());
				while(num != 'x' && num != 'q' && num != 'w'
					&& num != 'e' && num != 'r');
				if(num == 'x')
					goto cancelled;
				num = sym_to_idx(num);
				do input = tolower(getch());
				while(input != 'x' && (input < '1' || input > '4'));
				if(input == 'x')
				{
					cancelled:
					wmove(mainw, 5, 0);
					wclrtoeol(mainw);
					continue;
				}
				input -= '1';
				if(tmps[num] && valid_placement(last_card(tmps[num]),
					last_card(stacks[input]), input))
					add_pile_to_pile(&stacks[input],
						card_in_pile(last_card(tmps[num]), &tmps[num]));
				else
				{
					mvwaddstr(mainw, 5, 0, "Illegal move. Hit any key to continue.");
					wclrtoeol(mainw);
					wrefresh(mainw);
					getch();
				}
				wmove(mainw, 5, 0);
				wclrtoeol(mainw);
				draw_new = 0;
				break;
			}
			if(next_card)
			{
				for(num = 0; num < 4; ++num)
					if(input == '1'+num && valid_placement(next_card->top,
						last_card(stacks[num]), num))
					{
						add_pile_to_pile(&stacks[num], next_card);
						break;
					}
				if(num < 4)
					break;
			}
		}
	}

	quitting:
	wattrset(mainw, COLOR_PAIR(1));
	for(input = 0; input < 4; ++input)
		if(next_card || tmps[input])
		{
			waddstr(mainw, "\nYou lose!");
			break;
		}
	if(input == 4)
		waddstr(mainw, "\nYou win!");

	add_pile_to_pile(&deck, next_card);
	for(input = 0; input < 4; ++input)
	{
		add_pile_to_pile(&deck, stacks[input]);
		add_pile_to_pile(&deck, tmps[input]);
	}
	waddstr(mainw, "\nHit any key to quit.");
	wrefresh(mainw);
	getch();

	delwin(mainw);
	endwin();
	free_pile(deck);
	return 0;
}


int sym_to_idx(const int sym)
{
	switch(sym)
	{
	case 'q': return 0;
	case 'w': return 1;
	case 'e': return 2;
	}
	return 3;
}

int valid_placement(const card place, const card under, const int slot)
{
	return (card_rank(under) != KING
		&& card_rank(place) == (card_rank(under)+1+slot)%13);
}
