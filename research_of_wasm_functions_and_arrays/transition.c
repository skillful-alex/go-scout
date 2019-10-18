#include <stdint.h>

//deposit_t is struct for save single deposit
typedef struct {                       // sum size:
    int8_t pubKey[48];                 //   48
    int8_t withdrawalCredentials[48];  //   96
    uint64_t amount;                   //  104
} deposit_t;

//deposit_t is array of deposits
typedef struct {
  uint32_t  count;         // real amount of deposits
  deposit_t deposits[];    // array of deposits
} deposits_t;

typedef struct {            //       sum size:
  uint8_t preState[32];     // (in)  32
  uint8_t postState[32];    // (out) 64  initialization value is preState
  deposits_t* deposits;     // (out) 68  initialization value points address after block[]
  uint32_t blockSize;       // (in)  72
  uint8_t block[];          // (in)  72+blockSize
} args_t;

// Parameters and results are passed through the heap.
// The heap starts with args_t, followed by deposits_t initiated of zeros.
// The args_t.deposits_t pointer points to deposits_t, but can be changed in the program.
// The result of the program is passed to postState and deposits_t pointed by args_t.deposits.  
// On execution error, the main() should return a number other than 0.

int transition(args_t* args) {
  deposits_t* deposits = (*args).deposits;

  // Nothing to do
  if ((*args).blockSize==0) {
    return 1; // error
  }

  // Put deposits
  if ((*args).blockSize==1) {
    (*deposits).count = 2;
    (*deposits).deposits[0].pubKey[1] = 1;
    (*deposits).deposits[0].withdrawalCredentials[2] = 2;
    (*deposits).deposits[0].amount = 3;
    (*deposits).deposits[1].pubKey[4] = 4;
    (*deposits).deposits[1].withdrawalCredentials[5] = 5;
    (*deposits).deposits[1].amount = 0xFFFFFFFFFFFFFFFF;
  }

  // Write block[1] to postState byte indicated by block[0]
  if ((*args).blockSize==2) {  
    (*args).postState[ (*args).block[0] ] = (*args).block[1];
    return 0;
  }

  // Test change position of deposits result
  if ((*args).blockSize==3) {
    deposits += 777;
    (*args).deposits = deposits;
    (*deposits).count = 1;
    (*deposits).deposits[0].pubKey[0] = 7;
    (*deposits).deposits[0].withdrawalCredentials[1] = 77;
    (*deposits).deposits[0].amount = 777;
    return 0;
  }
  
  return 0;
}