// ### terminal xo ###

// ---------- libraries --------------------------------------------------------

#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

// ---------- macros -----------------------------------------------------------

#define wait(ms) this_thread::sleep_for(chrono::milliseconds(ms))
#define cls cout << "\e[2J\e[H"
#define hidecur cout << "\e[?25l"
#define showcur cout << "\e[?25h"
#define red "\e[91m"
#define blue "\e[96m"
#define purple "\e[95m"
#define yellow "\e[93m"
#define reset "\e[m"

// ---------- globals ----------------------------------------------------------

char human, first, second, grid[9], winner;
int mode, scorex, scoreo, draws, wincells[3] = {-1, -1, -1};

// ---------- functions --------------------------------------------------------

void startup() {
    // initialize program and play startup sequence
    cls;
    hidecur;
    srand(chrono::steady_clock::now().time_since_epoch().count());
    cout << unitbuf << "Loading";
    for (int i = 0; i < 3; i++, wait(500)) cout << '.';
    cout << "\nWelcome to terminal XO.";
    wait(1000);
    cls;
}

void setup() {
    string input;
    showcur;
    
    // ask to change initial configuration
    if (mode) {
        cout << "Change settings? [y/N] ";
        getline(cin, input);
        cls;
        hidecur;
        if (input != "y" && input != "Y") return;
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
    first = ((input == "n" || input == "N") ? (human == 'X' ? 'O' : 'X') : human);
    second = (first == 'X' ? 'O' : 'X');
    
    cout << "Play against AI? [Y/n] ";
    getline(cin, input);
    mode = ((input == "n" || input == "N") ? 2 : 1);
    
    cls;
    hidecur;
}

void printstate() {
    // draw grid
    cout << "\n ┌───┬───┬───┐\n";
    for (int i = 0; i < 9; i++) {
        bool iswincell = (i == wincells[0] || i == wincells[1] || i == wincells[2]);
        cout << " │ ";
        grid[i] ? cout << (iswincell ? yellow : (grid[i] == 'X' ? red : blue)) << grid[i] << reset : cout << i + 1;
        if (i == 2 || i == 5) cout << " │\n ├───┼───┼───┤\n";
    }
    cout << " │\n └───┴───┴───┘\n";
    
    // print scores
    cout << "\n " << red << "X: " << scorex << reset << "  |  " << purple << "Draws: " << draws << reset << "  |  " << blue << "O: " << scoreo << reset << "\n\n";
}

bool checkstate() {
    // check rows
    for (int i = 0; i < 7; i += 3) {
        if (grid[i] && grid[i] == grid[i + 1] && grid[i] == grid[i + 2]) {
            winner = grid[i];
            wincells[0] = i;
            wincells[1] = i + 1;
            wincells[2] = i + 2;
            return true;
        }
    }
    
    // check columns
    for (int i = 0; i < 3; i++) {
        if (grid[i] && grid[i] == grid[i + 3] && grid[i] == grid[i + 6]) {
            winner = grid[i];
            wincells[0] = i;
            wincells[1] = i + 3;
            wincells[2] = i + 6;
            return true;
        }
    }
    
    // check diagonals
    if (grid[4]) {
        if (grid[4] == grid[0] && grid[4] == grid[8]) {
            winner = grid[4];
            wincells[0] = 0;
            wincells[1] = 4;
            wincells[2] = 8;
            return true;
        }
        if (grid[4] == grid[2] && grid[4] == grid[6]) {
            winner = grid[4];
            wincells[0] = 2;
            wincells[1] = 4;
            wincells[2] = 6;
            return true;
        }
    }
    
    // otherwise reset win state and check for draw
    winner = 0;
    for (int &x : wincells) x = -1;
    for (char x : grid) if (!x) return false;
    return true;
}

void humanmove(char player) {
    // print player's turn
    cout << (player == 'X' ? red : blue) << player << reset << "'s turn. [1-9] ";
    
    // read and validate input
    int input;
    showcur;
    bool valid = (cin >> input && input >= 1 && input <= 9 && !grid[input - 1]);
    hidecur;
    cin.clear();
    cin.ignore(9e9, '\n');
    if (!valid) {
        cout << yellow << "Invalid input, try again." << reset;
        wait(1000);
        cls;
        printstate();
        humanmove(player);
        return;
    }
    grid[input - 1] = player;
}

void aimove(char player) {
    // print ai's turn
    cout << (player == 'X' ? red : blue) << player << reset << "'s turn. [AI] ";
    for (int i = 0; i < 3; i++, wait(500)) cout << '.';
    
    // 1. win if possible
    for (int i = 0; i < 9; i++) {
        if (!grid[i]) {
            grid[i] = player;
            if (checkstate() && winner) return;
            grid[i] = 0;
        }
    }
    
    // 2. block human if they can win
    for (int i = 0; i < 9; i++) {
        if (!grid[i]) {
            grid[i] = human;
            if (checkstate() && winner) {
                grid[i] = player;
                return;
            }
            grid[i] = 0;
        }
    }
    
    // 3. pick a random empty cell as a fallback
    int num;
    do num = rand() % 9;
    while (grid[num]);
    grid[num] = player;
}

bool gameover() {
    // update scores
    if (winner == 'X') scorex++;
    else if (winner == 'O') scoreo++;
    else draws++;
    
    // show result
    printstate();
    winner ? cout << (winner == 'X' ? red : blue) << winner << reset << " won the game. " : cout << "It's a " << purple << "draw" << reset << ". ";
    wait(1000);
    
    // prompt to play again
    cout << "Play again? [Y/n] ";
    string input;
    showcur;
    getline(cin, input);
    hidecur;
    
    // reset for the next round
    cls;
    for (char &x : grid) x = 0;
    return (input == "n" || input == "N");
}

void loop() {
    // main game loop
    do {
        setup();
        
        for (int i = 0; !checkstate(); i++) {
            printstate();
            
            // determine who plays
            char player = (i % 2 == 0 ? first : second);
            (mode == 1 && player != human) ? aimove(player) : humanmove(player);
            
            cls;
        }
    } while (!gameover());
}

void shutdown() {
    // play shutdown sequence
    cout << "Goodbye.";
    wait(1000);
    cls;
    showcur;
}

// ---------- main -------------------------------------------------------------

int main() {
    startup();
    loop();
    shutdown();
}
