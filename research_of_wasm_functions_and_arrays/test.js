function helperGetNum(arr, ptr, byteCount) {
    let result = 0;
    for ( var i = byteCount-1; i >= 0; --i) {
        result = (result * 256) + arr[ptr+i];
    }
    return result;
}

function helperDepositsToString(deposits) {
    let result = "\n";
    for (i = 0; i< deposits.length; ++i){
        result += 
            "{ pubKey:                ["+ deposits[i].pubKey.join(",") +"],\n"+
            "  withdrawalCredentials: ["+deposits[i].withdrawalCredentials.join(",") +"],\n"+
            "  amount: "+deposits[i].amount+" }\n";
    }
    return result;
}

function transition(wasmSource, preState, block) {
    if (preState.length != 32) {
        console.error("preState.length = ", preState.length)
        throw "preState.length != 32"
    }
    // ===========================
    // == PREPARE WASM INSTANCE ==
    // ===========================
    const memory = new WebAssembly.Memory( { initial: 256, maximum: 256  } );
    const heap = new Uint8Array(memory.buffer);
    const imports = { env: { memory: memory } };
    const wasmModule = new WebAssembly.Module(wasmSource);
    const wasmInstance = new WebAssembly.Instance(wasmModule, imports);
    // ===========================
    // ==== PREPARE WASM HEAP ====
    // ===========================
    // uint8_t preState[32];
    for (let i = 0; i < 32; ++i) {
        heap[i] = preState[i];
    }
    // uint8_t postState[32];
    for (let i = 32; i < 64; ++i) {
        heap[i] = preState[i-32];  // init postState to preState
    }
    // deposits_t* deposits;
    let depositsPtr = 72+block.length;
    heap[64] = (depositsPtr & 0x000000ff);
    heap[65] = (depositsPtr & 0x0000ff00) >> 8;
    heap[66] = (depositsPtr & 0x00ff0000) >> 16;
    heap[67] = (depositsPtr & 0xff000000) >> 24;
    // uint32_t blockSize;
    heap[68] = (block.length & 0x000000ff);
    heap[69] = (block.length & 0x0000ff00) >> 8;
    heap[70] = (block.length & 0x00ff0000) >> 16;
    heap[71] = (block.length & 0xff000000) >> 24;
    // uint8_t block[]
    for (let i = 72; i < 72+block.length; ++i) {
        heap[i] = block[i-72];
    }
    // (*args).(*deposits).count
    heap[72+block.length+0] = 0
    heap[72+block.length+1] = 0
    heap[72+block.length+2] = 0
    heap[72+block.length+3] = 0
    // ===========================
    // ======= RUN WASM ==========
    // ===========================
    const exitCode = wasmInstance.exports._transition();
    if (exitCode !== 0) {
        console.error("exit code", exitCode)
        return { postState: preState, deposits: [] }
    }
    // ===========================
    // ====== LOAD RESULT ========
    // ===========================
    // read postState
    const postState = heap.slice(32, 64);
    // read deposits
    depositsPtr = helperGetNum(heap, 64, 4);
    //TODO check that the depositsPtr does not point out the heap
    const depositsCount = helperGetNum(heap, depositsPtr, 4);

    const deposits = []
    for (let i = 0; i < depositsCount; ++i) {
        let depositPtr = depositsPtr+8/*TODO why 8?*/ + i*(48+48+8);
        //TODO check that the depositPtr does not point out the heap
        deposits.push({
            pubKey:                heap.slice(depositPtr, depositPtr+48),
            withdrawalCredentials: heap.slice(depositPtr+48, depositPtr+96),
            amount:                helperGetNum(heap, depositPtr+96, 8),
        });
    }

    return {postState, deposits};
}

const fs = require('fs');
const wasmSource = new Uint8Array(fs.readFileSync("out/transition.wasm"));

preState = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31];
block    = [1,2,3];

let {postState, deposits} = transition(wasmSource, preState, block);

console.log( "block     [", block.join(","), "]");
console.log( "preState  [", preState.join(","), "]" );
console.log( "postState [", postState.join(","), "]" );
console.log( "deposits", helperDepositsToString(deposits) );
