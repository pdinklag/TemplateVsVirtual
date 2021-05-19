#include <fstream>
#include <vector>

#include <lz78/lz78_ii.hpp>
#include <lz78/lz78_it.hpp>
#include <lz78/lz78_ti.hpp>
#include <lz78/lz78_tt.hpp>

#include <lz78/binary_trie.hpp>

#include <util/buffered_reader.hpp>
#include <util/time.hpp>

#include <tlx/cmdline_parser.hpp>

struct {
    std::string filename;
    bool dummy_trie = false;
    bool dummy_consumer = false;
} options;

struct LZ78ConsumerInline {
    std::vector<index_t> refs;
    std::vector<char_t> chars;
    
    inline void consume(const index_t ref, const char_t c) {
        refs.emplace_back(ref);
        chars.emplace_back(c);
    }
    
    inline size_t num_factors() const {
        return refs.size();
    }
};

struct LZ78ConsumerNoInline {
    std::vector<index_t> refs;
    std::vector<char_t> chars;
    
    void __attribute__((noinline)) consume(const index_t ref, const char_t c) {
        refs.emplace_back(ref);
        chars.emplace_back(c);
    }
    
    size_t __attribute__((noinline)) num_factors() const {
        return refs.size();
    }
};

struct LZ78ConsumerVirtual : public ILZ78Consumer {
    std::vector<index_t> refs;
    std::vector<char_t> chars;
    
    virtual void consume(const index_t ref, const char_t c) override {
        refs.emplace_back(ref);
        chars.emplace_back(c);
    }
    
    virtual size_t num_factors() const override {
        return refs.size();
    }
};

struct DummyLZ78Consumer : public ILZ78Consumer {
    virtual void consume(const index_t ref, const char_t c) override { }
    virtual size_t num_factors() const override { return 0; }
};

struct DummyLZ78Trie : public ILZ78Trie {
    virtual index_t root() const override { return 0; }
    virtual index_t get_child(const index_t v, const char_t c) override { return 0; }
    virtual index_t insert_child(const index_t v, const char_t c) override { return 0; }
    virtual size_t size() const override { return 0; }
};

template<typename ctor_t>
uint64_t bench(ctor_t ctor) {
    std::ifstream input(options.filename);
    {
        const auto t0 = time();
        {
            auto c = ctor();
            c.compress(input);
        }
        return time() - t0;
    }
}

void print_result(std::string&& name, const size_t num_factors, const uint64_t dt) {
    std::cout << "RESULT algo=" << name << " input=" << options.filename << " num_factors=" << num_factors << " time=" << dt << std::endl;
}

int main(int argc, char** argv) {
    tlx::CmdlineParser cp;
    cp.add_param_string("file", options.filename, "The input file.");
    cp.add_flag('X', "dummy_trie", options.dummy_trie, "Internal use only.");
    cp.add_flag('Y', "dummy_consumer", options.dummy_consumer, "Internal use only.");
    
    if(!cp.process(argc, argv)) {
        return -1;
    }
    
    // read the input file once to avoid bias
    {
        std::ifstream in(options.filename);
        BufferedReader<char_t> r(in, 1024 * 1024 * 1024);
        uint64_t chksum = 0;
        while(r) chksum += r.read();
        std::cout << "file chksum=" << chksum << std::endl;
    }
    
    // trie template, consumer template (TT)
    {
        LZ78ConsumerInline consumer;
        const auto dt = bench([&](){ return LZ78_TT<BinaryTrie_Inline, decltype(consumer)>(consumer); });
        print_result("TT", consumer.num_factors(), dt);
    }

    // trie template, consumer interface (TI)
    {
        ILZ78Consumer* consumer;
        if(options.dummy_consumer) {
            consumer = new DummyLZ78Consumer();
            std::cout << "you shouldn't actually use this flag, we just want to make sure that the consumer type is determined at runtime!" << std::endl;
        } else {
            consumer = new LZ78ConsumerVirtual();
        }
        
        const auto dt = bench([&](){ return LZ78_TI<BinaryTrie_Inline>(consumer); });
        print_result("TI", consumer->num_factors(), dt);
        delete consumer;
    }
    
    // trie interface, consumer template (IT)
    {
        LZ78ConsumerInline consumer;
        ILZ78Trie* trie;
        if(options.dummy_trie) {
            trie = new DummyLZ78Trie();
            std::cout << "you shouldn't actually use this flag, we just want to make sure that the consumer type is determined at runtime!" << std::endl;
        } else {
            trie = new BinaryTrie_Interface();
        }
        
        const auto dt = bench([&](){ return LZ78_IT<decltype(consumer)>(trie, consumer); });
        print_result("IT", consumer.num_factors(), dt);
        delete trie;
    }

    // trie interface, consumer interface (II)
    {
        ILZ78Consumer* consumer;
        if(options.dummy_consumer) {
            consumer = new DummyLZ78Consumer();
            std::cout << "you shouldn't actually use this flag, we just want to make sure that the consumer type is determined at runtime!" << std::endl;
        } else {
            consumer = new LZ78ConsumerVirtual();
        }
        
        ILZ78Trie* trie;
        if(options.dummy_trie) {
            trie = new DummyLZ78Trie();
            std::cout << "you shouldn't actually use this flag, we just want to make sure that the consumer type is determined at runtime!" << std::endl;
        } else {
            trie = new BinaryTrie_Interface();
        }
        
        const auto dt = bench([&](){ return LZ78_II(trie, consumer); });
        print_result("II", consumer->num_factors(), dt);
        delete consumer;
        delete trie;
    }
}
