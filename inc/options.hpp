#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <vector>
#include <regex>
#include "structs.hpp"

/// @brief Options class to handle command line arguments and user input for D&D attack calculations.
class Options {
public:
    /// @brief Parse command line arguments to set options.
    /// @param argc Number of command line arguments.
    /// @param argv Array of command line arguments.
    void parse(int argc, char **argv);

    /// @brief Sets the values of the options manually.
    void set_manual();

    /// @brief Accessor for the roll values.
    /// @return RollVals struct containing the values for rolling dice.
    RollVals &vals() { return _vals; }

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

    /// @brief Struct to hold the values for rolling dice.
    RollVals _vals{};
    /// @brief help flag
    bool _help{};
    /// @brief Vector to hold file names
    std::vector<std::string> _opts_files{};

    /// @brief Flag to indicate if only files should be processed
    bool _only_files{};
    /// @brief Regex for parsing damage strings
    std::regex _damage_regex{ R"((\d+)d(\d+)(?:\s*([+-]\s*\d+))?\s*([a-zA-Z]+))" };
};

#endif // OPTIONS_H