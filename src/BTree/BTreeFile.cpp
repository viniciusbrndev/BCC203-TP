#include "BTreeFile.hpp"
#include <File.hpp>
#include <Item.hpp>


namespace Algorithm::BTree {


    std::optional<BTreeFile::Node> BTreeFile::Search(int key){
        if(rootIndex_ < 0)
            return std::nullopt;
        
        uint64_t currentIndex = rootIndex_;
        Node currentNode;
        bool found;
        uint64_t idx;

        while(currentIndex >= 0){
            //lê um novo nó a partir do indice
            if(!ReadNode(this->file_, currentIndex, currentNode))
                return std::nullopt;
            //Busca a chave dentro do nó
            idx = FindKeyIndex(currentNode,key,found);
            //Achou? retorna o nó atual
            if(found){
                return currentNode;
            }
            //Se não encontrou o item e o nó for folha o item não existe
            else if(currentNode.isLeaf())
                return std::nullopt;
            //se não continua caminhando pelos nós
            currentIndex = currentNode.nodePos[idx];
        }
        return std::nullopt;
    }
















}











