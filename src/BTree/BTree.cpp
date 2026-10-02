#include "BTree.hpp"
#include <File.hpp>
#include <Item.hpp>



namespace Algorithm::BTree{

    std::optional<BTreeFile::Entry> BTree::FindEntryInNode(
    const BTreeFile::Node& node, int key){
        for(size_t i = 0; i < node.size; i++){
            if(node.entries[i].key == key){
                return node.entries[i];
            }
        }
        return std::nullopt;
    }


    std::optional<Item> BTree::SearchInPage(
    const std::array<Item, PAGE_SIZE>& page, int key){
        for(size_t i = 0; i < PAGE_SIZE; i++){
            if(page.at(i).key == key){
                return page.at(i);
            }
        }
        return std::nullopt;
    }
    std::optional<Item> BTree::Search(int key){
        //Busca o nó desejado na cache
        std::optional<BTreeFile::Node> node = this->file_.Search(key);
        if(!node){
            return std::nullopt;
        }
        //varre o nó e retorna o indice da pagina da chave correpondente
        std::optional<BTreeFile::Entry> entry = FindEntryInNode(*node, key);
        if(!entry){
            return std::nullopt;
        }
        //Busca a página correpondende ao indice da entrada
        u_int64_t pageIdx = entry->pageIndex;
        auto  page = input_.GetPageAt(pageIdx);
        //retorna o item completo se ele existir na pagina
        return SearchInPage(page, key);
    }

}