### Research go2wasm compilation

## Smallest wasm file size
The `docker run --rm -v $(pwd):/src tinygo/tinygo:0.8.0 tinygo build -gc=none -panic=trap -no-debug -o /src/out/transition-smallest.wasm /src/transition.go` result is 731 bytes.

But if you use the **append** function, you must remove `-gc=none` tinygo arg `docker run --rm -v $(pwd):/src tinygo/tinygo:0.8.0 tinygo build -panic=trap -no-debug -o /src/out/transition-gc.wasm /src/transition.go` and then the size will be 3149 bytes.

To get wat and c files, use the following command from https://github.com/WebAssembly/wabt :

```bash
cd out

wasm2c   transition-smallest.wasm                  -o transition-smallest.c
wasm2wat transition-smallest.wasm --generate-names -o transition-smallest.wat

wasm2c   transition-gc.wasm                  -o transition-gc.c
wasm2wat transition-gc.wasm --generate-names -o transition-gc.wat
```