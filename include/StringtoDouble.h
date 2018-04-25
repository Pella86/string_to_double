#ifndef STRINGTODOUBLE_H
#define STRINGTODOUBLE_H

#include <string>
#include <inttypes.h>
#include "int_96.h"

/*******************************************************************************
 prepNumber
    helper class storing the x * 10 ^ (y) number
*******************************************************************************/

class prepNumber{
public:

    prepNumber();

    bool negative; // negative flag for the negative
    int32_t exponent; // store the exponent of the numnber v= 10^e * x  e (-327:308)
    uint64_t mantissa; // x in the above form

};

/*******************************************************************************
 Parser
    parses the string into prepNumber
*******************************************************************************/

class Parser{
public:
    Parser();


    prepNumber run(std::string init_string);

    int get_machine_state();
    int get_parser_state();

    static int const PARSER_OK = 0;
    static int const PARSER_PZERO = 1;
    static int const PARSER_MZERO = 2;
    static int const PARSER_PINF = 3;
    static int const PARSER_MINF = 4;

private:
/*state machine states*/
    static int const FSM_A = 0;
    static int const FSM_B = 1;
    static int const FSM_C = 2;
    static int const FSM_D = 3;
    static int const FSM_E = 4;
    static int const FSM_F = 5;
    static int const FSM_G = 6;
    static int const FSM_H = 7;
    static int const FSM_I = 8;
    static int const FSM_STOP = 9;



    int state;
    int parser_state;
};

/*******************************************************************************
 conveerter
    converts the number into the double
*******************************************************************************/

double converter(prepNumber pn);

/*******************************************************************************
 stod
    wrapping function dealing with parse errors
*******************************************************************************/

double stod(std::string init_string);

#endif // STRINGTODOUBLE_H
