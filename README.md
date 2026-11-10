[readme_md.md](https://github.com/user-attachments/files/34649822/readme_md.md)
# Algoritmos de Pesquisa em Memória Secundária — BCC203 (UFOP)

![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)
![Build](https://img.shields.io/badge/build-CMake-brightgreen.svg)
![CI/CD](https://img.shields.io/badge/CI%2FCD-GitHub%20Actions-blueviolet.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

Este repositório contém a implementação em **C++23**, instrumentação de desempenho e análise comparativa de quatro estruturas de dados para pesquisa em memória secundária (arquivos de grande porte):

1. **Acesso Sequencial Indexado (ISA)**
2. **Árvore Binária de Pesquisa Externa**
3. **Árvore B (Ordem $M = 100$, Persistida em Disco)**
4. **Árvore B\*** (Multi-vias em Memória RAM com Acesso Direto a Disco)

Trabalho prático desenvolvido para a disciplina **BCC203 — Estrutura de Dados II** do Departamento de Computação (DECOM/ICEB) da **Universidade Federal de Ouro Preto (UFOP)**.

---

## 👨‍💻 Autores

- **Kaio Alves Simões**
- **Luiz Henrique Silva Sampaio**
- **Victor Kangussu**
- **Vinícius Brandão**

**Professor Orientador:** Prof. Guilherme Tavares de Assis

---

## 🛠️ Tecnologias e Especificações

- **Linguagem:** C++23 (`-std=c++23`) com recursos de `std::filesystem`, `std::optional` e `std::chrono`.
- **Compiladores Suportados:** GNU GCC 13.3.0 (`g++-13`) / Clang 18.0.
- **Sistema de Build:** CMake 3.28+ (`-O3 -Wall -Wextra -Wpedantic`).
- **Ambiente de Testes:** Runner virtualizado em nuvem do GitHub Actions (Ubuntu 24.04 LTS Noble Numbat, 4 vCPUs, 16 GB RAM, SSD Azure).
- **Formatadores/Linters:** `clang-format` e `clang-tidy`.

---

## 📐 Estrutura do Registro e Paginação

Cada elemento transferido entre a memória secundária e a memória principal é representado pela estrutura `Item`:

```cpp
struct Item {
    int key;                        // Chave primária de 32 bits
    long int value;                 // Valor numérico de 64 bits
    std::array<char, 5000> text;    // Dados estendidos / carga útil de 5.000 bytes
};
```

Devido à dimensão de cada `Item` ($\approx 5.012\text{ bytes}$), as operações de leitura/escrita em disco ocorrem em páginas contendo exatamente $\text{PAGE\_SIZE} = 100$ registros.

---

## 💻 Destaques de Código e Implementação

### 1. Acesso Sequencial Indexado (ISA)
Constrói uma tabela de índices esparsa mapeando a menor chave de cada página ao número ordinal da página no arquivo de dados.

```cpp
// Leitura e busca binária na tabela de índices (esparsa)
namespace Algorithm::IndexedSequentialAccess {
    std::optional<uint64_t> Cache::Search(int key) {
        // Realiza busca binária nas entradas do arquivo de índice esparso
        auto it = std::lower_bound(entries.begin(), entries.end(), key, 
            [](const Entry& e, int k) { return e.key < k; });
            
        if (it != entries.end() && it->key == key) return it->pageIndex;
        if (it != entries.begin()) return std::prev(it)->pageIndex;
        return std::nullopt;
    }
}
```

### 2. Árvore Binária de Pesquisa Externa
Estrutura persistida em disco onde os ponteiros de filhos são deslocamentos numéricos (offsets) em arquivo binário.

```cpp
namespace Algorithm::BinaryTree {
    struct Node {
        int key;              // Chave primária
        uint64_t pageIndex;   // Página no arquivo de dados
        uint64_t left;        // Offset do nó filho à esquerda (0 se nulo)
        uint64_t right;       // Offset do nó filho à direita (0 se nulo)
    };
}
```

### 3. Árvore B (Persistida em Memória Secundária)
Gravação e leitura assíncrona/direta de nós de ordem $M = 100$ serializados em arquivo binário `.btree`.

```cpp
namespace Algorithm::BTree {
    // Cálculo do offset de busca direta em disco via busca por bloco
    inline uint64_t GetNodeOffset(int64_t nodeIndex) {
        return sizeof(Metadata) + (nodeIndex * sizeof(Node));
    }

    // Leitura pontual do nó contido no arquivo .btree
    void BTreeFile::ReadNode(std::fstream& file, int64_t nodeIndex, Node& node) {
        file.seekg(GetNodeOffset(nodeIndex), std::ios::beg);
        file.read(reinterpret_cast<char*>(&node), sizeof(Node));
        Metrics::IncrementDiskReads(); // Registra a transferência I/O
    }
}
```

### 4. Árvore B\* (Indexação Avançada em RAM)
Segregação estrita entre nós internos de roteamento (`Index`) e folhas de mapeamento de dados (`Data`), acelerada em RAM.

```cpp
namespace Algorithm::BStarTree {
    struct Node {
        enum class Type { Index, Data } type;
        
        union {
            struct {
                std::array<int, PAGE_SIZE> indexes;
                std::array<int64_t, PAGE_SIZE + 1> nodePos;
            } indexNode;
            
            struct {
                std::array<Entry, PAGE_SIZE> entries;
                int64_t nextData; // Encadeamento sequencial entre folhas
            } dataNode;
        };
    };
}
```

---

## 📊 Quadro Comparativo dos Resultados Empíricos

Resultados extraídos de **100 repetições estritas** para cada cenário, com métricas mensuradas em percentis (p50, p75, p99) em um arquivo de $N = 2.000.000$ registros:

| Métrica / Característica | ISA | Árvore Binária Externa | Árvore B (Disco) | Árvore B\* (RAM) |
| :--- | :--- | :--- | :--- | :--- |
| **Residência do Índice** | Memória Secundária | Memória Secundária | Memória Secundária | Memória Principal (RAM) |
| **Complexidade de Busca (Pior Caso)** | $\mathcal{O}(\log(N / \text{PAGE\_SIZE}))$ | $\mathcal{O}(N)$ (Degenerada) | $\mathcal{O}(\log_M N)$ | $\mathcal{O}(\log_M N)$ |
| **Leituras de Disco ($N=2M$, p50)** | 15 leituras | Inviável (Timeout) | 5 leituras | **1 leitura** (Ótimo) |
| **Comparações em RAM ($N=2M$, p50)** | 72 comparações | Inviável (Timeout) | 104 a 109 comp. | 90 a 108 comp. |
| **Tempo de Busca ($N=2M$, p50)** | $0,09\text{ ms}$ (Cold) / $0,34\text{ ms}$ (Warm) | Inviável (Timeout) | $0,09\text{ ms}$ (Cold) / $0,21\text{ ms}$ (Warm) | $0,09\text{ ms}$ (Cold) |
| **Suporte a Dados Desordenados** | ❌ Não (Rejeitado) | ⚠️ Sim (Com degradação) | ✅ Sim (Imune) | ✅ Sim (Imune) |
| **Sensibilidade à Ordenação** | Extrema (Requer asc.) | Extrema (Degenera em lista) | Nula (Balanceada) | Nula (Balanceada) |
| **Regime Warm Start Suportado** | Sim ($R=1$, $0,02\text{ ms}$) | Sim ($R=1$, $0,02\text{ ms}$) | Sim ($R=1$, $0,01\text{ ms}$) | Não se aplica (Reside em RAM) |
| **Espaço Ocupado pelo Índice** | Mínimo ($\approx 1\%$) | Moderado ($\mathcal{O}(N)$) | Moderado ($\mathcal{O}(N)$) | Zero em disco (Reside em RAM) |

---

## 💡 Principais Conclusões

1. **Gargalo de I/O:** O tempo de execução das consultas em memória secundária é estritamente dominado pelo número de acessos/transferências de blocos do disco para a RAM. As comparações de chaves executadas em CPU têm impacto negligenciável.
2. **Inviabilidade da BST Externa:** A ausência de rebalanceamento provoca a degeneração da estrutura para $\mathcal{O}(N^2)$ durante a montagem a frio em arquivos ordenados, gerando *timeouts* para volumes $N \ge 20.000$.
3. **Eficiência do ISA:** Oferece excelente taxa de compressão de índice ($\approx 1\%$), mas exige rigidez absoluta na ordenação ascendente dos registros de entrada.
4. **Supremacia das Árvores Multi-vias (B e B\*):** Imunes à ordem dos dados. A **Árvore B** oferece garantia de persistência e baixo consumo de RAM ($h \le 5$ acessos), enquanto a **Árvore B\*** obtém o custo ótimo de **exatamente 1 leitura de disco** na busca por meio da navegação acelerada na RAM.

---

## ⚡ Compilação e Execução

### Pré-requisitos
- Compilador C++23 instalado (`g++-13` ou `clang++-18`).
- Build system `cmake` (versão 3.28 ou superior).

```bash
# 1. Clonar o repositório
git clone https://github.com/usuario/repositorio.git
cd repositorio

# 2. Criar diretório de build e compilar
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# 3. Executar o binário de benchmark
./bin/search_benchmark
```

---

## 📄 Licença

Este projeto é distribuído sob a licença [MIT](LICENSE).
```

eof

I have generated the `README.md` file incorporating the details from your practical report, including code snippet highlights, structural comparisons, benchmark statistics, and setup instructions.
