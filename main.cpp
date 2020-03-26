#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <variant>
#include <cstdlib>
#include <optional>
#include <algorithm>


template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

enum class NumberAcceptance {
    No, Single, Multiple
};

struct ArgumentType {
    char shortName;
    std::string longName;
    NumberAcceptance numberAcceptance;

    ArgumentType(char shortName, std::string&& longName, NumberAcceptance numberAcceptance)
        : shortName(shortName), longName(std::move(longName)), numberAcceptance(numberAcceptance) {}
};

struct ActualArgument {
    std::variant<char, std::string> name;
    std::vector<int> numbers;

    ActualArgument(std::variant<char, std::string>&& name, std::vector<int>&& numbers)
        : name(std::move(name)), numbers(std::move(numbers)) {}
};

std::optional<unsigned int> findByName(
        const std::variant<char, std::string>& name,
        const std::vector<ArgumentType>& arguments) {
    for (unsigned int i = 0; i < arguments.size(); i++) {
        const auto& arg = arguments[i];
        if (std::holds_alternative<char>(name) &&
                std::get<char>(name) == arg.shortName) return { i };
        else if (std::holds_alternative<std::string>(name) &&
                std::get<std::string>(name) == arg.longName) return { i };
    }
    return std::nullopt;
}

std::vector<ActualArgument> parseCommandLineArguments(std::vector<std::string>&& arguments) {
    std::vector<ActualArgument> actualArguments;

    const auto hasNext = [&arguments](auto it) {
        return std::next(it, 1) != arguments.end();
    };
    const auto getNext = [&arguments](auto it) {
        return std::next(it, 1);
    };
    const auto isFlag = [&arguments](auto it) {
        return (*it)[0] == '-';
    };
    const auto isLongFlag = [&arguments](auto it) {
        return ((*it)[0] == '-') && ((*it)[1] == '-');
    };

    for (auto it = arguments.begin(); it != arguments.end(); ++it) {
        std::variant<char, std::string> name;
        if (isFlag(it)) {
            if (isLongFlag(it)) {
                name = it->substr(2, it->size() - 2);
            } else {
                name = (*it)[1];
                for (unsigned int i = 2; i < it->size(); i++) {
                    std::vector<int> numbers;
                    actualArguments.emplace_back(std::move(name), std::move(numbers));
                    name = (*it)[i];
                }
            }
        } else {
            std::cerr << "Bad arguments format\n";
            continue;
        }

        std::vector<int> numbers;
        if (hasNext(it) && !isFlag(getNext(it))) {
            ++it;
            const auto stringNumbers = split(*it, ',');
            for (const auto& str : stringNumbers) {
                numbers.push_back(std::atoi(str.data()));
            }
        }

        actualArguments.emplace_back(std::move(name), std::move(numbers));
    }

    return actualArguments;
}

std::vector<std::string> intoStrings(unsigned int amount, char* args[]) {
    std::vector<std::string> strings;
    for (unsigned int i = 0; i < amount; i++) strings.push_back(args[i]);
    return strings;
}

void interpret(std::vector<ActualArgument>&& actualArguments, std::vector<ArgumentType>&& argumentTypes) {
    std::vector<unsigned int> presentArguments;
    for (const auto& [name, numbers] : actualArguments) {
        const auto indexOpt = findByName(name, argumentTypes);
        if (!indexOpt) continue;
        presentArguments.push_back(*indexOpt);
    }

    for (const auto& [name, numbers] : actualArguments) {
        const auto indexOpt = findByName(name, argumentTypes);
        if (!indexOpt) {
            std::cout << ">> Unknown argument ";
            if (std::holds_alternative<char>(name)) {
                std::cout << '-' << std::get<char>(name) << '\n';
            } else {
                std::cout << "--" << std::get<std::string>(name) << '\n';
            }
            continue;
        }

        const auto numberOfTimesPresent =
            std::count(presentArguments.begin(), presentArguments.end(), *indexOpt);
        if (numberOfTimesPresent > 1) {
            // A duplicate is found => skip.
            continue;
        }

        const auto& type = argumentTypes[*indexOpt];
        std::cout << ">> Argument --" << type.longName << "(-" << type.shortName << ")";
        if (!numbers.empty()) {
            if (type.numberAcceptance == NumberAcceptance::No) {
                std::cout << " doesn't expect numbers, so they were ignored";
            } else if (type.numberAcceptance == NumberAcceptance::Single) {
                if (numbers.size() > 1) {
                    std::cout << " expects a single number, so only it is accepted: ";
                } else {
                    std::cout << " with number: ";
                }
                std::cout << numbers[0];
            } else {
                std::cout << " with numbers: ";
                for (int a : numbers) {
                    std::cout << a << ' ';
                }
            }
        } else {
            if (type.numberAcceptance != NumberAcceptance::No) {
                std::cout << " espected numbers, but none were provided";
            }
        }
        std::cout << '\n';
    }
}

int main(int argc, char* argv[]) {
    std::vector<ArgumentType> argumentTypes;
    argumentTypes.emplace_back('h', "help", NumberAcceptance::Single);
    argumentTypes.emplace_back('v', "version", NumberAcceptance::No);
    argumentTypes.emplace_back('a', "args", NumberAcceptance::Multiple);
    argumentTypes.emplace_back('r', "repeat", NumberAcceptance::Single);

    interpret(parseCommandLineArguments(intoStrings(argc - 1, &argv[1])), std::move(argumentTypes));

    return 0;
}
