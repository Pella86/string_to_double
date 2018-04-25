#include "StringtoDouble.h"

/*******************************************************************************
 Constants
*******************************************************************************/

#define DPOINT '.'
#define DIGITS 18 //significant digits of the number (mantissa)

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif // INT_MAX

#define DOUBLE_PLUS_ZERO          0x0000000000000000ULL
#define DOUBLE_MINUS_ZERO         0x8000000000000000ULL
#define DOUBLE_PLUS_INFINITY      0x7FF0000000000000ULL
#define DOUBLE_MINUS_INFINITY     0xFFF0000000000000ULL

/*******************************************************************************
 Helper functions
*******************************************************************************/

// 0x09 : 0x13 \t\n\r... 0x20= ' '
inline bool is_space(const char& x){
    return (x >= 0x09 && x <= 0x13) || x == 0x20;
}

inline bool is_digit(const char& x){
    return x >= '0' && x <= '9';
}

inline bool is_exp(const char& x){
    return (x == 'e') || (x == 'E');
}

// scrolls the pointer
inline char GETC(std::string::iterator& s){ return *s++; }

/*******************************************************************************
 prepNumber
*******************************************************************************/

prepNumber::prepNumber() : negative(false), exponent(0), mantissa(0) {}

/*******************************************************************************
 Parser
*******************************************************************************/

Parser::Parser(){}

Parser::get_machine_state(){return state;}
Parser::get_parser_state() {return parser_state;}

prepNumber Parser::run(std::string init_string){
    std::string::iterator s = init_string.begin();

    parser_state = PARSER_OK; // reset the result
    state = FSM_A; // set the state to A to enter the cycle
    int c = ' '; // set c to ' ' to trigger getc.
    int digx  = 0;
    bool expneg = false;
    int32_t expexp = 0;
    prepNumber pn;

    while(state != FSM_STOP){
        switch(state){
            // start the cycle and fetch until c is not a space, so go to stateB
            case FSM_A:
                if(is_space(c)) c = GETC(s);
                else state = FSM_B;
            break;

            // fetches + or - or if is digit goes directly to C or if is a point
            case FSM_B:
                state = FSM_C;

                if      (c == '+') c = GETC(s);
                else if (c == '-') {
                    pn.negative = true;
                    c = GETC(s);
                }
                else if (is_digit(c)){
                    // go to state c
                }
                else if (c == DPOINT){
                    // go to state c
                }
                else{
                    state = FSM_STOP;
                }

            break;

            // decide if to go for the fractional part or the integer part
            // fracional part is D
            // integer part is E
            case FSM_C:
                if(c == '0'){
                    c = GETC(s);
                    // continue removing leading zeros
                }
                else if( c == DPOINT){
                    c = GETC(s);
                    state = FSM_D;
                }
                else{
                    state = FSM_E;
                }
            break;

            // this reads the 0s after the . and decreases the exp.
            case FSM_D:

                if(c == '0'){
                   c = GETC(s);
                   if(pn.exponent > -INT_MAX) pn.exponent--;
                }
                else{
                    state = FSM_F;
                }
            break;

            // read the mantissa int part
            // 18 digits precision, and increase exponent if the number is
            // longer than 18 digits
            case FSM_E:
                if(is_digit(c)){
                    if(digx < DIGITS){
                        pn.mantissa *= 10;
                        pn.mantissa += c - '0';
                        digx++;
                    }
                    else if(pn.exponent < INT_MAX){
                        pn.exponent++;
                    }

                    c = GETC(s);
                }
                else if(c == DPOINT){
                    c = GETC(s);
                    state = FSM_F;
                }
                else{
                    state = FSM_F;
                }
            break;

            // read the fractional part
            case FSM_F:
                 if(is_digit(c)){
                    if(digx < DIGITS){
                        pn.mantissa *= 10;
                        pn.mantissa += c - '0';
                        pn.exponent--;
                        digx++;
                    }

                    c = GETC(s);
                }
                else if(is_exp(c)){
                    c = GETC(s);
                    state = FSM_G;
                }
                else{
                    state = FSM_G;
                }
            break;

            // read the exponent sign
            case FSM_G:
                if(c == '+'){c = GETC(s);} // skip the exp +
                else if (c == '-'){
                    expneg = true;
                    c = GETC(s);
                }
                state = FSM_H;
            break;

            // skip exponent eventual 0s
            case FSM_H:
                if(c == '0'){c = GETC(s);}
                else{
                    state = FSM_I;
                }
            break;

            // read the exponent part
            case FSM_I:
                if(is_digit(c)){
                    if(expexp < INT_MAX){
                        expexp *= 10;
                        expexp += c - '0';
                    }
                    c = GETC(s);
                }
                else{
                    state = FSM_STOP;
                }
            break;
        }
    }

    if(expneg) {expexp = -expexp;}
    pn.exponent += expexp;

    // if mantissa is 0 or result < 10e-328 (double limit)
    if(pn.mantissa == 0 || pn.exponent < -328){
        parser_state = ((pn.negative)? PARSER_MZERO : PARSER_PZERO);
    }

    if(pn.exponent > 309){
        parser_state = ((pn.negative)? PARSER_MINF : PARSER_PINF);
    }

    return pn;
}

/*******************************************************************************
 converter
*******************************************************************************/

// helps conversion between uint64 and double
class HexDouble{
public:

    HexDouble(){u = 0;}

    void set(uint64_t n){u = n;}

    double get(){return d;}

private:
    union{
        double d;
        uint64_t u;
    };
};


double converter(prepNumber pn){

    int bin_exp = 92;
    int_96 r;
    int_96 s((uint64_t)pn.mantissa);

    // multiply by 10 till significand get to the left most bit
    while(pn.exponent > 0){

        // multiplication by 10 (2^2 * 2^8) = (n<<1 + n<<3)
        int_96 q = (s << 1) + (s << 3);
        pn.exponent--;

        //if the multiplication overflows in the last 4 of the 96 bits
        // then increase the binary exponent
        while(q.get_last_bits()){
            q >>= 1;
            bin_exp++;
        }
    }

    // divide by 10 to get the significand to the left most bit
    while(pn.exponent < 0){

        while(!s.get_last_bit()){
            s <<= 1;
            bin_exp--;
        }

        // divide by 10
        s.divide_by_10();
        pn.exponent++;
    }

    // correct for the last 4 bits;
    if(s != 0){
        while(!s.get_last_bits()){
            s<<=1;
            bin_exp--;
        }
    }

    // prepare the number <uint64 to double>
    HexDouble hd;

    //boundary checking
    bin_exp += 1023; // addin bias

    if(bin_exp > 2046){
        hd.set((pn.negative)? DOUBLE_MINUS_INFINITY : DOUBLE_PLUS_INFINITY);
    }
    else if(bin_exp < 1){
        hd.set((pn.negative)? DOUBLE_MINUS_ZERO : DOUBLE_PLUS_ZERO);
    }
    else if(s != 0){
        uint64_t binexs2 = (uint64_t) bin_exp;
        binexs2 <<= 52;

        // construct complete mantissa
        uint64_t signbit = (pn.negative)? (1ULL << 63) : 0;
        uint64_t q = signbit | binexs2 | s.get_mantissa();
        hd.set(q);
    }

    return hd.get();
}
/*******************************************************************************
 stod
*******************************************************************************/

double stod(std::string init_string){
    double result;
    Parser parser;

    prepNumber pn = parser.run(init_string);

    switch(parser.get_parser_state()){
        case parser.PARSER_OK:
            result = converter(pn);
        break;

        case parser.PARSER_PZERO:
            result = 0;
        break;

		case parser.PARSER_MZERO:
			result = DOUBLE_MINUS_ZERO;
		break;

		case parser.PARSER_PINF:
			result = DOUBLE_PLUS_INFINITY;
		break;

		case parser.PARSER_MINF:
			result = DOUBLE_MINUS_INFINITY;
		break;
    }

    return result;
}

