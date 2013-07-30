/* An example poker game illustrating the use of libclicards.
 *
 * The player is dealt a 5-card hand. He can choose which cards to exchange,
 * gets new cards, and the computer's hand is revealed. Determining who won
 * is left to the player, since it is a trivial task for a human but rather
 * involved to put into code (and would not serve much of a purpose in this
 * kind of an example). */

#include <clicards.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <locale.h>

int main(void)
{
	WINDOW *mainw;
	Pile *deck, *plr_hand, *cpu_hand, *tmp;
	int input, num;
	char change[5];

	setlocale(LC_ALL, "");
	srandom(time(NULL));

	deck = standard_deck(1);

    if(!initscr() || start_color() == ERR)
		return 1;
	noecho();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	mainw = newwin(5,48,0,0);
	clicards_init_ncurses();
	refresh();
	flushinp();

	for(;;)
	{
		for(input = 0; input < 5; ++input)
			change[input] = 0;
		shuffle_pile(&deck, &random);
		plr_hand = draw_n_cards(&deck, 5);
		cpu_hand = draw_n_cards(&deck, 5);
		tmp = NULL;
		wattrset(mainw, COLOR_PAIR(1));
		wclear(mainw);
		waddstr(mainw, "Your hand: ");
		wprint_pile(plr_hand, mainw);
		waddstr(mainw, "\n           0  1  2  3  4\n"
			"Select cards to change, 'd' when done.");
		wrefresh(mainw);
		while(tolower(input = getch()) != 'd')
		{
			if(input >= '0' && input <= '4')
			{
				input -= '0';
				change[input] = !change[input];
				wmove(mainw, 1, 12+input*3);
				waddch(mainw, change[input] ? '*' : ' ');
				wrefresh(mainw);
			}
		}
		for(input = num = 0; input < 5; ++input)
		{
			if(change[input])
			{
				add_pile_to_pile(&tmp, nth_card(&plr_hand, input-num));
				++num;
			}
		}
		if(num)
			add_pile_to_pile(&plr_hand, draw_n_cards(&deck, num));
		sort_pile(&plr_hand, 0, 1);
		mvprint_pile(plr_hand, mainw, 11, 0);
		wmove(mainw, 1, 0);
		wclrtoeol(mainw);
		wmove(mainw, 2, 0);
		waddstr(mainw, "Computer's hand: ");
		sort_pile(&cpu_hand, 0, 1);
		wprint_pile(cpu_hand, mainw);
		wclrtoeol(mainw);
		waddstr(mainw, "\nHit 'q' to quit or any other key to deal again.");
		add_pile_to_pile(&deck, tmp);
		add_pile_to_pile(&deck, plr_hand);
		add_pile_to_pile(&deck, cpu_hand);
		wrefresh(mainw);
		if(getch() == 'q')
			break;
	}
	delwin(mainw);
	endwin();
	free_pile(deck);
	return 0;
}
