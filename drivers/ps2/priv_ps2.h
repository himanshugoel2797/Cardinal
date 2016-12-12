#ifndef _PRIV_PS2_H_
#define _PRIV_PS2_H_

#include <cardinal/cardinal_types.h>
#include "ps2.h"

#define CMD_PORT 0x64
#define DATA_PORT 0x60


#define DISABLE_PORT1_CMD 0xAD
#define ENABLE_PORT1_CMD 0xAE
#define DISABLE_PORT2_CMD 0xA7
#define ENABLE_PORT2_CMD 0xA8
#define READ_CFG_CMD 0x20
#define WRITE_CFG_CMD 0x60
#define PERFORM_SELFTEST 0xAA
#define PERFORM_PORT1TEST 0xAB
#define PERFORM_PORT2TEST 0xA9

#define IS_CMD_SENT (!(PS2_ReadStatus() & 2))
#define IS_DATA_AVL (PS2_ReadStatus() & 1)

#define WAIT_CMD_SENT while(!IS_CMD_SENT)
#define WAIT_DATA_AVL while(!IS_DATA_AVL)
#define WAIT_DATA_SENT while(IS_DATA_AVL)


#endif /* end of include guard: _PRIV_PS2_H_ */
