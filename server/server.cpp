#include "./include/connectivity.hpp"
#include "./include/blockchain.hpp"
#include "./include/sha.hpp"
#include <unistd.h>

using namespace std;

string selfId = "0";
const int timer = 2;
Blockchain votesChain;

void runServer();
void transition();
vector<string> readConfig();

int main(){
    runServer();

    while(true){
        string msg = recieveFromNetwork();
        if (msg != ""){
            //somebody sent a msg
            vector<string> nodes;
            string tempNodes;
            stringstream X(msg);
            while(getline(X, tempNodes, ' ')){
                nodes.push_back(tempNodes);
            }

            if (nodes[0] != "0"){
                sleep(timer);
                continue;
            }

            cout << "Message to the server detected" << endl;
            cout << "Identifying the node";
            transition();

            if (nodes[2] == "0"){
                cout << "Node: " << nodes[1] << " requesting block";
                transition();
            }
            else if (nodes[2] == "1"){
                cout << "Node: " << nodes[1] << "sent a block" << endl;
            }

            if (nodes[0] == selfId && nodes[2] == "0"){
                sendToNetwork(nodes[1] + " " + nodes[0] + " 2");
                Block toSend = votesChain.giveTop();
                sendBlockToNetwork(toSend);
                cout << "Block sent" << endl;
            }
            else if (nodes[0] == selfId && nodes[2] == "1"){
                Block recievedBlock = recieveBlockFromNetwork();
                cout << "Block received" << endl;
                sendToNetwork(nodes[1] + " " + nodes[0] + " 2");
                cout << "Block appending to the blockchain";
                transition();
                votesChain.addBlock(recievedBlock);
                cout << "Block succesfully appended" << endl;
                cout << "updating Blockchain.dat";
                transition();
                votesChain.update();
                cout << "Blockchain.dat succesfully updated" << endl << endl;
                sendToNetwork("-1 0 3");
            }
        }
        sleep(timer);
    }
}

void runServer(){
    cout << "Starting the Server";
    transition();
    cout << "Server Started" << endl << endl;

    cout << "Reading the config file";
    transition();
    vector<string> voters = readConfig();
    cout << "Config file read" << endl << endl;

    votesChain.reset();
    reset(); //network clear

    cout << "Initialising the blockchain";
    transition();
    votesChain.initialise(voters);
    cout << "Blockchain initialisialised" << endl;
    votesChain.update();
    cout << "Blockchain.dat file updated" << endl << endl;
}

vector<string> readConfig(){
    ifstream fin;
    fin.open("voters.config");
    string name;
    static vector<string> voters;
    while(getline(fin, name)){
        voters.push_back(name);
    }
    return voters;
}

void transition(){
    sleep(1);
    cout << ".";
    cout << ".";
    cout << "." << endl;
}
