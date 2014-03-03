//-----------------------------------------------------------------------------
// Merlok - June 2011
// Gerhard de Koning Gans - May 2008
// Hagen Fritsch - June 2010
//
// This code is licensed to you under the terms of the GNU GPL, version 2 or,
// at your option, any later version. See the LICENSE.txt file for the text of
// the license.
//-----------------------------------------------------------------------------
// Routines to support ISO 14443 type A.
//-----------------------------------------------------------------------------

#ifndef __ISO14443A_H
#define __ISO14443A_H
#include "common.h"
#include "mifaresniff.h"

// mifare reader                      over DMA buffer (SnoopIso14443a())!!!
#define MIFARE_BUFF_OFFSET 3560  //              \/   \/   \/
// card emulator memory
#define EML_RESPONSES      4000
#define CARD_MEMORY        6000
#define CARD_MEMORY_LEN    4096

typedef struct {
	enum {
		DEMOD_UNSYNCD,
		// DEMOD_HALF_SYNCD,
		// DEMOD_MOD_FIRST_HALF,
		// DEMOD_NOMOD_FIRST_HALF,
		DEMOD_MANCHESTER_DATA
	} state;
	uint16_t twoBits;
	uint16_t highCnt;
	uint16_t bitCount;
	uint16_t collisionPos;
	uint16_t syncBit;
	uint32_t parityBits;
	uint16_t shiftReg;
	uint16_t samples;
	uint16_t len;
	uint32_t startTime, endTime;
	uint8_t  *output;
} tDemod;

typedef enum {
	MOD_NOMOD = 0,
	MOD_SECOND_HALF,
	MOD_FIRST_HALF,
	MOD_BOTH_HALVES
	} Modulation_t;

typedef struct {
	enum {
		STATE_UNSYNCD,
		STATE_START_OF_COMMUNICATION,
		STATE_MILLER_X,
		STATE_MILLER_Y,
		STATE_MILLER_Z,
		// DROP_NONE,
		// DROP_FIRST_HALF,
		} state;
	uint16_t shiftReg;
	uint16_t bitCount;
	uint16_t len;
	uint16_t byteCntMax;
	uint16_t posCnt;
	uint16_t syncBit;
	uint32_t parityBits;
	uint16_t highCnt;
	uint16_t twoBits;
	uint32_t startTime, endTime;
    uint8_t *output;
} tUart;



extern byte_t oddparity (const byte_t bt);
extern uint32_t GetParity(const uint8_t *pbtCmd, int iLen);
extern void AppendCrc14443a(uint8_t *data, int len);

extern void ReaderTransmit(uint8_t *frame, int len, uint32_t *timing);
extern void ReaderTransmitBitsPar(uint8_t *frame, int bits, uint32_t par, uint32_t *timing);
extern void ReaderTransmitPar(uint8_t *frame, int len, uint32_t par, uint32_t *timing);
extern int ReaderReceive(uint8_t *receivedAnswer);
extern int ReaderReceivePar(uint8_t *receivedAnswer, uint32_t *parptr);

extern void iso14443a_setup(uint8_t fpga_minor_mode);
extern int iso14_apdu(uint8_t *cmd, size_t cmd_len, void *data);
extern int iso14443a_select_card(uint8_t *uid_ptr, iso14a_card_select_t *resp_data, uint32_t *cuid_ptr);
extern void iso14a_set_trigger(bool enable);
extern void iso14a_set_timeout(uint32_t timeout);

extern void iso14a_clear_tracelen();
extern void iso14a_set_tracing(bool enable);

#endif /* __ISO14443A_H */
