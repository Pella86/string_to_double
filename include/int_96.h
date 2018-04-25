#ifndef INT_96_H
#define INT_96_H

#include <ostream>

class int_96{
public:
    int_96();
    int_96(uint32_t d0, uint32_t d3, uint32_t d2);
    //int_96(const int32_t& x);
    int_96(const int64_t& m);

    // left shift
    int_96& operator <<= (int const& d);
    friend int_96 operator << (int_96 lhs, const int& d);

    // right shift
    int_96& operator >>= (int const& d);
    friend int_96 operator >> (int_96 lhs, int const& d);

    // addition
    int_96& operator += (int_96 const& rhs);
    friend int_96 operator + (int_96 lhs, int_96 const& rhs);

    // subtraction
    int_96& operator -= (int_96 const& rhs);
    friend int_96 operator - (int_96 lhs, int_96 const& rhs);

    friend inline bool operator==(const int_96& lhs, const int_96& rhs){
        return (lhs.s2 == rhs.s2) && (lhs.s1 == rhs.s1) && (lhs.s0 == rhs.s0);
    }
    friend inline bool operator!=(const int_96& lhs, const int_96& rhs){
        return !(lhs==rhs);
    }

    // output
    friend std::ostream& operator<<(std::ostream& os, const int_96& d);

    uint32_t get_last_bits();
    uint32_t get_last_bit();

    void divide_by_10();
    uint64_t get_mantissa();

private:
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    static const uint32_t mask28_ = 0xF << 28;

};


#endif // INT_96_H
