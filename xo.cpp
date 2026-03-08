// ### XO game written in C++ ###

// ---------- libraries --------------------------------------------------------

#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

// ---------- macros -----------------------------------------------------------

#define wait(ms) this_thread::sleep_for(chrono::milliseconds(ms))
#define dots for (int i = 0; i < 3; i++, wait(500)) cout << '.'
#define cls cout << "\e[2J\e[H"
#define red "\e[91m"
#define blue "\e[96m"
#define purple "\e[95m"
#define yellow "\e[93m"
#define reset "\e[m"

// ---------- globals ----------------------------------------------------------

int players, scoreX = 0, scoreO = 0, draws = 0, winLine[3] = {-1, -1, -1};
bool startFirst;
char grid[9], winner;

// ---------- functions --------------------------------------------------------

void init() {
    // clear screen, seed rng, and disable output buffering
    cls;
    srand(chrono::steady_clock::now().time_since_epoch().count());
    cout << unitbuf;
}

void intro() {
    // play startup sequence
    cout << "Loading";
    dots;
    cout << "\nWelcome to terminal XO.";
    wait(1000);
    cls;
}

void setup() {
    // prompt to change settings after initial setup
    string input;
    if (players) {
        cout << "Change settings? [y/N] ";
        getline(cin, input);
        if (!(input == "y" || input == "Y")) return;
    }
    
    // configure game mode and turn order
    cout << "Play against AI? [Y/n] ";
    getline(cin, input);
    players = ((input == "n" || input == "N") ? 2 : 1);
    if (players == 1) {
        cout << "Start first? [Y/n] ";
        getline(cin, input);
        startFirst = !(input == "n" || input == "N");
    }
}

void printState() {
    // draw grid
    cout << "\n " << string(13, '-') << '\n';
    for (int i = 0; i < 9; i++) {
        // highlight winning cells in yellow, x in red, o in blue
        bool isWinCell = (i == winLine[0] || i == winLine[1] || i == winLine[2]);
        grid[i] ? cout << " | " << (isWinCell ? yellow : (grid[i] == 'X' ? red : blue)) << grid[i] << reset : cout << " | " << (i + 1);
        
        // draw horizontal dividers
        if (i % 3 == 2) cout << " |\n " << string(13, '-') << '\n';
    }

    // print scores
    cout << "\n " << red << "X: " << scoreX << reset << "  |  " << purple << "Draws: " << draws << reset << "  |  " << blue << "O: " << scoreO << reset << "\n\n";
}

bool checkGameOver() {
    // check rows
    for (int i = 0; i < 7; i += 3) {
        if (grid[i] && grid[i] == grid[i + 1] && grid[i] == grid[i + 2]) {
            winner = grid[i];
            winLine[0] = i; winLine[1] = i + 1; winLine[2] = i + 2;
            return true;
        }
    }
    
    // check columns
    for (int i = 0; i < 3; i++) {
        if (grid[i] && grid[i] == grid[i + 3] && grid[i] == grid[i + 6]) {
            winner = grid[i];
            winLine[0] = i; winLine[1] = i + 3; winLine[2] = i + 6;
            return true;
        }
    }
    
    // check diagonals
    if (grid[4]) {
        if (grid[4] == grid[0] && grid[4] == grid[8]) {
            winner = grid[4];
            winLine[0] = 0; winLine[1] = 4; winLine[2] = 8;
            return true;
        }
        if (grid[4] == grid[2] && grid[4] == grid[6]) {
            winner = grid[4];
            winLine[0] = 2; winLine[1] = 4; winLine[2] = 6;
            return true;
        }
    }

    // otherwise reset win variables and check for draw
    winner = 0;
    for (int &x : winLine) x = -1;
    for (char x : grid) if (!x) return false;
    return true;
}

bool humanMove(char player) {
    // print player's turn
    cout << (player == 'X' ? red : blue) << player << reset << "'s turn. [1-9] ";

    // read and validate input
    int position;
    bool valid = (cin >> position) && position >= 1 && position <= 9 && !grid[position - 1];
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (!valid) {
        cout << yellow << "Invalid input, try again." << reset;
        wait(1000);
        return false;
    }

    // apply move
    grid[position - 1] = player;
    return true;
}

void aiMove(char player) {
    // print ai's turn
    cout << (player == 'X' ? red : blue) << player << reset << "'s turn. [AI] ";
    dots;

    // 1. win if possible
    for (int i = 0; i < 9; i++) {
        if (!grid[i]) {
            grid[i] = player;
            if (checkGameOver() && winner) return;
            grid[i] = 0;
        }
    }
    
    // 2. check if opponent can win and block them
    char opponent = (player == 'O' ? 'X' : 'O');
    for (int i = 0; i < 9; i++) {
        if (!grid[i]) {
            grid[i] = opponent;
            if (checkGameOver() && winner) {
                grid[i] = player;
                return;
            }
            grid[i] = 0;
        }
    }
    
    // 3. pick a random empty cell as a fallback
    int num;
    do num = rand() % 9; while (grid[num]);
    grid[num] = player;
}

bool endRound() {
    // update scores
    if (winner == 'X') scoreX++;
    else if (winner == 'O') scoreO++;
    else draws++;

    // display final state and winner or draw message
    printState();
    winner ? cout << (winner == 'X' ? red : blue) << winner << reset << " won the game. " : cout << "It's a " << purple << "draw" << reset << ". ";
    wait(1000);

    // prompt user to continue playing
    cout << "Play again? [Y/n] ";
    string input;
    getline(cin, input);
    
    // reset grid for the next round
    cls;
    for (char &x : grid) x = 0;
    return (input == "n" || input == "N");
}

void loop() {
    // main game loop
    do {
        setup();
        cls;
        
        for (int i = 0; !checkGameOver(); i++) {
            printState();

            // determine current player
            char player = (i % 2 == 0 ? 'X' : 'O');

            // determine who plays (ai or human?)
            if (players == 1 && player == (startFirst ? 'O' : 'X')) aiMove(player);
            else if (!humanMove(player)) i--;

            cls;
        }
    } while (!endRound());
}

void goodbye() {
    // play shutdown sequence
    cout << "Goodbye.";
    wait(1000);
    cls;
}

// ---------- main -------------------------------------------------------------

int main() {
    init();
    intro();
    loop();
    goodbye();
}
