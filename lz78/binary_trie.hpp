#pragma once

#include <vector>
#include <util/typedefs.hpp>

#include "interfaces.hpp"

class BinaryTrie_Interface : public ILZ78Trie {
private:
    static constexpr index_t ROOT = 0;

    std::vector<char_t>  m_char;
    std::vector<index_t> m_first_child;
    std::vector<index_t> m_next_sibling;

    index_t emplace_back(char_t c) {
        const size_t sz = size();
        m_char.emplace_back(c);
        m_first_child.emplace_back(ROOT);
        m_next_sibling.emplace_back(ROOT);
        return (index_t)sz;
    }

public:
    inline BinaryTrie_Interface() {
        m_char.reserve(16);
        m_first_child.reserve(16);
        m_next_sibling.reserve(16);
        
        emplace_back(0); // node 0 is the root
    }

    virtual index_t root() const override {
        return ROOT;
    }
    
    virtual size_t size() const override {
        return m_char.size();
    }
    
    virtual index_t get_child(const index_t node, const char_t c) override {
        const auto first_child = m_first_child[node];
        auto v = first_child;

        {
            // MTF
            auto prev_sibling = ROOT;
            while(v != ROOT && m_char[v] != c) {
                prev_sibling = v;
                v = m_next_sibling[v];
            }
            
            if(v && v != first_child) {
                m_next_sibling[prev_sibling] = m_next_sibling[v];
                m_next_sibling[v] = first_child;
                m_first_child[node] = v;
            }
        }
        
        return v;
    }
    
    virtual index_t insert_child(const index_t parent, const char_t c) override {
        auto new_child = emplace_back(c);
        m_next_sibling[new_child] = m_first_child[parent];
        m_first_child[parent] = new_child;
        return new_child;
    }
};

class BinaryTrie_Inline {
private:
    static constexpr index_t ROOT = 0;

    std::vector<char_t>  m_char;
    std::vector<index_t> m_first_child;
    std::vector<index_t> m_next_sibling;

    index_t emplace_back(char_t c) {
        const size_t sz = size();
        m_char.emplace_back(c);
        m_first_child.emplace_back(ROOT);
        m_next_sibling.emplace_back(ROOT);
        return (index_t)sz;
    }

public:
    inline BinaryTrie_Inline() {
        m_char.reserve(16);
        m_first_child.reserve(16);
        m_next_sibling.reserve(16);
        
        emplace_back(0); // node 0 is the root
    }

    inline index_t root() const {
        return ROOT;
    }
    
    inline size_t size() const {
        return m_char.size();
    }
    
    inline index_t get_child(const index_t node, const char_t c) {
        const auto first_child = m_first_child[node];
        auto v = first_child;

        {
            // MTF
            auto prev_sibling = ROOT;
            while(v != ROOT && m_char[v] != c) {
                prev_sibling = v;
                v = m_next_sibling[v];
            }
            
            if(v && v != first_child) {
                m_next_sibling[prev_sibling] = m_next_sibling[v];
                m_next_sibling[v] = first_child;
                m_first_child[node] = v;
            }
        }
        
        return v;
    }
    
    inline index_t insert_child(const index_t parent, const char_t c) {
        auto new_child = emplace_back(c);
        m_next_sibling[new_child] = m_first_child[parent];
        m_first_child[parent] = new_child;
        return new_child;
    }
};
