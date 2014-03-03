//-----------------------------------------------------------------------------
// Merlok - June 2011, 2012
// Gerhard de Koning Gans - May 2008
// Hagen Fritsch - June 2010
//
// This code is licensed to you under the terms of the GNU GPL, version 2 or,
// at your option, any later version. See the LICENSE.txt file for the text of
// the license.
//-----------------------------------------------------------------------------
// Routines to support ISO 14443 type A.
//-----------------------------------------------------------------------------

#include "mifarecmd.h"
#include "apps.h"

//-----------------------------------------------------------------------------
// Select, Authenticaate, Read an MIFARE tag. 
// read block
//-----------------------------------------------------------------------------
void MifareReadBlock(uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t *datain)
{
  // params
	uint8_t blockNo = arg0;
	uint8_t keyType = arg1;
	uint64_t ui64Key = 0;
	ui64Key = bytes_to_num(datain, 6);
	
	// variables
	byte_t isOK = 0;
	byte_t dataoutbuf[16];
	uint8_t uid[10];
	uint32_t cuid;
	struct Crypto1State mpcs = {0, 0};
	struct Crypto1State *pcs;
	pcs = &mpcs;

	// clear trace
	iso14a_clear_trace();
//	iso14a_set_tracing(false);

	iso14443a_setup(FPGA_HF_ISO14443A_READER_LISTEN);

	LED_A_ON();
	LED_B_OFF();
	LED_C_OFF();

	while (true) {
		if(!iso14443a_select_card(uid, NULL, &cuid)) {
		if (MF_DBGLEVEL >= 1)	Dbprintf("Can't select card");
			break;
		};

		if(mifare_classic_auth(pcs, cuid, blockNo, keyType, ui64Key, AUTH_FIRST)) {
		if (MF_DBGLEVEL >= 1)	Dbprintf("Auth error");
			break;
		};
		
		if(mifare_classic_readblock(pcs, cuid, blockNo, dataoutbuf)) {
		if (MF_DBGLEVEL >= 1)	Dbprintf("Read block error");
			break;
		};

		if(mifare_classic_halt(pcs, cuid)) {
		if (MF_DBGLEVEL >= 1)	Dbprintf("Halt error");
			break;
		};
		
		isOK = 1;
		break;
	}
	
	//  ----------------------------- crypto1 destroy
	crypto1_destroy(pcs);
	
	if (MF_DBGLEVEL >= 2)	DbpString("READ BLOCK FINISHED");

	// add trace trailer
	memset(uid, 0x44, 4);
	LogTrace(uid, 4, 0, 0, TRUE);

//	UsbCommand ack = {CMD_ACK, {isOK, 0, 0}};
//	memcpy(ack.d.asBytes, dataoutbuf, 16);
	
	LED_B_ON();
  cmd_send(CMD_ACK,isOK,0,0,dataoutbuf,16);
//	UsbSendPacket((uint8_t *)&ack, sizeof(UsbCommand));
	LED_B_OFF();


  // Thats it...
	FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
	LEDsoff();
//  iso14a_set_tracing(TRUE);

}

void MifareUReadBlock(uint8_t arg0,uint8_t *datain)
{
    // params
	uint8_t blockNo = arg0;
	
	// variables
	byte_t isOK = 0;
	byte_t dataoutbuf[16];
	uint8_t uid[10];
	uint32_t cuid;
    
	// clear trace
	iso14a_clear_trace();
	iso14443a_setup(FPGA_HF_ISO14443A_READER_LISTEN);
    
	LED_A_ON();
	LED_B_OFF();
	LED_C_OFF();
    
	while (true) {
		if(!iso14443a_select_card(uid, NULL, &cuid)) {
            if (MF_DBGLEVEL >= 1)	Dbprintf("Can't select card");
			break;
		};
        
		if(mifare_ultra_readblock(cuid, blockNo, dataoutbuf)) {
            if (MF_DBGLEVEL >= 1)	Dbprintf("Read block error");
			break;
		};
        
		if(mifare_ultra_halt(cuid)) {
            if (MF_DBGLEVEL >= 1)	Dbprintf("Halt error");
			break;
		};
		
		isOK = 1;
		break;
	}
	
	if (MF_DBGLEVEL >= 2)	DbpString("READ BLOCK FINISHED");
    
	// add trace trailer
	memset(uid, 0x44, 4);
	LogTrace(uid, 4, 0, 0, TRUE);
	LED_B_ON();
        cmd_send(CMD_ACK,isOK,0,0,dataoutbuf,16);
	LED_B_OFF();
    
    
    // Thats it...
	FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
	LEDsoff();
}

//-----------------------------------------------------------------------------
// Select, Authenticaate, Read an MIFARE tag. 
// read sector (data = 4 x 16 bytes = 64 bytes)
//-----------------------------------------------------------------------------
void MifareReadSector(uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t *datain)
{
  // params
	uint8_t sectorNo = arg0;
	uint8_t keyType = arg1;
	uint64_t ui64Key = 0;
	ui64Key = bytes_to_num(datain, 6);
	
	// variables
	byte_t isOK = 0;
	byte_t dataoutbuf[16 * 4];
	uint8_t uid[10];
	uint32_t cuid;
	struct Crypto1State mpcs = {0, 0};
	struct Crypto1State *pcs;
	pcs = &mpcs;

	// clear trace
	iso14a_clear_trace();
//	iso14a_set_tracing(false);

	iso14443a_setup(FPGA_HF_ISO14443A_READER_LISTEN);

	LED_A_ON();
	LED_B_OFF();
	LED_C_OFF();

	while (true) {
		if(!iso14443a_select_card(uid, NULL, &cuid)) {
		if (MF_DBGLEVEL >= 1)	Dbprintf("Can't select card");
			break;
		};

		if(mifare_classic_auth(pcs, cuid, sectorNo * 4, keyType, ui64Key, AUTH_FIRST)) {
		if (MF_DBGLEVEL >= 1)	Dbprintf("Auth error");
			break;
		};
		
		if(mifare_classic_readblock(pcs, cuid, sectorNo * 4 + 0, dataoutbuf + 16 * 0)) {
		if (MF_DBGLEVEL >= 1)	Dbprintf("Read block 0 error");
			break;
		};
		if(mifare_classic_readblock(pcs, cuid, sectorNo * 4 + 1, dataoutbuf + 16 * 1)) {
		if (MF_DBGLEVEL >= 1)	Dbprintf("Read block 1 error");
			break;
		};
		if(mifare_classic_readblock(pcs, cuid, sectorNo * 4 + 2, dataoutbuf + 16 * 2)) {
		if (MF_DBGLEVEL >= 1)	Dbprintf("Read block 2 error");
			break;
		};
		if(mifare_classic_readblock(pcs, cuid, sectorNo * 4 + 3, dataoutbuf + 16 * 3)) {
		if (MF_DBGLEVEL >= 1)	Dbprintf("Read block 3 error");
			break;
		};
		
		if(mifare_classic_halt(pcs, cuid)) {
		if (MF_DBGLEVEL >= 1)	Dbprintf("Halt error");
			break;
		};

		isOK = 1;
		break;
	}
	
	//  ----------------------------- crypto1 destroy
	crypto1_destroy(pcs);
	
	if (MF_DBGLEVEL >= 2) DbpString("READ SECTOR FINISHED");

	// add trace trailer
	memset(uid, 0x44, 4);
	LogTrace(uid, 4, 0, 0, TRUE);

//	UsbCommand ack = {CMD_ACK, {isOK, 0, 0}};
//	memcpy(ack.d.asBytes, dataoutbuf, 16 * 2);
	
	LED_B_ON();
  cmd_send(CMD_ACK,isOK,0,0,dataoutbuf,32);
//	UsbSendPacket((uint8_t *)&ack, sizeof(UsbCommand));
//	SpinDelay(100);
	
//	memcpy(ack.d.asBytes, dataoutbuf + 16 * 2, 16 * 2);
//	UsbSendPacket((uint8_t *)&ack, sizeof(UsbCommand));
  cmd_send(CMD_ACK,isOK,0,0,dataoutbuf+32, 32);
	LED_B_OFF();

	// Thats it...
	FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
	LEDsoff();
//  iso14a_set_tracing(TRUE);

}

void MifareUReadCard(uint8_t arg0, uint8_t *datain)
{
  // params
        uint8_t sectorNo = arg0;
        
        // variables
        byte_t isOK = 0;
        byte_t dataoutbuf[16 * 4];
        uint8_t uid[10];
        uint32_t cuid;

        // clear trace
        iso14a_clear_trace();
//      iso14a_set_tracing(false);

		iso14443a_setup(FPGA_HF_ISO14443A_READER_LISTEN);

        LED_A_ON();
        LED_B_OFF();
        LED_C_OFF();

        while (true) {
                if(!iso14443a_select_card(uid, NULL, &cuid)) {
                if (MF_DBGLEVEL >= 1)   Dbprintf("Can't select card");
                        break;
                };
		for(int sec=0;sec<16;sec++){
                    if(mifare_ultra_readblock(cuid, sectorNo * 4 + sec, dataoutbuf + 4 * sec)) {
                    if (MF_DBGLEVEL >= 1)   Dbprintf("Read block %d error",sec);
                        break;
                    };
                }
                if(mifare_ultra_halt(cuid)) {
                if (MF_DBGLEVEL >= 1)   Dbprintf("Halt error");
                        break;
                };

                isOK = 1;
                break;
        }
        
        if (MF_DBGLEVEL >= 2) DbpString("READ CARD FINISHED");

        // add trace trailer
        memset(uid, 0x44, 4);
        LogTrace(uid, 4, 0, 0, TRUE);
        
        LED_B_ON();
		cmd_send(CMD_ACK,isOK,0,0,dataoutbuf,64);
  //cmd_send(CMD_ACK,isOK,0,0,dataoutbuf+32, 32);
        LED_B_OFF();

        // Thats it...
        FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
        LEDsoff();
//  iso14a_set_tracing(TRUE);

}


//-----------------------------------------------------------------------------
// Select, Authenticaate, Read an MIFARE tag. 
// read block
//-----------------------------------------------------------------------------
void MifareWriteBlock(uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t *datain)
{
	// params
	uint8_t blockNo = arg0;
	uint8_t keyType = arg1;
	uint64_t ui64Key = 0;
	byte_t blockdata[16];

	ui64Key = bytes_to_num(datain, 6);
	memcpy(blockdata, datain + 10, 16);
	
	// variables
	byte_t isOK = 0;
	uint8_t uid[10];
	uint32_t cuid;
	struct Crypto1State mpcs = {0, 0};
	struct Crypto1State *pcs;
	pcs = &mpcs;

	// clear trace
	iso14a_clear_trace();
//  iso14a_set_tracing(false);

	iso14443a_setup(FPGA_HF_ISO14443A_READER_LISTEN);

	LED_A_ON();
	LED_B_OFF();
	LED_C_OFF();

	while (true) {
			if(!iso14443a_select_card(uid, NULL, &cuid)) {
			if (MF_DBGLEVEL >= 1)	Dbprintf("Can't select card");
			break;
		};

		if(mifare_classic_auth(pcs, cuid, blockNo, keyType, ui64Key, AUTH_FIRST)) {
			if (MF_DBGLEVEL >= 1)	Dbprintf("Auth error");
			break;
		};
		
		if(mifare_classic_writeblock(pcs, cuid, blockNo, blockdata)) {
			if (MF_DBGLEVEL >= 1)	Dbprintf("Write block error");
			break;
		};

		if(mifare_classic_halt(pcs, cuid)) {
			if (MF_DBGLEVEL >= 1)	Dbprintf("Halt error");
			break;
		};
		
		isOK = 1;
		break;
	}
	
	//  ----------------------------- crypto1 destroy
	crypto1_destroy(pcs);
	
	if (MF_DBGLEVEL >= 2)	DbpString("WRITE BLOCK FINISHED");

	// add trace trailer
	memset(uid, 0x44, 4);
	LogTrace(uid, 4, 0, 0, TRUE);

//	UsbCommand ack = {CMD_ACK, {isOK, 0, 0}};
	
	LED_B_ON();
	cmd_send(CMD_ACK,isOK,0,0,0,0);
//	UsbSendPacket((uint8_t *)&ack, sizeof(UsbCommand));
	LED_B_OFF();


	// Thats it...
	FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
	LEDsoff();
//  iso14a_set_tracing(TRUE);

}

void MifareUWriteBlock(uint8_t arg0, uint8_t *datain)
{
        // params
        uint8_t blockNo = arg0;
        byte_t blockdata[16];

        memset(blockdata,'\0',16);
        memcpy(blockdata, datain,16);
        
        // variables
        byte_t isOK = 0;
        uint8_t uid[10];
        uint32_t cuid;

        // clear trace
        iso14a_clear_trace();
	//  iso14a_set_tracing(false);

		iso14443a_setup(FPGA_HF_ISO14443A_READER_LISTEN);

        LED_A_ON();
        LED_B_OFF();
        LED_C_OFF();

        while (true) {
                if(!iso14443a_select_card(uid, NULL, &cuid)) {
                        if (MF_DBGLEVEL >= 1)   Dbprintf("Can't select card");
                        break;
                };

                if(mifare_ultra_writeblock(cuid, blockNo, blockdata)) {
                        if (MF_DBGLEVEL >= 1)   Dbprintf("Write block error");
                        break;
                };

                if(mifare_ultra_halt(cuid)) {
                        if (MF_DBGLEVEL >= 1)   Dbprintf("Halt error");
                        break;
                };
                
                isOK = 1;
                break;
        }
        
        if (MF_DBGLEVEL >= 2)   DbpString("WRITE BLOCK FINISHED");

        // add trace trailer
        memset(uid, 0x44, 4);
        LogTrace(uid, 4, 0, 0, TRUE);

        LED_B_ON();
  	cmd_send(CMD_ACK,isOK,0,0,0,0);
//      UsbSendPacket((uint8_t *)&ack, sizeof(UsbCommand));
        LED_B_OFF();


        // Thats it...
        FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
        LEDsoff();
//  iso14a_set_tracing(TRUE);

}

void MifareUWriteBlock_Special(uint8_t arg0, uint8_t *datain)
{
        // params
        uint8_t blockNo = arg0;
        byte_t blockdata[4];
        
	memcpy(blockdata, datain,4);

        // variables
        byte_t isOK = 0;
        uint8_t uid[10];
        uint32_t cuid;

        // clear trace
        iso14a_clear_trace();
        //  iso14a_set_tracing(false);

		iso14443a_setup(FPGA_HF_ISO14443A_READER_LISTEN);

        LED_A_ON();
        LED_B_OFF();
        LED_C_OFF();

        while (true) {
                        if(!iso14443a_select_card(uid, NULL, &cuid)) {
                        if (MF_DBGLEVEL >= 1)   Dbprintf("Can't select card");
                        break;
                };

                if(mifare_ultra_special_writeblock(cuid, blockNo, blockdata)) {
                        if (MF_DBGLEVEL >= 1)   Dbprintf("Write block error");
                        break;
                };

                if(mifare_ultra_halt(cuid)) {
                        if (MF_DBGLEVEL >= 1)   Dbprintf("Halt error");
                        break;
                };

                isOK = 1;
                break;
        }

        if (MF_DBGLEVEL >= 2)   DbpString("WRITE BLOCK FINISHED");

        // add trace trailer
	memset(uid, 0x44, 4);
        LogTrace(uid, 4, 0, 0, TRUE);

       LED_B_ON();
        cmd_send(CMD_ACK,isOK,0,0,0,0);
//      UsbSendPacket((uint8_t *)&ack, sizeof(UsbCommand));
        LED_B_OFF();


        // Thats it...
        FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
        LEDsoff();
//  iso14a_set_tracing(TRUE);

}

// Return 1 if the nonce is invalid else return 0
int valid_nonce(uint32_t Nt, uint32_t NtEnc, uint32_t Ks1, byte_t * parity) {
	return ((oddparity((Nt >> 24) & 0xFF) == ((parity[0]) ^ oddparity((NtEnc >> 24) & 0xFF) ^ BIT(Ks1,16))) & \
	(oddparity((Nt >> 16) & 0xFF) == ((parity[1]) ^ oddparity((NtEnc >> 16) & 0xFF) ^ BIT(Ks1,8))) & \
	(oddparity((Nt >> 8) & 0xFF) == ((parity[2]) ^ oddparity((NtEnc >> 8) & 0xFF) ^ BIT(Ks1,0)))) ? 1 : 0;
}



//-----------------------------------------------------------------------------
// MIFARE nested authentication. 
// 
//-----------------------------------------------------------------------------
void MifareNested(uint32_t arg0, uint32_t arg1, uint32_t calibrate, uint8_t *datain)
{
	// params
	uint8_t blockNo = arg0 & 0xff;
	uint8_t keyType = (arg0 >> 8) & 0xff;
	uint8_t targetBlockNo = arg1 & 0xff;
	uint8_t targetKeyType = (arg1 >> 8) & 0xff;
	uint64_t ui64Key = 0;

	ui64Key = bytes_to_num(datain, 6);
	
	// variables
	uint16_t rtr, i, j, len;
	uint16_t davg;
	static uint16_t dmin, dmax;
	uint8_t uid[10];
	uint32_t cuid, nt1, nt2, nttmp, nttest, par, ks1;
	uint32_t target_nt[2], target_ks[2];
	
	uint8_t par_array[4];
	uint16_t ncount = 0;
	struct Crypto1State mpcs = {0, 0};
	struct Crypto1State *pcs;
	pcs = &mpcs;
	uint8_t* receivedAnswer = mifare_get_bigbufptr();

	uint32_t auth1_time, auth2_time;
	static uint16_t delta_time;

	// clear trace
	iso14a_clear_trace();
	iso14a_set_tracing(false);
	
	iso14443a_setup(FPGA_HF_ISO14443A_READER_LISTEN);

	LED_A_ON();
	LED_C_OFF();


	// statistics on nonce distance
	if (calibrate) {	// for first call only. Otherwise reuse previous calibration
		LED_B_ON();

		davg = dmax = 0;
		dmin = 2000;
		delta_time = 0;
		
		for (rtr = 0; rtr < 17; rtr++) {

			// prepare next select. No need to power down the card.
			if(mifare_classic_halt(pcs, cuid)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Nested: Halt error");
				rtr--;
				continue;
			}

			if(!iso14443a_select_card(uid, NULL, &cuid)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Nested: Can't select card");
				rtr--;
				continue;
			};

			auth1_time = 0;
			if(mifare_classic_authex(pcs, cuid, blockNo, keyType, ui64Key, AUTH_FIRST, &nt1, &auth1_time)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Nested: Auth1 error");
				rtr--;
				continue;
			};

			if (delta_time) {
				auth2_time = auth1_time + delta_time;
			} else {
				auth2_time = 0;
			}
			if(mifare_classic_authex(pcs, cuid, blockNo, keyType, ui64Key, AUTH_NESTED, &nt2, &auth2_time)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Nested: Auth2 error");
				rtr--;
				continue;
			};

			nttmp = prng_successor(nt1, 100);				//NXP Mifare is typical around 840,but for some unlicensed/compatible mifare card this can be 160
			for (i = 101; i < 1200; i++) {
				nttmp = prng_successor(nttmp, 1);
				if (nttmp == nt2) break;
			}

			if (i != 1200) {
				if (rtr != 0) {
					davg += i;
					dmin = MIN(dmin, i);
					dmax = MAX(dmax, i);
				}
				else {
					delta_time = auth2_time - auth1_time + 32;  // allow some slack for proper timing
				}
				if (MF_DBGLEVEL >= 3) Dbprintf("Nested: calibrating... ntdist=%d", i);
			}
		}
		
		if (rtr <= 1)	return;

		davg = (davg + (rtr - 1)/2) / (rtr - 1);
		
		if (MF_DBGLEVEL >= 3) Dbprintf("min=%d max=%d avg=%d, delta_time=%d", dmin, dmax, davg, delta_time);

		dmin = davg - 2;
		dmax = davg + 2;
		
		LED_B_OFF();
	
	}
//  -------------------------------------------------------------------------------------------------	
	
	LED_C_ON();

	//  get crypted nonces for target sector
	for(i=0; i < 2; i++) { // look for exactly two different nonces

		target_nt[i] = 0;
		while(target_nt[i] == 0) { // continue until we have an unambiguous nonce
		
			// prepare next select. No need to power down the card.
			if(mifare_classic_halt(pcs, cuid)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Nested: Halt error");
				continue;
			}

			if(!iso14443a_select_card(uid, NULL, &cuid)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Nested: Can't select card");
				continue;
			};
		
			auth1_time = 0;
			if(mifare_classic_authex(pcs, cuid, blockNo, keyType, ui64Key, AUTH_FIRST, &nt1, &auth1_time)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Nested: Auth1 error");
				continue;
			};

			// nested authentication
			auth2_time = auth1_time + delta_time;
			len = mifare_sendcmd_shortex(pcs, AUTH_NESTED, 0x60 + (targetKeyType & 0x01), targetBlockNo, receivedAnswer, &par, &auth2_time);
			if (len != 4) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Nested: Auth2 error len=%d", len);
				continue;
			};
		
			nt2 = bytes_to_num(receivedAnswer, 4);		
			if (MF_DBGLEVEL >= 3) Dbprintf("Nonce#%d: Testing nt1=%08x nt2enc=%08x nt2par=%02x", i+1, nt1, nt2, par);
			
			// Parity validity check
			for (j = 0; j < 4; j++) {
				par_array[j] = (oddparity(receivedAnswer[j]) != ((par & 0x08) >> 3));
				par = par << 1;
			}
			
			ncount = 0;
			nttest = prng_successor(nt1, dmin - 1);
			for (j = dmin; j < dmax + 1; j++) {
				nttest = prng_successor(nttest, 1);
				ks1 = nt2 ^ nttest;

				if (valid_nonce(nttest, nt2, ks1, par_array)){
					if (ncount > 0) { 		// we are only interested in disambiguous nonces, try again
						if (MF_DBGLEVEL >= 3) Dbprintf("Nonce#%d: dismissed (ambigous), ntdist=%d", i+1, j);
						target_nt[i] = 0;
						break;
					}
					target_nt[i] = nttest;
					target_ks[i] = ks1;
					ncount++;
					if (i == 1 && target_nt[1] == target_nt[0]) { // we need two different nonces
						target_nt[i] = 0;
						if (MF_DBGLEVEL >= 3) Dbprintf("Nonce#2: dismissed (= nonce#1), ntdist=%d", j);
						break;
					}
					if (MF_DBGLEVEL >= 3) Dbprintf("Nonce#%d: valid, ntdist=%d", i+1, j);
				}
			}
			if (target_nt[i] == 0 && j == dmax+1 && MF_DBGLEVEL >= 3) Dbprintf("Nonce#%d: dismissed (all invalid)", i+1);
		}
	}

	LED_C_OFF();
	
	//  ----------------------------- crypto1 destroy
	crypto1_destroy(pcs);
	
	// add trace trailer
	memset(uid, 0x44, 4);
	LogTrace(uid, 4, 0, 0, TRUE);

	byte_t buf[4 + 4 * 4];
	memcpy(buf, &cuid, 4);
	memcpy(buf+4, &target_nt[0], 4);
	memcpy(buf+8, &target_ks[0], 4);
	memcpy(buf+12, &target_nt[1], 4);
	memcpy(buf+16, &target_ks[1], 4);
	
	LED_B_ON();
	cmd_send(CMD_ACK, 0, 2, targetBlockNo + (targetKeyType * 0x100), buf, sizeof(buf));
	LED_B_OFF();

	if (MF_DBGLEVEL >= 3)	DbpString("NESTED FINISHED");

	FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
	LEDsoff();
	iso14a_set_tracing(TRUE);
}

//-----------------------------------------------------------------------------
// MIFARE check keys. key count up to 85. 
// 
//-----------------------------------------------------------------------------
void MifareChkKeys(uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t *datain)
{
  // params
	uint8_t blockNo = arg0;
	uint8_t keyType = arg1;
	uint8_t keyCount = arg2;
	uint64_t ui64Key = 0;
	
	// variables
	int i;
	byte_t isOK = 0;
	uint8_t uid[10];
	uint32_t cuid;
	struct Crypto1State mpcs = {0, 0};
	struct Crypto1State *pcs;
	pcs = &mpcs;
	
	// clear debug level
	int OLD_MF_DBGLEVEL = MF_DBGLEVEL;	
	MF_DBGLEVEL = MF_DBG_NONE;
	
	// clear trace
	iso14a_clear_trace();
	iso14a_set_tracing(TRUE);

	iso14443a_setup(FPGA_HF_ISO14443A_READER_LISTEN);

	LED_A_ON();
	LED_B_OFF();
	LED_C_OFF();

//	SpinDelay(300);
	for (i = 0; i < keyCount; i++) {
//		FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
//		SpinDelay(100);
//		FpgaWriteConfWord(FPGA_MAJOR_MODE_HF_ISO14443A | FPGA_HF_ISO14443A_READER_MOD);
		// prepare next select by sending a HALT. There is no need to power down the card.
		if(mifare_classic_halt(pcs, cuid)) {
			if (MF_DBGLEVEL >= 1)	Dbprintf("ChkKeys: Halt error");
		}

		// SpinDelay(50);
		
		if(!iso14443a_select_card(uid, NULL, &cuid)) {
			if (OLD_MF_DBGLEVEL >= 1)	Dbprintf("ChkKeys: Can't select card");
			break;
		};

		ui64Key = bytes_to_num(datain + i * 6, 6);
		if(mifare_classic_auth(pcs, cuid, blockNo, keyType, ui64Key, AUTH_FIRST)) {
			continue;
		};
		
		isOK = 1;
		break;
	}
	
	//  ----------------------------- crypto1 destroy
	crypto1_destroy(pcs);
	
	// add trace trailer
	memset(uid, 0x44, 4);
	LogTrace(uid, 4, 0, 0, TRUE);

	LED_B_ON();
    cmd_send(CMD_ACK,isOK,0,0,datain + i * 6,6);
	LED_B_OFF();

  // Thats it...
	FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
	LEDsoff();

	// restore debug level
	MF_DBGLEVEL = OLD_MF_DBGLEVEL;	
}

//-----------------------------------------------------------------------------
// MIFARE commands set debug level
// 
//-----------------------------------------------------------------------------
void MifareSetDbgLvl(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint8_t *datain){
	MF_DBGLEVEL = arg0;
	Dbprintf("Debug level: %d", MF_DBGLEVEL);
}

//-----------------------------------------------------------------------------
// Work with emulator memory
// 
//-----------------------------------------------------------------------------
void MifareEMemClr(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint8_t *datain){
	emlClearMem();
}

void MifareEMemSet(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint8_t *datain){
	emlSetMem(datain, arg0, arg1); // data, block num, blocks count
}

void MifareEMemGet(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint8_t *datain){
//	UsbCommand ack = {CMD_ACK, {arg0, arg1, 0}};

  byte_t buf[48];
	emlGetMem(buf, arg0, arg1); // data, block num, blocks count

	LED_B_ON();
  cmd_send(CMD_ACK,arg0,arg1,0,buf,48);
//	UsbSendPacket((uint8_t *)&ack, sizeof(UsbCommand));
	LED_B_OFF();
}

//-----------------------------------------------------------------------------
// Load a card into the emulator memory
// 
//-----------------------------------------------------------------------------
void MifareECardLoad(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint8_t *datain){
	int i;
	uint8_t sectorNo = 0;
	uint8_t keyType = arg1;
	uint64_t ui64Key = 0;
	uint32_t cuid;
	struct Crypto1State mpcs = {0, 0};
	struct Crypto1State *pcs;
	pcs = &mpcs;

	// variables
	byte_t dataoutbuf[16];
	byte_t dataoutbuf2[16];
	uint8_t uid[10];

	// clear trace
	iso14a_clear_trace();
	iso14a_set_tracing(false);
	
	iso14443a_setup(FPGA_HF_ISO14443A_READER_LISTEN);

	LED_A_ON();
	LED_B_OFF();
	LED_C_OFF();
	
	while (true) {
		if(!iso14443a_select_card(uid, NULL, &cuid)) {
			if (MF_DBGLEVEL >= 1)	Dbprintf("Can't select card");
			break;
		};
		
		for (i = 0; i < 16; i++) {
			sectorNo = i;
			ui64Key = emlGetKey(sectorNo, keyType);
	
			if (!i){
				if(mifare_classic_auth(pcs, cuid, sectorNo * 4, keyType, ui64Key, AUTH_FIRST)) {
					if (MF_DBGLEVEL >= 1)	Dbprintf("Sector[%d]. Auth error", i);
					break;
				}
			} else {
				if(mifare_classic_auth(pcs, cuid, sectorNo * 4, keyType, ui64Key, AUTH_NESTED)) {
					if (MF_DBGLEVEL >= 1)	Dbprintf("Sector[%d]. Auth nested error", i);
					break;
				}
			}
		
			if(mifare_classic_readblock(pcs, cuid, sectorNo * 4 + 0, dataoutbuf)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Read block 0 error");
				break;
			};
			emlSetMem(dataoutbuf, sectorNo * 4 + 0, 1);
			
			if(mifare_classic_readblock(pcs, cuid, sectorNo * 4 + 1, dataoutbuf)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Read block 1 error");
				break;
			};
			emlSetMem(dataoutbuf, sectorNo * 4 + 1, 1);

			if(mifare_classic_readblock(pcs, cuid, sectorNo * 4 + 2, dataoutbuf)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Read block 2 error");
				break;
			};
			emlSetMem(dataoutbuf, sectorNo * 4 + 2, 1);

			// get block 3 bytes 6-9
			if(mifare_classic_readblock(pcs, cuid, sectorNo * 4 + 3, dataoutbuf)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Read block 3 error");
				break;
			};
			emlGetMem(dataoutbuf2, sectorNo * 4 + 3, 1);
			memcpy(&dataoutbuf2[6], &dataoutbuf[6], 4);
			emlSetMem(dataoutbuf2,  sectorNo * 4 + 3, 1);
		}

		if(mifare_classic_halt(pcs, cuid)) {
			if (MF_DBGLEVEL >= 1)	Dbprintf("Halt error");
			break;
		};
		
		break;
	}	

	//  ----------------------------- crypto1 destroy
	crypto1_destroy(pcs);

	FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
	LEDsoff();
	
	if (MF_DBGLEVEL >= 2) DbpString("EMUL FILL SECTORS FINISHED");

	// add trace trailer
	memset(uid, 0x44, 4);
	LogTrace(uid, 4, 0, 0, TRUE);
}

//-----------------------------------------------------------------------------
// MIFARE 1k emulator
// 
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Work with "magic Chinese" card (email him: ouyangweidaxian@live.cn)
// 
//-----------------------------------------------------------------------------
void MifareCSetBlock(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint8_t *datain){
  
  // params
	uint8_t needWipe = arg0;
	// bit 0 - need get UID
	// bit 1 - need wupC
	// bit 2 - need HALT after sequence
	// bit 3 - need init FPGA and field before sequence
	// bit 4 - need reset FPGA and LED
	uint8_t workFlags = arg1;
	uint8_t blockNo = arg2;
	
	// card commands
	uint8_t wupC1[]       = { 0x40 }; 
	uint8_t wupC2[]       = { 0x43 }; 
	uint8_t wipeC[]       = { 0x41 }; 
	
	// variables
	byte_t isOK = 0;
	uint8_t uid[10];
	uint8_t d_block[18];
	uint32_t cuid;
	
	memset(uid, 0x00, 10);
	uint8_t* receivedAnswer = mifare_get_bigbufptr();
	
	if (workFlags & 0x08) {
		// clear trace
		iso14a_clear_trace();
		iso14a_set_tracing(TRUE);

		iso14443a_setup(FPGA_HF_ISO14443A_READER_LISTEN);

		LED_A_ON();
		LED_B_OFF();
		LED_C_OFF();
	
		SpinDelay(300);
		FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
		SpinDelay(100);
		FpgaWriteConfWord(FPGA_MAJOR_MODE_HF_ISO14443A | FPGA_HF_ISO14443A_READER_MOD);
	}

	while (true) {
		// get UID from chip
		if (workFlags & 0x01) {
			if(!iso14443a_select_card(uid, NULL, &cuid)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Can't select card");
				break;
			};

			if(mifare_classic_halt(NULL, cuid)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Halt error");
				break;
			};
		};
	
		// reset chip
		if (needWipe){
      ReaderTransmitBitsPar(wupC1,7,0, NULL);
			if(!ReaderReceive(receivedAnswer) || (receivedAnswer[0] != 0x0a)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("wupC1 error");
				break;
			};

			ReaderTransmit(wipeC, sizeof(wipeC), NULL);
			if(!ReaderReceive(receivedAnswer) || (receivedAnswer[0] != 0x0a)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("wipeC error");
				break;
			};

			if(mifare_classic_halt(NULL, cuid)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Halt error");
				break;
			};
		};	

		// write block
		if (workFlags & 0x02) {
      ReaderTransmitBitsPar(wupC1,7,0, NULL);
			if(!ReaderReceive(receivedAnswer) || (receivedAnswer[0] != 0x0a)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("wupC1 error");
				break;
			};

			ReaderTransmit(wupC2, sizeof(wupC2), NULL);
			if(!ReaderReceive(receivedAnswer) || (receivedAnswer[0] != 0x0a)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("wupC2 error");
				break;
			};
		}

		if ((mifare_sendcmd_short(NULL, 0, 0xA0, blockNo, receivedAnswer, NULL) != 1) || (receivedAnswer[0] != 0x0a)) {
			if (MF_DBGLEVEL >= 1)	Dbprintf("write block send command error");
			break;
		};
	
		memcpy(d_block, datain, 16);
		AppendCrc14443a(d_block, 16);
	
		ReaderTransmit(d_block, sizeof(d_block), NULL);
		if ((ReaderReceive(receivedAnswer) != 1) || (receivedAnswer[0] != 0x0a)) {
			if (MF_DBGLEVEL >= 1)	Dbprintf("write block send data error");
			break;
		};	
	
		if (workFlags & 0x04) {
			if (mifare_classic_halt(NULL, cuid)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Halt error");
				break;
			};
		}
		
		isOK = 1;
		break;
	}
	
//	UsbCommand ack = {CMD_ACK, {isOK, 0, 0}};
//	if (isOK) memcpy(ack.d.asBytes, uid, 4);
	
	// add trace trailer
	/**
	*	Removed by Martin, the uid is overwritten with 0x44, 
	*	which can 't be intended. 
	*
	*	memset(uid, 0x44, 4);
	*	LogTrace(uid, 4, 0, 0, TRUE);
	**/
	

	LED_B_ON();
  cmd_send(CMD_ACK,isOK,0,0,uid,4);
//	UsbSendPacket((uint8_t *)&ack, sizeof(UsbCommand));
	LED_B_OFF();

	if ((workFlags & 0x10) || (!isOK)) {
		// Thats it...
		FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
		LEDsoff();
	}
}

void MifareCGetBlock(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint8_t *datain){
  
  // params
	// bit 1 - need wupC
	// bit 2 - need HALT after sequence
	// bit 3 - need init FPGA and field before sequence
	// bit 4 - need reset FPGA and LED
	uint8_t workFlags = arg0;
	uint8_t blockNo = arg2;
	
	// card commands
	uint8_t wupC1[]       = { 0x40 }; 
	uint8_t wupC2[]       = { 0x43 }; 
	
	// variables
	byte_t isOK = 0;
	uint8_t data[18];
	uint32_t cuid = 0;
	
	memset(data, 0x00, 18);
	uint8_t* receivedAnswer = mifare_get_bigbufptr();
	
	if (workFlags & 0x08) {
		// clear trace
		iso14a_clear_trace();
		iso14a_set_tracing(TRUE);

		iso14443a_setup(FPGA_HF_ISO14443A_READER_LISTEN);

		LED_A_ON();
		LED_B_OFF();
		LED_C_OFF();
	
		SpinDelay(300);
		FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
		SpinDelay(100);
		FpgaWriteConfWord(FPGA_MAJOR_MODE_HF_ISO14443A | FPGA_HF_ISO14443A_READER_MOD);
	}

	while (true) {
		if (workFlags & 0x02) {
			ReaderTransmitBitsPar(wupC1,7,0, NULL);
			if(!ReaderReceive(receivedAnswer) || (receivedAnswer[0] != 0x0a)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("wupC1 error");
				break;
			};

			ReaderTransmit(wupC2, sizeof(wupC2), NULL);
			if(!ReaderReceive(receivedAnswer) || (receivedAnswer[0] != 0x0a)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("wupC2 error");
				break;
			};
		}

		// read block
		if ((mifare_sendcmd_short(NULL, 0, 0x30, blockNo, receivedAnswer, NULL) != 18)) {
			if (MF_DBGLEVEL >= 1)	Dbprintf("read block send command error");
			break;
		};
		memcpy(data, receivedAnswer, 18);
		
		if (workFlags & 0x04) {
			if (mifare_classic_halt(NULL, cuid)) {
				if (MF_DBGLEVEL >= 1)	Dbprintf("Halt error");
				break;
			};
		}
		
		isOK = 1;
		break;
	}
	
//	UsbCommand ack = {CMD_ACK, {isOK, 0, 0}};
//	if (isOK) memcpy(ack.d.asBytes, data, 18);
	
	// add trace trailer
	/*
	* Removed by Martin, this piece of overwrites the 'data' variable 
	* which is sent two lines down, and is obviously not correct. 
	* 
	* memset(data, 0x44, 4);
	* LogTrace(data, 4, 0, 0, TRUE);
	*/
	LED_B_ON();
  cmd_send(CMD_ACK,isOK,0,0,data,18);
//	UsbSendPacket((uint8_t *)&ack, sizeof(UsbCommand));
	LED_B_OFF();

	if ((workFlags & 0x10) || (!isOK)) {
		// Thats it...
		FpgaWriteConfWord(FPGA_MAJOR_MODE_OFF);
		LEDsoff();
	}
}

