#ifndef __blockchain__
#define __blockchain__
#include <iostream>
#include <vector>
#include <map>

class Block
{
    public:
        //Header
        std::string prevBlockHash;
        std::string currentBlockHash;
        //Block data
        std::map<std::string, int> votesOf;
        //Footer
        std::vector<std::string> votedUsers;
};

class Blockchain
{
    private:
        std::vector<Block> chain;

    public:
        void addBlock(Block);
        void initialise(std::vector<std::string>);
        void reset();
        void update();
        void print();
        Block giveTop();
};
#endif