#ifndef PASSGENMAINLOGIC_H
#define PASSGENMAINLOGIC_H

#include <iostream>
#include <string>
#include <random>
#include <cmath>
#include <cstdlib>
#include <string>
#include <string.h>

/* Symbolic constants */
#define MIN_PASS_LEN 12
#define MAX_PASS_LEN 100

/* Global variables */
bool filled[MAX_PASS_LEN];

/* Function declarations */
bool isdigit(std::string c);
bool isalphabetic(std::string c);
void show_help();
void fill_arr_zero(bool arr[], int len);
void fill_bytwo(bool arr[], int pos);
std::string generatePassword(int len, int year, std::string color);

/* Class declaration */
class Rand_int {
private:
    std::random_device rd;
    std::default_random_engine engine;
    std::uniform_int_distribution<int> distrib;
public:
    // class constructor
    Rand_int(int low, int high);
    int operator()();
};

#endif // PASSGENMAINLOGIC_H
