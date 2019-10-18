package ewasm

import (
	wasm "github.com/wasmerio/go-ext-wasm/wasmer"
)

func initMemory(memory *wasm.Memory, preState [32]byte, block []byte) error {
	heap := memory.Data()
	// uint8_t preState[32];
	copyArray(heap, 0, preState[:], 0, 32)
	// uint8_t postState[32];
	copyArray(heap, 32, preState[:], 0, 32)
	// deposits_t* deposits;
	depositsPtr := 72 + len(block)
	putVarint(heap, uint64(depositsPtr), 64, 4)
	// uint32_t blockSize;
	putVarint(heap, uint64(len(block)), 68, 4)
	// uint8_t block[]
	copyArray(heap, 72, block, 0, len(block))
	// (*args).(*deposits).count
	putVarint(heap, 0, 72+len(block), 4)
	return nil
}

func readMemory(memory *wasm.Memory) ([32]byte, []Deposit, error) {
	heap := memory.Data()
	var postState [32]byte
	copyArray(postState[:], 0, heap, 32, 32)
	// read deposits
	depositsPtr := int(helperGetNum(heap, 64, 4))
	//TODO check that the depositsPtr does not point out the heap
	depositsCount := int(helperGetNum(heap, depositsPtr, 4))

	deposits := make([]Deposit, depositsCount)
	for i := 0; i < depositsCount; i++ {
		depositPtr := depositsPtr + 8 /*TODO why 8?*/ + i*(48+48+8)
		copyArray(deposits[i].PubKey[:], 0, heap, depositPtr, 48)
		copyArray(deposits[i].WithdrawalCredentials[:], 0, heap, depositPtr+48, 48)
		deposits[i].Amount = helperGetNum(heap, depositPtr+96, 8)
	}

	return postState, deposits, nil
}
