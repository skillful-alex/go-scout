package ewasm

import (
	"encoding/binary"
)

func copyArray(dst []byte, dstIndex int, src []byte, srcIndex int, count int) {
	for i := 0; i < count; i++ {
		dst[dstIndex+i] = src[srcIndex+i]
	}
}

func putVarint(buf []byte, x uint64, index int, n int) {
	tmpBuf := make([]byte, 8)
	binary.LittleEndian.PutUint64(tmpBuf, x)
	copyArray(buf, index, tmpBuf, 0, n)
}

func helperGetNum(arr []byte, ptr int, byteCount int) uint64 {
	result := uint64(0)
	for i := byteCount - 1; i >= 0; i-- {
		result = result*256 + uint64(arr[ptr+i])
	}
	return result
}
