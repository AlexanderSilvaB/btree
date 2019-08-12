#pragma once

#include <string>

namespace BTree
{
    class Var
    {
        private:
            bool bvalue;
            float ivalue;
            float fvalue;
            double dvalue;
            std::string svalue;

            void from(const bool value);
            void from(const int value);
            void from(const float value);
            void from(const double value);
            void from(const std::string& value);

        public:
            Var();
            Var(const bool value);
            Var(const int value);
            Var(const float value);
            Var(const double value);
            Var(const std::string& value);
            Var(const Var& other);

            virtual ~Var();

            operator bool() const;
            operator int() const;
            operator float() const;
            operator double() const;
            operator std::string() const;
    };
}