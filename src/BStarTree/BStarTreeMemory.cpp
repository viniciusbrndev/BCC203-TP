#include <array>
#include <cstdint>
#include <optional>

#include "../Common.hpp"
#include "BStarTreeMemory.hpp"


namespace Algorithm::BStarTree {


std::optional<IBStarTreeData::Node> BStarTreeMemory::Search(int key) {
    if(rootIndex_ < 0)
        return std::nullopt;

    IBStarTreeData::Node currentNode;
    uint64_t currentIndex = rootIndex_;
    //desce na arvore até encontrar o nó folha desejado
    while(currentIndex >= 0 && static_cast<u_int64_t>(currentIndex) < this->nodes_.size()){
        currentNode = nodes_[currentIndex];
        if(currentNode.isLeaf())
            return currentNode;
        
        bool found = true;
        size_t chdIndex = FindKeyIndex(currentNode, key, found);

        currentIndex = currentNode.index.nodePos[chdIndex];
    }
    return std::nullopt;
}

size_t BStarTreeMemory::FindKeyIndex(const Node& node, int key, bool& found){
    found = false;

    if(node.isLeaf()){
        for(uint64_t i = 0; i < node.size;i++){
            if(node.data.entries[i].key == key){
                found = true;
                return i;
            }
            if(node.data.entries[i].key > key)
                return i;
        }
    }
    else{
        for(uint64_t i = 0; i < node.size;i++){
            if(node.index.indexes[i] > key){
                return i;
            }
        }
    }
    return node.size;
}










}