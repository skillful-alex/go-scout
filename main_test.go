package ewasm

import (
	"bytes"
	"encoding/hex"
	"io/ioutil"
	"testing"

	"github.com/sirupsen/logrus"
	"gopkg.in/yaml.v2"
)

func init() {
	//TODO remove logLevel set
	logrus.SetLevel(logrus.TraceLevel)
}

type YamlFile []struct {
	Test struct {
		Script    string `yaml:"script"`
		PreState  string `yaml:"pre_state"`
		Block     string `yaml:"block"`
		PostState string `yaml:"post_state"`
		Deposits  string `yaml:"deposits"`  //TODO
		Exception string `yaml:"exception"` //TODO
	} `yaml:"test"`
}

type TestCase struct {
	Script    string
	PreState  [32]byte
	Block     []byte
	PostState [32]byte
}

func readHex(t *testing.T, hexString string) []byte {
	result, err := hex.DecodeString(hexString)
	if err != nil {
		t.Fatalf("can't read hex string: %v err: %v", hexString, err)
	}
	return result
}

func readHex32(t *testing.T, hexString string) [32]byte {
	slice := readHex(t, hexString)
	var result [32]byte
	n := copy(result[:], slice)
	if n != 32 {
		t.Fatalf("can't read 32 bytes. n = %v", n)
	}
	return result
}

func readYaml(t *testing.T, yamlFileName string) []TestCase {
	yamlBytes, err := ioutil.ReadFile(yamlFileName)
	if err != nil {
		t.Fatalf("can't read the %v: %v", yamlFileName, err)
	}
	var yamlFile YamlFile
	if err := yaml.Unmarshal(yamlBytes, &yamlFile); err != nil {
		t.Fatalf("can't unmarshal the %v: %v", yamlFileName, err)
	}

	var testCases []TestCase
	for _, yamlTestCase := range yamlFile {
		preState := readHex32(t, yamlTestCase.Test.PreState)
		block := readHex(t, yamlTestCase.Test.Block)
		postState := readHex32(t, yamlTestCase.Test.PostState)
		testCases = append(testCases, TestCase{
			Script:    yamlTestCase.Test.Script,
			PreState:  preState,
			Block:     block,
			PostState: postState,
		})
	}

	return testCases
}

func TestExecuteCode(t *testing.T) {
	testCases := readYaml(t, "./test.yaml")
	for _, test := range testCases {
		wasm, err := ioutil.ReadFile(test.Script)
		if err != nil {
			t.Fatalf("can't read file %v", err)
		}
		postState, deposits, err := ExecuteCode(wasm, test.PreState, test.Block)
		if !bytes.Equal(postState[:], test.PostState[:]) {
			t.Errorf("ExecuteCode incorrect result.\nwait:   %v\nresult: %v", test.PostState, postState)
		}
		if deposits != nil {
			panic("TODO impliment deposits test")
		}
	}
}
