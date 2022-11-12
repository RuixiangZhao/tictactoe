#include <iostream>

using namespace std;

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27015
#define DEFAULT_IP "127.0.0.1"

const char client = 'X';
const char server = 'O';

enum Player { CLIENT, SERVER };

struct Move {
	int x;
	int y;
};

class Game {
	char board[3][3];

public:
	Game();

	void printBoard();
	// Prints the board pretty-ly

	bool gameOver();
	// Returns true if a winner has been found or there are no empty spaces

	bool checkWin(Player player);
	// Checks for a win

	int play_network_server();
	// Primary game driver(network version), two people play, client first default

    Move getServerMove();
	// Takes in values from the input stream and places them on the board
	// if valid.  Expects input in coordinate notation, ex (1,3)
};