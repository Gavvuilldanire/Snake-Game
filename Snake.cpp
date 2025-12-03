#include <iostream>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <string>
#include <ctime> // Penting untuk random seed

using namespace std;

// --- GLOBAL VARIABLES ---
bool gameOver;
const int width = 40;
const int height = 20;
int x, y, score;
int tailX[100], tailY[100];
int nTail;
int highScore = 0;
string playerName;

// Variabel Buah Dinamis
int fruitX, fruitY;
char fruitType; // F, A, atau G
int fruitScore; // 10, 30, atau 50

enum eDirecton { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirecton dir;

// --- UTILITIES ---
void SetCursorPosition(int x, int y) {
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(output, pos);
}

void HideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

// --- LOGIC RNG BUAH ---
void GenerateFruit() {
    fruitX = rand() % width;
    fruitY = rand() % height;

    // Logika Probabilitas (0 - 99)
    int chance = rand() % 100;

    if (chance < 60) { 
        // 0 - 59 (60%)
        fruitType = 'F';
        fruitScore = 10;
    } 
    else if (chance < 90) { 
        // 60 - 89 (30%)
        fruitType = 'A';
        fruitScore = 30;
    } 
    else { 
        // 90 - 99 (10%)
        fruitType = 'G';
        fruitScore = 50;
    }
}

// --- GAME FUNCTIONS ---
void LoadHighScore() {
    ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
}

void SaveHighScore() {
    if (score > highScore) {
        ofstream file("highscore.txt");
        file << score;
        file.close();
    }
}

void ShowInfo() {
    system("cls");
    cout << "==========================================" << endl;
    cout << "          INFORMASI ITEM & POIN           " << endl;
    cout << "==========================================" << endl;
    cout << " [F] Food  : 10 Poin (Common - 60%)" << endl;
    cout << " [A] Apple : 30 Poin (Rare   - 30%)" << endl;
    cout << " [G] Gold  : 50 Poin (Epic   - 10%)" << endl;
    cout << "==========================================" << endl;
    cout << " Hindari menabrak dinding atau ekor sendiri." << endl;
    cout << " Tekan sembarang tombol untuk MULAI..." << endl;
    _getch();
}

void Setup() {
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    score = 0;
    nTail = 0;
    GenerateFruit(); // Spawn buah pertama
    LoadHighScore();
}

void Draw() {
    SetCursorPosition(0, 0);

    cout << "==========================================" << endl;
    cout << " PLAYER: " << playerName << " | SCORE: " << score << endl;
    cout << " TARGET: [" << fruitType << "] +" << fruitScore << " pts" << endl; 
    cout << "==========================================" << endl;

    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) cout << "#";

            if (i == y && j == x)
                cout << "O"; 
            else if (i == fruitY && j == fruitX)
                cout << fruitType; // Render tipe buah sesuai RNG
            else {
                bool print = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "o";
                        print = true;
                    }
                }
                if (!print) cout << " ";
            }

            if (j == width - 1) cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;
}

void Input() {
    if (_kbhit()) {
        char key = _getch();
        switch (tolower(key)) {
        case 'a': if (dir != RIGHT || nTail == 0) dir = LEFT; break;
        case 'd': if (dir != LEFT || nTail == 0) dir = RIGHT; break;
        case 'w': if (dir != DOWN || nTail == 0) dir = UP; break;
        case 's': if (dir != UP || nTail == 0) dir = DOWN; break;
        case 'x': gameOver = true; break;
        }
    }
}

void Logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;

    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir) {
    case LEFT: x--; break;
    case RIGHT: x++; break;
    case UP: y--; break;
    case DOWN: y++; break;
    default: break;
    }

    if (x >= width || x < 0 || y >= height || y < 0) gameOver = true;

    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y) gameOver = true;

    // Logika Makan
    if (x == fruitX && y == fruitY) {
        score += fruitScore; // Tambah skor dinamis
        GenerateFruit();     // Spawn buah baru dengan tipe baru
        nTail++;
    }
}

int main() {
    srand(time(0)); // Seed Random agar tidak repetitif
    HideCursor();

    cout << "Masukkan Nama Player: ";
    getline(cin, playerName);
    if (playerName.empty()) playerName = "Player";

    ShowInfo(); // Tampilkan Info Rate
    Setup();

    while (!gameOver) {
        Draw();
        Input();
        Logic();
        Sleep(150);
    }

    SaveHighScore();
    
    system("cls");
    cout << "\n=== PERMAINAN SELESAI ===" << endl;
    cout << "Player     : " << playerName << endl;
    cout << "Skor Akhir : " << score << endl;
    cout << "High Score : " << (score > highScore ? score : highScore) << endl;
    cout << "=========================" << endl;
    _getch();

    return 0;
}