#pragma once

#include <util/buffered_reader.hpp>

template<typename Trie, typename Consumer>
class LZ78_TT {
private:
    Consumer* m_consumer;
    Trie m_trie;
    
    index_t m_current;

public:
    inline LZ78_TT(Consumer& consumer) : m_consumer(&consumer) {
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
