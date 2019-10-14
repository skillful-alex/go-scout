package ewasm

import (
	"errors"
	logrus "github.com/sirupsen/logrus"
	wasm "github.com/wasmerio/go-ext-wasm/wasmer"
)

var log = logrus.WithField("prefix", "ewasm")

//Deposit TODO define type
type Deposit struct{}

//ExecuteCode executes wasm code in the ethereum environment
func ExecuteCode(execCode []byte, preState []byte, blockData []byte) (postState []byte, deposits []Deposit, error error) {
	//TODO  check preState, postState len == 32

	imports, err := getImports()
	if err != nil {
		log.WithError(err).Error("imports error")
		return nil, nil, err
	}

	instance, err := wasm.NewInstanceWithImports(execCode, imports)
	if err != nil {
		log.WithError(err).Error("error creating instance")
		return nil, nil, err
	}
	defer instance.Close()

	main := instance.Exports["main"]
	if main == nil {
		log.Warnf("main function not exported. All exports: %v", instance.Exports)
		return nil, nil, errors.New("main function not exported")
	}
	result, err := main()

	if err != nil {
		log.WithError(err).Error("error executing instance")
		return nil, nil, err
	}

	log.WithField("result", result).Debug("executing instance")

	//TODO Get post_state_root and deposits
	return nil, nil, nil
}
