#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "CardFunctions.h"

void main() {
	//== Variables ==
	// application 
	int mainMenuOption;
	int inGameOption;
	char exitOption = 'x';
	bool isLoadedGame;

	// cards 
	card_t deck[DECK_SIZE];
	card_t playedCards[6];
	card_t trump, lead;
	int cardChoice;
	int trickWinCard;

	// players 
	player_t players[6];
	int numOfPlayers;
	int currentPlayer;
	int trickWinPlayer = -1;

	// other 
	int playedCardRank;
	int trickWinRank;
	int highScore;
	int r;

	// renege & robbing
	bool legalRenege;
	bool legalRob;
	bool legalDealerRob;
	bool trumpRobbed;
	char robOption;

	// counters
	int i, j, k, illegalCount, trickCount;

	srand(time(NULL));

	// Main Menu - title
	printf("Welcome to 25!");
	do
	{
		// menu options
		printf("\n\n[Main Menu]\n 1.New Game \n 2.Load Game \n 3.How to Play \n 4.Exit");
		printf("\n>");
		scanf("%d", &mainMenuOption);

		// 1.0 - New Game
		if (mainMenuOption == 1) {
			// 1.1 - Players Setup
			printf("\n\t[Enter Number of Players]\n\t>"); // prompt number of players
			scanf("%d", &numOfPlayers);
			while (numOfPlayers < 2 || numOfPlayers > 6) { // input validation 
				printf("\n\tINVALID ENTRY - Number of Players must be within the range 2-6 (inclusive)\n\n\t[Enter Number of Players]\n\t>");
				scanf("%d", &numOfPlayers);
			}

			for (i = 0; i < numOfPlayers; i++) { // populate players array
				players[i].number = i + 1; // assign player numbers
				players[i].score = 0; // initialize player scores to 0
			}


			highScore = 0; trickCount = 1;
			// 2.0 - Hand
			do
			{
				// 2.1 - Initialize Deck
				for (i = 0; i < DECK_SIZE; i++) {
					deck[i].id = i;
					deck[i].isActive = 1;
					deck[i].rank = calculateRank(deck[i].id);
					deck[i].suit = calculateSuit(deck[i].id);
				}

				//for (i = 0; i < DECK_SIZE; i++) {
				//	printf("\nDEBUG - id=:%d rank=%d suit=%d", deck[i].id, deck[i].rank, deck[i].suit);
				//}
				// 2.2 - Deal
				for (i = 0; i < numOfPlayers; i++) { // deal 5 cards to each player
					for (j = 0; j < HAND_SIZE; j++) {
						do
						{
							r = rand() % DECK_SIZE; // N.B. - Don't + 1 to DECK_SIZE to ensure a card can be picked within ID range 0 - 51 inclusivly
						} while (deck[r].isActive == 0); // check if card has already been drawn

						moveCard(&deck[r], &players[i].hand[j]); // move the card from deck to players hand

					}// j
				}// i


				// 2.3 - Trump
				do
				{
					r = rand() % DECK_SIZE; // N.B. - Don't + 1 to DECK_SIZE to ensure a card can be picked within ID range 0 - 51 inclusivly
				} while (deck[r].isActive == 0); // check if card has already been drawn

				moveCard(&deck[r], &trump); // assign random card to be trump


				// 2.4 - First to Play
				if (highScore == 0)
					currentPlayer = rand() % numOfPlayers; // pick a player from 0 - n inclusivly


				// 3.0 - Trick
				i = 0; inGameOption = 1;  trumpRobbed = false; // pre-trick initialization
				do{
					// 3.1 - Initialize Trick
					trickWinRank = 0; // reset trick winning rank


					// 3.2  - Previous Trick Winner Leads
					if (i > 0) 
						currentPlayer = trickWinPlayer; // winner of last trick starts the next

					printf("\n\n\tTrick %d", trickCount);
					printf("\n\t________________________________________________________________________");
					// 4.0 - Play
					for (j = 0; j < numOfPlayers; j++) {
						legalRob = false;
						legalDealerRob = false;


						if (currentPlayer == numOfPlayers) // if starting player is 4, then play order is 4,5,0,1,2,3
							currentPlayer = 0;

						// 4.1 - Display
						printf("\n\n\tPlayer %d's turn\n", players[currentPlayer].number);
						if (trumpRobbed == true)
							printf("\n\t\tSomeone has robbed the pack!"); // 4.1.1 - Display Rob Status

						if (j == 0) { // first play of the trick
							printf("\n\t\tTrump:"); printCard(trump); // 4.1.2 - Display Trump
						}
						else { // not first play of the trick
							printf("\n\t\tTrump:"); printCard(trump); // 4.1.2 - Display Trump
							printf(" Lead: "); printCard(lead); //  4.1.3 - Display Lead

							// 4.1.4 - Display Played Cards
							printf("\n\t\tPlayed Cards:"); 
							for (k = 0; k < 6; k++) {
								if (playedCards[k].isActive == 1)
									printCard(playedCards[k]);
							}// k
						}

						// 4.1.5 - Display Hand
						printf("\n\t\tPlayer %d's Hand:", currentPlayer + 1); 
						for (k = 0; k < HAND_SIZE; k++) {
							if (players[currentPlayer].hand[k].isActive == 1) {// only show active cards 
								printf("|%d:", k + 1); printCard(players[currentPlayer].hand[k]); printf("|");
								if (players[currentPlayer].hand[k].suit == trump.suit && players[currentPlayer].hand[k].rank == 1) { // check if rob is legal while cycling through cards in hand
									if (i == 0) {
										legalRob = true; // if you hold the AT & it is first trick
									}
								}
							}
							else
								printf("| X |"); // print 'X' if card has already been played
						}// k


						// 4.2 Determine Rob Legality
						if (trump.rank == 1 && j == numOfPlayers - 1 && i == 0)
							legalDealerRob = true; // legal rob if A is trump and you are dealer

						do {
							if (legalRob == true || legalDealerRob == true) { // if it is legal for dealer or player to rob
								if (legalDealerRob == true) // prompt them to either rob or not
									printf("\n\n\t\tThe Trump is Ace, as the dealer you may rob\n\n\t\t[Rob?(y/n)]\n\t\t>");
								else 
									printf("\n\n\t\tYou hold the Ace of Trump, you may rob\n\n\t\t[Rob?(y/n)]\n\t\t>");


								scanf(" %c", &robOption);
								if (robOption == 'y' || robOption == 'Y') {
									legalDealerRob = false; legalRob = false; // set both illegal for next iterations
									printf("\n\t\t[Select a card to swap]\n\t\t>"); // prompt choice
									scanf("%d", &cardChoice);
									cardChoice--; // decrement choice i.e. if player chose card 1, take card from array position 0

									while (players[currentPlayer].hand[cardChoice].isActive == 0 || cardChoice < 0 || cardChoice > HAND_SIZE - 1) // input validation on card choice //TESTING
									{
										printf("\n\t\tINAVLID ENTRY - Must choose an active card within the range 1-5 (inclusive)\n\n\t\t[Choose card to swap]\n\t\t>"); // prompt choice
										scanf("%d", &cardChoice);
										cardChoice--; // decrement choice i.e. if player chose card 1, take card from array position 0
									}

									copyCard(trump, &players[currentPlayer].hand[cardChoice]); // copy values of trump card to chosen card position
									trumpRobbed = true; // for later display message

									printf("\n\t\tPlayer %d's Hand:", currentPlayer + 1); // re-display hand
									for (k = 0; k < HAND_SIZE; k++) {
										if (players[currentPlayer].hand[k].isActive == 1) {// only show cards in the players hand
											printf("|%d:", k + 1); printCard(players[currentPlayer].hand[k]); printf("|");
										}
										else
											printf("| X |");
									}// k

								}// if - robOption = y
							}// if legalRob = true;

							// 4.3 - Card Selection
							printf("\n\t\t[Select a card to play]\n\t\t>"); // prompt choice of card to play
							scanf("%d", &cardChoice);
							cardChoice--; // decrement choice i.e. if player chose card 1, take card from array position 0
							legalRenege = false; // initialize legalRenege

							while (players[currentPlayer].hand[cardChoice].isActive == 0 || cardChoice < 0 || cardChoice > HAND_SIZE - 1) // input validation for choice
							{
								printf("\n\t\tINAVLID ENTRY - Must choose an active card within the range 1-5 (inclusive)\n\n\t\t[Select a card to play]\n\t\t>"); // prompt choice
								scanf("%d", &cardChoice);
								cardChoice--; // decrement choice i.e. if player chose card 1, take card from array position 0
							}


							// 4.4 Legal Renege Validation
							if (j == 0) { // if no lead card played yet
								break; // any card played is valid for lead player
							}
							else if (trump.suit == lead.suit) { // if trump is led
								if (players[currentPlayer].hand[cardChoice].suit == trump.suit) { // played card suit must match trump suit (with exceptions)
									legalRenege = true; 
								}
							}
							else { // non-trump led
								if (players[currentPlayer].hand[cardChoice].suit == trump.suit || players[currentPlayer].hand[cardChoice].suit == lead.suit) { // played card suit must match trump suit or lead suit (with exceptions)
									legalRenege = true; 
								}
							}

							// renege exceptions
							for (k = 0; k < HAND_SIZE; k++) { // cycle through active cards in players hand 
								if (players[currentPlayer].hand[k].isActive == 1) { 
									if (players[currentPlayer].hand[k].id == 13 && trickWinRank < 25) // Ace of Hearts
										legalRenege = true;
									else if (players[currentPlayer].hand[k].rank == 5 && players[currentPlayer].hand[k].suit == trump.suit) // 5 of Trump suit
										legalRenege = true;
									else if (players[currentPlayer].hand[k].rank == 11 && players[currentPlayer].hand[k].suit == trump.suit && trickWinRank < 26) // Jack of Trump suit
										legalRenege = true;
								}// if - active card
							}// k 

							illegalCount = 0;
							for (k = 0; k < HAND_SIZE; k++) { // cycle through cards in hand to check if any are of trump or lead suit
								if (players[currentPlayer].hand[k].isActive == 1) {
									if (players[currentPlayer].hand[k].suit == trump.suit || players[currentPlayer].hand[k].suit == lead.suit)
										illegalCount++; // if player has card of trump or lead suit, excluding 5, J or AH they must play it
								}
							}

							if (illegalCount == 0) // if no cards of lead or trump suit in hand, renege is legal
								legalRenege = true;

							if (legalRenege == false)
								printf("\n\t\tINVALID ENTRY - Illegal Renege\n"); // error message for illegal renege
							//legalDealerRob = false; legalRob = false; // set both illegal for next player
						} while (legalRenege == false);
							
						// once a valid card has been selected, move it from hand to playedCards array
						moveCard(&players[currentPlayer].hand[cardChoice], &playedCards[j]); 

						if (j == 0) { // first play of the trick
							copyCard(playedCards[j], &lead); // copy played card values to lead
						}


						// 4.5 - Check Card Ranking
						playedCardRank = calculateRanking(playedCards[j], trump.suit, lead.suit); // get the ranking of played card
						if (playedCardRank > trickWinRank) { // if played card beats previous best card
							trickWinRank = playedCardRank; // played card becomes new best
							trickWinPlayer = currentPlayer;
							trickWinCard = cardChoice;
						}

						currentPlayer++;
					}// j

					// declare trick winner, show player and card
					printf("\n\tPlayer %d wins with trick with the", trickWinPlayer + 1); printCard(players[trickWinPlayer].hand[trickWinCard]);
					printf("\n\t________________________________________________________________________");
					

					// 4.6 - Award Points to Winner
					players[trickWinPlayer].score += 5;
					if (players[trickWinPlayer].score > highScore)
						highScore = players[trickWinPlayer].score;

					for (j = 0; j < 6; j++) {// clear played cards array at the end of play cycle
						playedCards[j].isActive = 0;
					}

					if (highScore >= WINNING_SCORE) // if any player has reached 25
						break;

					do
					{
						printf("\n\n\t[Game Menu]\n\t 1.Next Trick \n\t 2.Save \n\t 3.Output Status \n\t 4.Exit to Main Menu");
						printf("\n\t>");
						scanf("%d", &inGameOption);

						switch (inGameOption)
						{
						case 2:
							printf("\n\t\tsaving game...");
							saveGame(deck, trump, players, numOfPlayers, trickWinPlayer, highScore, trumpRobbed, i, trickCount);
							break;
						case 3:
							printf("\n\t\t----Score Table----");
							for (j = 0; j < numOfPlayers; j++) {
								printf("\n\t\tPlayer %d's Score: %d", players[j].number, players[j].score);
							}
							break;
						case 4: 
							printf("\n\t\texiting game - any unsaved progress is now lost");
							break;
						default:
							break;
						}
					} while (inGameOption == 3 || inGameOption == 2); // re-display menu if option 3 was selected

					i++; trickCount++;
				} while (i < HAND_SIZE && inGameOption == 1); 				

			} while (highScore < WINNING_SCORE && inGameOption == 1);

			if (inGameOption != 4) { // don't output winner if exit game was selected before game was concluded
				printf("\n\n\tPlayer %d Wins the Game!", players[trickWinPlayer].number);
				printf("\n\n\t\t----Score Table----");
				for (j = 0; j < numOfPlayers; j++) {
					printf("\n\t\tPlayer %d's Score: %d", players[j].number, players[j].score);
				}
				printf("\n\n\tThanks for playing!");
				printf("\n\t________________________________________________________________________");
			}


		}// if - new game
		else if (mainMenuOption == 2) {
			printf("\n\tloading game...");
			isLoadedGame = true;// if true the game will continue with the current deal
			int x, y, z;
			FILE* fptr;
			fptr = fopen("savegame.txt", "r");

			// read in all essential data stored in the "savegame.txt" file
			for (x = 0; x < DECK_SIZE; x++) {
				fscanf(fptr, "%d %d %d %d", &deck[x].id, &deck[x].rank, &deck[x].suit, &deck[x].isActive);
			}

			fscanf(fptr, "%d %d %d %d ", &trump.id, &trump.rank, &trump.suit, &trump.isActive);

			for (x = 0; x < 6; x++) {
				fscanf(fptr, "%d %d ", &players[x].number, &players[x].score);
				for (y = 0; y < HAND_SIZE; y++) {
					fscanf(fptr, "%d %d %d %d ", &players[x].hand[y].id, &players[x].hand[y].rank, &players[x].hand[y].suit, &players[x].hand[y].isActive);					
				}
			}

			fscanf(fptr, "%d ", &numOfPlayers);
			fscanf(fptr, "%d ", &trickWinPlayer);
			fscanf(fptr, "%d ", &highScore);
			int tempTrumpRobbed;
			fscanf(fptr, "%d ", &tempTrumpRobbed);
			if (tempTrumpRobbed == 1)
				trumpRobbed = true;
			else
				trumpRobbed = false;

			// increment both counters, to save moving the position they're incremented above in game
			fscanf(fptr, "%d ", &i);
			i++;
			fscanf(fptr, "%d ", &trickCount);
			trickCount++;

			// 2.0 - Hand
			do
			{
				if (isLoadedGame == false) { // these steps will be ignored the first time a game is loaded, so players can resume the tricks, if they run out of cards a new deal will occur
					// 2.1 - Initialize Deck
					for (i = 0; i < DECK_SIZE; i++) {
						deck[i].id = i;
						deck[i].isActive = 1;
						deck[i].rank = calculateRank(deck[i].id);
						deck[i].suit = calculateSuit(deck[i].id);
					}


					// 2.2 - Deal
					for (i = 0; i < numOfPlayers; i++) { // deal 5 cards to each player
						for (j = 0; j < HAND_SIZE; j++) {
							do
							{
								r = rand() % DECK_SIZE; // N.B. - Don't + 1 to DECK_SIZE to ensure a card can be picked within ID range 0 - 51 inclusivly
							} while (deck[r].isActive == 0); // check if card has already been drawn

							moveCard(&deck[r], &players[i].hand[j]); // move the card from deck to players hand

						}// j
					}// i


					// 2.3 - Trump
					do
					{
						r = rand() % DECK_SIZE; // N.B. - Don't + 1 to DECK_SIZE to ensure a card can be picked within ID range 0 - 51 inclusivly
					} while (deck[r].isActive == 0); // check if card has already been drawn

					moveCard(&deck[r], &trump); // assign random card to be trump


					// 2.4 - First to Play
					if (highScore == 0)
						currentPlayer = rand() % numOfPlayers; // pick a player from 0 - n inclusivly


					// 3.0 - Trick
					i = 0; inGameOption = 1;  trumpRobbed = false; // pre-trick initialization
				}

				do {
					// 3.1 - Initialize Trick
					trickWinRank = 0; // reset trick winning rank


					// 3.2  - Previous Trick Winner Leads
					if (i > 0)
						currentPlayer = trickWinPlayer; // winner of last trick starts the next

					printf("\n\n\tTrick %d", trickCount);
					printf("\n\t________________________________________________________________________");
					// 4.0 - Play
					for (j = 0; j < numOfPlayers; j++) {
						legalRob = false;
						legalDealerRob = false;


						if (currentPlayer == numOfPlayers) // if starting player is 4, then play order is 4,5,0,1,2,3
							currentPlayer = 0;

						// 4.1 - Display
						printf("\n\n\tPlayer %d's turn\n", players[currentPlayer].number);
						if (trumpRobbed == true)
							printf("\n\t\tSomeone has robbed the pack!"); // 4.1.1 - Display Rob Status

						if (j == 0) { // first play of the trick
							printf("\n\t\tTrump:"); printCard(trump); // 4.1.2 - Display Trump
						}
						else { // not first play of the trick
							printf("\n\t\tTrump:"); printCard(trump); // 4.1.2 - Display Trump
							printf(" Lead: "); printCard(lead); //  4.1.3 - Display Lead

							// 4.1.4 - Display Played Cards
							printf("\n\t\tPlayed Cards:");
							for (k = 0; k < 6; k++) {
								if (playedCards[k].isActive == 1)
									printCard(playedCards[k]);
							}// k
						}

						// 4.1.5 - Display Hand
						printf("\n\t\tPlayer %d's Hand:", currentPlayer + 1);
						for (k = 0; k < HAND_SIZE; k++) {
							if (players[currentPlayer].hand[k].isActive == 1) {// only show active cards 
								printf("|%d:", k + 1); printCard(players[currentPlayer].hand[k]); printf("|");
								if (players[currentPlayer].hand[k].suit == trump.suit && players[currentPlayer].hand[k].rank == 1) { // check if rob is legal while cycling through cards in hand
									if (i == 0) {
										legalRob = true; // if you hold the AT & it is first trick
									}
								}
							}
							else
								printf("| X |"); // print 'X' if card has already been played
						}// k


						// 4.2 Determine Rob Legality
						if (trump.rank == 1 && j == numOfPlayers - 1 && i == 0)
							legalDealerRob = true; // legal rob if A is trump and you are dealer

						do {
							if (legalRob == true || legalDealerRob == true) { // if it is legal for dealer or player to rob
								if (legalDealerRob == true) // prompt them to either rob or not
									printf("\n\n\t\tThe Trump is Ace, as the dealer you may rob\n\n\t\t[Rob?(y/n)]\n\t\t>");
								else
									printf("\n\n\t\tYou hold the Ace of Trump, you may rob\n\n\t\t[Rob?(y/n)]\n\t\t>");


								scanf(" %c", &robOption);
								if (robOption == 'y' || robOption == 'Y') {
									legalDealerRob = false; legalRob = false; // set both illegal for next iterations
									printf("\n\t\t[Select a card to swap]\n\t\t>"); // prompt choice
									scanf("%d", &cardChoice);
									cardChoice--; // decrement choice i.e. if player chose card 1, take card from array position 0

									while (players[currentPlayer].hand[cardChoice].isActive == 0 || cardChoice < 0 || cardChoice > HAND_SIZE - 1) // input validation on card choice //TESTING
									{
										printf("\n\t\tINAVLID ENTRY - Must choose an active card within the range 1-5 (inclusive)\n\n\t\t[Choose card to swap]\n\t\t>"); // prompt choice
										scanf("%d", &cardChoice);
										cardChoice--; // decrement choice i.e. if player chose card 1, take card from array position 0
									}

									copyCard(trump, &players[currentPlayer].hand[cardChoice]); // copy values of trump card to chosen card position
									trumpRobbed = true; // for later display message

									printf("\n\t\tPlayer %d's Hand:", currentPlayer + 1); // re-display hand
									for (k = 0; k < HAND_SIZE; k++) {
										if (players[currentPlayer].hand[k].isActive == 1) {// only show cards in the players hand
											printf("|%d:", k + 1); printCard(players[currentPlayer].hand[k]); printf("|");
										}
										else
											printf("| X |");
									}// k

								}// if - robOption = y
							}// if legalRob = true;

							// 4.3 - Card Selection
							printf("\n\t\t[Select a card to play]\n\t\t>"); // prompt choice of card to play
							scanf("%d", &cardChoice);
							cardChoice--; // decrement choice i.e. if player chose card 1, take card from array position 0
							legalRenege = false; // initialize legalRenege

							while (players[currentPlayer].hand[cardChoice].isActive == 0 || cardChoice < 0 || cardChoice > HAND_SIZE - 1) // input validation for choice
							{
								printf("\n\t\tINAVLID ENTRY - Must choose an active card within the range 1-5 (inclusive)\n\n\t\t[Select a card to play]\n\t\t>"); // prompt choice
								scanf("%d", &cardChoice);
								cardChoice--; // decrement choice i.e. if player chose card 1, take card from array position 0
							}


							// 4.4 Legal Renege Validation
							if (j == 0) { // if no lead card played yet
								break; // any card played is valid for lead player
							}
							else if (trump.suit == lead.suit) { // if trump is led
								if (players[currentPlayer].hand[cardChoice].suit == trump.suit) { // played card suit must match trump suit (with exceptions)
									legalRenege = true;
								}
							}
							else { // non-trump led
								if (players[currentPlayer].hand[cardChoice].suit == trump.suit || players[currentPlayer].hand[cardChoice].suit == lead.suit) { // played card suit must match trump suit or lead suit (with exceptions)
									legalRenege = true;
								}
							}

							// renege exceptions
							for (k = 0; k < HAND_SIZE; k++) { // cycle through active cards in players hand 
								if (players[currentPlayer].hand[k].isActive == 1) {
									if (players[currentPlayer].hand[k].id == 13 && trickWinRank < 25) // Ace of Hearts
										legalRenege = true;
									else if (players[currentPlayer].hand[k].rank == 5 && players[currentPlayer].hand[k].suit == trump.suit) // 5 of Trump suit
										legalRenege = true;
									else if (players[currentPlayer].hand[k].rank == 11 && players[currentPlayer].hand[k].suit == trump.suit && trickWinRank < 26) // Jack of Trump suit
										legalRenege = true;
								}// if - active card
							}// k 

							illegalCount = 0;
							for (k = 0; k < HAND_SIZE; k++) { // cycle through cards in hand to check if any are of trump or lead suit
								if (players[currentPlayer].hand[k].isActive == 1) {
									if (players[currentPlayer].hand[k].suit == trump.suit || players[currentPlayer].hand[k].suit == lead.suit)
										illegalCount++; // if player has card of trump or lead suit, excluding 5, J or AH they must play it
								}
							}

							if (illegalCount == 0) // if no cards of lead or trump suit in hand, renege is legal
								legalRenege = true;

							if (legalRenege == false)
								printf("\n\t\tINVALID ENTRY - Illegal Renege\n"); // error message for illegal renege
							//legalDealerRob = false; legalRob = false; // set both illegal for next player
						} while (legalRenege == false);

						// once a valid card has been selected, move it from hand to playedCards array
						moveCard(&players[currentPlayer].hand[cardChoice], &playedCards[j]);

						if (j == 0) { // first play of the trick
							copyCard(playedCards[j], &lead); // copy played card values to lead
						}


						// 4.5 - Check Card Ranking
						playedCardRank = calculateRanking(playedCards[j], trump.suit, lead.suit); // get the ranking of played card
						if (playedCardRank > trickWinRank) { // if played card beats previous best card
							trickWinRank = playedCardRank; // played card becomes new best
							trickWinPlayer = currentPlayer;
							trickWinCard = cardChoice;
						}

						currentPlayer++;
					}// j

					// declare trick winner, show player and card
					printf("\n\tPlayer %d wins with trick with the", trickWinPlayer + 1); printCard(players[trickWinPlayer].hand[trickWinCard]);
					printf("\n\t________________________________________________________________________");


					// 4.6 - Award Points to Winner
					players[trickWinPlayer].score += 5;
					if (players[trickWinPlayer].score > highScore)
						highScore = players[trickWinPlayer].score;

					for (j = 0; j < 6; j++) {// clear played cards array at the end of play cycle
						playedCards[j].isActive = 0;
					}

					if (highScore >= WINNING_SCORE) // if any player has reached 25
						break;

					do
					{
						printf("\n\n\t[Game Menu]\n\t 1.Next Trick \n\t 2.Save \n\t 3.Output Status \n\t 4.Exit to Main Menu");
						printf("\n\t>");
						scanf("%d", &inGameOption);

						switch (inGameOption)
						{
						case 2:
							printf("\n\t\tsaving game...");
							saveGame(deck, trump, players, numOfPlayers, trickWinPlayer, highScore, trumpRobbed, i, trickCount);
							break;
						case 3:
							printf("\n\t\t----Score Table----");
							for (j = 0; j < numOfPlayers; j++) {
								printf("\n\t\tPlayer %d's Score: %d", players[j].number, players[j].score);
							}
							break;
						case 4:
							printf("\n\t\texiting game - any unsaved progress is now lost");
							break;
						default:
							break;
						}
					} while (inGameOption == 3 || inGameOption == 2); // re-display menu if option 3 was selected

					i++; trickCount++;
				} while (i < HAND_SIZE && inGameOption == 1); // while players have cards in hand, and selected the "play next trick" option

				isLoadedGame = false;

			} while (highScore < WINNING_SCORE && inGameOption == 1); // while the current highest score is < 25, & the "play next trick" option was selected



			fclose(fptr);
		}// if - load game
		else if (mainMenuOption == 3) {
			printf("\n\t25 - How to Play\n\n\tGameplay Tutorial");
			printf("\n\t 1.[Enter Number of Players]\n\t >Enter the number of players - this game supports 2-6 players");
			printf("\n\t 2.[Select a card to play]\n\t >Enter the number corresponding to the card you want to play\n\t -The first card on the left is card 1, then 2 and so on...\n\t -You cannot play a card marked 'X' as this card has already been played");
			printf("\n\t 3.[Rob?(y/n)]\n\t >Enter the letter 'y' or 'n' to indicated whether you would like to rob or not\n\t -You will then be promted to pick a card from your hand to scrap in exchange for the trump");
			printf("\n\t 4.INVALID ENTRY - Illegal Renege\n\t -This error message indicates you have attemped to illegally renege\n\t -In 25 you must always follow the suit of the trump or lead\n\t -Exceptions to this rule can be seen in the below section");
			printf("\n\n\tRules of 25");
			printf("\n\t 1.25 is played with a standard 52-card pack, in each deal one suit will be chosen as trumps\n\t -The rank of cards is different depending on the colour of the suit and whether it is trumps\n\t -For a detailed list of card rankings follow the link below\n\t https://www.pagat.com/spoil5/25.html");
			printf("\n\t 2.Play\n\t -The highest card of the suit led wins unless a trump is played, in which case the highest trump wins\n\t -The winner of each trick leads to the next");
			printf("\n\t 3.Robbing\n\t -If you hold the ace of trumps you may 'rob' the trump\n\t -If the dealer turns up the ace, the dealer can rob it\n\t -In this verion of the game, the dealer is the last player to play the first round"
				"\n\t -It is not obligatory for the holder of the ace of trumps to rob\n\t -If you do not rob before you play a card, you loose the privilege");
			printf("\n\t 4.Reneging\n\t -When a card of a non-trump suit is led, you must follow suit or trump\n\t -When a trump is led, you must play trumps if possible\n\t -If you hold the 5T, JT or AH you may renege (so long as a you hold a higher trump than any played that trick)"
				"\n\t -If you hold not lead suit or trump suit cards, you may renege");
		}
		else if (mainMenuOption == 4) {
			printf("\nThanks for playing!");
			break;
		}
		else {
			printf("\nINVALID OPTION - TRY AGAIN");
		}
	} while (mainMenuOption != 4);
}