#include <fstream>
#include <vector>

#include <lz78/lz78_interface.hpp>
#include <lz78/lz78_template.hpp>

#include <util/buffered_reader.hpp>
#include <util/time.hpp>

#include <tlx/cmdline_parser.hpp>

struct {
    std::string filename;
    bool noop = false;
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

struct LZ78ConsumerVirtualNoop : public ILZ78Consumer {
    virtual void consume(const index_t ref, const char_t c) override {
    }
    
    virtual size_t num_factors() const override {
        return 0;
    }
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
    cp.add_flag('N', "noop", options.noop, "Internal use only.");
    
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
    
    // template, inline
    {
        LZ78ConsumerInline consumer;
        const auto dt = bench([&](){ return LZ78Template(consumer); });
        print_result("TemplateInline", consumer.num_factors(), dt);
    }

    // template, noinline
    {
        LZ78ConsumerNoInline consumer;
        const auto dt = bench([&](){ return LZ78Template(consumer); });
        print_result("TemplateNoInline", consumer.num_factors(), dt);
    }
    
    // interface (virtual calls)
    {
        ILZ78Consumer* consumer;
        if(options.noop) {
            consumer = new LZ78ConsumerVirtualNoop();
            std::cout << "you shouldn't actually use this flag, we just want to make sure that the consumer type is determined at runtime!" << std::endl;
        } else {
            consumer = new LZ78ConsumerVirtual();
        }
        
        const auto dt = bench([&](){ return LZ78Interface(consumer); });
        print_result("Interface", consumer->num_factors(), dt);
        delete consumer;
    }
}
