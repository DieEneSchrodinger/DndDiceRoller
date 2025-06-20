#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <vector>
#include <regex>

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

/// @brief Options class to handle command line arguments and user input for D&D attack calculations.
class Options {
public:
    /// @brief Parse command line arguments to set options.
    /// @param argc Number of command line arguments.
    /// @param argv Array of command line arguments.
    void parse(int argc, char **argv);

    /// @brief Sets the values of the options manually.
    void set_manual();

    /// @brief Accessor for the attack modifier.
    /// @return Attack modifier value.
    int attack_modifier() const { return _modifier; }

    /// @brief Accessor for the attack count.
    /// @return Attack count value.
    int attack_count() const { return _attack_count; }

    /// @brief Accessor for the damages vector.
    /// @return Damages vector containing all damage types and values.
    const std::vector<Damage> &damages() const { return _damages; }

    /// @brief Accessor for the critical range.
    /// @return Critical range value.
    int crit_range() const { return _crit_range; }

    /// @brief Accessor for the armor class (AC).
    /// @return Armor class value.
    int ac() const { return _ac; }

    /// @brief Accessor for the attack type.
    /// @return Attack type enum value.
    AttackType attack_type() const { return _attack_type; }

    /// @brief Accessor for the only files flag.
    /// @return Only files flag value.
    bool only_files() { return _only_files; }

    /// @brief Accessor for the options files.
    /// @return Vector of option files.
    const std::vector<std::string> &opts_files() const { return _opts_files; }

    /// @brief Accessor for the help flag.
    /// @return Help flag value.
    bool help() const { return _help; }

    /// @brief Help message printer
    void help_msg();

private:
    /// @brief Checks if the options are valid.
    void check_opts();

    /// @brief Sets the attack modifier value through user input.
    void set_attack_modifier();

    /// @brief Sets the attack count value through user input.
    void set_attack_count();

    /// @brief Sets the damages vector through user input.
    void set_damages();

    /// @brief Sets the critical range value through user input.
    void set_crit_range();

    /// @brief Sets the armor class (AC) value through user input.
    void set_ac();

    /// @brief Sets the attack type value through user input.
    void set_attack_type();

    int _modifier{};
    int _attack_count{};
    int _crit_range{ 20 };
    int _ac{};
    std::vector<Damage> _damages{};
    AttackType _attack_type{ NORMAL };
    bool _help{};
    std::vector<std::string> _opts_files{};

    bool _only_files{};
    std::regex _damage_regex{ R"((\d+)d(\d+)(?:\s*([+-]\s*\d+))?\s*([a-zA-Z]+))" };
};

#endif // OPTIONS_H