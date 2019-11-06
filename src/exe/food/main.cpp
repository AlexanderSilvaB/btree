#include <btree/BT.hpp>
#include <iostream>

using namespace btree;
using namespace std;

void Show(const string& name, Blackboard& blackboard)
{
    BT::current().show(30);
}

void PassTime(const string& name, Blackboard& blackboard)
{
    if(!blackboard.has("food"))
        blackboard["food"] = 100;
    if(!blackboard.has("hungry"))
        blackboard["hungry"] = 20;

    int food = blackboard["food"];
    int hungry = blackboard["hungry"];

    cout << "Food: " << food << endl;
    cout << "Hungry: " << hungry << endl;
    hungry--;

    blackboard["hungry"] = hungry;

    if(hungry == 0)
    {
        BT::current().stop();
    }
}

NodeStates AmIHungry(const string& name, Blackboard& blackboard)
{
    int hungry = blackboard["hungry"];
    cout << "Am I Hungry: ";
    if (hungry < 10) 
    {
        cout << "yes" << endl;
        return SUCCESS;
    } 
    else 
    {
        cout << "no" << endl;
        return FAILURE;
    }
}

NodeStates DoIHaveFood(const string& name, Blackboard& blackboard) 
{
    int food = blackboard["food"];
    cout << "Do I Have Food: ";
    if (food > 0) 
    {
        cout << "yes" << endl;
        return SUCCESS;
    } 
    else 
    {
        cout << "no" << endl;
        return FAILURE;
    }
}

NodeStates EnemiesAround(const string& name, Blackboard& blackboard) 
{
    cout << "Enemies Around: ";
    int en = rand() % 100;
    if (en > 50) 
    {
        cout << "yes" << endl;
        return SUCCESS;
    } 
    else 
    {
        cout << "no" << endl;
        return FAILURE;
    }
}

void EatFood(const string& name, Blackboard& blackboard)
{
    int food = blackboard["food"];
    int hungry = blackboard["hungry"];

    int eat = min(food, rand() % 30);
    food -= eat;
    hungry += eat;
    cout << "EatFood (" << eat << ")" << endl;

    blackboard["food"] = food;
    blackboard["hungry"] = hungry;
}

int main(int argc, char *argv[])
{
    BT bt;

    NodePtr rootNode = bt.root();
    rootNode->asSequence();

    NodePtr draw = rootNode->add();
    draw->asCall(Show);
    draw->setName("Show");

    NodePtr passTime = rootNode->add();
    passTime->asCall(PassTime);
    passTime->setName("Time");
    
    NodePtr amIHungry = rootNode->add();
    amIHungry->asAction(AmIHungry);
    amIHungry->setName("Am I Hungry");

    NodePtr doIHaveFood = rootNode->add();
    doIHaveFood->asAction(DoIHaveFood);
    doIHaveFood->setName("Do I Have Food");

    NodePtr noEnemiesAround = rootNode->add();
    NodePtr enemiesAround = noEnemiesAround->asInverter();
    enemiesAround->asAction(EnemiesAround);
    enemiesAround->setName("Enemies Around");

    NodePtr eatFood = rootNode->add();
    eatFood->asCall(EatFood);
    eatFood->setName("Eat Food");

    bt.start(10);
    bt.wait();
    bt.draw("bt.png");
    bt.show();
    
    return 0;
}

