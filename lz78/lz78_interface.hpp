#pragma once

#include "binary_trie.hpp"
#include <util/buffered_reader.hpp>

class ILZ78Consumer {
public:
    virtual void consume(const index_t ref, const char_t c) = 0;
    virtual size_t num_factors() const = 0;
};

class LZ78Interface {
private:
    ILZ78Consumer* m_consumer;
    
    BinaryTrie m_trie;
    index_t m_current;

public:
    inline LZ78Interface(ILZ78Consumer* consumer) : m_consumer(consumer) {
        m_current = m_trie.root();
    }
    
    inline void compress(std::istream& in) {
        // process stream
        BufferedReader<char_t> r(in, 1024 * 1024 * 1024); // 1Mi
        while(r) {
            const auto c = r.read();
            
            // try to navigate trie
            auto child = m_trie.get_child(m_current, c);
            if(child) {
                m_current = child;
            } else {
                m_consumer->consume(m_current, c);
                m_trie.insert_child(m_current, c);
                m_current = m_trie.root();
            }
        }
        
        // possibly output final factor
        if(m_current) {
            m_consumer->consume(m_current, 0);
        }
    }
};
