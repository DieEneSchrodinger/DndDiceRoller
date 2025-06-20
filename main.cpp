#include <iostream>
#include <string>
#include "inc/options.hpp"
#include "inc/dice_roller.hpp"

int main(int argc, char **argv) {
    Options options{};
    if (argc != 1) {
        // Parse command line arguments if there is more than one argument (first is the program name)
        try {
            options.parse(argc, argv);
            // If help is passed, print help message and exit
            if (options.help()) {
                options.help_msg();
                return 0;
            }
        }
        catch (const std::exception &e) {
            // If there was an error parsing the options, print the error message and help message
            std::cerr << "Error: " << e.what() << std::endl;
            options.help_msg();
            return 1;
        }
    }
    else {
        // If no arguments are passed, set options through user input
        options.set_manual();
    }

    DiceRoller roller{};
    // If only_files is false, roll the attack(s) based on the options provided
    if (!options.only_files()) {
        std::cout << "Rolling dice...\n";
        roller.roll(options);
    }
    // If there are files specified, roll attack(s) with the values in those files
    if (!options.opts_files().empty()) {
        // For each file specified in the options, roll the attack(s) defined in the file
        for (size_t i = 0; i < options.opts_files().size(); i++) {
            std::cout << "Rolling dice of file: " << options.opts_files().at(i) << "..." << std::endl;
            roller.roll(options.opts_files().at(i));
            std::cout << std::endl;
        }
    }
    return 0;
}