#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <stdexcept>

namespace GameEngine {

    enum class SlotState { EMPTY, PLAYER_X, PLAYER_O };

    struct Player {
        std::string name;
        SlotState symbol;
        int score = 0;
    };

    class Board {
    private:
        std::vector<std::vector<SlotState>> grid;
        const int size = 3;

    public:
        Board() : grid(size, std::vector<SlotState>(size, SlotState::EMPTY)) {}

        void display() const {
            std::cout << "\n    1   2   3\n";
            for (int i = 0; i < size; ++i) {
                std::cout << i + 1 << " ";
                for (int j = 0; j < size; ++j) {
                    char symbol = ' ';
                    if (grid[i][j] == SlotState::PLAYER_X) symbol = 'X';
                    else if (grid[i][j] == SlotState::PLAYER_O) symbol = 'O';

                    std::cout << "[ " << symbol << " ]";
                }
                std::cout << "\n";
            }
            std::cout << std::endl;
        }

        bool update(int r, int c, SlotState player) {
            if (r < 0 || r >= size || c < 0 || c >= size || grid[r][c] != SlotState::EMPTY) {
                return false;
            }
            grid[r][c] = player;
            return true;
        }

        bool isFull() const {
            for (const auto& row : grid)
                for (auto slot : row)
                    if (slot == SlotState::EMPTY) return false;
            return true;
        }

        SlotState checkWinner() const {
            // Rows and Columns
            for (int i = 0; i < size; ++i) {
                if (grid[i][0] != SlotState::EMPTY && grid[i][0] == grid[i][1] && grid[i][1] == grid[i][2])
                    return grid[i][0];
                if (grid[0][i] != SlotState::EMPTY && grid[0][i] == grid[1][i] && grid[1][i] == grid[2][i])
                    return grid[0][i];
            }
            // Diagonals
            if (grid[0][0] != SlotState::EMPTY && grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2])
                return grid[0][0];
            if (grid[0][2] != SlotState::EMPTY && grid[0][2] == grid[1][1] && grid[1][1] == grid[2][0])
                return grid[0][2];

            return SlotState::EMPTY;
        }

        void reset() {
            for (auto& row : grid) std::fill(row.begin(), row.end(), SlotState::EMPTY);
        }
    };

    class GameController {
    private:
        Board board;
        Player p1, p2;
        bool isP1Turn;

        void getValidMove(int& r, int& c) {
            while (true) {
                std::cout << (isP1Turn ? p1.name : p2.name) << " (" 
                          << (isP1Turn ? 'X' : 'O') << "), enter row and column (1-3): ";
                if (!(std::cin >> r >> c)) {
                    std::cout << "Invalid input. Please enter numbers.\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                } else if (!board.update(r - 1, c - 1, isP1Turn ? p1.symbol : p2.symbol)) {
                    std::cout << "Invalid move! Spot taken or out of bounds.\n";
                } else {
                    break;
                }
            }
        }

    public:
        GameController() : isP1Turn(true) {
            std::cout << "--- Advanced Tic-Tac-Toe ---\n";
            std::cout << "Enter Player 1 Name: "; std::getline(std::cin, p1.name);
            p1.symbol = SlotState::PLAYER_X;
            std::cout << "Enter Player 2 Name: "; std::getline(std::cin, p2.name);
            p2.symbol = SlotState::PLAYER_O;
        }

        void run() {
            bool keepPlaying = true;
            while (keepPlaying) {
                board.reset();
                bool gameOver = false;
                
                while (!gameOver) {
                    board.display();
                    int r, c;
                    getValidMove(r, c);

                    SlotState winner = board.checkWinner();
                    if (winner != SlotState::EMPTY) {
                        board.display();
                        std::cout << "CONGRATULATIONS! " << (isP1Turn ? p1.name : p2.name) << " wins!\n";
                        isP1Turn ? p1.score++ : p2.score++;
                        gameOver = true;
                    } else if (board.isFull()) {
                        board.display();
                        std::cout << "IT'S A DRAW!\n";
                        gameOver = true;
                    }
                    isP1Turn = !isP1Turn;
                }

                std::cout << "\nScoreboard: " << p1.name << ": " << p1.score 
                          << " | " << p2.name << ": " << p2.score << "\n";
                
                char choice;
                std::cout << "Play another round? (y/n): ";
                std::cin >> choice;
                if (choice != 'y' && choice != 'Y') keepPlaying = false;
            }
            std::cout << "Thanks for playing!\n";
        }
    };
}

int main() {
    try {
        GameEngine::GameController game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "A critical error occurred: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}