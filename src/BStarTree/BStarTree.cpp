#include "BStarTreeFile.hpp"
#include "BStarTree.hpp"
#include "File.hpp"
#include "Common.hpp"

namespace Algorithm::BStarTree {

    std::optional<Item> BStarTree::Search(int key) {
        //Busca um nó folha na árvore
        std::optional<BStarTreeFile::Node> leafNode = file_.Search(key);
        if(!leafNode.has_value())
            return std::nullopt;
        
        std::optional<BStarTreeFile::Entry> entry = FindEntryInNode(*leafNode, key);
        
        if(!entry.has_value())
            return std::nullopt;
        
        const auto page = input_.GetPageAt(entry->pageIndex);

        return SearchInPage(page, key);
    }

    std::optional<BStarTreeFile::Entry> BStarTree::FindEntryInNode(
        const BStarTreeFile::Node& node, int key){

        if(node.isLeaf())
            return std::nullopt;
        
        for(u_int64_t i = 0; i < node.size; i++){
            if(node.data.entries[i].key == key)
                return node.data.entries[i];
        }
        return std::nullopt;
    }

    std::optional<Item> BStarTree::SearchInPage(
            const std::array<Item, PAGE_SIZE>& page, int key){
        for(Item item : page){
            if(item.key == key)
                return item;
        } 
        return std::nullopt;
    }










}
