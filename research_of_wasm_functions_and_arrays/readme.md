## Research of the transition function signature

* transition.c          - defines the transition function to compile in wasm
* test.js               - script for test/execution wasm
* out/transition.wasm   - compilation result

Compilation c to wasm:
```bash
docker run --rm -v $(pwd):/src trzeci/emscripten  \
   emcc -O3                                       \
   /src/transition.c -o /src/out/transition.wasm  \
   -s EXPORTED_FUNCTIONS='["_transition"]'        \
   -s MALLOC="emmalloc"
```
* Wasm size: 258 bytes.
* The only import is memory.
* The only export is "transition" function.

Test:
```bash
node test.js 
```

### What lies ahead:
  0) golang implementation
  1) wasm code for ETH transfer
  2) define the gas calculation
  3) define the WASM validation:
      * wasm version
      * memory requirements
      * remove non-deterministic operations
