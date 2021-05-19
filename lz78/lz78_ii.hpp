#pragma once

#include "interfaces.hpp"
#include <util/buffered_reader.hpp>
#include <util/typedefs.hpp>

class LZ78_II {
private:
    ILZ78Trie* m_trie;
    ILZ78Consumer* m_consumer;
    
    index_t m_current;

public:
    inline LZ78_II(ILZ78Trie* trie, ILZ78Consumer* consumer) : m_trie(trie), m_consumer(consumer) {
        m_current = m_trie->root();
    }
    
    inline void compress(std::istream& in) {
        // process stream
        BufferedReader<char_t> r(in, 1024 * 1024 * 1024); // 1Mi
        while(r) {
            const auto c = r.read();
            
            // try to navigate trie
            auto child = m_trie->get_child(m_current, c);
            if(child) {
                m_current = child;
            } else {
                m_consumer->consume(m_current, c);
                m_trie->insert_child(m_current, c);
                m_current = m_trie->root();
            }
        }
        
        // possibly output final factor
        if(m_current) {
            m_consumer->consume(m_current, 0);
        }
    }
};
