#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <vector>
#include "enums.hpp"
#include "defines.hpp"

/// @brief Struct to hold the damage type and its associated values.
struct Damage {
    std::string type;
    int dice_count;
    int dice_sides;
    int modifier;
};

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

#endif // STRUCTS_H