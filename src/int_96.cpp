#include "int_96.h"

/*******************************************************************************
  Constructors
*******************************************************************************/

int_96::int_96() : s0(0), s1(0), s2(0) {}

int_96::int_96(uint32_t d0, uint32_t d1, uint32_t d2){
    s0 = d0;
    s1 = d1;
    s2 = d2;
}

int_96::int_96(const int64_t& m){
    s0 = (uint32_t)(m & 0xFFFFFFFF);
    s1 = (uint32_t)(m >> 32);
    s2 = 0;
}

/*******************************************************************************
 Operators
*******************************************************************************/

int_96& int_96::operator >>= (const int& d){
    for(int i = 0; i < d; ++i){
        // bitshift by 1
        uint32_t d0 = (s0 >> 1 ) | ((s1 & 1) << 31);
        uint32_t d1 = (s1 >> 1) | ((s2 & 1) << 31);
        uint32_t d2 = s2 >> 1;

        this->s0 = d0;
        this->s1 = d1;
        this->s2 = d2;
    }
    return *this;
}

int_96 operator >> (int_96 lhs, const int& d){
    lhs >>= d;
    return lhs;
}

int_96& int_96::operator <<= (const int& d){
    for(int i = 0; i < d; ++i){
        uint32_t d2 = (s2 << 1) | ((s1 & (1 << 31)) >> 31);
        uint32_t d1 = (s1 << 1) | ((s0 & (1 << 31)) >> 31);
        uint32_t d0 = s0 << 1;

        this->s0 = d0;
        this->s1 = d1;
        this->s2 = d2;
    }
    return *this;
}

int_96 operator << (int_96 lhs, int const& d){
    lhs <<= d;
    return lhs;
}

int_96& int_96::operator+=(int_96 const& rhs){
    long long w;
    w =  (long long)(s0) + (long long)(rhs.s0);
    s0 = w;
    w >>= 32;
    w += (long long)(s1) + (long long)(rhs.s1);
    s1 = w;
    w >>= 32;
    w += (long long)(s2) + (long long)(rhs.s2);
    s2 = w;
    return *this;
}

int_96 operator+ (int_96 lhs, int_96 const& rhs){
    lhs += rhs;
    return lhs;
}

int_96& int_96::operator -= (int_96 const& rhs){
    long long w;
    w =  (long long)(s0) - (long long)(rhs.s0);
    s0 = w;
    w >>= 32;
    w += (long long)(s1) - (long long)(rhs.s1);
    s1 = w;
    w >>= 32;
    w += (long long)(s2) - (long long)(rhs.s2);
    s2 = w;

    return *this;
}

int_96 operator - (int_96 lhs, int_96 const& rhs){
    lhs -= rhs;
    return lhs;
}

/*******************************************************************************
 Stream Operators
*******************************************************************************/

std::string to_binary(long long int x){
    std::string res = ""; // resulting string
    long long unsigned int mask = 1; // mask
    bool neg = false; // sign bit

    // transform in unsigned int
    if(x < 0){
        x *= -1;
        neg = true;
    }

    uint64_t a = (x & ( (long long int)0xFFFFFFFF << 32));
    unsigned int word_size = ( a > 0)? 64 : 32;

    // go bit by bit and append a 1 to the string or a 0
    for(unsigned int j = 0; j < word_size; ++j){
        res += ( (x & mask) > 0 )? "1" : "0";
        mask <<= 1;
    }

    // represent it in big endian
    std::string rev = "";
    for(size_t i = 0; i < res.size(); ++i){
        rev += res[res.size() - i - 1];
    }

    // switch the sign
    res[0] = (neg)? '1' : '0';

    return res;
}

std::ostream& operator<<(std::ostream& os, const int_96& d){
    os << "s0: " << d.s0 << " |" << to_binary(d.s0) << std::endl;
    os << "s1: " << d.s1 << " |" << to_binary(d.s1) << std::endl;
    os << "s2: " << d.s2 << " |" << to_binary(d.s2);
    return os;

}

/*******************************************************************************
 Special functions for conversion
*******************************************************************************/

uint32_t int_96::get_last_bits(){
    return s2 & mask28_;
}

uint32_t int_96::get_last_bit(){
    uint32_t mask = 0x80000000;
    return s2 & mask;
}

void int_96::divide_by_10(){
        uint32_t q2 = s2 / 10;
		uint32_t r1 = s2 % 10;
		uint32_t r2 = (s1 >> 8) | (r1 << 24);
		uint32_t q1 = r2 / 10;
		r1 = r2 % 10;
		r2 = ((s1 & 0xFF) << 16) | (s0 >> 16) | (r1 << 24);
		uint32_t r0 = r2 / 10;
		r1 = r2 % 10;
		q1 = (q1 << 8) | ((r0 & 0x00FF0000) >> 16);
		uint32_t q0 = r0 << 16;
		r2 = (s0 & 0xFFFF) | (r1 << 16);
		q0 |= r2 / 10;
		s2 = q2;
		s1 = q1;
		s0 = q0;
}

uint64_t int_96::get_mantissa(){
    //the left part of the mantissa
    uint64_t a = (uint64_t) (s2 & ~mask28_) << 24;
    // the right part of the mantissa rounded
    uint64_t b = ((uint64_t)s1 + 128) >> 8;
    // final mantissa in stacked with the right 52 bits
    return a | b;
}
