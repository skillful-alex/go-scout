package ewasm

import (
	"errors"

	logrus "github.com/sirupsen/logrus"
	wasm "github.com/wasmerio/go-ext-wasm/wasmer"
)

var log = logrus.WithField("prefix", "ewasm")

//Deposit define type returned
type Deposit struct {
	PubKey                [48]byte
	WithdrawalCredentials [48]byte
	Amount                uint64
}

//ExecuteCode executes wasm code in the ethereum environment
func ExecuteCode(execCode []byte, preState [32]byte, blockData []byte) (postState [32]byte, deposits []Deposit, error error) {
	imports, err := getImports()
	if err != nil {
		log.WithError(err).Error("imports error")
		return preState, nil, err
	}

	instance, err := wasm.NewInstanceWithImports(execCode, imports)
	if err != nil {
		log.WithError(err).Error("error creating instance")
		return preState, nil, err
	}
	defer instance.Close()

	exec := instance.Exports["_Z4execPc"]
	if exec == nil {
		log.Warnf("exec function not exported. All exports: %v", instance.Exports)
		return preState, nil, errors.New("exec function not exported")
	}
	result, err := exec(preState)

	if err != nil {
		log.WithError(err).Error("error executing instance")
		return preState, nil, err
	}

	log.WithField("result", result).Debug("executing instance")

	//TODO Get post_state_root and deposits
	return preState, nil, nil
}
