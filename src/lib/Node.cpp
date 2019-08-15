#include "Node.hpp"
#include "ParallelPool.hpp"

using namespace btree;
using namespace std;

Node::Node()
{
    nodeState = SUCCESS;
    nodeType = LEAF;
    counter = -1;
    initialized = false;
}

Node::~Node()
{

}

NodeStates Node::state()
{
    return nodeState;
}

NodeTypes Node::type()
{
    return nodeType;
}

NodePtr Node::child()
{
    return nodes.front();
}


void Node::setName(const std::string& name)
{
    this->name = name;
}

std::string Node::getName()
{
    return name;
}

void Node::setCounter(int value)
{
    counter = value;
}

int Node::getCounter()
{
    return counter;
}

NodePtr Node::add()
{
    nodes.push_back( make_shared<Node>() );
    return nodes.back();
}

NodePtr Node::at(int index)
{
    if(index >= nodes.size())
        return NodePtr();

    list< NodePtr >::iterator it = nodes.begin();
    advance(it, index);
    return *it;
}
void Node::remove(NodePtr node)
{
    nodes.remove(node);
}

void Node::asAction(function<NodeStates (Blackboard&)> handler)
{
    this->handlerAction = handler;
    nodeType = ACTION;
}

void Node::asCall(function<void (Blackboard&)> handler)
{
    this->handlerCall = handler;
    nodeType = CALL;
}

void Node::asSelector()
{
    nodeType = SELECTOR;
}

void Node::asSequence()
{
    nodeType = SEQUENCE;
}

void Node::asParallel()
{
    nodeType = PARALLEL;
}

void Node::asRandom()
{
    nodeType = RANDOM;
}

NodePtr Node::asInverter()
{
    nodeType = INVERTER;
    if(nodes.size() == 0)
    {
        nodes.push_back( make_shared<Node>() );
    }
    return nodes.front();
}

NodePtr Node::asSucceeder()
{
    nodeType = SUCCEEDER;
    if(nodes.size() == 0)
    {
        nodes.push_back( make_shared<Node>() );
    }
    return nodes.front();
}

NodePtr Node::asRepeater()
{
    nodeType = REPEATER;
    if(nodes.size() == 0)
    {
        nodes.push_back( make_shared<Node>() );
    }
    return nodes.front();
}

NodePtr Node::asUntil()
{
    nodeType = UNTIL;
    if(nodes.size() == 0)
    {
        nodes.push_back( make_shared<Node>() );
    }
    return nodes.front();
}

NodeStates Node::evaluate(Blackboard& blackboard)
{
    switch (nodeType)
    {
        case LEAF:
            nodeState = SUCCESS;
            break;
        case CALL:
            if(handlerCall)
                handlerCall(blackboard);
            nodeState = SUCCESS;
            break;
        case ACTION:
            nodeState = evaluateAction(blackboard);
            break;
        case RANDOM:
            nodeState = evaluateRandom(blackboard);
            break;
        case SELECTOR:
            nodeState = evaluateSelector(blackboard);
            break;
        case SEQUENCE:
            nodeState = evaluateSequence(blackboard);
            break;
        case PARALLEL:
            nodeState = evaluateParallel(blackboard);
            break;
        case INVERTER:
            nodeState = evaluateInverter(blackboard);
            break;
        case SUCCEEDER:
            nodeState = evaluateSucceeder(blackboard);
            break;
        case REPEATER:
            nodeState = evaluateRepeater(blackboard);
            break;
        case UNTIL:
            nodeState = evaluateUntil(blackboard);
            break;
        default:
            nodeState = FAILURE;
            break;
    }
    return nodeState;
}

NodeStates Node::evaluateAction(Blackboard& blackboard)
{
    if(!handlerAction)
        return FAILURE;

    switch (handlerAction(blackboard)) 
    { 
        case SUCCESS: 
            return SUCCESS; 
        case FAILURE: 
            return FAILURE; 
        case RUNNING: 
            return RUNNING; 
        default:
            break;
    } 
    return FAILURE; 
}

NodeStates Node::evaluateSelector(Blackboard& blackboard)
{
    for(list< NodePtr >::iterator it = nodes.begin(); it != nodes.end(); it++)
    {
        switch ((*it)->evaluate(blackboard))
        {
            case FAILURE:
                continue;
            case SUCCESS:
                return SUCCESS;
            case RUNNING:
                return RUNNING;
            default:
                continue;
        }
    }
    return FAILURE;
}

NodeStates Node::evaluateSequence(Blackboard& blackboard)
{
    bool anyChildRunning = false;
    for(list< NodePtr >::iterator it = nodes.begin(); it != nodes.end(); it++)
    {
        switch ((*it)->evaluate(blackboard))
        {
            case FAILURE:
                return FAILURE;
            case SUCCESS:
                continue;
            case RUNNING:
                anyChildRunning = true;
                continue;
            default:
                return SUCCESS;
        }
    }
    return anyChildRunning ? RUNNING : SUCCESS;
}

NodeStates Node::evaluateParallel(Blackboard& blackboard)
{
    ParallelPool pool;
    for(list< NodePtr >::iterator it = nodes.begin(); it != nodes.end(); it++)
        pool.attach(*it);
    
    pool.start(blackboard);
    return pool.wait();
}

NodeStates Node::evaluateRandom(Blackboard& blackboard)
{
    if(nodes.size() == 0)
        return SUCCESS;

    int index = rand() % nodes.size();
    return at(index)->evaluate(blackboard);
}

NodeStates Node::evaluateInverter(Blackboard& blackboard)
{
    switch (child()->evaluate(blackboard)) 
    { 
        case FAILURE: 
            return SUCCESS; 
        case SUCCESS: 
            return FAILURE; 
        case RUNNING: 
            return RUNNING; 
        default:
            break;
    } 
    return SUCCESS; 
}

NodeStates Node::evaluateSucceeder(Blackboard& blackboard)
{
    child()->evaluate(blackboard);
    return SUCCESS; 
}

NodeStates Node::evaluateRepeater(Blackboard& blackboard)
{
    NodeStates state;
    if(counter < 0)
    {
        while(true)
        {
            state = child()->evaluate(blackboard);
        }
    }
    else
    {
        for(int i = 0; i < counter; i++)
        {
            state = child()->evaluate(blackboard);
        }
    }
    
    return SUCCESS; 
}

NodeStates Node::evaluateUntil(Blackboard& blackboard)
{
    NodeStates state;
    do
    {
        state = child()->evaluate(blackboard);
    }while(state != FAILURE);
    return SUCCESS; 
}

struct NodeDims Node::measure(int width, int height, int margin)
{
    NodeDims dims, tmp;
    dims.w = 0;
    dims.h = 0;

    switch (nodeType)
    {
        case LEAF:
        case CALL:
        case ACTION:
            dims.w = width + margin;
            dims.h = height + margin;
            break;
        case RANDOM:
        case SEQUENCE:
        case SELECTOR:
        case PARALLEL:
        {
            dims.w = 0;
            dims.h = height + margin;
            if(nodes.size() > 0)
                dims.h += margin;
            else
                dims.w = width + margin;

            int n = 0;
            int h = 0;
            for(list< NodePtr >::iterator it = nodes.begin(); it != nodes.end(); it++)
            {
                tmp = (*it)->measure(width, height, margin);
                dims.w += tmp.w;
                if(tmp.h > h)
                    h = tmp.h;
                n++;

                if(n < nodes.size()-1)
                    dims.w += margin;
            }
            dims.h += h;
        }
            break;
        case INVERTER:
        case SUCCEEDER:
        case REPEATER:
        case UNTIL:
            tmp = child()->measure(width, height, margin);
            dims.w = tmp.w;
            dims.h = tmp.h + height + 2 * margin;
            break;
        default:
            dims.w = width + margin;
            dims.h = height + margin;
            break;
    }
    return dims;
}

struct NodeShape Node::place(int x, int y, int width, int height, int margin)
{
    struct NodeShape shape;
    int w2 = width / 2;
    int h2 = height / 2;
    int m2 = margin / 2;

    shape.x = x - w2;
    shape.y = y + m2;
    shape.w = width;
    shape.h = height;
    shape.type = nodeType;
    shape.name = name;
    shape.state = nodeState;

    switch (nodeType)
    {
        case LEAF:
        case CALL:
        case ACTION:
            break;
        case RANDOM:
        case SEQUENCE:
        case SELECTOR:
        case PARALLEL:
        {
            y += height + 2*margin;
            int W = ( width + margin ) * nodes.size();
            int W2 = W / 2;
            x -= W2;
            x += m2 + w2;
            int inc = width + margin;

            for(list< NodePtr >::iterator it = nodes.begin(); it != nodes.end(); it++)
            {
                shape.nodes.push_back( (*it)->place(x, y, width, height, margin) );
                x += inc;
            }
        }
            break;
        case INVERTER:
        case SUCCEEDER:
        case REPEATER:
        case UNTIL:
            y += height + margin;
            shape.nodes.push_back( child()->place(x, y, width, height, margin) );
            break;
        default:
            break;
    }
    return shape;
}