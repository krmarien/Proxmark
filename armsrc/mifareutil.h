//-----------------------------------------------------------------------------
// Merlok, May 2011
// Many authors, that makes it possible
//
// This code is licensed to you under the terms of the GNU GPL, version 2 or,
// at your option, any later version. See the LICENSE.txt file for the text of
// the license.
//-----------------------------------------------------------------------------
// code for work with mifare cards.
//-----------------------------------------------------------------------------

#ifndef __MIFAREUTIL_H
#define __MIFAREUTIL_H

// mifare authentication
#define CRYPT_NONE    0
#define CRYPT_ALL     1
#define CRYPT_REQUEST 2
#define AUTH_FIRST    0
#define AUTH_NESTED   2

// mifare 4bit card answers
#define CARD_ACK      0x0A  // 1010 - ACK
#define CARD_NACK_NA  0x04  // 0100 - NACK, not allowed (command not allowed)
#define CARD_NACK_TR  0x05  // 0101 - NACK, transmission error

// reader voltage field detector
#define MF_MINFIELDV      4000

// debug
// 0 - no debug messages 1 - error messages 2 - all messages 4 - extended debug mode
#define MF_DBG_NONE          0
#define MF_DBG_ERROR         1
#define MF_DBG_ALL           2
#define MF_DBG_EXTENDED      4

extern int MF_DBGLEVEL;

//mifare emulator states
#define MFEMUL_NOFIELD      0
#define MFEMUL_IDLE         1
#define MFEMUL_SELECT1      2
#define MFEMUL_SELECT2      3
#define MFEMUL_AUTH1        4
#define MFEMUL_AUTH2        5
#define MFEMUL_WORK	        6
#define MFEMUL_WRITEBL2     7
#define MFEMUL_INTREG_INC   8
#define MFEMUL_INTREG_DEC   9
#define MFEMUL_INTREG_REST 10
#define MFEMUL_HALTED      11

#define cardSTATE_TO_IDLE() cardSTATE = MFEMUL_IDLE; LED_B_OFF(); LED_C_OFF();

//functions
uint8_t* mifare_get_bigbufptr(void);
int mifare_sendcmd_short(struct Crypto1State *pcs, uint8_t crypted, uint8_t cmd, uint8_t data, uint8_t* answer, uint32_t *timing);
int mifare_sendcmd_short_special(struct Crypto1State *pcs, uint8_t crypted, uint8_t cmd, uint8_t *data, uint8_t* amswer, uint8_t *timing);
int mifare_sendcmd_shortex(struct Crypto1State *pcs, uint8_t crypted, uint8_t cmd, uint8_t data, uint8_t* answer, uint32_t * parptr, uint32_t *timing);

int mifare_classic_auth(struct Crypto1State *pcs, uint32_t uid, \
												uint8_t blockNo, uint8_t keyType, uint64_t ui64Key, uint64_t isNested);
int mifare_classic_authex(struct Crypto1State *pcs, uint32_t uid, \
													uint8_t blockNo, uint8_t keyType, uint64_t ui64Key, uint64_t isNested, uint32_t * ntptr, uint32_t *timing);
int mifare_classic_readblock(struct Crypto1State *pcs, uint32_t uid, uint8_t blockNo, uint8_t *blockData); 
int mifare_ultra_readblock(uint32_t uid, uint8_t blockNo, uint8_t *blockData);
int mifare_classic_writeblock(struct Crypto1State *pcs, uint32_t uid, uint8_t blockNo, uint8_t *blockData);
int mifare_ultra_writeblock(uint32_t uid, uint8_t blockNo, uint8_t *blockData);
int mifare_ultra_special_writeblock(uint32_t uid, uint8_t blockNo, uint8_t *blockData);
int mifare_classic_halt(struct Crypto1State *pcs, uint32_t uid); 
int mifare_ultra_halt(uint32_t uid);

// crypto functions
void mf_crypto1_decrypt(struct Crypto1State *pcs, uint8_t *receivedCmd, int len);
void mf_crypto1_encrypt(struct Crypto1State *pcs, uint8_t *data, int len, uint32_t *par);
uint8_t mf_crypto1_encrypt4bit(struct Crypto1State *pcs, uint8_t data);

// memory management
uint8_t* mifare_get_bigbufptr(void);
uint8_t* eml_get_bigbufptr_sendbuf(void);
uint8_t* eml_get_bigbufptr_recbuf(void);

// emulator functions
void emlClearMem(void);
void emlSetMem(uint8_t *data, int blockNum, int blocksCount);
void emlGetMem(uint8_t *data, int blockNum, int blocksCount);
void emlGetMemBt(uint8_t *data, int bytePtr, int byteCount);
uint64_t emlGetKey(int sectorNum, int keyType);
int emlGetValBl(uint32_t *blReg, uint8_t *blBlock, int blockNum);
int emlSetValBl(uint32_t blReg, uint8_t blBlock, int blockNum);
int emlCheckValBl(int blockNum);

#endif
