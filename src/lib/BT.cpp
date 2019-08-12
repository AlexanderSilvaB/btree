#include "BT.hpp"
#include <iostream>
#include <chrono>

using namespace BTree;
using namespace std;

map< thread::id, BT* > BT::btrees;

static BT defaultBT;

BT::BT()
{
    running = false;
    node = NodePtr(new Node());
}

BT::~BT()
{
    stop();
}

NodePtr BT::root()
{
    return node;
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
void BT::draw(cv::Mat& img, struct NodeShape& shape, cv::Point2i& archor)
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

    ptArchor.y += shape.h;
    for(list<struct NodeShape>::iterator it = shape.nodes.begin(); it != shape.nodes.end(); it++)
    {
        draw(img, *it, ptArchor);
    }
} 
#endif

bool BT::draw(const string& fileName, int width, int height, int margin)
{
    struct NodeDims dims = node->measure(width, height, margin);
    struct NodeShape shape = node->place(dims.w / 2, 0, width, height, margin);

    #ifndef HAS_OPENCV
    return false;
    #else
    cv::Scalar white(255, 255, 255);
    cv::Mat img(dims.h, dims.w, CV_8UC3, white);

    cv::Point2i archor(-1, -1);
    draw(img, shape, archor);

    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

    cv::imwrite(fileName, img);
    cv::imshow( "Diagram", img );
    cv::waitKey(0);
    #endif
    return true;
}