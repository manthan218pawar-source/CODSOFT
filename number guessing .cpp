#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <chrono>   // Added for high-precision timing
#include <stdexcept> // Added for exception handling

using namespace std;

// ==========================================
// CUSTOM EXCEPTION HANDLING
// ==========================================
class GameException : public runtime_error {
public:
    GameException(const string& msg) : runtime_error(msg) {}
};

// ==========================================
// CORE DATA STRUCTURES
// ==========================================
namespace GameData {
    struct SessionResult {
        int attempts;
        double timeTaken;
        bool won;
        string difficultyStr;
    };

    struct Statistics {
        int wins = 0;
        int totalGames = 0;
        double bestTime = 99999.0;
    };
}

// ==========================================
// SYSTEM UTILITIES
// ==========================================
class Utils {
public:
    static void clear() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    static int getInt(const string& prompt, int min = -1, int max = -1) {
        int val;
        while (true) {
            cout << prompt;
            if (cin >> val && (min == -1 || (val >= min && val <= max))) {
                return val;
            }
            cout << "Invalid input. Try again." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
};

// ==========================================
// GAME ENGINE CLASS
// ==========================================
class GuessMaster {
private:
    int secret;
    int maxN;
    int limit;
    string diffName;

public:
    GuessMaster(int choice) {
        srand(time(0));
        switch (choice) {
            case 1: maxN = 20; limit = 5; diffName = "Easy"; break;
            case 2: maxN = 100; limit = 8; diffName = "Medium"; break;
            case 3: maxN = 500; limit = 12; diffName = "Hard"; break;
            default: throw GameException("Invalid Difficulty Selection.");
        }
        secret = rand() % maxN + 1;
    }

    GameData::SessionResult run() {
        auto start = chrono::high_resolution_clock::now();
        int guess;
        int count = 0;
        bool victory = false;

        Utils::clear();
        cout << "--- MODE: " << diffName << " (1 to " << maxN << ") ---" << endl;

        while (count < limit) {
            cout << "\nAttempt " << (count + 1) << "/" << limit << endl;
            guess = Utils::getInt("Your Guess: ", 1, maxN);
            count++;

            if (guess == secret) {
                victory = true;
                break;
            }
            
            if (guess < secret) cout << "Higher! ^" << endl;
            else cout << "Lower! v" << endl;

            // Mathematical Hint Logic
            if (count == limit - 2) {
                cout << "[PRO-HINT] The number is " << (secret % 2 == 0 ? "Even" : "Odd") << "." << endl;
            }
        }

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;

        if (victory) cout << "\n*** WINNER! ***" << endl;
        else cout << "\n*** DEFEAT! The number was " << secret << " ***" << endl;

        return {count, elapsed.count(), victory, diffName};
    }
};

// ==========================================
// PROFILE & PERSISTENCE MANAGER
// ==========================================
class Profile {
private:
    string username;
    GameData::Statistics stats;
    vector<GameData::SessionResult> history;

public:
    Profile(string name) : username(name) {}

    void addResult(GameData::SessionResult r) {
        stats.totalGames++;
        if (r.won) {
            stats.wins++;
            if (r.timeTaken < stats.bestTime) stats.bestTime = r.timeTaken;
        }
        history.push_back(r);
        saveToFile();
    }

    void saveToFile() {
        ofstream f(username + "_profile.txt");
        f << "PLAYER: " << username << "\n";
        f << "WINS: " << stats.wins << "/" << stats.totalGames << "\n";
        f << "BEST_TIME: " << fixed << setprecision(2) << stats.bestTime << "s\n";
        f << "--------------------\n";
        for (auto const& h : history) {
            f << (h.won ? "[W] " : "[L] ") << h.difficultyStr << " | " << h.attempts << " tries | " << h.timeTaken << "s\n";
        }
        f.close();
    }

    void showDashboard() {
        Utils::clear();
        cout << "===== PLAYER DASHBOARD: " << username << " =====" << endl;
        cout << "Win Rate: " << (stats.totalGames > 0 ? (stats.wins * 100 / stats.totalGames) : 0) << "%" << endl;
        cout << "Fastest Win: " << (stats.bestTime > 9000 ? 0 : stats.bestTime) << " seconds" << endl;
        cout << "Recent Games: " << history.size() << endl;
        cout << "========================================" << endl;
        cout << "Press Enter to return to menu...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
};

// ==========================================
// MAIN CONTROLLER
// ==========================================
int main() {
    try {
        string name;
        cout << "Enter Player Name: ";
        getline(cin, name);
        Profile player(name);

        bool exitApp = false;
        while (!exitApp) {
            Utils::clear();
            cout << "1. Play Game\n2. View Stats\n3. Exit\nSelection: ";
            int choice = Utils::getInt("", 1, 3);

            if (choice == 1) {
                cout << "\nDifficulty:\n1. Easy\n2. Medium\n3. Hard\nChoice: ";
                int d = Utils::getInt("", 1, 3);
                
                GuessMaster game(d);
                GameData::SessionResult res = game.run();
                player.addResult(res);
                
                cout << "Time: " << fixed << setprecision(2) << res.timeTaken << "s" << endl;
                cout << "Press Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
            } 
            else if (choice == 2) {
                player.showDashboard();
            } 
            else {
                exitApp = true;
            }
        }
    } catch (const exception& e) {
        cerr << "CRITICAL ERROR: " << e.what() << endl;
        return 1;
    }

    cout << "System shutting down... Session saved." << endl;
    return 0;
}