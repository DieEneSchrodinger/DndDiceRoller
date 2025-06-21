#include "../inc/options.hpp"
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
            if (i + 1 <= argc) {
                auto begin = std::sregex_iterator(arg.begin(), arg.end(), _damage_regex);
                auto end = std::sregex_iterator();
                // Match all occurrences of the damage regex in the input string
                for (auto j = begin; j != end; j++) {
                    Damage damage;
                    std::smatch match = *j;
                    damage.dice_count = std::stoi(match[1].str());
                    damage.dice_sides = std::stoi(match[2].str());
                    std::string temp = match[3].matched ? match[3].str() : "0";
                    size_t idx = temp.find_first_of(" \t\r\n");
                    if (idx != std::string::npos) {
                        temp.erase(temp.find_first_of(" \t\r\n"), 1);
                    }
                    damage.modifier = std::stoi(temp);
                    damage.type = match[4].str();
                    _damages.push_back(damage);
                }
                i++;
            }
            else {
                throw std::invalid_argument("No damage provided after --damage");
            }
        }
        // Check for the --modifier option and parse the modifier value
        else if (arg == "--modifier") {
            if (i + 1 <= argc) {
                try {
                    _modifier = std::stoi(argv[++i]);
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
                    _attack_count = std::stoi(argv[++i]);
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
                    _attack_type = ADVANTAGE;
                }
                else if (type == "D" || type == "d") {
                    _attack_type = DISADVANTAGE;
                }
                else if (type == "N" || type == "n" || type.empty()) {
                    _attack_type = NORMAL;
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
        else if (arg == "--armor-class") {
            if (i + 1 <= argc) {
                try {
                    _ac = std::stoi(argv[i++]);
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
                    _crit_range = std::stoi(argv[++i]);
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
            // If the argument is not a valid file, throw an error
            else {
                throw std::invalid_argument("File does not exist: " + arg);
            }
        }
    }
    // Check if the options are valid after parsing and set the _only_files flag if necessary
    check_opts();
}

void Options::check_opts() {
    // If no files were passed, check if the required options are set
    // _modifier can be 0, so is not checked here
    if (_opts_files.size() == 0) {
        if ((_attack_count == 0 || _ac == 0 || _damages.empty()) && !_help) {
            throw std::invalid_argument("Not enough options were passed to roll the attack(s)");
        }
    }
    // If files were passed, but other options are not set, set _only_files to true
    else {
        if (_attack_count == 0 || _ac == 0 || _damages.empty()) {
            _only_files = true;
        }
    }

}

void Options::help_msg() {
    std::cout << "Usage: dnd_calculator [options] [files...]" << std::endl
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
            _modifier = std::stoi(input);
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
            _attack_count = std::stoi(input);
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
            // Sets the regex to matc
            auto begin = std::sregex_iterator(input.begin(), input.end(), _damage_regex);
            auto end = std::sregex_iterator();
            for (auto i = begin; i != end; i++) {
                Damage damage;
                std::smatch match = *i;
                damage.dice_count = std::stoi(match[1].str());
                damage.dice_sides = std::stoi(match[2].str());
                std::string temp = match[3].matched ? match[3].str() : "0";
                size_t idx = temp.find_first_of(" \t\r\n");
                if (idx != std::string::npos) {
                    temp.erase(temp.find_first_of(" \t\r\n"), 1);
                }
                damage.modifier = std::stoi(temp);
                damage.type = match[4].str();
                _damages.push_back(damage);
            }
            break;
        }
        catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cout << "Please enter a valid damage format: ";
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
            _crit_range = std::stoi(input);
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
            _ac = std::stoi(input);
            break;
        }
        catch (const std::invalid_argument &) {
            std::cout << "Invalid input. Please enter a valid number: ";
        }
    }
}

void Options::set_attack_type() {
    std::cout << "Do the attack(s) have (A)dvantage or (D)isadvantage, leave empty for standard: ";
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) {
            break;
        }
        else if (input == "A" || input == "a") {
            _attack_type = ADVANTAGE;
            break;
        }
        else if (input == "D" || input == "d") {
            _attack_type = DISADVANTAGE;
            break;
        }
        else {
            std::cout << "Invalid input. Please enter A, D, or leave empty for normal: ";;
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