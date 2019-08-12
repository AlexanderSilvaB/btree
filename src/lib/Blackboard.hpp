#pragma once

#include <unordered_map>
#include <string>
#include "Var.hpp"

namespace BTree
{
    typedef std::unordered_map< std::string, Var > DataSet;

    class Blackboard
    {
        private:
            DataSet data;
        public:
            Blackboard();
            virtual ~Blackboard();

            DataSet getData();

            Var& at(const std::string& key);
            bool has(const std::string& key);

            Var& operator[](const std::string& key);
    };
}