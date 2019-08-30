#pragma once

#include "NodeStates.hpp"
#include "NodeTypes.hpp"
#include "Blackboard.hpp"
#include <functional>
#include <list>
#include <memory>
#include <utility>
#include <string>

namespace btree
{
    class Node;
    typedef std::shared_ptr<Node> NodePtr;
    struct NodeDims
    {
        int w, h;
    };

    struct NodeShape
    {
        int x, y, w, h;
        std::string name;
        NodeTypes type;
        NodeStates state;
        std::list< struct NodeShape > nodes;
    };

    typedef std::function<void (Blackboard&)> CallFunc;
    typedef std::function<NodeStates (Blackboard&)> ActionFunc;

    class Node
    {
        protected:
            bool initialized;
            std::string id;
            std::string name;
            int counter;
            NodeStates nodeState;
            NodeTypes nodeType;
            CallFunc handlerCall;
            ActionFunc handlerAction;
            std::list< NodePtr > nodes;

            NodeStates evaluateAction(Blackboard& blackboard);
            NodeStates evaluateSelector(Blackboard& blackboard);
            NodeStates evaluateSequence(Blackboard& blackboard);
            NodeStates evaluateInverter(Blackboard& blackboard);
            NodeStates evaluateRandom(Blackboard& blackboard);
            NodeStates evaluateSucceeder(Blackboard& blackboard);
            NodeStates evaluateRepeater(Blackboard& blackboard);
            NodeStates evaluateUntil(Blackboard& blackboard);
            NodeStates evaluateParallel(Blackboard& blackboard);
        
        public:
            Node();
            virtual ~Node();
            
            NodeStates state();
            NodeTypes type();
            NodePtr child();

            NodePtr find(const std::string& id);
            NodePtr findByName(const std::string& name);

            void setId(const std::string& id);
            std::string getId();

            void setName(const std::string& name);
            std::string getName();

            void setCounter(int value);
            int getCounter();

            int index(NodePtr node);
            NodePtr add(); 
            NodePtr at(int index);
            NodePtr insert(int pos);
            NodePtr insertAfter(NodePtr node);
            NodePtr insertBefore(NodePtr node);
            void remove(NodePtr node); 
            int size();

            void asAction(std::function<NodeStates (Blackboard&)> handler);
            void asCall(std::function<void (Blackboard&)> handler);
            void asSelector();
            void asSequence();
            void asParallel();
            void asRandom();
            NodePtr asInverter();
            NodePtr asSucceeder();
            NodePtr asRepeater();
            NodePtr asUntil();

            struct NodeDims measure(int width = 100, int height = 60, int margin = 10);
            struct NodeShape place(int x, int y, int width = 100, int height = 60, int margin = 10);

            NodeStates evaluate(Blackboard& blackboard);
    };
}