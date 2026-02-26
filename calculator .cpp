#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>
#include <fstream>
#include <iomanip>
#include <cmath>

// ==========================================
// 1. CUSTOM EXCEPTION CLASS
// ==========================================
class CalculatorException : public std::runtime_error {
public:
    CalculatorException(const std::string& message) : std::runtime_error(message) {}
};

// ==========================================
// 2. LOGGING SYSTEM
// ==========================================
class Logger {
public:
    static void log(const std::string& message) {
        std::ofstream logFile("calc_history.log", std::ios::app);
        if (logFile.is_open()) {
            logFile << "[LOG]: " << message << std::endl;
            logFile.close();
        }
    }
};

// ==========================================
// 3. MATHEMATICAL ENGINE (TEMPLATE BASED)
// ==========================================
template <typename T>
class MathEngine {
public:
    T add(T a, T b) { return a + b; }
    T subtract(T a, T b) { return a - b; }
    T multiply(T a, T b) { return a * b; }
    T divide(T a, T b) {
        if (b == 0) {
            throw CalculatorException("Division by Zero Error!");
        }
        return a / b;
    }
    T power(T base, T exp) { return std::pow(base, exp); }
};

// ==========================================
// 4. USER INTERFACE & INPUT VALIDATION
// ==========================================
class CalculatorUI {
private:
    MathEngine<double> engine;

    void displayHeader() {
        std::cout << "========================================" << std::endl;
        std::cout << "      EXTENDED C++ CALCULATOR PRO       " << std::endl;
        std::cout << "========================================" << std::endl;
    }

    void displayMenu() {
        std::cout << "\nSelect an Operation:" << std::endl;
        std::cout << "1. Addition (+)" << std::endl;
        std::cout << "2. Subtraction (-)" << std::endl;
        std::cout << "3. Multiplication (*)" << std::endl;
        std::cout << "4. Division (/)" << std::endl;
        std::cout << "5. Power (^)" << std::endl;
        std::cout << "6. View Log History" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Choice: ";
    }

    double getValidatedNumber(const std::string& prompt) {
        double num;
        while (true) {
            std::cout << prompt;
            if (std::cin >> num) {
                return num;
            } else {
                std::cout << "Invalid Input! Please enter a numeric value." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

public:
    void run() {
        displayHeader();
        int choice;
        bool running = true;

        while (running) {
            displayMenu();
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (choice == 0) {
                running = false;
                break;
            }

            if (choice == 6) {
                showLogs();
                continue;
            }

            try {
                double n1 = getValidatedNumber("Enter first number: ");
                double n2 = getValidatedNumber("Enter second number: ");
                double result = 0;
                std::string opLabel = "";

                switch (choice) {
                    case 1: 
                        result = engine.add(n1, n2); 
                        opLabel = "Addition";
                        break;
                    case 2: 
                        result = engine.subtract(n1, n2); 
                        opLabel = "Subtraction";
                        break;
                    case 3: 
                        result = engine.multiply(n1, n2); 
                        opLabel = "Multiplication";
                        break;
                    case 4: 
                        result = engine.divide(n1, n2); 
                        opLabel = "Division";
                        break;
                    case 5: 
                        result = engine.power(n1, n2); 
                        opLabel = "Power";
                        break;
                    default:
                        throw CalculatorException("Invalid menu selection.");
                }

                std::cout << "\n>>> Result: " << std::fixed << std::setprecision(2) << result << std::endl;
                
                // Log the successful operation
                Logger::log(opLabel + " of " + std::to_string(n1) + " and " + std::to_string(n2) + " = " + std::to_string(result));

            } catch (const CalculatorException& e) {
                std::cerr << "\n[ERROR]: " << e.what() << std::endl;
                Logger::log("Error encountered: " + std::string(e.what()));
            }
        }
        std::cout << "Closing Calculator. Goodbye!" << std::endl;
    }

    void showLogs() {
        std::ifstream logFile("calc_history.log");
        std::string line;
        std::cout << "\n--- Calculation History ---" << std::endl;
        if (logFile.is_open()) {
            while (getline(logFile, line)) {
                std::cout << line << std::endl;
            }
            logFile.close();
        } else {
            std::cout << "No history found." << std::endl;
        }
    }
};

// ==========================================
// 5. MAIN ENTRY POINT
// ==========================================
int main() {
    CalculatorUI app;
    app.run();
    return 0;
}