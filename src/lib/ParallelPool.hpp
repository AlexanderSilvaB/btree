#pragma once

#include "Node.hpp"
#include <thread>
#include <list>

namespace btree
{
    class Task
    {
        private:
            std::thread task;
            NodePtr node;
            Blackboard *blackboard;
            void run();
        public:
            Task(NodePtr node, Blackboard *blackboard);
            void wait();

    };

    class ParallelPool
    {
        private:
            bool running;
            std::list< NodePtr > nodes;
            std::list< Task > tasks;
        public:
            ParallelPool();
            virtual ~ParallelPool();

            void attach(NodePtr node);
            void clear();

            void start(Blackboard& blackboard);
            NodeStates wait();
    };
}