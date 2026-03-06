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
#define red "\e[31m"
#define blue "\e[34m"
#define purple "\e[35m"
#define yellow "\e[33m"
#define reset "\e[m"

// ---------- globals ----------------------------------------------------------

char grid[9], winner;
int winLine[3] = {-1, -1, -1};
bool solo, startFirst;

// ---------- functions --------------------------------------------------------

void init() {
    // clear screen, seed rng, and disable output buffering
    cls;
    srand(chrono::steady_clock::now().time_since_epoch().count());
    cout << unitbuf;
}

void intro() {
    // play startup sequence
    cout << "Starting";
    dots;
    cout << "\nWelcome to terminal XO.";
    wait(1000);
    cls;
}

void setup() {
    // configure game mode and turn order
    solo = true; startFirst = true;
    string input;

    cout << "Play solo? [Y/n] ";
    getline(cin, input);
    if (input == "n" || input == "N") {
        solo = false;
    } else {
        cout << "Start first? [Y/n] ";
        getline(cin, input);
        if (input == "n" || input == "N") startFirst = false;
    }

    cls;
}

void printGrid() {
    cout << "\n " << string(13, '-') << '\n';
    for (int i = 0; i < 9; i++) {
        // highlight winning cells in yellow, x in red, o in blue
        bool isWinCell = (i == winLine[0] || i == winLine[1] || i == winLine[2]);
        grid[i] ? cout << " | " << (isWinCell ? yellow : (grid[i] == 'X' ? red : blue)) << grid[i] << reset : cout << " | " << (i + 1);

        // draw horizontal dividers
        if (i % 3 == 2) cout << " |\n " << string(13, '-') << '\n';
    }
    cout << '\n';
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
    for (int &i : winLine) i = -1;
    for (int i = 0; i < 9; i++) if (!grid[i]) return false;
    return true;
}

bool humanMove(char player) {
    // read and validate input
    int position;
    if (!(cin >> position) || position > 9 || position < 1 || grid[position - 1]) {
        cout << yellow << "Invalid input, try again." << reset;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        wait(1000);
        return false;
    }

    // apply move and clear input buffer
    grid[position - 1] = player;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
}

void aiMove(char ai) {
    // determine human's character
    char human = (ai == 'O' ? 'X' : 'O');

    // 1. check if ai can win on this turn
    for (int i = 0; i < 9; i++) {
        if (!grid[i]) {
            grid[i] = ai;
            if (checkGameOver() && winner) {
                dots;
                cout << ' ' << i + 1;
                wait(500);
                return;
            }
            grid[i] = 0;
        }
    }

    // 2. check if human can win on next turn and block them
    for (int i = 0; i < 9; i++) {
        if (!grid[i]) {
            grid[i] = human;
            if (checkGameOver() && winner) {
                grid[i] = ai;
                dots;
                cout << ' ' << i + 1;
                wait(500);
                return;
            }
            grid[i] = 0;
        }
    }

    // 3. pick a random empty cell as a fallback
    int num;
    do num = rand() % 9; while (grid[num]);
    grid[num] = ai;
    dots;
    cout << ' ' << num + 1;
    wait(500);
}

bool gameEnd() {
    // display winner or draw message at the end of a round
    printGrid();
    winner ? cout << (winner == 'X' ? red : blue) << winner << reset " won the game. " : cout << "It's a " purple "draw" reset ". ";
    wait(1000);

    // prompt user to continue playing
    string input;
    cout << "Play again? [Y/n] ";
    getline(cin, input);
    cls;
    return (input == "n" || input == "N");
}

void resetGame() {
    // clear grid for the next round
    for (char &c : grid) c = 0;
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

    do {
        setup();

        // main game loop
        for (int i = 0; !checkGameOver(); i++) {
            printGrid();

            // determine current player
            char player = (i % 2 == 0 ? 'X' : 'O');
            cout << (player == 'X' ? red : blue) << player << reset;

            // determine who plays (ai or human?)
            if (solo && player == (startFirst ? 'O' : 'X')) {
                cout << "'s turn. [AI] ";
                aiMove(player);
            } else {
                cout << "'s turn. [1-9] ";
                if (!humanMove(player)) i--;
            }

            cls;
        }

        if (gameEnd()) break;
        resetGame();
    } while (true);

    goodbye();
}
