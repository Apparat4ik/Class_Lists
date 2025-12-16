#pragma once

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

bool is_file_empty(const string& filename) {
    ifstream file(filename);
    char first_char;
    file >> first_char;
    bool empty = file.eof();
    file.close();

    return empty;
}
