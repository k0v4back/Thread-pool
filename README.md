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

## Run tests 

After build go to build directory and run:
```
ctest
```

Or just start in build directory:
```
./test_ThreadPool
```