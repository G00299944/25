#pragma once
// Constants
#define DECK_SIZE 52
#define WINNING_SCORE 25
#define HAND_SIZE 5


// Structs
typedef struct {
	int id; // unique id within the deck (0-51)
	int isActive; // active status (0 || 1)
	int rank; // rank of the card (1-13) where 1 is Ace, 11-J, 12-Q, 13-K
	int suit; // suit 0-Spades, 1-Hearts, 2-Clubs, 3-Diamonds
}card_t;

typedef struct {
	int number; // number as aposed to array position, (player at array position 0 will get number 1)
	int score;
	card_t hand[HAND_SIZE]; // struct array of cards in the players hand
}player_t;


// Functions
int calculateRank(int cardID) { // takes the id of a card and returns it's rank
	int cardRank;
	cardID++;

	if (cardID < 14) { // if id is less than 14, then it's rank is the same as it's id
		cardRank = cardID;
	}
	else {
		while (cardID > 13) { // keep subtracting 13 til the id is less than 13, then it's rank is equal to it's id
			cardID -= 13;
		}
		cardRank = cardID;
	}

	return cardRank;
};

int calculateSuit(int cardID) { // takes the id of a card and returns it's suit
	int suitCount = 0;

	while (cardID >= 13) { // keep subtracting 13 til the card id is less than 13
		cardID -= 13;
		suitCount++; // increment counter every time you subtract
	}

	return suitCount; // suit is the number of times you had to subtract to get an id < 13
};

char getSuitChar(int suitIndex) { // takes a suit(int) and returns a suit(char)
	char suit;

	switch (suitIndex)
	{
	case 0:
		suit = 'S';
		break;
	case 1:
		suit = 'H';
		break;
	case 2:
		suit = 'C';
		break;
	case 3:
		suit = 'D';
		break;
	default:
		suit = '?';
		break;
	}

	return suit;
};

void moveCard(card_t *srcCard, card_t *dstCard) { // takes two card pointers, source and destination, and moves source card to destination
	// copy id, rank and suit 
	dstCard->id = srcCard->id;
	dstCard->rank = srcCard->rank;
	dstCard->suit = srcCard->suit;

	// 'move' card by de-activating it at source, and activating it at destination
	dstCard->isActive = 1;
	srcCard->isActive = 0;
};

void copyCard(card_t srcCard, card_t *dstCard) { // simpler form of moveCard function where active status does not matter
	dstCard->id = srcCard.id;
	dstCard->rank = srcCard.rank;
	dstCard->suit = srcCard.suit;
};

void printCard(card_t card) { // takes a card and prints a string display of that card for user
	char cardString[4];

	switch (card.rank) // for face cards [0] is a letter
	{
	case 1:
		cardString[0] = 'A';
		break;
	case 10: // special case, 10's need 4 characters to display properly eg: '1' '0' 'D'  '\n'
		cardString[0] = '1';
		cardString[1] = '0';
		break;
	case 11:
		cardString[0] = 'J';
		break;
	case 12:
		cardString[0] = 'Q';
		break;
	case 13:
		cardString[0] = 'K';
		break;
	default:
		cardString[0] = card.rank + '0'; // all other cards, the rank is the first character
		break;
	}

	if (card.rank == 10) {
		cardString[2] = getSuitChar(calculateSuit(card.id)); // apply suit character to string
		cardString[3] = '\0';
	}
	else {
		cardString[1] = getSuitChar(calculateSuit(card.id)); // apply suit character to string
		cardString[2] = '\0';
	}

	printf(" %s ", cardString);
};

int calculateRanking(card_t playedCard, int trumpSuit, int leadSuit) { // takes card, trump suit(int) & lead suit(int) and returns and absolute ranking of that card
	// given the trump and lead suit, a played card can be given an absolute ranking for that trick
	// I have included a spreadsheet with my documentation that can show this visually
	int id = playedCard.id;
	int rank = playedCard.rank;
	int suit = playedCard.suit;
	int ranking = 0;

	if (id == 13) { // AH
		ranking = 25;
	}
	else if (suit == trumpSuit) {
		if (rank == 5) // 5T
			ranking = 27;
		else if (rank == 11) // JT
			ranking = 26;
		else if (rank == 1) // AT
			ranking = 24;
		else if (rank == 13) // KT
			ranking = 23;
		else if (rank == 12) // QT
			ranking = 22;
		else {
			if (suit == 1 || suit == 3) { // red suits (Hearts & Diamonds)
				if (rank > 5)
					ranking = rank + 11;
				else
					ranking = rank + 12;
			}
			else if (suit == 0 || suit == 2) { // black suits (Spades & Clubs)
				if (rank < 5)
					ranking = (rank - 23)* -1;
				else
					ranking = (rank - 24)* -1;

			}
		}
	}// if - trumpsuit
	else if (suit == leadSuit) {
		if (rank == 13) // KL
			ranking = 13;
		else if (rank == 12) // QL
			ranking = 12;
		else if (rank == 11) // JL
			ranking = 11;
		else {
			if (suit == 1 || suit == 3) { // red suits (Hearts & Diamonds)
				ranking = rank;
			}
			else if (suit == 0 || suit == 2) { // black suits (Spades & Clubs)
				ranking = 11 - rank;
			}
		}
	}

	return ranking;
};

// takes all essential data and writes it to a file "savegame.txt" for later use
void saveGame(card_t deck[HAND_SIZE], card_t trump, player_t players[6], int numOfPlayers, int trickWinPlayer, int highScore, bool trumpRobbed, int i, int trickCount) {
	int x, y, z;
	int tempTrumpRobbed;
	FILE* fptr;
	fptr = fopen("savegame.txt", "w");


	if (trumpRobbed == true) // didn't know how to write a bool to file, so used a temporary integer to store it
		tempTrumpRobbed = 1;
	else
		tempTrumpRobbed = 0;

	for (x = 0; x < DECK_SIZE; x++) {
		fprintf(fptr, "%d %d %d %d ", deck[x].id, deck[x].rank, deck[x].suit, deck[x].isActive);
	}

	fprintf(fptr, "%d %d %d %d ", trump.id, trump.rank, trump.suit, trump.isActive);

	for (x = 0; x < 6; x++) {
		fprintf(fptr, "%d %d ", players[x].number, players[x].score);
		for (y = 0; y < HAND_SIZE; y++) {		
			fprintf(fptr, "%d %d %d %d ", players[x].hand[y].id, players[x].hand[y].rank, players[x].hand[y].suit, players[x].hand[y].isActive);	
		}
	}

	fprintf(fptr, "%d %d %d %d %d %d ", numOfPlayers, trickWinPlayer, highScore, tempTrumpRobbed, i, trickCount);


	fclose(fptr);
};
