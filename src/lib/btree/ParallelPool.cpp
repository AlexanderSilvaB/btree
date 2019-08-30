#include "ParallelPool.hpp"

using namespace btree;
using namespace std;

// Task
Task::Task(NodePtr node, Blackboard *blackboard)
{
    this->node = node;
    this->blackboard = blackboard;
    task = thread(&Task::run, this);
}

void Task::wait()
{
    task.join();
}

void Task::run()
{
    node->evaluate(*blackboard);
}


// ParallelPool
ParallelPool::ParallelPool()
{
    running = false;
}

ParallelPool::~ParallelPool()
{

}

void ParallelPool::attach(NodePtr node)
{
    nodes.push_back(node);
}

void ParallelPool::clear()
{
    if(running)
        return;
    nodes.clear();
    tasks.clear();
}

void ParallelPool::start(Blackboard& blackboard)
{
    if(running)
        return;
    
    tasks.clear();

    running = true;
    for(list< NodePtr >::iterator it = nodes.begin(); it != nodes.end(); it++)
    {
        tasks.push_back( Task(*it, &blackboard) );
    }
}

NodeStates ParallelPool::wait()
{
    for(list< Task >::iterator it = tasks.begin(); it != tasks.end(); it++)
    {
        it->wait();
    }
    running = false;
    return SUCCESS;
}