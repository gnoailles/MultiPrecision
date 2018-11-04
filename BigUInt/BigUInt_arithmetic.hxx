#pragma once

#pragma region Addition
template <unsigned BitCount>
BigUInt<BitCount> BigUInt<BitCount>::operator+(const uint64_t other) const
{
    BigUInt<BitCount> res(*this);
    res.number[0] = number[0] + other;
    //Propagate overflow
    for (int i = 0; res.number[i] < number[i] || res.number[i] < other && i < ULL_COUNT - 1; ++i)
    {
        res.number[i + 1] = number[i + 1] + 1;
    }
    return res;
}

template <unsigned BitCount>
BigUInt<BitCount> BigUInt<BitCount>::operator+(const BigUInt other) const
{
    BigUInt<BitCount> res;
    uint8_t overflow = 0;
    for (int i = 0; i < ULL_COUNT; ++i)
    {
        res.number[i] = number[i] + other.number[i] + overflow;
        overflow = 0;
        if (res.number[i] < number[i] || res.number[i] < other.number[i])
            overflow = 1;
    }
    return res;
}

template <unsigned BitCount>
BigUInt<BitCount>& BigUInt<BitCount>::operator+=(const uint64_t other)
{
    uint64_t add = number[0] + other;
    //Propagate overflow
    int i;
    for (i = 0; add < number[i] || add < other && i < ULL_COUNT - 1; ++i)
    {
        number[i] = add;
        add = number[i + 1] + 1;
    }
    number[i] = add;
    return *this;
}

template <unsigned BitCount>
BigUInt<BitCount>& BigUInt<BitCount>::operator+=(const BigUInt other)
{
    uint8_t overflow = 0;
    for (int i = 0; i < ULL_COUNT; ++i)
    {
        uint64_t add = number[i] + other.number[i] + overflow;
        overflow = 0;
        if (add < number[i] || add < other.number[i])
            overflow = 1;

        number[i] = add;
    }
    return *this;
}

#pragma endregion 

#pragma region Subtraction
template <unsigned BitCount>
BigUInt<BitCount> BigUInt<BitCount>::operator-(const uint64_t other) const
{
    if (other == 0)
        return *this;
    BigUInt<BitCount> result = *this;
    result.number[0] -= other;
    return result;
}

template <unsigned BitCount>
BigUInt<BitCount> BigUInt<BitCount>::operator-(const BigUInt& other) const
{
    if (other.IsZero())
        return *this;
    BigUInt<BitCount> invertOther = ~other;
    invertOther += 1;
    return *this + invertOther;
}

template <unsigned BitCount>
BigUInt<BitCount>& BigUInt<BitCount>::operator-=(const BigUInt& other)
{
    if (other.IsZero())
        return *this;
    BigUInt<BitCount> invertOther = ~other;
    invertOther += 1;
    *this += invertOther;
    return *this;
}
#pragma endregion 

#pragma region Multiplication
template <unsigned BitCount>
BigUInt<BitCount> BigUInt<BitCount>::operator*(BigUInt b) const
{
    if (b.IsZero())
        return b;

    BigUInt<BitCount> a = *this;
    BigUInt<BitCount> result;

    while (!a.IsZero())
    {
        if (a.IsOdd())
            result += b;

        a.RightShift();
        b.LeftShift();
    }

    return result;
}

template <unsigned BitCount>
BigUInt<BitCount>& BigUInt<BitCount>::operator*=(BigUInt b)
{
    if (b.IsZero())
    {
        memset(number, 0, BitCount / 8);
        return *this;
    }

    BigUInt<BitCount> a = *this;
    BigUInt<BitCount> result;

    while (!a.IsZero())
    {
        if (a.IsOdd())
            result += b;

        a.RightShift();
        b.LeftShift();
    }

    *this = result;
    return *this;
}
#pragma endregion 

#pragma region Division
template <unsigned int BitCount>
template <unsigned int OtherBitCount, unsigned int Remainder>
BigUInt<BitCount> BigUInt<BitCount>::Divide(const BigUInt<OtherBitCount>& b, BigUInt<Remainder>& r) const
{
    if(b.IsZero())
        throw std::overflow_error("Divide by zero exception");
    BigUInt<BitCount> divisor = b;
    BigUInt quotient;
    r = *this;

    if (divisor.Compare(r) > 0)
        return 0;
    while (divisor.Compare(r) <= 0 && !b.number[b.ULL_COUNT - 1] & ((uint64_t)1 << 63) )
    {
        divisor.LeftShift();
    }
    
    divisor.RightShift();
    
    while (r.Compare(b) >= 0)
    {
        if(r.Compare(divisor) >= 0)
        {
            r -= b;
            quotient.number[0] |= 1;
        }
        quotient.LeftShift();
        divisor.RightShift();
    }
    return quotient;
}

template <unsigned int BitCount>
template<unsigned int OtherBitCount>
BigUInt<BitCount> BigUInt<BitCount>::operator/(BigUInt<OtherBitCount> b) const
{
    BigUInt r;
    return Divide(b,r);
}
template <unsigned int BitCount>
template<unsigned int OtherBitCount>
BigUInt<BitCount>& BigUInt<BitCount>::operator/=(BigUInt<OtherBitCount> b)
{
    BigUInt r;
    *this = Divide(b,r);
    return *this;
}

template <unsigned int BitCount>
template<unsigned int OtherBitCount>
BigUInt<BitCount> BigUInt<BitCount>::operator%(BigUInt<OtherBitCount> b) const
{
    BigUInt r;
    Divide(b,r);
    return r;
}

template <unsigned int BitCount>
template<unsigned int OtherBitCount>
BigUInt<BitCount>& BigUInt<BitCount>::operator%=(BigUInt<OtherBitCount> b)
{
    BigUInt r;
    Divide(b,r);
    *this = r;
    return *this;
}
#pragma endregion 

#pragma region Exponent
template <unsigned BitCount>
BigUInt<BitCount> BigUInt<BitCount>::Power(uint64_t power) const
{
    BigUInt result;
    result += 1;
    BigUInt a = *this;


    while (power != 0)
    {
        const bool isOdd = power % 2;
        power >>= 1;

        if (isOdd)
            result *= a;

        a *= a;
    }

    return result;
}

template <unsigned BitCount>
BigUInt<BitCount> BigUInt<BitCount>::Power(BigUInt power) const
{
    BigUInt result;
    BigUInt a = *this;
    result += 1;


    while (!power.IsZero())
    {
        const bool isOdd = power.IsOdd();
        power.RightShift();

        if (isOdd)
            result *= a;

        a *= a;
    }

    return result;
}
#pragma endregion 