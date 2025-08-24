# Thread-pool

A Thread Pool consists of a queue of tasks and several threads
that take tasks out of the queue and execute them in parallel.

## Compile

### Debug:
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../
cmake --build . --config Debug
```

### Release:
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
cmake --build . --config Release
```

## Build and run tests

Build:
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON ../
cmake --build . --config Debug
```

Run
```
ctest
```