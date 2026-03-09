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

char human, first, second, grid[9], winner;
int mode, scoreX, scoreO, draws, winCells[3] = {-1, -1, -1};

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
    // ask to change initial configuration
    string input;
    if (mode) {
        cout << "Change settings? [y/N] ";
        getline(cin, input);
        cls;
        if (!(input == "y" || input == "Y")) return;
    }

    // configure settings
    cout << "Choose your character. [" << red << 'X' << reset << '/' << blue << 'O' << reset << "] ";
    getline(cin, input);
    if (input == "o" || input == "O") human = 'O';
    else if (input == "x" || input == "X") human = 'X';
    else {
        cout << yellow << "Invalid input, try again." << reset;
        wait(1000);
        mode = 0;
        cls;
        setup();
        return;
    }

    cout << "Start first? [Y/n] ";
    getline(cin, input);
    first = (input == "n" || input == "N") ? (human == 'X' ? 'O' : 'X') : human;
    second = (first == 'X' ? 'O' : 'X');

    cout << "Play against AI? [Y/n] ";
    getline(cin, input);
    mode = ((input == "n" || input == "N") ? 2 : 1);

    cls;
}

void printState() {
    // draw grid
    cout << "\n " << string(13, '-') << '\n';
    for (int i = 0; i < 9; i++) {
        // highlight winning cells in yellow, x in red, o in blue
        bool isWinCell = (i == winCells[0] || i == winCells[1] || i == winCells[2]);
        grid[i] ? cout << " | " << (isWinCell ? yellow : (grid[i] == 'X' ? red : blue)) << grid[i] << reset : cout << " | " << (i + 1);
        if (i % 3 == 2) cout << " |\n " << string(13, '-') << '\n';
    }

    // print scores
    cout << "\n " << red << "X: " << scoreX << reset << "  |  " << purple << "Draws: " << draws << reset << "  |  " << blue << "O: " << scoreO << reset << "\n\n";
}

bool checkState() {
    // check rows
    for (int i = 0; i < 7; i += 3) {
        if (grid[i] && grid[i] == grid[i + 1] && grid[i] == grid[i + 2]) {
            winner = grid[i];
            winCells[0] = i; winCells[1] = i + 1; winCells[2] = i + 2;
            return true;
        }
    }

    // check columns
    for (int i = 0; i < 3; i++) {
        if (grid[i] && grid[i] == grid[i + 3] && grid[i] == grid[i + 6]) {
            winner = grid[i];
            winCells[0] = i; winCells[1] = i + 3; winCells[2] = i + 6;
            return true;
        }
    }

    // check diagonals
    if (grid[4]) {
        if (grid[4] == grid[0] && grid[4] == grid[8]) {
            winner = grid[4];
            winCells[0] = 0; winCells[1] = 4; winCells[2] = 8;
            return true;
        }
        if (grid[4] == grid[2] && grid[4] == grid[6]) {
            winner = grid[4];
            winCells[0] = 2; winCells[1] = 4; winCells[2] = 6;
            return true;
        }
    }

    // otherwise reset win state and check for draw
    winner = 0;
    for (int &x : winCells) x = -1;
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
    cin.ignore(1e9, '\n');
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
            if (checkState() && winner) return;
            grid[i] = 0;
        }
    }

    // 2. block human if they can win
    for (int i = 0; i < 9; i++) {
        if (!grid[i]) {
            grid[i] = human;
            if (checkState() && winner) {
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

bool gameOver() {
    // update scores
    if (winner == 'X') scoreX++;
    else if (winner == 'O') scoreO++;
    else draws++;

    // show result
    printState();
    winner ? cout << (winner == 'X' ? red : blue) << winner << reset << " won the game. " : cout << "It's a " << purple << "draw" << reset << ". ";
    wait(1000);

    // prompt to play again
    cout << "Play again? [Y/n] ";
    string input;
    getline(cin, input);

    // reset for the next round
    cls;
    for (char &x : grid) x = 0;
    return (input == "n" || input == "N");
}

void loop() {
    // main game loop
    do {
        setup();

        for (int i = 0; !checkState(); i++) {
            printState();

            // determine current player
            char player = (i % 2 == 0 ? first : second);

            // determine who plays (ai or human)
            if (mode == 1 && player != human) aiMove(player);
            else if (!humanMove(player)) i--;

            cls;
        }
    } while (!gameOver());
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
