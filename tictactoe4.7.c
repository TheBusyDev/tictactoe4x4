/*
TIC TAC TOE GAME
UPDATES: 
'outcomes' reintroduced
'debug mode' added
Easter Egg added
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define VERSION "4.7"
#define LEN 4
#define SPOTS LEN*LEN
#define MAX_SCORE SPOTS+1
#define MAXLEN 30
#define PASSWORD "1010"

int tab[SPOTS]; // game board
int tab_highlight[SPOTS]; // 0=position NOT highlighted, 1=position highlighted
int first_move; // AI first move 
int outcomes; // outcomes calculated by AI
int debug; // 1=debug mode activated
char level;
char filename_scores[MAXLEN], filename_level[MAXLEN];

void PrintTableWithNumbers (int);
void PrintTable ();
int ExtractTurn (); // +1 = maximising player turn, -1 = minimising player turn
int AImove (int, int);
int minimax (int ,int, int, int, int, int);
int evaluation (int);
int CheckWinner (int); // this function returns 0 if there is no winner, 10 if the winner is the computer, -10 if you are the winner
void clear ();
int insert (int, int);
int max (int, int);
int min (int, int);
void reset (int []); // reset to 0 all the values of a given array - dimension must be equal to SPOTS
void SaveAndPrintScores (int); // save and print scores from a file
void SetLevelAndDebug ();
void ChangeLevelAndDebug ();
void EasterEgg ();

int main ()
{
	int i, count; // counters
	int winner, move, turn, error;
	int max_depth; // max depth reached by minimax function
	char ans;
	char pwd[MAXLEN];

	srand (time(NULL));

	strcpy (filename_scores, "tictactoe");
	strcat (filename_scores, VERSION);
	strcat (filename_scores, ".score");

	strcpy (filename_level, "tictactoe");
	strcat (filename_level, VERSION);
	strcat (filename_level, ".level");

	SetLevelAndDebug ();

	do 
	{	
		clear ();
		printf ("\n\033[1;32mTIC TAC TOE %dx%d, %s VERSION\033[0m\n", LEN, LEN, VERSION);
		printf ("\n\033[1mThese are the input positions:\033[0m\n");

		// initialize the tables
		reset (tab);
		reset (tab_highlight);

		count=0; // move counter

		PrintTableWithNumbers (count);

		printf ("\033[1m");

		if (level == 'e')
		{
			printf ("\nEasy mode!\n");
			max_depth = 2;
		}
		else if (level == 'm')
		{
			printf ("\nLevel: medium\n");
			max_depth = 4;
		}
		else if (level == 'h')
		{
			printf ("\nLevel: hard. You have no chance to win!\n");
			max_depth = 6;
		}

		printf ("\nYou are 'X' and I am 'O'.\nYour move must be a number between 1 and %d.\nPress [ENTER] to start the game...\033[0m\n", SPOTS);
		getchar ();

		clear ();
		turn=ExtractTurn ();

		do 
		{	
			if (turn == +1)
			{
				if (!count)
					printf ("\n\033[1mThat's my move!\n");
				
				count++;
				insert (+1, AImove (count, max_depth)); // insert 'O' in the position calculated by 'AImove'
			}

			if (turn == -1)
			{
				if (!count)
					printf ("\n\033[1mIt's your turn!\n");

				count++;

				do 
				{
					PrintTableWithNumbers (count);
					PrintTable ();

					printf ("\n\n\n\033[1mYour move?\033[0m ");
					scanf ("%d", &move);
					getchar ();
					clear ();

					if (move>=1 && move <= SPOTS)
					{
						if (insert (-1, move-1))
							error=0;
						else
						{
							printf ("\nError: position %d already occupied!\n", move);
							error=1;
						}
					}
					else if (move == 999) // Game Over secret code: stop the game even if there is no winner
					{
						printf ("\n\033[1;36mGame Over code...\033[0m\n\n");
						error=0;
					}
					else
					{
						printf ("\nError: your move must be a number between 1 and %d!\n", SPOTS);
						error=1;
					}
				} while (error);
			}

			turn=turn*(-1);
			winner=CheckWinner (1);

			if (count == SPOTS && !winner) // if there is no empty space and there is no winner
				winner = -2;

			if (move == 999)
			{
				winner = -2;
				move = 0;
			}

		} while (!winner);

		PrintTable ();

		printf ("\n\n\n\033[1;36m");

		if (winner == -2)
			printf ("No winner :(");
		else if (winner == +1)
			printf ("I am the winner!!");
		else if (winner == -1)
			printf ("Oh no, you defeated me!!");

		SaveAndPrintScores (winner);

		do 
		{
			printf ("\n\033[1;36mDo you like another match?\n[y]=yes, [n]=no, [c]=change level, [r]=reset scores\033[0m\n");
			ans=getchar ();
			getchar ();

			if (ans == 'c')
				do 
				{
					printf ("\n\033[1;36mType the level you want:\n[e]=easy, [m]=medium, [h]=hard\033[0m\n");
					level=getchar ();
					getchar ();

					if (level != 'e' && level != 'm' && level != 'h')
						printf ("\nError: answer not allowed...\n");
					else
					{
						ChangeLevelAndDebug ();
						printf ("\n\033[1mLevel changed successfully!\033[0m\n");
					}
				} while (level != 'e' && level != 'm' && level != 'h');				
			else if (ans == 'r')
			{
				remove (filename_scores);
				printf ("\n\033[1mScores reset successfully!\033[0m\n");
			}
			else if (ans == 'd')
			{
				printf ("\nPassword: ");
				scanf ("%s", pwd);
				getchar ();

				if (!strcmp (pwd, PASSWORD) && debug)
				{
					debug=0;
					ChangeLevelAndDebug ();
					printf ("\n\033[1mDebug mode deactivated successfully!\033[0m\n");
				}
				else if (!strcmp (pwd, PASSWORD) && !debug)
				{
					debug=1;
					ChangeLevelAndDebug ();
					printf ("\n\033[1mDebug mode activated successfully!\033[0m\n");
				}
				else
					printf ("\nError: password not correct!\n");
			}
			else if (ans != 'y' && ans != 'n')
				printf ("\nError: answer not allowed...\n");

		} while (ans != 'y' && ans != 'n');

	} while (ans == 'y');
	
	return 0;
}

void PrintTableWithNumbers (int count)
{
	int i, j;

	printf ("\n\033[1m");

	if (count)
		printf ("Available spots:\n\n");

	for (i=1; i <= SPOTS; i++)
	{
		if (tab[i-1])
			printf ("  -  |");
		else if (i<10)
			printf ("  %d  |", i);
		else
			printf ("  %d |", i);

		if (! (i%LEN))
		{
			printf ("\n");

			for (j=0; j < LEN*6; j++)
				printf ("-");

			printf ("\n");
		}
	}

	printf ("\033[0m");
}

void PrintTable ()
{
	int i, j;

	printf ("\n\033[1m");

	for (i=0; i<SPOTS; i++)
	{
		if (tab_highlight[i])
			printf ("\033[1;36m");

		if (tab[i] == +1)
			printf ("  O  ");
		else if (tab[i] == -1)
			printf ("  X  ");
		else
			printf ("     ");

		if (tab_highlight[i])
			printf ("\033[0m\033[1m");

		printf ("|");

		if (! ((i+1)%LEN))
		{
			printf ("\n");

			for (j=0; j < LEN*6; j++)
				printf ("-");

			printf ("\n");
		}
	}

	printf ("\033[0m");
}

int ExtractTurn ()
{
	int turn;
	
	turn=rand()%2;

	if (!turn)
		turn=-1;

	return turn;
}

int AImove (int count, int max_depth)
{	
	int i, j, k; // counters
	int turn;
	int possible_moves[SPOTS];
	int eval = 0, max_eval = 0;
	clock_t begin = clock ();

	// check whether one player has the chance to win in only one move
	for (turn = +1; turn; turn=turn*(-1))
	{
		for (i=0; i<SPOTS; i++) 
			if (insert (turn, i)) 
			{
				if (CheckWinner (0)) // AI or human has the chance to win in only one move
				{
					tab[i]=0;
					return i;
				}

				tab[i]=0;
			}

		if (turn == -1)
			turn=0;
	}

	if (count == 1 || count == 2) // first move and second move
	{
		int central_position = LEN*(LEN/2) + LEN/2; // formula to get the position: position = (LEN*row) + col

		if (! (LEN%2)) // LEN is an even number
		{
			possible_moves[0] = central_position-(LEN+1);
			possible_moves[1] = central_position-LEN;
			possible_moves[2] = central_position-1;
			possible_moves[3] = central_position;

			do
			{
				first_move = possible_moves[rand()%4];
			} while (! insert (+1, first_move));
		}
		else if (insert (+1, central_position)) // LEN is an odd number and the central position is NOT occupied
			first_move = central_position;

		else // LEN is an odd number and the central position is already occupied
		{
			possible_moves[0] = central_position-(LEN+1);
			possible_moves[1] = central_position-LEN;
			possible_moves[2] = central_position-(LEN-1);
			possible_moves[3] = central_position-1;
			possible_moves[4] = central_position+1;
			possible_moves[5] = central_position+(LEN-1);
			possible_moves[6] = central_position+LEN;
			possible_moves[7] = central_position+(LEN+1);

			first_move = possible_moves[rand()%8];
		}

		return first_move;
	}
	else if (count <= 6)
	{
		if (count == 4 || count == 6) // forth and sixth moves are defensive moves
		{
			if (level == 'e') // activated only in easy mode
			{
				for (i=0, k=0; i<SPOTS; i++)
					if (insert (-1, i))
					{
						eval = evaluation (-1);
						tab[i]=0;

						if (eval > 0)
						{
							possible_moves[k] = i;
							k++;
						}
					}
			}
			else
			{
				for (i=0, k=0; i<SPOTS; i++)
					if (insert (-1, i))
					{
						eval = evaluation (-1);
						tab[i]=0;

						if (eval > max_eval)
						{
							max_eval=eval;					
							possible_moves[0] = i;
							k=1;
						}
						else if (max_eval && eval == max_eval)
						{
							possible_moves[k] = i;
							k++;
						}
					}

				if (count == 4 && level == 'h') // // activated only in hard mode, block one of algorithm weak points...
				{
					int delete;

					i=0;

					while (i<k && k>1)
					{
						delete=0;
						
						// check whether one possible move is on the same row of 'first_move'
						for (j = first_move - (first_move%LEN); j/LEN == first_move/LEN && !delete; j++)
							if (possible_moves[i] == j)
								delete=1;


						// check whether one possible move is on the same coloumn of 'first_move'
						for (j= first_move - LEN*(first_move/LEN); j<SPOTS && !delete; j = j + LEN)
							if (possible_moves[i] == j)
								delete=1;

						if (delete)
							for (j=i, k--; j<k; j++)
								possible_moves[j] = possible_moves[j+1];
						else
							i++;
					}
				}
			}

			if (k)
			{
				if (debug)
				{
					printf ("\nPossible moves: ");

					for (i=0; i<k; i++)
						printf ("%d ", possible_moves[i]+1);

					printf ("\nEval: %d\nTime elapsed: %f\n", max_eval, (float) (clock () - begin)/CLOCKS_PER_SEC);
				}

				return possible_moves[rand()%k];
			}
		}

		possible_moves[0] = first_move-(LEN+1);
		possible_moves[1] = first_move-LEN;
		possible_moves[2] = first_move-(LEN-1);
		possible_moves[3] = first_move-1;
		possible_moves[4] = first_move+1;
		possible_moves[5] = first_move+(LEN-1);
		possible_moves[6] = first_move+LEN;
		possible_moves[7] = first_move+(LEN+1);

		if (count == 3 && level != 'h') // activated only in easy and medium mode
		{	
			for (i=0, k=0; i<8; i++)
				if (insert (+1, possible_moves[i]))
				{
					tab[possible_moves[i]]=0;
					possible_moves[k] = possible_moves[i];
					k++;
				}
		}
		else
		{
			for (i=0, k=0; i<8; i++) // third and fifth moves are offensive moves
				if (insert (+1, possible_moves[i]))
				{
					eval = evaluation (+1);
					tab[possible_moves[i]]=0;

					if (eval > max_eval)
					{
						max_eval=eval;
						possible_moves[0] = possible_moves[i];
						k=1;
					}
					else if (max_eval && eval == max_eval)
					{
						possible_moves[k] = possible_moves[i];
						k++;
					}
				}

			if (!k)
				for (i=0; i<8; i++)
					if (insert (+1, possible_moves[i]))
					{
						tab[possible_moves[i]]=0;
						possible_moves[k] = possible_moves[i];
						k++;
					}
		}

		if (k)
		{
			if (debug)
			{
				printf ("\nPossible moves: ");

				for (i=0; i<k; i++)
					printf ("%d ", possible_moves[i]+1);

				printf ("\nEval: %d\nTime elapsed: %f\n", max_eval, (float) (clock () - begin)/CLOCKS_PER_SEC);
			}
			else if (max_eval == 6 || max_eval == 7)
				EasterEgg ();
			
			return possible_moves[rand()%k];
		}
	}
	
	int score, best_score = -MAX_SCORE-1;
	int alpha = -MAX_SCORE-1, beta = +MAX_SCORE+1;

	if (count <= 7 && max_depth > 4)
		max_depth = 4;
	else if (count == 8 && max_depth > 5)
		max_depth = 5;

	for (i=0, outcomes=0; i<SPOTS; i++)
		if (insert (+1, i))
		{
			score = minimax (count, 0, max_depth, alpha, beta, -1);
			tab[i]=0; // reset the spot occupied

			alpha = max (score, alpha);
			
			if (score > best_score)
			{
				best_score = score;
				possible_moves[0] = i;
				j=1;
			}
			else if (score == best_score)
			{
				possible_moves[j] = i;
				j++;
			}
		}

	for (i=0, k=0; !best_score && i<j; i++)
		if (insert (+1, possible_moves[i]))
		{
			eval = evaluation (+1);
			tab[possible_moves[i]]=0;

			if (eval > max_eval)
			{
				max_eval=eval;
				possible_moves[0] = possible_moves[i];
				k=1;
			}
			else if (max_eval && eval == max_eval)
			{
				possible_moves[k] = possible_moves[i];
				k++;
			}
		}

	if (debug)
		printf ("\nOutcomes calculated: %d\nPossible moves: ", outcomes);

	if (k)
	{
		if (debug)
		{
			for (i=0; i<k; i++)
				printf ("%d ", possible_moves[i]+1);
			
			printf ("\nScore: %d, eval: %d\nTime elapsed: %f seconds\n", best_score, max_eval, (float) (clock () - begin)/CLOCKS_PER_SEC);
		}

		return possible_moves[rand()%k];
	}

	if (debug)
	{
		for (i=0; i<j; i++)
			printf ("%d ", possible_moves[i]+1);

		printf ("\nScore: %d\nTime elapsed: %f seconds\n", best_score, (float) (clock () - begin)/CLOCKS_PER_SEC);
	}
	else if (best_score > 0)
		EasterEgg ();

	return possible_moves[rand()%j];
}

int minimax (int move_counter, int depth, int max_depth, int alpha, int beta, int turn)
{	
	int i;
	int score, best_score;
	int winner;

	winner=CheckWinner (0);

	if (winner)
	{
		if (debug)
		{
			outcomes++;
			printf ("\nDepth reached: %d, score: %d\n", depth, winner*(+MAX_SCORE - depth));
		}

		return winner*(+MAX_SCORE - depth);
	}
	else if (depth == max_depth || depth == SPOTS-move_counter)
	{
		if (debug)
		{
			outcomes++;
			printf ("\nDepth: %d, tie!\n", depth);
		}

		return 0;
	}

	if (turn == +1)
	{
		best_score = -MAX_SCORE-1;

		for (i=0; i<SPOTS; i++)
			if (insert (+1, i))
			{
				score = minimax (move_counter, depth+1, max_depth, alpha, beta, -1);
				tab[i]=0; // reset the spot occupied

				best_score = max (score, best_score);
				alpha = max (score, alpha);

				if (alpha > beta || best_score == +MAX_SCORE - (depth+1)) // maximising player will win in 1 move - no more searching needed + alpha-beta pruning
					return best_score;
			}
	}
	else
	{
		best_score = +MAX_SCORE+1;

		for (i=0; i<SPOTS; i++)
			if (insert (-1, i))
			{
				score = minimax (move_counter ,depth+1, max_depth, alpha, beta, +1);
				tab[i]=0; // reset the spot occupied

				best_score = min (score, best_score);
				beta = min (score, beta);

				if (alpha > beta || best_score == -MAX_SCORE + (depth+1)) // minimising player will win in 1 move - no more searching needed + alpha-beta pruning
					return best_score;
			}
	}
		
	return best_score;
}

int evaluation (int turn)
{
	int i, j;
	int eval1, eval2, max_eval = 0;

	for (i=0, eval1=0; i<SPOTS; i++)
		if (insert (turn, i))
		{
			if (CheckWinner (1))
				eval1 = eval1 + 2;

			tab[i]=0;
		}

	if (eval1 == 4)
		max_eval = 7;
	else if (eval1 == 2)
		max_eval = 2;

	if (level == 'h' && max_eval != 7) // activated only in hard mode
		for (i=0; i<SPOTS; i++)
			if (!tab_highlight[i] && insert (turn, i))
			{
				for (j=0, eval2=0; j<SPOTS; j++)
					if (!tab_highlight[j] && insert (turn, j))
					{
						if (CheckWinner (0))
							eval2 = eval2 + 1;

						tab[j]=0;
					}

				if (eval2 == 1)
					max_eval = max (eval1+1, max_eval);
				else if (eval2 == 2)
					max_eval = max (eval1+4, max_eval);

				tab[i]=0;
			}

	reset (tab_highlight);

	return max_eval;
}

int CheckWinner (int highlight)
{
	int i, j, win;

	// check rows
	for (i=0; i<SPOTS; i=i+LEN)
	{
		if (!tab[i])
			win=0;
		else
			win=1;

		for (j=1; j<LEN && win; j++)
			if (tab[i] != tab [i+j])
				win=0;

		if (win)
		{
			if (highlight)
				for (j=0; j<LEN; j++)
					tab_highlight[i+j]=1;

			return tab[i];
		}
	}

	// check coloumns
	for (i=0; i<LEN; i++)
	{
		if (!tab[i])
			win=0;
		else
			win=1;

		for (j=LEN; j<SPOTS && win; j=j+LEN)
			if (tab[i] != tab [i+j])
				win=0;

		if (win)
		{
			if (highlight)
				for (j=0; j<SPOTS; j=j+LEN)
					tab_highlight[i+j]=1;
			
			return tab[i];
		}
	}
	
	// check diagonals
	// check first diagonal
	if (!tab[0])
		win=0;
	else
		win=1;

	for (i = LEN+1; i < SPOTS && win; i = i + (LEN+1))
		if (tab[0] != tab[i])
			win=0;

	if (win)
	{
		if (highlight)
			for (i=0; i<SPOTS; i = i + (LEN+1))
				tab_highlight[i]=1;

		return tab[0];
	}

	// check second diagonal
	if (!tab[LEN-1])
		win=0;
	else
		win=1;

	for (i = (LEN-1)*2; i <= (LEN-1)*LEN && win; i = i + (LEN-1))
		if (tab[LEN-1] != tab[i])
			win=0;

	if (win)
	{
		if (highlight)
			for (i = LEN-1; i <= (LEN-1)*LEN; i = i + (LEN-1))
				tab_highlight[i]=1;

		return tab[LEN-1];
	}

	// new rule: win with a square
	// check squares
	for (i=0; i < LEN*(LEN-1) - 1; i++)
		if ((i+1)%LEN)
		{
			if (!tab[i] || tab[i] != tab[i+1] || tab[i] != tab[i+LEN] || tab[i] != tab[i+LEN+1])
				win=0;
			else
				win=1;

			if (win)
			{
				if (highlight)
				{
					tab_highlight[i]=1;
					tab_highlight[i+1]=1;
					tab_highlight[i+LEN]=1;
					tab_highlight[i+LEN+1]=1;
				}

				return tab[i];
			}
		}

	return 0;
}

void clear ()
{
	int i;

	for (i=0; i<60; i++)
		printf ("\n");
}

int insert (int player, int position)
{
	if (tab[position]) // position already occupied
		return 0;
	
	tab[position] = player;

	return 1;
}

int max (int a, int b)
{
	if (a>b)
		return a;
	else
		return b;
}

int min (int a, int b)
{
	if (a<b)
		return a;
	else
		return b;
}

void reset (int arr[])
{
	int i;

	for (i=0; i<SPOTS; i++)
		arr[i]=0;
}

void SaveAndPrintScores (int winner)
{
	FILE *fp;
	int human_score, ai_score;

	fp=fopen (filename_scores, "r");

	if (fp) // file exists
	{
		fscanf (fp, "%d %d", &human_score, &ai_score);
		fclose (fp);
	}
	else // file does not exist
	{
		human_score=0;
		ai_score=0;
	}

	if (winner != -2) // there is actually a winner
	{
		if (winner == -1)
			human_score++;
		else if (winner == +1)
			ai_score++;

		fp=fopen (filename_scores, "w");
		fprintf (fp, "%d %d", human_score, ai_score);
		fclose (fp);
	}

	printf ("\nScore: human: %d vs AI: %d\n", human_score, ai_score);
}

void SetLevelAndDebug ()
{
	FILE *fp;

	fp=fopen (filename_level, "r");

	if (fp) // file exists
	{
		fscanf (fp, "%c %d", &level, &debug);
		fclose (fp);
	}
	else // file does not exist
	{
		level = 'e';
		debug = 0;
	}
}

void ChangeLevelAndDebug ()
{
	FILE *fp;

	fp=fopen (filename_level, "w");
	fprintf (fp, "%c %d", level, debug);
	fclose (fp);
}

void EasterEgg ()
{
	printf ("\n\"I went forward in time, to see alternative futures,\nto see all the possible outcomes\nof the coming conflict\"\n\"How many did you see?\"\n\"14,000,605\"\n\"How many did we win?\"\n\"One\"\n- Doctor Strange, Avengers Infinity War");
	clear ();
}
