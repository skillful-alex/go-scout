package ewasm

// #include <stdint.h>
//
// extern void eth2_loadPreStateRoot(void *context, uint32_t x);
// extern uint32_t eth2_blockDataSize(void *context);
// extern void eth2_savePostStateRoot(void *context, uint32_t x);
import "C"

import (
	"unsafe"

	logrus "github.com/sirupsen/logrus"
	wasm "github.com/wasmerio/go-ext-wasm/wasmer"
)

var log = logrus.WithField("prefix", "ewasm")

//Deposit TODO define type
type Deposit struct{}

//export eth2_loadPreStateRoot
func eth2_loadPreStateRoot(context unsafe.Pointer, x uint32) {
	return
}

//export eth2_blockDataSize
func eth2_blockDataSize(context unsafe.Pointer) uint32 {
	return 0
}

//export eth2_savePostStateRoot
func eth2_savePostStateRoot(context unsafe.Pointer, x uint32) {
	return
}

//ExecuteCode executes wasm code in the ethereum environment
func ExecuteCode(execCode []byte, preState []byte, blockData []byte) (postState []byte, deposits []Deposit, error error) {
	//TODO  check preState, postState len == 32

	imports := wasm.NewImports().Namespace("env")
	imports, err := imports.Append("eth2_loadPreStateRoot", eth2_loadPreStateRoot, C.eth2_loadPreStateRoot)
	if err != nil {
		log.WithError(err).Errorf("execCode: %v", imports)
	}

	imports, _ = imports.Append("eth2_blockDataSize", eth2_blockDataSize, C.eth2_blockDataSize)
	imports, _ = imports.Append("eth2_savePostStateRoot", eth2_savePostStateRoot, C.eth2_savePostStateRoot)

	instance, err := wasm.NewInstanceWithImports(execCode, imports)
	if err != nil {
		lastErrorString, lastError := wasm.GetLastError()
		log.WithError(lastError).Error(lastErrorString)
		return nil, nil, nil
	}
	defer instance.Close()

	main := instance.Exports["main"]
	if main == nil {
		log.Warnf("not export main function. All exports: %v", instance.Exports)

		return nil, nil, nil
	}
	result, err := main(postState, deposits)

	log.WithFields(logrus.Fields{
		"result": result,
		"error":  err}).Debug("ExecuteCode")

	return nil, nil, nil
}
