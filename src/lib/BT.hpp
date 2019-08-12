#pragma once

#include "Node.hpp"
#include "Blackboard.hpp"
#include <thread>
#include <atomic>  
#include <map>
#include <string>

#ifdef HAS_OPENCV
#include <opencv2/opencv.hpp>
#endif

namespace btree
{
    class BT;

    typedef std::shared_ptr< BT > BTPtr;

    class BT
    {
        private:
            static std::map< std::thread::id, BT* > btrees;

            Blackboard _blackboard;

            std::atomic<bool> running;
            NodePtr node;
            std::thread tickThread;
            void tick(unsigned long ms);

            #ifdef HAS_OPENCV
            void draw(cv::Mat& img, struct NodeShape& shape, cv::Point2i& archor);
            #endif
        public:
            BT();
            virtual ~BT();

            NodePtr root();

            void start(unsigned long ms);
            void stop();
            void wait();
            bool isMe();

            bool draw(const std::string& fileName, int width = 100, int height = 60, int margin = 10);

            static BT& current();
            static Blackboard& blackboard();
    };
}