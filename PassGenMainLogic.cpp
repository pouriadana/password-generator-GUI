#include "PassGenMainLogic.h"

bool isdigit(std::string c)
// Given a string, return true if all of its characters are digits
{
    bool result;
    result = true;
    for (int i = 0; i < c.size(); ++i) {
        if (!(c[i] - '0' >= 0 && c[i] - '0' <= 9)) {
            result = false;
            break;
        }
    }
    return result;
}

bool isalphabetic(std::string c)
// given a string, return true of all characters belong to the English alphabet
{
    std::string digits {"0123456789"};
    bool truth_flag = true;
    for (int i = 0; i < digits.size(); ++i) {
        if (c.find(digits[i]) != c.npos) {
            truth_flag = false;
        }
    }
    return truth_flag;
}

void show_help()
// shows information on how to use the program
{
    std::cout << "\nUsage: passgen [-SPECIFIER COMMAND]\n\n";
    std::cout << "    Commands:\n";
    std::cout << "\t-l <number>\t\t\tcommand specifies the length of the password\n"
              << "\t-y <number>\t\t\tcommand specifies the optional integration of (birth) year into the password\n"
              << "\t-fc <English word>\t\t\tcommand specifies the optional integration of your favorite color into the password"
              << "\n\n\n";
}

void fill_arr_zero(bool arr[], int len)
// given a built-in array, fill all the elements with 0/false
{
    for (int i = 0; i < len; arr[i++] = false)
        ;
}

void fill_bytwo(bool arr[], int pos)
// fill pos and pos+1 in arr with 1/true values
{
    for (int i = pos; i < pos+2; arr[i++] = true)
        ;
}

/* Class definitions */
class Rand_int {
private:
    std::random_device rd;
    std::default_random_engine engine;
    std::uniform_int_distribution<int> distrib;
public:
    Rand_int(int low, int high) : distrib{low, high} {
        engine.seed(rd());
    }
    int operator()() {
        return distrib(engine);
    }
};
