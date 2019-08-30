#include <btree/BT.hpp>
#include <iostream>

using namespace std;
using namespace btree;

NodeStates IsGhostClose(Blackboard& blackboard);
void Escape(Blackboard& blackboard);
void Greedy(Blackboard& blackboard);
void Time(Blackboard& blackboard);

int main(int argc, char *argv[])
{
    BT bt;

    bt.registerCall("Time", Time);
    bt.registerAction("IsGhostClose", IsGhostClose);
    bt.registerCall("Escape", Escape);
    bt.registerCall("Greedy", Greedy);

    bt.load("src/exe/pacman/pacman.json");

    bt.start(10);
    bt.wait();
    bt.show();

    return 0;
}

void Time(Blackboard& blackboard)
{
    cout << "Time" << endl;
    BT::current().show(30);
}

NodeStates IsGhostClose(Blackboard& blackboard) 
{
    NodeStates isGhostClose = (rand() % 100) > 50 ? SUCCESS : FAILURE;
    cout << "IsGhostClose? " << isGhostClose << endl;
    return isGhostClose;
}

void Escape(Blackboard& blackboard) 
{
    cout << "Escape" << endl;
}


void Greedy(Blackboard& blackboard)
{
    cout << "Greedy" << endl;
}