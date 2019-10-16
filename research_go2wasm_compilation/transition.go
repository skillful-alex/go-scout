package main

type Deposit struct {
	PublicKey             [48]byte
	WithdrawalCredentials [48]byte
	Amount                uint64
}

func main() {}

//go:export Transition
func Transition(preState [32]byte, blockData []byte) (postState [32]byte, deposits []Deposit) {
	//check blockData access
	switch blockData[1] {
	case 1:
		// check pre/postState access
		for i := 0; i < 32; i = i + 1 {
			postState[0] += preState[i]
		}
	case 2:
		// check deposits access
		// for compile with append, remove -gc=none tinygo arg. wasm result: 3149 bytes
		//		deposits = append(deposits,
		//			Deposit{
		//				PublicKey:             []byte{},
		//				WithdrawalCredentials: []byte{},
		//				Amount:                1,
		//			})
	}
	return preState, deposits
}
