# Template vs. Virtual

This repository contains benchmarks to test performance differences of choosing class templates over interfaces, the latter causing method invocation indirections via a vtable, in selected use cases.

## Setup

### Benchmark Systems

These are the systems we ran the benchmark on. We compiled the code using *g++ 10.3.0* on all systems using the `-O3` flag for optimizations (CMake `Release` build).

| System | CPU                             | Frequency [GHz] | L1 [K] | L2 [K] | L3 [K] |
| ------ | ------------------------------- | --------------- | ------ | ------ | ------ |
| flint  | Intel(R) Core(TM) i7-8700       | 3.20            | 32     | 256    | 12288  |
| cstd01 | Intel(R) Xeon(R) CPU E5-2640 v4 | 2.40            | 32     | 256    | 25600  |

## LZ78 Compression

In our first use case, we compute the LZ78 factorization of an input file.

For this, we implement an *LZ78 Trie* as well as a *LZ78 Consumer*. The trie is used to find factors, while the consumer processes them in some way. In our case, the trie is implemented as a *binary* trie (first-child-next-sibling representation) and the consumer simply inserts the factors into a `std::vector`.

Let *N* be the number of input characters and *Z* be the number of LZ78 factors. Then our LZ78 compressor performs between *N* and *3N* method invocations on the trie and exactly *Z* invocations on the consumer.

We pass the trie and consumer implementations as either a template parameter &ndash; then, their types are known at compile time and the compiler may perform optimizations and inlining. Alternatively, we pass them as a pointer to an interface type that they implement &ndash; then, method invocations must be done via a vtable indirection and the same optimizations are not possible. We ensure that no devirtualization can be done by the compiler by making the actual implementation depend on command-line arguments, i.e., it is only known at runtime.

Note that in the case of interface usage, the vtables are very small: the consumer interface declares two methods and the trie interface defines four. Therefore, vtables are very likely to be cached in their entirety.

| Code | LZ78 Trie          | LZ78 Consumer      | Virtual Method Invocations |
| ---- | ------------------ | ------------------ | -------------------------- |
| TT   | Template Parameter | Template Parameter | 0                          |
| TI   | Template Parameter | Interface          | *Θ(Z)*                     |
| IT   | Interface          | Template Parameter | *Θ(N)*                     |
| II   | Interface          | Interface          | *Θ(N+Z)*                   |

### Input File

We choose the 200 MiB prefix of the *dna* text from the [Pizza & Chili Corpus](http://pizzachili.dcc.uchile.cl/), and thus we have *N=209,715,200* and *Z=16,373,735*.

### Results

The results can be viewn in [results/lz78.pdf](results/lz78.pdf).
