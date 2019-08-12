#include "Var.hpp"
#include <iostream>
#include <sstream>

using namespace BTree;
using namespace std;

Var::Var()
{
    from(false);
}

Var::Var(const bool value)
{
    from(value);
}

Var::Var(const int value)
{
    from(value);
}

Var::Var(const float value)
{
    from(value);
}

Var::Var(const double value)
{
    from(value);
}

Var::Var(const string& value)
{
    from(value);
}

Var::Var(const Var& value)
{
    bvalue = value.bvalue;
    ivalue = value.ivalue;
    fvalue = value.fvalue;
    dvalue = value.dvalue;
    svalue = value.svalue;
}

Var::~Var()
{

}

Var::operator bool() const
{
    return bvalue;
}

Var::operator int() const
{
    return ivalue;
}

Var::operator float() const
{
    return fvalue;
}

Var::operator double() const
{
    return dvalue;
}

Var::operator string() const
{
    return svalue;
}

void Var::from(const bool value)
{
    bvalue = value;
    ivalue = value ? 1 : 0;
    fvalue = value ? 1.0f : 0.0f;
    dvalue = value ? 1.0 : 0.0;
    svalue = value ? "true" : "false";
}

void Var::from(const int value)
{
    bvalue = value > 0;
    ivalue = value;
    fvalue = (float)value;
    dvalue = (double)value;

    stringstream ss;
    ss << value;
    svalue = ss.str();
}

void Var::from(const float value)
{
    bvalue = value > 0;
    ivalue = (int)value;
    fvalue = value;
    dvalue = (double)value;

    stringstream ss;
    ss << value;
    svalue = ss.str();
}

void Var::from(const double value)
{
    bvalue = value > 0;
    ivalue = (int)value;
    fvalue = (float)value;
    dvalue = value;

    stringstream ss;
    ss << value;
    svalue = ss.str();
}

void Var::from(const string& value)
{
    bvalue = value == "true" || value == "TRUE" || value == "1";

    stringstream ssi(value);
    ssi >> ivalue;

    stringstream ssf(value);
    ssi >> fvalue;

    stringstream ssd(value);
    ssi >> dvalue;

    svalue = value;
}