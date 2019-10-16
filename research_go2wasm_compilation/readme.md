### Research go2wasm compilation

## Smallest wasm file size
The `docker run --rm -v $(pwd):/src tinygo/tinygo:0.8.0 tinygo build -gc=none -panic=trap -no-debug -o /src/out/transition-smallest.wasm /src/transition.go` result is 731 bytes.

But if you wont use **append** function, then you must remove `-gc=none` tinygo arg    
`docker run --rm -v $(pwd):/src tinygo/tinygo:0.8.0 tinygo build -panic=trap -no-debug -o /src/out/transition-gc.wasm /src/transition.go` and have result size 3149 bytes.

for get wat and c file used next command from https://github.com/WebAssembly/wabt :

```bash
export PATH=$PATH:$HOME/wabt/out/
cd out

wasm2c   transition-smallest.wasm                  -o transition-smallest.c
wasm2wat transition-smallest.wasm --generate-names -o transition-smallest.wat

wasm2c   transition-gc.wasm                  -o transition-gc.c
wasm2wat transition-gc.wasm --generate-names -o transition-gc.wat
```