#include "./include/connectivity.hpp"
#include <vector>
#include <sstream>

#define netcom "../network"

void reset(){
    std::ofstream fout;
    fout.open(netcom);
    fout.close();
}

void sendBlockToNetwork(Block b){
    std::ofstream fout;
    std::string line;
    fout.open(netcom, std::ios::app);
    fout << b.prevBlockHash << std::endl;
    fout << b.currentBlockHash << std::endl;
    for(auto voter : b.votedUsers){
        fout << voter << " ";
    }
    fout << std::endl;
    for(auto vote : b.votesOf){
        fout << vote.first << " " << vote.second << std::endl;
    }
    fout.close();

}

void sendToNetwork(std::string toSend){
    std::ofstream fout;
    fout.open(netcom);
    fout << toSend<< std::endl;
    fout.close();
}

std::string recieveFromNetwork(){
    std::ifstream fin;
    fin.open(netcom);
    std::string recievedString;
    std::getline(fin, recievedString);
    fin.close();
    return recievedString;
}

Block recieveBlockFromNetwork(){
    std::ifstream fin;
    fin.open(netcom);
    std::string recievedString;
    //garbage
    std::getline(fin, recievedString);

    Block b;
    std::getline(fin, recievedString);
    b.prevBlockHash = recievedString;

    std::getline(fin, recievedString);
    b.currentBlockHash = recievedString;

    std::getline(fin, recievedString);
    std::string T;
    std::stringstream X(recievedString);
    while(getline(X, T, ' ')){
        (b.votedUsers).push_back(T);
    }

    while(std::getline(fin, recievedString)){
        std::string T;
        std::stringstream X(recievedString);
        std::vector<std::string> stemp;
        while(getline(X, T, ' ')){
            stemp.push_back(T);
        }
        b.votesOf[stemp[0]] = stoi(stemp[1]);
    }
    fin.close();
    return b;
}