#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "StringtoDouble.h"

using namespace std;

int main()
{
    vector<string> myinputs = {"0.120", "123.345", "-123.345", "12.123e-0021",
                               "890.123e123", "-1e320", "1e-312", "    .2131",
                               "123 . 345"};

    for(auto s : myinputs){
        cout << "input " << left << setw(15) << s;
        double result = stod(s);
        cout << " result " << result << endl;
    }
    return 0;
}

// references
// http://sandbox.mc.edu/~bennet/cs110/flt/dtof.html
// https://ryanstutorials.net/binary-tutorial/binary-floating-point.php

// http://krashan.ppa.pl/articles/stringtofloat/
// https://github.com/grzegorz-kraszewski/stringtofloat/blob/master/str2dbl.c

