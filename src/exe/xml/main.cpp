#include <btree/BT.hpp>
#include <iostream>

using namespace std;
using namespace btree;

NodeStates Perception(const string& name, Blackboard& blackboard);
NodeStates UpdateIA(const string& name, Blackboard& blackboard);
bool IsCommited(const string& name, Blackboard& blackboard);
bool CanContinueIA(const string& name, Blackboard& blackboard);
NodeStates TransferTask(const string& name, Blackboard& blackboard);
NodeStates CallHelp(const string& name, Blackboard& blackboard);
bool JustTransfered(const string& name, Blackboard& blackboard);
NodeStates ExecuteTask(const string& name, Blackboard& blackboard);
NodeStates NotifyFinish(const string& name, Blackboard& blackboard);


int main(int argc, char *argv[])
{
    BT bt;

    bt.registerAction("update_(ia)", UpdateIA);
    bt.registerAction("perception", Perception);
    bt.registerCondition("is_commited", IsCommited);
    bt.registerCondition("can_continue_(ia)", CanContinueIA);
    bt.registerAction("transfer_task", TransferTask);
    bt.registerAction("call_help", CallHelp);
    bt.registerCondition("just_transfered", JustTransfered);
    bt.registerAction("execute_the_task", ExecuteTask);
    bt.registerAction("notify_finish", NotifyFinish);

    bt.load("src/exe/xml/BT-ContextManagement_V2.xml");

    bt.start(10);
    bt.wait();
    bt.show();

    return 0;
}

NodeStates UpdateIA(const string& name, Blackboard& blackboard) 
{
    BT::current().show(30);
    return SUCCESS;
}

NodeStates Perception(const string& name, Blackboard& blackboard) 
{
    return SUCCESS;
}

bool IsCommited(const string& name, Blackboard& blackboard)
{
    return false;
}

bool CanContinueIA(const string& name, Blackboard& blackboard)
{
    return true;
}

NodeStates TransferTask(const string& name, Blackboard& blackboard) 
{
    return SUCCESS;
}

NodeStates CallHelp(const string& name, Blackboard& blackboard) 
{
    return SUCCESS;
}

bool JustTransfered(const string& name, Blackboard& blackboard)
{
    return true;
}

NodeStates ExecuteTask(const string& name, Blackboard& blackboard) 
{
    return SUCCESS;
}

NodeStates NotifyFinish(const string& name, Blackboard& blackboard) 
{
    return SUCCESS;
}