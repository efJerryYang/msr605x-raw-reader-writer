#include "msr_common.h"
#include <limits>

enum class Operation {
    READ,
    WRITE,
    EXIT
};

Operation get_operation() {
    char choice;
    std::cout << "Choose operation (R for Read, W for Write, E for Exit): ";
    std::cin >> choice;
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

    switch (toupper(choice)) {
        case 'R': return Operation::READ;
        case 'W': return Operation::WRITE;
        case 'E': return Operation::EXIT;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
            return get_operation();
    }
}

int main() {
    try {
        init_reader_device();
        init_writer_device();

        while (true) {
            Operation op = get_operation();

            if (op == Operation::EXIT) {
                break;
            }

            switch (op) {
                case Operation::READ:
                    std::cout << "Reading card..." << std::endl;
                    read_card_once();
                    break;
                case Operation::WRITE:
                    std::cout << "Writing card..." << std::endl;
                    write_card_once();
                    break;
                default:
                    break;
            }

            std::cout << "Press Enter to continue...";
            std::cin.get();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    reset_and_exit_device();
    return 0;
}
