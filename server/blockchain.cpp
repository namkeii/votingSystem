#include "./include/blockchain.hpp"
#include "./include/sha.hpp"
#include "./include/connectivity.hpp"
using namespace std;

void Blockchain::addBlock(Block next){
    std::string blockData = "";
    for(auto vote : next.votesOf){
        blockData += vote.first + std::to_string(vote.second);
    }
    for(auto voter : next.votedUsers){
        blockData += voter;
    }

    next.currentBlockHash = sha256(blockData);
    (this->chain).push_back(next);
}

void Blockchain::initialise(std::vector<std::string> voters){
    Block genesis;
    genesis.prevBlockHash = "0";

    for(auto voter : voters){
        genesis.votesOf[voter] = 0;
    }
    this->addBlock(genesis);
}

void Blockchain::reset(){
    std::ofstream fout;
    fout.open("blockchain.dat");
    fout.close();
}

void Blockchain::update(){
    cout << "updated lobee>> " << endl;
    std::ofstream fout;
    fout.open("blockchain.dat");

    for(Block b : chain){
        fout << "--------------------------------------------" << std::endl;
        fout << "Previous Hash: " << b.prevBlockHash << std::endl;
        fout << "Current Hash: " << b.currentBlockHash << std::endl;
        fout << "******" << endl;
        for(auto vote : b.votesOf){
            fout << vote.first << ": " << vote.second << std::endl;
        }
        fout << "******" << endl;
        for(auto voter : b.votedUsers){
            fout << voter << " ";
        }
        fout << endl;
        fout << "--------------------------------------------" << std::endl;
        fout << std::endl;
    }
    fout.close();
}

void Blockchain::print(){

    for(Block b : chain){
        std::cout << "--------------------------------------------" << std::endl;
        std::cout << "Previous Hash: " << b.prevBlockHash << std::endl;
        std::cout << "Current Hash: " << b.currentBlockHash << std::endl;
        std::cout << "******" << endl;
        for(auto vote : b.votesOf){
            std::cout << vote.first << ": " << vote.second << std::endl;
        }
        std::cout << "******" << endl;
        for(auto voter : b.votedUsers){
            std::cout << voter << " ";
        }
        cout << endl;
        std::cout << "--------------------------------------------" << std::endl;
        std::cout << std::endl;
    }
}

Block Blockchain::giveTop(){
    Block b = chain.back();
    return b;
}