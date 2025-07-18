#include "options.hpp"
#include <filesystem>
#include <stdexcept>
#include <regex>
#include <iostream>

void Options::parse(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        // Check for the --help option
        if (arg == "--help") {
            _help = true;
        }
        // Check for the --damage option and parse the damage string with _damage_regex
        else if (arg == "--damage") {
            if (++i <= argc) {
                arg = argv[i];
                auto begin = std::sregex_iterator(arg.begin(), arg.end(), _damage_regex);
                auto end = std::sregex_iterator();
                // Match all occurrences of the damage regex in the input string
                if (begin == end) {
                    throw std::invalid_argument("Invalid damage format: " + arg);
                }
                for (auto j = begin; j != end; j++) {
                    Damage damage;
                    std::smatch match = *j;
                    if (match.size() < 5) {
                        throw std::invalid_argument("Invalid damage format: " + arg);
                    }
                    damage.dice_count = std::stoi(match[1].str());
                    damage.dice_sides = std::stoi(match[2].str());
                    std::string temp = match[3].matched ? match[3].str() : "0";
                    size_t idx = temp.find_first_of(" \t\r\n");
                    if (idx != std::string::npos) {
                        temp.erase(temp.find_first_of(" \t\r\n"), 1);
                    }
                    damage.modifier = std::stoi(temp);
                    damage.type = match[4].str();
                    _vals.damages.push_back(damage);
                }
            }
            else {
                throw std::invalid_argument("No damage provided after --damage");
            }
        }
        // Check for the --modifier option and parse the modifier value
        else if (arg == "--modifier") {
            if (i + 1 <= argc) {
                try {
                    _vals.modifier = std::stoi(argv[++i]);
                }
                catch (const std::invalid_argument &e) {
                    throw std::invalid_argument("Invalid modifier value: " + std::string(argv[i]));
                }
            }
            else {
                throw std::invalid_argument("No modifier provided after --modifier");
            }
        }
        // Check for the --attack-count option and parse the attack count value
        else if (arg == "--attack-count") {
            if (i + 1 <= argc) {
                try {
                    _vals.attack_count = std::stoi(argv[++i]);
                }
                catch (const std::invalid_argument &e) {
                    throw std::invalid_argument("Invalid modifier value: " + std::string(argv[i]));
                }
            }
            else {
                throw std::invalid_argument("No attack count provided after --attack-count");
            }
        }
        // Check for the --attack-type option and parse the attack type
        else if (arg == "--attack-type") {
            if (i + 1 <= argc) {
                std::string type = argv[++i];
                if (type == "A" || type == "a") {
                    _vals.attack_type = ADVANTAGE;
                }
                else if (type == "D" || type == "d") {
                    _vals.attack_type = DISADVANTAGE;
                }
                else if (type == "N" || type == "n") {
                    _vals.attack_type = NORMAL;
                }
                else {
                    throw std::invalid_argument("Invalid attack type: " + type);
                }
            }
            else {
                throw std::invalid_argument("No attack type provided after --attack-type");
            }
        }
        // Check for the --ac option and parse the armor class value
        else if (arg == "--ac") {
            if (i + 1 <= argc) {
                try {
                    _vals.ac = std::stoi(argv[++i]);
                }
                catch (const std::invalid_argument &e) {
                    throw std::invalid_argument("Invalid AC value: " + std::string(argv[i]));
                }
            }
            else {
                throw std::invalid_argument("No AC provided after --armor-class");
            }
        }
        // Check for the --crit-range option and parse the critical range value
        else if (arg == "--crit-range") {
            if (i + 1 <= argc) {
                try {
                    _vals.crit_range = std::stoi(argv[++i]);
                }
                catch (const std::invalid_argument &e) {
                    throw std::invalid_argument("Invalid critical range value: " + std::string(argv[i]));
                }
            }
            else {
                throw std::invalid_argument("No critical range provided after --crit-range");
            }
        }
        // Check for short options starting with a single dash
        else if (arg.starts_with("-") && !arg.starts_with("--")) {
            for (char c : arg.substr(1)) {
                switch (c) {
                case 'h': _help = true; break;
                default: throw std::invalid_argument("Unknown short option: -" + std::string{ c });
                }
            }
        }
        // If the argument is not an option, treat it as a file name
        else {
            if (std::filesystem::exists(arg)) {
                _opts_files.push_back(arg);
            }
            else {
                throw std::invalid_argument("File does not exist: " + arg + ".");
            }
        }
    }
    // Check if the options are valid after parsing and set the _only_files flag if necessary
    check_opts();
}

void Options::check_opts() {
    // Check if the required options are set
    // _modifier can be 0, so is not checked here
    if (_opts_files.size() == 0 && !_help) {
        if (_vals.attack_count == 0) throw std::invalid_argument("attack-count wasn\'t passed, can\'t roll attack(s).");
        else if (_vals.ac == 0) throw std::invalid_argument("ac wasn\'t passed, can\'t roll attack(s).");
        else if (_vals.damages.empty()) throw std::invalid_argument("damage wasn\'t passed, can\'t roll attack(s).");
        else if (_vals.attack_type == UNSET) throw std::invalid_argument("attack type wasn\'t passed, can\'t roll attack(s).");
    }
    // If files were passed, but other options are not set, set _only_files to true
    else {
        if (_vals.attack_count == 0 || _vals.ac == 0 || _vals.damages.empty()) {
            _only_files = true;
        }
    }

}

void Options::help_msg() {
    std::cout << "Usage: DndDiceRoller [options] [files...]" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "  --help or -h            Show this help message" << std::endl
              << "  --damage <dmg>          Specify damage (format: 1d6 + 7 piercing + 1d6 poison)" << std::endl
              << "  --modifier <mod>        Specify attack modifier value" << std::endl
              << "  --attack-count <count>  Specify number of attacks" << std::endl
              << "  --ac <ac>               Specify target's Armor Class" << std::endl
              << "  --attack-type <type>    Specify attack type (A or a for Advantage, D or d for Disadvantage, N or n for Normal)" << std::endl
              << "  --crit-range <range>    Specify critical hit range (default is 20)" << std::endl
              << std::endl
              << "File formatting:" << std::endl
              << "  attacks:<amount of attacks>     format: integer greater than 0" << std::endl
              << "  modifier:<attack modifier>      format: integer" << std::endl
              << "  crit range:<crit range>         format: integer between 1 and 20" << std::endl
              << "  damage:<damage format>          format: 1d6 + 7 piercing + 1d6 poison" << std::endl
              << "  ac:<ac>                         format: integer greater than 0" << std::endl
              << "  attack type:<attack type>       format: A or a for Advantage, D or d for Disadvantage, N or n for Normal" << std::endl
              << std::endl
              << "Above values can be in any order." << std::endl
              << "All values except \"crit range\", \"ac\" and \"attack type\"  must be present or program will exit." << std::endl
              << "Seperate blocks must be seperated by a linebreak and there can be an practically infinite amount of blocks" << std::endl;
}

void Options::set_attack_modifier() {
    std::cout << "What is the attack modifier: ";
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        try {
            _vals.modifier = std::stoi(input);
            break;
        }
        catch (const std::invalid_argument &) {
            std::cout << "Invalid input. Please enter a valid number: ";
        }
    }
}

void Options::set_attack_count() {
    std::cout << "How many attacks will be made: ";
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        try {
            _vals.attack_count = std::stoi(input);
            if (_vals.attack_count < 1) {
                throw std::invalid_argument(input);
            }
            break;
        }
        catch (const std::invalid_argument &) {
            std::cout << "Invalid input. Please enter a valid number: ";
        }
    }
}

void Options::set_damages() {
    std::cout << "Enter damage (format: 1d8 piercing + 5 poison): ";
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        try {
            // Sets the regex to match
            auto begin = std::sregex_iterator(input.begin(), input.end(), _damage_regex);
            auto end = std::sregex_iterator();
            if( begin == end) {
                throw std::invalid_argument(input);
            }
            for (auto i = begin; i != end; i++) {
                Damage damage;
                std::smatch match = *i;
                if (match.size() < 5) {
                    throw std::invalid_argument(input);
                }
                damage.dice_count = std::stoi(match[1].str());
                damage.dice_sides = std::stoi(match[2].str());
                std::string temp = match[3].matched ? match[3].str() : "0";
                size_t idx = temp.find_first_of(" \t\r\n");
                // If there is a space or tab in the modifier, remove it
                if (idx != std::string::npos) {
                    temp.erase(temp.find_first_of(" \t\r\n"), 1);
                }
                damage.modifier = std::stoi(temp);
                damage.type = match[4].str();
                _vals.damages.push_back(damage);
            }
            break;
        }
        catch (const std::exception &e) {
            std::cout << "Invalid input. Please enter a valid damage format: ";
        }
    }
}

void Options::set_crit_range() {
    std::cout << "What is the critical range (leave empty for standard): ";
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) {
            break;
        }
        try {
            _vals.crit_range = std::stoi(input);
            if (_vals.crit_range < 1 || _vals.crit_range > 20) {
                throw std::invalid_argument(input);
            }
            break;
        }
        catch (const std::invalid_argument &) {
            std::cout << "Invalid input. Please enter a valid number: ";
        }
    }
}

void Options::set_ac() {
    std::cout << "What is the AC of the target: ";
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        try {
            _vals.ac = std::stoi(input);
            if (_vals.ac < 1) {
                throw std::invalid_argument(input);
            }
            break;
        }
        catch (const std::invalid_argument &) {
            std::cout << "Invalid input. Please enter a valid number: ";
        }
    }
}

void Options::set_attack_type() {
    std::cout << "Do the attack(s) have (A)dvantage or (D)isadvantage, (N)ormal, leave empty for standard: ";
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (input.empty() || input == "N" || input == "n") {
            _vals.attack_type = NORMAL;
            break;
        }
        else if (input == "A" || input == "a") {
            _vals.attack_type = ADVANTAGE;
            break;
        }
        else if (input == "D" || input == "d") {
            _vals.attack_type = DISADVANTAGE;
            break;
        }
        else {
            std::cout << "Invalid input. Please enter A, D, N or leave empty: ";;
        }
    }
}

void Options::set_manual() {
    // If no command line arguments or files are provided, prompt the user to enter options manually
    set_attack_count();
    set_attack_modifier();
    set_attack_type();
    set_crit_range();
    set_damages();
    set_ac();
}