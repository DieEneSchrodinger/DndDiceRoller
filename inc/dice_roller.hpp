#ifndef DICE_ROLLER_H
#define DICE_ROLLER_H

#include "options.hpp"
#include <vector>
#include <string>
#include <regex>

#define D20 20
#define CRIT 20
#define CRIT_MISS 1
#define CRIT_MULTIPLIER 2

/// @brief Struct to hold the values for the current roll
struct RollVals {
    int ac{};
    int attack_count{};
    int modifier{};
    AttackType attack_type{ UNSET };
    int crit_range{ CRIT };
    std::vector<Damage> damages;
    bool empty{ true };
};

/// @brief Enum to represent the type of attack being made.
enum AttackType {
    UNSET = -1,
    NORMAL,
    ADVANTAGE,
    DISADVANTAGE
};

/// @brief Struct to hold the damage type and its associated values.
struct Damage {
    std::string type;
    int dice_count;
    int dice_sides;
    int modifier;
};

/// @brief DiceRoller class for rolling dice based on provided Options class or file input
class DiceRoller {
public:
    /// @brief Constructor for DiceRoller, initializes random seed.
    DiceRoller();

    /// @brief Rolls the dice based on the values in the file.
    /// @param file_name name of the file to read values from.
    void roll(const std::string &file_name);

    /// @brief Rolls the dice based on the values set in the class.
    void roll() const;

    /// @brief Sets the values for the current roll.
    /// @param vals Values to set for the current roll.
    void set_vals(const RollVals &vals) { _vals = vals; }

private:
    /// @brief Sets the attack type based on user input.
    void set_attack_type();

    /// @brief Sets the armor class (AC) based on user input.
    void set_ac();

    /// @brief Rolls the damage based on the number of dice and sides of the dice.
    /// @param dice_count Number of dice to roll.
    /// @param dice_sides Sides of the dice to roll.
    /// @return Total number(damage) rolled.
    int damage(int dice_count, int dice_sides) const;

    /// @brief Generates a random number between 1 and the number of sides on the dice.
    /// @param dice_sides Number of sides on the dice.
    /// @return Random number between 1 and dice_sides.
    int rand(int dice_sides) const;

    /// @brief Parses the values from the string buffer and sets them in _vals.
    /// @param buf String buffer containing the values to parse.
    void get_values(const std::string &buf);

    /// @brief Checks if the values in _vals are valid.
    /// @return True if the values are valid, false otherwise.
    bool check_values() const;

    /// @brief Regex for parsing damage strings
    std::regex _damage_regex{ R"((\d+)d(\d+)(?:\s*([+-]\s*\d+))?\s*([a-zA-Z]+))" };
    /// @brief Values for the current roll
    RollVals _vals{};
};

#endif // DICE_ROLLER_H