#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include "../inc/dice_roller.hpp"
#include "../inc/options.hpp"

DiceRoller::DiceRoller() {
    std::srand(static_cast<uint>(time(0)));
}

void DiceRoller::roll(const Options &options) {
    // Initialize the RollVals struct with values from options
    _vals = RollVals{};
    _vals.ac = options.ac();
    _vals.attack_num = options.attack_count();
    _vals.modifier = options.attack_modifier();
    _vals.attack_type = options.attack_type();
    _vals.crit_range = options.crit_range();
    _vals.damages = options.damages();
    // Check if the values are valid before rolling
    if (check_values()) {
        roll();
    }
    else {
        std::cerr << "Invalid values provided in options." << std::endl;
        exit(1);
    }
}

void DiceRoller::roll() const {
    std::map<std::string, int> total{};

    for (int i = 0; i < _vals.attack_num; i++) {
        int roll = 0;
        if (_vals.attack_type == NORMAL) {
            roll = rand(D20);
        }
        else if (_vals.attack_type == ADVANTAGE) {
            roll = std::max(rand(D20), rand(D20));
        }
        else if (_vals.attack_type == DISADVANTAGE) {
            roll = std::min(rand(D20), rand(D20));
        }
        std::cout << "Attack " << i + 1 << ": ";
        if (((roll + _vals.modifier) > _vals.ac || roll == CRIT) && roll != CRIT_MISS) {
            int multiplier{ 1 };
            if (roll >= _vals.crit_range) {
                multiplier = 2;
            }
            for (size_t j = 0; j < _vals.damages.size(); j++) {
                Damage current = _vals.damages.at(j);
                int attack_damage = (damage(current.dice_count, current.dice_sides) * multiplier) + current.modifier;

                if (total.contains(current.type)) {
                    total[current.type] += attack_damage;
                }
                else {
                    total.insert({ current.type, attack_damage });
                }

                std::cout << attack_damage << " " << current.type;
                if (j < _vals.damages.size() - 1) {
                    std::cout << " + ";
                }
            }
            std::cout << " Damage";
            if (multiplier == 2) {
                std::cout << " (Critical Hit)";
            }
            std::cout << std::endl;
        }
        else {
            std::cout << "Missed";
            if (roll == CRIT_MISS) {
                std::cout << " (Critical Miss)";
            }
            std::cout << std::endl;
        }
    }
    std::cout << "Total Damage:" << std::endl;
    for(const auto& [key, value] : total){
        std::cout << value << " " << key << " Damage" << std::endl;
    }
}

void DiceRoller::roll(const std::string &file_name) {
    // Initialize variables
    _vals = RollVals{};
    std::ifstream file{ file_name };
    int attack_num{ 1 };
    // Check if the file is open and keep reading until the end of the file
    while (file.good()) {
        if (_vals.empty) {
            std::cout << "Rolling attack set: " << attack_num << " from file: " << file_name << std::endl;
            _vals.empty = false;
        }
        std::string buf{};
        std::getline(file, buf);
        // If the line is empty, check if we have valid values to roll
        if (buf.empty()) {
            if (check_values()) {
                // If the attack type is UNSET, ask the user for input
                if (_vals.attack_type == UNSET) {
                    set_attack_type();
                }
                // If the AC is not set, ask the user for input
                if (_vals.ac == 0) {
                    set_ac();
                }
                // Roll the attack with the values set
                roll();
                attack_num++;
            }
            else {
                std::cerr << "Invalid values in file: " << file_name << " at attack set: " << attack_num << std::endl;
                exit(1);
            }
            _vals = RollVals{};
            continue;
        }
        get_values(buf);
    }
}

void DiceRoller::set_attack_type(){
    std::cout << "Are the attacks (A)dvantage or (D)isadvantage, leave empty for standard: ";
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) {
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
            std::cout << "Invalid input. Please enter A, D, or leave empty for normal: ";
        }
    }
}

void DiceRoller::set_ac(){
    std::cout << "What is the AC of the target: ";
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        try {
            _vals.ac = std::stoi(input);
            break;
        }
        catch (const std::invalid_argument &) {
            std::cout << "Invalid input. Please enter a AC: ";
        }
    }
}

int DiceRoller::damage(int dice_count, int dice_sides) const {
    // Rolls the damage based on the number of dice and sides of the dice
    int sum{};
    for (int i = 0; i < dice_count; i++) {
        sum += rand(dice_sides);
    }
    return sum;
}

int DiceRoller::rand(int dice_sides) const {
    // Generates a random number between 1 and the number of sides on the dice
    return (std::rand() % dice_sides) + 1;
}

void DiceRoller::get_values(const std::string &buf) {
    // Parses the values from the string buffer and sets them in _vals
    if (buf.empty()) {
        return;
    }
    else if (buf.starts_with("ac:")) {
        std::string ac = buf.substr(3);
        try {
            _vals.ac = std::stoi(ac);
        }
        catch (const std::invalid_argument &e) {
            std::cerr << "Invalid AC value: " << ac << std::endl;
            exit(1);
        }
    }
    else if (buf.starts_with("attacks:")) {
        std::string attacks = buf.substr(8);
        try {
            _vals.attack_num = std::stoi(attacks);
        }
        catch (const std::invalid_argument &e) {
            std::cerr << "Invalid attack amount value: " << attacks << std::endl;
            exit(1);
        }
    }
    else if (buf.starts_with("crit range:")) {
        std::string crit_range = buf.substr(11);
        try {
            _vals.crit_range = std::stoi(crit_range);
        }
        catch (const std::invalid_argument &e) {
            std::cerr << "Invalid crit range value: " << crit_range << std::endl;
            exit(1);
        }
    }
    else if (buf.starts_with("modifier:")) {
        std::string modifier = buf.substr(9);
        try {
            _vals.modifier = std::stoi(modifier);
        }
        catch (const std::invalid_argument &e) {
            std::cerr << "Invalid modifier value: " << modifier << std::endl;
            exit(1);
        }
    }
    else if (buf.starts_with("attack type:")) {
        std::string attack_type = buf.substr(12);
        if (attack_type == "a" || attack_type == "A") {
            _vals.attack_type = ADVANTAGE;
        }
        else if (attack_type == "d" || attack_type == "D") {
            _vals.attack_type = DISADVANTAGE;
        }
        else if (attack_type == "n" || attack_type == "N" || attack_type.empty()) {
            _vals.attack_type = NORMAL;
        }
        else {
            std::cerr << "Invalid attack type: " << attack_type << std::endl;
            exit(1);
        }
    }
    else if (buf.starts_with("damage:")) {
        std::string damage_str = buf.substr(7);
        auto begin = std::sregex_iterator(damage_str.begin(), damage_str.end(), _damage_regex);
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
            _vals.damages.push_back(damage);
        }
    }
    else {
        std::cerr << "Invalid line in file: " << buf << std::endl;
        exit(1);
    }
}

bool DiceRoller::check_values() const {
    // Checks if the values in _vals are valid
    if (_vals.attack_num == 0 || _vals.damages.empty()) {
        return false;
    }
    return true;
}
