# Weechess

A wee-litle chess engine library and application.

## Getting Started

Build the project from source using CMake:

```bash
mkdir build && cd build
cmake ../
make
```

To run the tests after building the project:

```bash
make && make test
```

To re-generate the book data that is bundled in the library:

```bash
make && ./weechess-bookc ../data/*.txt > ../lib/generated/book_data.cpp
```
