#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include "dice_roller.hpp"

DiceRoller::DiceRoller() {
    std::srand(static_cast<unsigned int>(time(0)));
}

void DiceRoller::roll() const {
    std::map<std::string, int> total{};
    // Start rolling attacks based on the values set in _vals
    std::cout << "Rolling " << _vals.attack_count << " attacks with AC: " << _vals.ac << std::endl;
    for (int i = 0; i < _vals.attack_count; i++) {
        // Roll attack roll based on the attack type
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
        // Check if the roll hits or misses based on the AC and critical hit/miss conditions
        if (((roll + _vals.modifier) >= _vals.ac || roll == CRIT) && roll != CRIT_MISS) {
            int multiplier{ 1 };
            // Set the multiplier to 2 if the roll is a critical hit
            if (roll >= _vals.crit_range) {
                multiplier = CRIT_MULTIPLIER;
            }
            // Calculate the total damage for each damage type
            for (size_t j = 0; j < _vals.damages.size(); j++) {
                Damage current_damage = _vals.damages.at(j);
                int attack_damage = (damage(current_damage.dice_count, current_damage.dice_sides) * multiplier) + current_damage.modifier;

                if (total.contains(current_damage.type)) {
                    total[current_damage.type] += attack_damage;
                }
                else {
                    total.insert({ current_damage.type, attack_damage });
                }

                std::cout << attack_damage << " " << current_damage.type;
                if (j < _vals.damages.size() - 1) {
                    std::cout << " + ";
                }
            }
            std::cout << " Damage";
            if (multiplier == CRIT_MULTIPLIER) {
                std::cout << " (Critical Hit)";
            }
            std::cout << std::endl;
        }
        // Else print that the attack missed
        else {
            std::cout << "Missed";
            if (roll == CRIT_MISS) {
                std::cout << " (Critical Miss)";
            }
            std::cout << std::endl;
        }
    }
    // Print the total damage for each damage type
    std::cout << "Total Damage:" << std::endl;
    for (const auto &[key, value] : total) {
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
                exit(EXIT_FAILURE);
            }
            _vals = RollVals{};
            std::cout << std::endl;
            continue;
        }
        try {
            get_values(buf);
        }
        catch (const std::invalid_argument &e) {
            std::cerr << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }

    }
}

void DiceRoller::set_attack_type() {
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

void DiceRoller::set_ac() {
    std::cout << "What is the AC of the target: ";
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        try {
            _vals.ac = std::stoi(input);
            break;
            if (_vals.ac < 1) {
                throw std::invalid_argument(input);
            }
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
    // Check if the line starts with "ac:" and parse the AC value
    if (buf.starts_with("ac:")) {
        std::string ac = buf.substr(3);
        try {
            _vals.ac = std::stoi(ac);
            if (_vals.ac < 1) {
                throw std::invalid_argument(ac);
            }
        }
        catch (const std::invalid_argument &e) {
            throw std::invalid_argument("Invalid AC value: " + ac + "\n");
        }
    }
    // Check if the line starts with "attacks:" and parse the attack count value
    else if (buf.starts_with("attacks:")) {
        std::string attacks = buf.substr(8);
        try {
            _vals.attack_count = std::stoi(attacks);
            if (_vals.attack_count < 1) {
                throw std::invalid_argument(attacks);
            }
        }
        catch (const std::invalid_argument &e) {
            throw std::invalid_argument("Invalid attacks value: " + attacks + "\n");
        }
    }
    // Check if the line starts with "crit range:" and parse the critical range value
    else if (buf.starts_with("crit range:")) {
        std::string crit_range = buf.substr(11);
        try {
            _vals.crit_range = std::stoi(crit_range);
            if (_vals.crit_range < 1 || _vals.crit_range > 20) {
                throw std::invalid_argument(crit_range);
            }
        }
        catch (const std::invalid_argument &e) {
            throw std::invalid_argument("Invalid crit range value: " + crit_range + "\n");
        }
    }
    // Check if the line starts with "modifier:" and parse the modifier value
    else if (buf.starts_with("modifier:")) {
        std::string modifier = buf.substr(9);
        try {
            _vals.modifier = std::stoi(modifier);
        }
        catch (const std::invalid_argument &e) {
            throw std::invalid_argument("Invalid modifier value: " + modifier + "\n");
        }
    }
    // Check if the line starts with "attack type:" and parse the attack type value
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
            throw std::invalid_argument("Invalid attack type value: " + attack_type + "\n");
        }
    }
    // Check if the line starts with "damage:" and parse the damage values
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
    // If the line does not match any of the expected formats, throw an error
    else {
        throw std::invalid_argument("Invalid line in file: " + buf + "\n");
    }
}

bool DiceRoller::check_values() const {
    // Checks if the values in _vals are valid
    if (_vals.attack_count == 0 || _vals.damages.empty()) {
        return false;
    }
    return true;
}
