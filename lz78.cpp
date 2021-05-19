#include <fstream>
#include <vector>

#include <lz78/lz78_ii.hpp>
#include <lz78/lz78_it.hpp>
#include <lz78/lz78_ti.hpp>
#include <lz78/lz78_tt.hpp>

#include <lz78/consumers.hpp>
#include <lz78/tries.hpp>

#include <util/buffered_reader.hpp>
#include <util/time.hpp>

#include <tlx/cmdline_parser.hpp>

struct {
    std::string filename;
    size_t file_size;
    
    bool dummy_trie = false;
    bool dummy_consumer = false;
} options;

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
    std::cout << "RESULT algo=" << name << " input=" << options.filename << " input_size=" << options.file_size << ", num_factors=" << num_factors << " time=" << dt << std::endl;
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
        options.file_size = 0;
        while(r) { chksum += r.read(); ++options.file_size; }
        std::cout << "file chksum=" << chksum << std::endl;
    }
    
    // trie template, consumer template (TT)
    {
        LZ78Consumer_Inline consumer;
        const auto dt = bench([&](){ return LZ78_TT<BinaryTrie_Inline, decltype(consumer)>(consumer); });
        print_result("TT", consumer.num_factors(), dt);
    }

    // trie template, consumer interface (TI)
    {
        ILZ78Consumer* consumer;
        if(options.dummy_consumer) {
            consumer = new LZ78Consumer_Dummy();
            std::cout << "you shouldn't actually use this flag, we just want to make sure that the type is determined at runtime!" << std::endl;
        } else {
            consumer = new LZ78Consumer_Interface();
        }
        
        const auto dt = bench([&](){ return LZ78_TI<BinaryTrie_Inline>(consumer); });
        print_result("TI", consumer->num_factors(), dt);
        delete consumer;
    }
    
    // trie interface, consumer template (IT)
    {
        LZ78Consumer_Inline consumer;
        ILZ78Trie* trie;
        if(options.dummy_trie) {
            trie = new LZ78Trie_Dummy();
            std::cout << "you shouldn't actually use this flag, we just want to make sure that the type is determined at runtime!" << std::endl;
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
            consumer = new LZ78Consumer_Dummy();
            std::cout << "you shouldn't actually use this flag, we just want to make sure that the type is determined at runtime!" << std::endl;
        } else {
            consumer = new LZ78Consumer_Interface();
        }
        
        ILZ78Trie* trie;
        if(options.dummy_trie) {
            trie = new LZ78Trie_Dummy();
            std::cout << "you shouldn't actually use this flag, we just want to make sure that the type is determined at runtime!" << std::endl;
        } else {
            trie = new BinaryTrie_Interface();
        }
        
        const auto dt = bench([&](){ return LZ78_II(trie, consumer); });
        print_result("II", consumer->num_factors(), dt);
        delete consumer;
        delete trie;
    }
}
