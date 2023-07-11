#include "./include/connectivity.hpp"
#include "./include/blockchain.hpp"
#include "./include/sha.hpp"
#include <unistd.h>
#include <ios>
#include <limits>
#include <chrono>

std::string selfId = sha256("2");
bool condition = false;
std::string serverIdd = "0";
bool serverRunning = true;
const int timer = 2;
Blockchain votesChain;
bool backupAvailable = false;
Block backup;

void runNode();
bool isNumber(std::string s);
std::vector<std::string> readConfig();
void createBackup();
std::vector<std::string> voters;
void transition();

int main(){
    voters = readConfig();
    runNode();
    if (condition == true){
        condition = false;
        runNode();
    }
    while (true){
        //running as a server
        while(true){
            std::string msg = recieveFromNetwork();
            if (msg == ""){
                break;
            }
            std::vector<std::string> nodes;
            std::string tempNodes;
            std::stringstream X(msg);
            while(getline(X, tempNodes, ' ')){
                nodes.push_back(tempNodes);
            }
            if (nodes[2] == "5"){
                break;
            }
            sleep(6);
        }
        std::string testt= "";
        testt += selfId;
        testt += " ";
        testt += selfId;
        testt += " 4";

        sendToNetwork(testt);
        sleep(timer);

        while(1){
            std::string msg = recieveFromNetwork();
            if (msg != ""){
                sleep(2);
                std::vector<std::string> nodes;
                std::string tempNodes;
                std::stringstream X(msg);
                while(getline(X, tempNodes, ' ')){
                    nodes.push_back(tempNodes);
                }

                if (nodes[0] != selfId){
                    sleep(timer);
                    continue;
                }
                if (nodes[0] == selfId && nodes[2] == "0"){
                    sendToNetwork(nodes[1] + " " + nodes[0] + " 2");
                    Block toSend;
                    if (votesChain.giveSize() != 0){
                        toSend = votesChain.giveTop();
                    }
                    else{ //when server and nodes crashes in between
                        std::cout << "Server failed without registering any votes" << std::endl;
                        return 0;
                    }

                    sendBlockToNetwork(toSend);
                    std::cout << "Block Sent" << std::endl;
                }
                else if (nodes[0] == selfId && nodes[2] == "1"){
                    Block recievedBlock = recieveBlockFromNetwork();
                    std::cout << "Block received" << std::endl;
                    sendToNetwork(nodes[1] + " " + nodes[0] + " 2");
                    votesChain.addBlock(recievedBlock);
                    std::cout << "Block succesfully appended" << std::endl;
                    votesChain.update();
                    std::cout << "Blockchain.dat succesfully updated" << std::endl;
                    sendToNetwork("-1 " + selfId + " 4");
                }
            }
            sleep(timer);
        }
    }
}

void runNode(){
    auto begin = std::chrono::high_resolution_clock::now();
    int tttime = 20;

    while(true){
        std::string msg = recieveFromNetwork();
        if (msg == ""){
            break;
        }
        std::vector<std::string> nodes;
        std::string tempNodes;
        std::stringstream X(msg);
        while(getline(X, tempNodes, ' ')){
            nodes.push_back(tempNodes);
        }

	std::cout << "MsgScript in the network sent: ";
        for(auto n : nodes){
            std::cout << n << " ";
        }
        std::cout << std::endl;

        if (nodes[2] == "3"){
            break;
        }
        else if (nodes[2] == "4"){
            serverIdd = nodes[1];
            serverRunning = true;
            sleep(4);
            break;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto timeEl = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
        float count = timeEl.count()*1e-9;
        if (count >= tttime){
            serverRunning = false;
            std::cout << "No server running" << std::endl;
            sendToNetwork("-1 " + selfId + " 5");
            sleep(4);
            if (tttime == 40){
                break;
            }
            tttime = 40;
        }
        sleep(4);
    }

    if (serverRunning == false){
        return;
    }

    sendToNetwork(serverIdd + " " + selfId + " 0");

    while(true){
        auto end = std::chrono::high_resolution_clock::now();
        auto timeEl = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
        float count = timeEl.count()*1e-9;
        if (count >= 20){
            serverRunning = false;
            std::cout << "Server Not responding" << std::endl;
            sendToNetwork("-1 " + selfId + " 5");
            condition = true;
            sleep(4);
            return;
        }
        std::string msg = recieveFromNetwork();
        std::vector<std::string> nodes;
        std::string tempNodes;
        std::stringstream X(msg);
        while(getline(X, tempNodes, ' ')){
            nodes.push_back(tempNodes);
        }

        std::cout << "Nodes" << std::endl;
        for(auto n : nodes){
            std::cout << n << " ";
        }
        std::cout << std::endl;

        if (nodes[0] == selfId && nodes[1] == serverIdd && nodes[2] == "2"){
            break;
        }
        sleep(4);
    }

    int choice = 0;
    std::cout << "Choose your vote (Enter the digit)" << std::endl;

    while(true){
        std::string tempChoice;
        for(int i_v = 1; i_v <= voters.size(); i_v++){
            std::cout << i_v << ". " << voters[i_v-1] << std::endl;
        }
        std::cin >> tempChoice;

        if (isNumber(tempChoice)){
            choice = stoi(tempChoice);
        }

        if (choice >= 1 && choice <= voters.size()){
            std::cout << "You have chosen " << voters[choice-1] << std::endl;
            break;
        }
        else{
            choice = 0;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Enter valid voter number" << std::endl;
        }
    }

    Block recievedBlock;
    if (serverRunning){
        recievedBlock = recieveBlockFromNetwork();
        recievedBlock.prevBlockHash = recievedBlock.currentBlockHash;
        recievedBlock.votesOf[voters[choice-1]]++;
        recievedBlock.votedUsers.push_back(selfId);
        sendToNetwork(serverIdd + " " + selfId + " 1");
        sendBlockToNetwork(recievedBlock);
        sleep(4);
        votesChain.reset();
        votesChain.addBlock(recievedBlock);
        votesChain.update();
    }
}

std::vector<std::string> readConfig(){
    std::ifstream fin;
    fin.open("voters.config");
    std::string name;
    static std::vector<std::string> voters;
    while(std::getline(fin, name)){
        voters.push_back(name);
    }
    return voters;
}

bool isNumber(std::string s){
    for(int i = 0; i < s.length(); i++){
        if (isdigit(s[i]) == false){
            return false;
        }
    }
    return true;
}

void createBackup(){
    backupAvailable = false;
    //1,2,3,4 backup there
    std::string msg = recieveFromNetwork();
    if (msg == ""){
        return;
    }
    std::vector<std::string> nodes;
    std::string tempNodes;
    std::stringstream X(msg);
    while(getline(X, tempNodes, ' ')){
        nodes.push_back(tempNodes);
    }

    if (nodes[2] != "0"){
        backupAvailable = true;
        backup = recieveBlockFromNetwork();
    }
}

void transition(){
    sleep(1);
    std::cout << ".";
    std::cout << ".";
    std::cout << "." << std::endl;
}
