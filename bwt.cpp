#include <fstream>
#include <vector>

#include <bwt/bwt.hpp>

#include <bwt/bwt_builders.hpp>
#include <bwt/sa_accessors.hpp>

#include <util/buffered_reader.hpp>
#include <util/time.hpp>

#include <divsufsort.h>

#include <tlx/cmdline_parser.hpp>

static_assert(sizeof(index_t) == sizeof(saidx_t));
static_assert(sizeof(char_t) == sizeof(sauchar_t));

struct {
    std::string filename;
    size_t file_size;
    
    bool dummy_sa = false;
    bool dummy_bwt = false;
} options;

template<typename algorithm_t>
uint64_t bench(algorithm_t algorithm) {
    const auto t0 = time();
    algorithm();
    return time() - t0;
}

void print_result(std::string&& name, const size_t bwt_length, const uint64_t dt) {
    std::cout << "RESULT algo=" << name << " input=" << options.filename << " input_size=" << options.file_size << " bwt_length=" << bwt_length << " time=" << dt << std::endl;
}

int main(int argc, char** argv) {
    tlx::CmdlineParser cp;
    cp.add_param_string("file", options.filename, "The input file.");
    cp.add_flag('X', "dummy_sa", options.dummy_sa, "Internal use only.");
    cp.add_flag('Y', "dummy_bwt", options.dummy_bwt, "Internal use only.");
    
    if(!cp.process(argc, argv)) {
        return -1;
    }
    
    // read the input file
    std::string input;
    {
        std::ifstream in(options.filename);
        BufferedReader<char_t> r(in, 1024 * 1024 * 1024);
        while(r) { input.push_back(r.read()); }
        options.file_size = input.size();
    }
    
    // construct the suffix array
    index_t* sa;
    {
        input.push_back(0); // divsufsort needs this
        sa = new index_t[input.size()];
        
        const auto t0 = time();
        divsufsort((const sauchar_t*)input.data(), (saidx_t*)sa, (saidx_t)input.size());
        const auto dt = time() - t0;
        std::cout << "constructed suffix array in " << dt << " ms" << std::endl;
    }
    
    // accessor template, bwt template (TT)
    {
        SuffixArrayAccessor_Inline sa_access { sa };
        BWTBuilder_Inline bwt;
        const auto dt = bench([&](){ BWT_TT(input, sa_access, bwt); });
        print_result("TT", bwt.length(), dt);
    }
    
    // accessor template, bwt interface (TI)
    {
        SuffixArrayAccessor_Inline sa_access { sa };
        IBWTBuilder* bwt;
        if(options.dummy_bwt) {
            bwt = new BWTBuilder_Dummy();
            std::cout << "you shouldn't actually use this flag, we just want to make sure that the type is determined at runtime!" << std::endl;
        } else {
            bwt = new BWTBuilder_Interface();
        }
        
        const auto dt = bench([&](){ BWT_TI(input, sa_access, bwt); });
        print_result("TI", bwt->length(), dt);
        
        delete bwt;
    }

    // accessor interface, bwt template (IT)
    {
        ISuffixArrayAccess* sa_access;
        if(options.dummy_sa) {
            sa_access = new SuffixArrayAccessor_Dummy();
            std::cout << "you shouldn't actually use this flag, we just want to make sure that the type is determined at runtime!" << std::endl;
        } else {
            sa_access = new SuffixArrayAccessor_Interface(sa);
        }
        
        BWTBuilder_Inline bwt;
        
        const auto dt = bench([&](){ BWT_IT(input, sa_access, bwt); });
        print_result("IT", bwt.length(), dt);
        
        delete sa_access;
    }

    // accessor interface, bwt interface (II)
    {
        ISuffixArrayAccess* sa_access;
        if(options.dummy_sa) {
            sa_access = new SuffixArrayAccessor_Dummy();
            std::cout << "you shouldn't actually use this flag, we just want to make sure that the type is determined at runtime!" << std::endl;
        } else {
            sa_access = new SuffixArrayAccessor_Interface(sa);
        }
        
        IBWTBuilder* bwt;
        if(options.dummy_bwt) {
            bwt = new BWTBuilder_Dummy();
            std::cout << "you shouldn't actually use this flag, we just want to make sure that the type is determined at runtime!" << std::endl;
        } else {
            bwt = new BWTBuilder_Interface();
        }
        
        const auto dt = bench([&](){ BWT_II(input, sa_access, bwt); });
        print_result("II", bwt->length(), dt);
        
        delete sa_access;
        delete bwt;
    }
    
    /*
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
    */
    
    // clean up
    delete[] sa;
}
