package ewasm

import (
	"bytes"
	"encoding/hex"
	"io/ioutil"
	"testing"
	"flag"
	"path"
	"os"

	"github.com/sirupsen/logrus"
	"gopkg.in/yaml.v2"
)

const testsPath = "tests"

type testCase struct {
	Script    []byte
	PreState  [32]byte
	Block     []byte
	PostState [32]byte
	Deposits  string
	Exception string
}

type YamlFile []struct {
	Test struct {
		Script    string `yaml:"script"`
		PreState  string `yaml:"pre_state"`
		Block     string `yaml:"block"`
		PostState string `yaml:"post_state"`
		Deposits  string `yaml:"deposits"`
		Exception string `yaml:"exception"`
	} `yaml:"test"`
}

func init() {
	//TODO remove set of logLevel (or not include the _test to prod build)
	if flag.Lookup("test.v") != nil {
		logrus.SetLevel(logrus.TraceLevel)
    }
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

func readScript(t *testing.T, fileNameOrHex string) []byte {
	fileName := path.Join(testsPath, fileNameOrHex)
	if _, err := os.Stat(fileName); !os.IsNotExist(err) {
		wasm, err := ioutil.ReadFile(fileName)
		if err != nil {
			t.Fatalf("can't read file. %v", err)
		}
		return wasm
	} else {
		return readHex(t, fileNameOrHex)
	}
}

func readYaml(t *testing.T, yamlFileName string) []testCase {
	yamlBytes, err := ioutil.ReadFile(yamlFileName)
	if err != nil {
		t.Fatalf("can't read the %v: %v", yamlFileName, err)
	}
	var yamlFile YamlFile
	if err := yaml.Unmarshal(yamlBytes, &yamlFile); err != nil {
		t.Fatalf("can't unmarshal the %v: %v", yamlFileName, err)
	}

	var testCases []testCase
	for _, yamlTestCase := range yamlFile {
		testCases = append(testCases, testCase{
			Script:    readScript(t, yamlTestCase.Test.Script),
			PreState:  readHex32(t, yamlTestCase.Test.PreState),
			Block:     readHex(t, yamlTestCase.Test.Block),
			PostState: readHex32(t, yamlTestCase.Test.PostState),
			Deposits:  yamlTestCase.Test.Deposits,
			Exception: yamlTestCase.Test.Exception,
		})
	}

	return testCases
}

func TestExecuteCode(t *testing.T) {
	testCases := readYaml(t, path.Join(testsPath, "_tests.yaml"))
	for _, test := range testCases {
		postState, deposits, err := ExecuteCode(test.Script, test.PreState, test.Block)
		if err != nil && err.Error() != test.Exception {
			//TODO impliment Exception test
			t.Errorf("ExecuteCode error: %v", err)
		}
		if !bytes.Equal(postState[:], test.PostState[:]) {
			t.Errorf("ExecuteCode incorrect result.\nwait:   %v\nresult: %v", test.PostState, postState)
		}
		if deposits != nil || test.Deposits != "" {
			panic("TODO impliment deposits test")
		}
	}
}
