# Computing [TF-IDF](https://en.wikipedia.org/wiki/Tf%E2%80%93idf)
Short for term frequency–inverse document frequency, is a numerical statistic that is intended to reflect how important a word is to a document in a collection or corpus.

Can be usefull for retrieval informations.

## Versions
cmake 3.21.1
clang version 12.0.5
## Scripts
- cmake --build ./build
- .\build\src\TfIdf_run.exe
- cp ./build/src/TfIdf_run .
- ./TfIdf_run resources hey

> $ Term frequency: 28.571%
> $ Inverse document frequency: 33.333%
## Roadmap
- In the pseudo "threadpool" try to wait for the smallest file using stat.h

> Fork of [Basic cmake project](https://github.com/adrien-zinger/basic_cpp_cmake)
