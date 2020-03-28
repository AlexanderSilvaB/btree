#include "BT.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
#include <iomanip>


using namespace btree;
using namespace std;
using namespace tinyxml2;

map< thread::id, BT* > BT::btrees;

static BT defaultBT;

static bool hasEnding (std::string const &fullString, std::string const &ending) 
{
    if (fullString.length() >= ending.length()) 
    {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } 
    else 
    {
        return false;
    }
}

static void defaultCall(const std::string& name, Blackboard& blackboard)
{
    cout << "[Call(" << name << ")] Not implemented" << endl;
}

static NodeStates defaultAction(const std::string& name, Blackboard& blackboard)
{
    cout << "[Action(" << name << ")] Not implemented" << endl;
    return FAILURE;
}

static bool defaultCondition(const std::string& name, Blackboard& blackboard)
{
    cout << "[Condition(" << name << ")] Not implemented" << endl;
    return false;
}

BT::BT()
{
    running = false;
    node = NodePtr(new Node());
}

BT::~BT()
{
    stop();
}

bool BT::load(const std::string& path)
{
    if(hasEnding(path, ".json"))
    {
        std::ifstream i(path);
        json j;
        try 
        {
            j = json::parse(i);
        }
        catch (json::exception &e) 
        { 
            std::cout << e.what() << std::endl; 
            return false;
        } 

        if(j.size() == 0)
            return false;

        node = NodePtr(new Node());

        json::iterator it = j.begin();
        bool valid = fromJson(node, it.key(), it.value());
        if(!valid)
            return false;

        updateFunctions();
        return true;
    }
    else if(hasEnding(path, ".xml"))
    {
        XMLDocument doc;
	    if(doc.LoadFile( path.c_str() ) != XML_SUCCESS)
            return false;
        
        XMLElement *root = doc.FirstChildElement("root");
        if(!root)
            return false;

        XMLElement *bt = root->FirstChildElement("BehaviorTree");
        if(!bt)
            return false;

        node = NodePtr(new Node());
        bool valid = fromXML(node, bt->FirstChildElement());
        if(!valid)
            return false;

        updateFunctions();

    }
    return false;
}

bool BT::save(const std::string& path)
{
    if(hasEnding(path, ".json"))
    {
        json n;
        bool valid = toJson(node, n);
        if(!valid)
            return false;

        json j;
        j[ node->getName() ] = n;

        std::ofstream o(path);
        o << std::setw(4) << j << std::endl;
        return true;
    }
    else if(hasEnding(path, ".xml"))
    {

    }
    return false;
}

void BT::registerCall(const std::string& name, CallFunc call)
{
    calls[name] = call;
}

void BT::registerAction(const std::string& name, ActionFunc action)
{
    actions[name] = action;
}

void BT::registerCondition(const std::string& name, ConditionFunc condition)
{
    conditions[name] = condition;
}

bool BT::fromJson(NodePtr node, const string& name, json& j)
{
    node->setName(name);

    if (j.find("id") != j.end()) 
    {
        node->setId(j["id"]);
    }

    if (j.find("counter") != j.end()) 
    {
        node->setCounter(j["counter"]);
    }

    string type = "call";
    if (j.find("type") != j.end()) 
    {
        type = j["type"];
    }

    if(type == "call")
    {
        node->asCall(NULL);
    }
    else if(type == "action")
    {
        node->asAction(NULL);
    }
    else if(type == "condition")
    {
        node->asCondition(NULL);
    }
    else if(type == "inverter")
    {
        node->asInverter();
    }
    else if(type == "succeeder")
    {
        node->asSucceeder();
    }
    else if(type == "repeator")
    {
        node->asRepeater();
    }
    else if(type == "until")
    {
        node->asUntil();
    }
    else if(type == "selector")
    {
        node->asSelector();
    }
    else if(type == "sequence")
    {
        node->asSequence();
    }
    else if(type == "random")
    {
        node->asRandom();
    }
    else if(type == "parallel")
    {
        node->asParallel();
    }
    else if(type == "flipper")
    {
        node->asFlipper();
    }
    else
    {
        cout << "Invalid node type: " << type << endl;
        return false;
    }

    if (j.find("nodes") != j.end()) 
    {
        json o = j["nodes"];
        bool result = true;
        for (json::iterator it = o.begin(); it != o.end() && result == true; ++it) 
        {
            NodePtr leaf = node->add();
            result = fromJson(leaf, it.key(), it.value());
        }

        return result;
    }

    return true;
}

bool BT::toJson(NodePtr node, json& j)
{
    if(node->getId().length() > 0)
        j["id"] = node->getId();
    
    if(node->getCounter() >= 0)
        j["counter"] = node->getCounter();

    switch (node->type())
    {
        case LEAF:
            cout << "Invalid node type: leaf (not initialized)" << endl;
            return false;
            break;
        case CALL:
            j["type"] = "call";
            break;
        case ACTION:
            j["type"] = "action";
            break;
        case CONDITION:
            j["type"] = "condition";
            break;
        case FLIPPER:
            j["type"] = "flipper";
            break;
        case RANDOM:
            j["type"] = "random";
            break;
        case SELECTOR:
            j["type"] = "selector";
            break;
        case SEQUENCE:
            j["type"] = "sequence";
            break;
        case PARALLEL:
            j["type"] = "parallel";
            break;
        case INVERTER:
            j["type"] = "inverter";
            break;
        case SUCCEEDER:
            j["type"] = "succeeder";
            break;
        case REPEATER:
            j["type"] = "repeater";
            break;
        case UNTIL:
            j["type"] = "until";
            break;
        default:
            cout << "Invalid node type: " << node->type() << endl;
            return false;
            break;
    }
    
    int sz = node->size();
    bool valid = true;

    if(sz > 0)
    {
        json jj;
        for(int i = 0; i < sz && valid == true; i++)
        {
            json o;
            valid = toJson(node->at(i), o);
            jj[ node->at(i)->getName() ] = o;
        }
        j["nodes"] = jj;
    }

    return valid;
}

bool BT::fromXML(NodePtr node, XMLElement* xml)
{
    if(!xml)
        return false;
    
    string type = xml->Name();

    if(type == "Call")
    {
        node->asCall(NULL);
    }
    else if(type == "Action")
    {
        node->asAction(NULL);
    }
    else if(type == "Condition")
    {
        node->asCondition(NULL);
    }
    else if(type == "Inverter")
    {
        node->asInverter();
    }
    else if(type == "Succeeder")
    {
        node->asSucceeder();
    }
    else if(type == "Repeater")
    {
        node->asRepeater();
    }
    else if(type == "Until")
    {
        node->asUntil();
    }
    else if(type == "Selector")
    {
        node->asSelector();
    }
    else if(type == "Sequence")
    {
        node->asSequence();
    }
    else if(type == "Random")
    {
        node->asRandom();
    }
    else if(type == "Parallel")
    {
        node->asParallel();
    }
    else if(type == "Flipper")
    {
        node->asFlipper();
    }
    else if(type == "Success")
    {
        node->asSucceeder();
    }
    else
    {
        cout << "Invalid node type: " << type << endl;
        return false;
    }

    const char *name = xml->Attribute("name");
    if(!name)
        return false;
    node->setName(string(name));
    cout << name << endl;

    const char *id = xml->Attribute("ID");
    if(!id)
        return false;
    node->setId(string(id));

    int counter = xml->IntAttribute("counter", -1);
    node->setCounter(counter);

    XMLElement *child = xml->FirstChildElement();
    while(child)
    {
        NodePtr leaf = node->add();
        if(!fromXML(leaf, child))
            return false;
        child = child->NextSiblingElement();
    }

    return true;
}

void BT::updateFunctions()
{
    updateFunction(node);
}

void BT::updateFunction(NodePtr node)
{
    if(node->type() == CALL)
    {
        if(calls.find(node->getName()) != calls.end())
        {
            node->asCall(calls[ node->getName() ]);
        }
        else
        {
            node->asCall( defaultCall );
        }
    }
    else if(node->type() == ACTION)
    {
        if(actions.find(node->getName()) != actions.end())
        {
            node->asAction(actions[ node->getName() ]);
        }
        else
        {
            node->asAction( defaultAction );
        }
    }
    else if(node->type() == CONDITION)
    {
        if(conditions.find(node->getName()) != conditions.end())
        {
            node->asCondition(conditions[ node->getName() ]);
        }
        else
        {
            node->asCondition( defaultCondition );
        }
    }

    int sz = node->size();
    for(int i = 0; i < sz; i++)
    {
        updateFunction(node->at(i));
    }
}

NodePtr BT::root()
{
    return node;
}

NodePtr BT::find(const std::string& id)
{
    if(node->getId() == id)
        return node;
    return node->find(id);
}

NodePtr BT::findByName(const std::string& name)
{
    if(node->getName() == name)
        return node;
    return node->findByName(name);
}

void BT::start(unsigned long ms)
{
    if(running)
        return;

    running = true;
    tickThread = thread(&BT::tick, this, ms);
    btrees[tickThread.get_id()] = this;
}

void BT::stop()
{
    if(!running)
        return;

    running = false;

    if(isMe())
        return;

    if(tickThread.joinable())
    {
        tickThread.join();
    }
}

void BT::wait()
{
    if(!running)
        return;
    
    if(isMe())
        return;

    tickThread.join();
}

void BT::tick(unsigned long ms)
{
    while(running)
    {
        node->evaluate(_blackboard);
        this_thread::sleep_for(chrono::milliseconds(ms));
    }
}

bool BT::isMe()
{
    thread::id id = this_thread::get_id();
    if(id == tickThread.get_id())
        return true;
    return false;
}

BT& BT::current()
{
    thread::id id = this_thread::get_id();
    if(btrees.find(id) == btrees.end())
        return defaultBT;

    BT* bt = btrees.at(id);   
    return *bt;
}

Blackboard& BT::blackboard()
{
    return BT::current()._blackboard;
}

#ifdef HAS_OPENCV
void BT::draw(cv::Mat& img, struct NodeShape& shape, cv::Point2i& archor, bool showState)
{
    int fontFace = cv::FONT_HERSHEY_COMPLEX_SMALL;
    double fontScaleHeader = 0.005 * shape.w;
    double fontScaleText = 0.005 * shape.w;
    int thickness = 1;

    cv::Scalar bg(85, 87, 86);
    cv::Scalar fg(255, 255, 255);
    string header = "(None)";
    switch (shape.type)
    {
        case LEAF:
            bg = cv::Scalar(85, 87, 86);
            header = "(Leaf)";
            break;
        case CALL:
            bg = cv::Scalar(29, 128, 47);
            header = "(Call)";
            break;
        case ACTION:
            bg = cv::Scalar(2, 99, 163);
            header = "(Action)";
            break;
        case CONDITION:
            bg = cv::Scalar(63, 99, 163);
            header = "(Condition)";
            break;
        case RANDOM:
            bg = cv::Scalar(163, 128, 2);
            header = "(Random)";
            break;
        case SELECTOR:
            bg = cv::Scalar(163, 2, 96);
            header = "(Selector)";
            break;
        case SEQUENCE:
            bg = cv::Scalar(150, 163, 2);
            header = "(Sequence)";
            break;
        case INVERTER:
            bg = cv::Scalar(85, 2, 163);
            header = "(Inverter)";
            break;
        case SUCCEEDER:
            bg = cv::Scalar(33, 37, 89);
            header = "(Succeeder)";
            break;
        case REPEATER:
            bg = cv::Scalar(130, 23, 64);
            header = "(Repeater)";
            break;
        case UNTIL:
            bg = cv::Scalar(4, 71, 10);
            header = "(Until)";
            break;
        default:
            break;
    }

    cv::Rect2i rect(shape.x, shape.y, shape.w, shape.h);
    cv::Point2i ptArchor(shape.x + shape.w / 2, shape.y);

    int baseline;
    cv::Size szHeader = cv::getTextSize(header, fontFace, fontScaleHeader, thickness, &baseline);
    cv::Size szName = cv::getTextSize(shape.name, fontFace, fontScaleText, thickness, &baseline);

    while(szName.width >= shape.w)
    {
        fontScaleText *= 0.9;
        szName = cv::getTextSize(shape.name, fontFace, fontScaleText, thickness, &baseline);
    }
    
    cv::Point2i ptTextName(shape.x + (shape.w - szName.width)/2, shape.y + (shape.h + szName.height)/2);
    cv::Point2i ptTextHeader(shape.x + (shape.w - szHeader.width)/2, shape.y + 3*(shape.h + szHeader.height)/4);

    if(archor.x >= 0)
    {
        cv::Scalar black(0, 0, 0);
        cv::line(img, archor, ptArchor, black, thickness, cv::LINE_AA);
    }
    
    cv::rectangle(img, rect, bg, CV_FILLED);
    cv::putText(img, shape.name, ptTextName, fontFace, fontScaleText, fg, thickness , cv::LINE_AA, false);
    cv::putText(img, header, ptTextHeader, fontFace, fontScaleHeader, fg, thickness , cv::LINE_AA, false);

    if(showState)
    {
        cv::Rect2i stateRect(shape.x + shape.w - 10, shape.y + shape.h - 10, 10, 10);
        cv::Scalar bgState(255, 0, 0);
        switch(shape.state)
        {
            case SUCCESS:
                bgState = cv::Scalar(0, 255, 0);
                break;
            case RUNNING:
                bgState = cv::Scalar(0, 0, 255);
                break;
            case FAILURE:
                bgState = cv::Scalar(255, 0, 0);
                break;
            default:
                break;
        }
        cv::rectangle(img, stateRect, bgState, CV_FILLED);
    }

    ptArchor.y += shape.h;
    for(list<struct NodeShape>::iterator it = shape.nodes.begin(); it != shape.nodes.end(); it++)
    {
        draw(img, *it, ptArchor, showState);
    }
} 

cv::Mat BT::draw(int width, int height, int margin, bool showState)
{
    struct NodeDims dims = node->measure(width, height, margin);
    struct NodeShape shape = node->place(dims.w / 2, 0, width, height, margin);

    cv::Scalar white(255, 255, 255);
    cv::Mat img(dims.h, dims.w, CV_8UC3, white);

    cv::Point2i archor(-1, -1);
    draw(img, shape, archor, showState);

    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
    return img;
}

#endif

bool BT::draw(const string& fileName, int width, int height, int margin)
{
    #ifndef HAS_OPENCV
    return false;
    #else
    cv::Mat img = draw(width, height, margin, false);
    cv::imwrite(fileName, img);
    #endif
    return true;
}

int BT::show(int ms, int width, int height, int margin)
{
    #ifndef HAS_OPENCV
    return 0;
    #else
    cv::Mat img = draw(width, height, margin, true);
    cv::imshow( "Behaviour Tree", img );
    return cv::waitKey(ms);
    #endif
}