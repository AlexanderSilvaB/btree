#include "Blackboard.hpp"

using namespace btree;
using namespace std;

Blackboard::Blackboard()
{

}

Blackboard::~Blackboard()
{

}

DataSet Blackboard::getData()
{
    return data;
}

Var& Blackboard::at(const std::string& key)
{
    return data.at(key);
}

bool Blackboard::has(const std::string& key)
{
    return data.find(key) != data.end();
}

Var& Blackboard::operator[](const std::string& key)
{
    return data[key];
}