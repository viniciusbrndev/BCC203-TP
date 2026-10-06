#include "BStarTreeFile.hpp"
#include "BStarTree.hpp"
#include "File.hpp"
#include "Common.hpp"

namespace Algorithm::BStarTree {
    std::optional<BStarTreeFile::Node> BStarTreeFile::Search(int key){
        if(rootIndex_ <= 0)
            return std::nullopt;
        BStarTreeFile::Node currentNode;
        uint64_t currentIdx = rootIndex_;
        while(ReadNode(file_, currentIdx, currentNode)){
            if(currentNode.isLeaf())
                return currentNode;
            
            bool found = true;
            u_int64_t chdIndex = FindKeyIndex(currentNode, key,found);

            currentIdx = currentNode.index.nodePos[chdIndex];
        }
    
        return std::nullopt;
    }

}