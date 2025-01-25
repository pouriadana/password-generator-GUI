#include "PassGenMainLogic.h"
#define INVALID_INT_VAL -9999
#define INVALID_STR_VAL "EmptyValueForColor"

bool filled[MAX_PASS_LEN];

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
Rand_int::Rand_int(int low, int high) : distrib{low, high} {
        engine.seed(rd());
    }
int Rand_int::operator()() {
        return distrib(engine);
    }

std::string generatePassword(int length, int year, std::string fcolor)
// given any combination of arguments, or none, generate a random
// meaningfull password with a default length of 12
{
    const std::string alphabet {"abcdefghijklmnopqrstuvwxyz"};                          // length == 26
    const std::string alphabet_capital {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};                  // length == 26
    const std::string numsymbol {"0123456789!?<>(){}@#$%^&*"};                          // length == 25
    std::string password;

    bool len_flag    = length == INVALID_INT_VAL ? false : true;
    bool year_flag   = year   == INVALID_INT_VAL ? false : true;
    bool fcolor_flag = fcolor == INVALID_STR_VAL ? false : true;

    /* Begin password creation with a set or default length (no year or color yet) */
    int passlen = len_flag ? length : 12;                                               // if length is set by the user, use it. Otherwise, use 12
    // Allow for the random selection of the type of character
    Rand_int rfor_alphabet(0, alphabet.size()-1);                                       // create a generator for the alphabet data size
    Rand_int rfor_numsymbol(0, numsymbol.size()-1);                                     // create a generator for the number/symbol data size
    Rand_int rfor_candidate(0, 1000);
    char candidates[3] {0,0,0};                                                         // hold, in each iteration, 3 different chars of different type: small, capital & num/sym
    if (passlen != INVALID_INT_VAL) {
        for (int i = 0; i < passlen; ++i) {
            candidates[0] = alphabet[rfor_alphabet()];
            candidates[1] = alphabet_capital[rfor_alphabet()];
            candidates[2] = numsymbol[rfor_numsymbol()];
            int rsubscript{rfor_candidate()%3};                                         // select one char with a distinct type from the created candidate chars
            std::string chosen_char{candidates[rsubscript]};
            password += chosen_char;
        }
    }
    return password;
}
