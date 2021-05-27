# Template vs. Virtual

This repository contains benchmarks to test performance differences of choosing class templates over interfaces, the latter causing method invocation indirections via a vtable, in selected use cases.

## Setup

### Benchmark Systems

These are the systems we ran the benchmark on. We compiled the code using *g++ 10.3.0* on all systems using the `-O3` flag for optimizations (CMake `Release` build).

| System  | CPU                             | Frequency [GHz] | L1 [K] | L2 [K] | L3 [K] |
| ------- | ------------------------------- | --------------- | ------ | ------ | ------ |
| flint   | Intel(R) Core(TM) i7-8700       | 3.20            | 32     | 256    | 12,288 |
| cstd01  | Intel(R) Xeon(R) CPU E5-2640 v4 | 2.40            | 32     | 256    | 25,600 |
| snail04 | AMD EPYC 7452                   | 2.35            | 32     | 512    | 16,384 |

## LZ78 Compression

In our first use case, we compute the LZ78 factorization of an input file.

For this, we implement an *LZ78 Trie* as well as a *LZ78 Consumer*. The trie is used to find factors, while the consumer processes them in some way. In our case, the trie is implemented as a *binary* trie (first-child-next-sibling representation) and the consumer simply inserts the factors into a `std::vector`.

Let *N* be the number of input characters and *Z* be the number of LZ78 factors. Then our LZ78 compressor performs between *N* and *3N* method invocations on the trie and exactly *Z* invocations on the consumer.

We pass the trie and consumer implementations as either a template parameter &ndash; then, their types are known at compile time and the compiler may perform optimizations and inlining. Alternatively, we pass them as a pointer to an interface type that they implement &ndash; then, method invocations must be done via a vtable indirection and the same optimizations are not possible. We ensure that no devirtualization can be done by the compiler by making the actual implementation depend on command-line arguments, i.e., it is only known at runtime.

Note that in the case of interface usage, the vtables are very small: the consumer interface declares two methods and the trie interface defines four. Therefore, vtables are very likely to be cached in their entirety.

| Code | LZ78 Trie          | LZ78 Consumer      | Virtual Method Invocations |
| ---- | ------------------ | ------------------ | -------------------------- |
| TT   | Template Parameter | Template Parameter | 0                          |
| TI   | Template Parameter | Interface          | *Z*                        |
| IT   | Interface          | Template Parameter | *Θ(N)*                     |
| II   | Interface          | Interface          | *Θ(N) + Z*                 |

### Input File

We choose the 200 MiB prefix of the *dna* text from the [Pizza & Chili Corpus](http://pizzachili.dcc.uchile.cl/), and thus we have *N=209,715,200* and *Z=16,373,735*.

### Results

The results are given as median throughputs over three iterations can be viewn in [results/lz78.pdf](results/lz78.pdf). The raw data is listed in [results/lz78.txt](results/lz78.txt).

## Burrows-Wheeler Transform

In the second use case, we compute the Burrows-Wheeler Transform (BWT) of an input file using its suffix array.

We access the suffix array entries indirectly via a class that overloads the `[]` operator - in practice, to name an example, this may be necessary if we use a sparse suffix array and don't store all the entries explicitly. Furthermore, the BWT is constructed step-by-step using a `push_back` operation.

Let *N* be the number of input characters. Then, in this use case, there are exactly *N* random suffix array accesses and exactly *N* pushes to the BWT in left-to-right order.

Both the suffix array and the BWT builder implementations are passed either as template parameters &ndash; allowing for compile-time optimizations &ndash; or as pointers to interface instances &ndash; requiring vtable lookups for each call. Like for [LZ78 Compression](#lz78-compression), the vtables are very small.

The suffix array for the input file is precomputed once and not included in the time measurements.

| Code | Suffix Array Accessor | BWT Builder        | Virtual Method Invocations |
| ---- | --------------------- | ------------------ | -------------------------- |
| TT   | Template Parameter    | Template Parameter | 0                          |
| TI   | Template Parameter    | Interface          | *N*                        |
| IT   | Interface             | Template Parameter | *N*                        |
| II   | Interface             | Interface          | *2N*                       |

### Input File

We choose the 200 MiB prefix of the *dna* text from the [Pizza & Chili Corpus](http://pizzachili.dcc.uchile.cl/), and thus we have *N=209,715,200*.

### Results

The results are given as median throughputs over three iterations can be viewn in [results/bwt.pdf](results/bwt.pdf). The raw data is listed in [results/bwt.txt](results/bwt.txt).
