#pragma once

#include "Node.hpp"
#include "Blackboard.hpp"
#include "nlohmann/json.hpp"
#include "nlohmann/fifo_map.hpp"
#include <thread>
#include <atomic>  
#include <map>
#include <string>
#include <functional>
#include "tinyxml2.h"

#ifdef HAS_OPENCV
#include <opencv2/opencv.hpp>
#endif

template<class K, class V, class dummy_compare, class A>
using ordered_fifo_map = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
using json = nlohmann::basic_json<ordered_fifo_map>;

namespace btree
{
    class BT;

    typedef std::shared_ptr< BT > BTPtr;

    class BT
    {
        private:
            static std::map< std::thread::id, BT* > btrees;

            Blackboard _blackboard;
            std::map<std::string, ActionFunc> actions;
            std::map<std::string, ConditionFunc> conditions;
            std::map<std::string, CallFunc> calls;

            std::atomic<bool> running;
            NodePtr node;
            std::thread tickThread;
            void tick(unsigned long ms);
            bool fromJson(NodePtr node, const std::string& name, json& j);
            bool fromXML(NodePtr node, tinyxml2::XMLElement *xml);
            bool toJson(NodePtr node, json& j);
            void updateFunction(NodePtr node);

            #ifdef HAS_OPENCV
            void draw(cv::Mat& img, struct NodeShape& shape, cv::Point2i& archor, bool showState);
            cv::Mat draw(int width = 100, int height = 60, int margin = 10, bool showState = false);
            #endif
        public:
            BT();
            virtual ~BT();

            bool load(const std::string& path);
            bool save(const std::string& path);

            void registerCall(const std::string& name, CallFunc call);
            void registerAction(const std::string& name, ActionFunc action);
            void registerCondition(const std::string& name, ConditionFunc condition);
            void updateFunctions();

            NodePtr root();
            NodePtr find(const std::string& id);
            NodePtr findByName(const std::string& name);

            void start(unsigned long ms);
            void stop();
            void wait();
            bool isMe();

            bool draw(const std::string& fileName, int width = 100, int height = 60, int margin = 10);
            int show(int ms = 0, int width = 100, int height = 60, int margin = 10);

            static BT& current();
            static Blackboard& blackboard();
    };
}