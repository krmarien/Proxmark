#include "proxmark3.h"
#include "apps.h"
#include "util.h"
#include "string.h"
#include "cmd.h"

//=============================================================================
// The relay connection
//=============================================================================
void RAMFUNC RelayMasterIso14443a(uint8_t param) {
	uint8_t cmd[] = { 0xFD, 0xFE, 0xFA, 0xFD, 0xFB, 0xFE, 0xFE, 0xFF };
	int c = 0;

	LED_A_ON();
	LED_B_ON();
	LED_C_ON();
	LED_D_ON();
	DbpString("Do relay test (Master)");

	// Setup for the DMA.
	FpgaSetupSsc();

	FpgaWriteConfWord(FPGA_MAJOR_MODE_RELAY | FPGA_HF_RELAY_SEND);

	while(true) {
		if(AT91C_BASE_SSC->SSC_SR & (AT91C_SSC_TXRDY)) {
			AT91C_BASE_SSC->SSC_THR = cmd[c];
			Dbprintf("Send 0x%02x", cmd[c]);
			c++;
			if (c >= 8)
				c = 0;
				//break;
		}

		if(BUTTON_PRESS()) {
			DbpString("Cancelled by button");
			goto done;
		}
	}

done:
	AT91C_BASE_PDC_SSC->PDC_PTCR = AT91C_PDC_RXTDIS;
	Dbprintf("Relay test finished");
	LEDsoff();
}

void RAMFUNC RelaySlaveIso14443a(uint8_t param) {
	int c = 0;

	LED_A_ON();
	LED_C_ON();
	DbpString("Do relay test (Slave)");

	// Setup for the DMA.
	FpgaSetupSsc();

	FpgaWriteConfWord(FPGA_MAJOR_MODE_RELAY | FPGA_HF_RELAY_RECEIVE);

	while(true) {
		if(AT91C_BASE_SSC->SSC_SR & (AT91C_SSC_RXRDY)) {
			c++;
			if ((uint8_t)AT91C_BASE_SSC->SSC_RHR > 0)
				Dbprintf("Received (%d): 0x%02x", c, (uint8_t)AT91C_BASE_SSC->SSC_RHR);
		}

		if(BUTTON_PRESS()) {
			DbpString("Cancelled by button");
			goto done;
		}
	}

done:
	AT91C_BASE_PDC_SSC->PDC_PTCR = AT91C_PDC_RXTDIS;
	Dbprintf("Relay test finished");
	LEDsoff();
}

void RAMFUNC RelayDelayIso14443a(uint8_t param) {
	int c = 0;

	unsigned int delay = 0;

	DbpString("Get relay delay");

	// Setup for the DMA.
	FpgaSetupSsc();

	FpgaWriteConfWord(FPGA_MAJOR_MODE_RELAY | FPGA_HF_RELAY_DELAY);

	while(true) {
		if(AT91C_BASE_SSC->SSC_SR & (AT91C_SSC_RXRDY)) {
			c++;
			if ((uint8_t)AT91C_BASE_SSC->SSC_RHR > 0) {
				Dbprintf("Received (%d): 0x%08x", c, ((uint8_t)AT91C_BASE_SSC->SSC_RHR << ((4-c)*8)));
				delay += ((uint8_t)AT91C_BASE_SSC->SSC_RHR << ((4-c)*8));
			}
		}

		if(BUTTON_PRESS()) {
			DbpString("Cancelled by button");
			goto done;
		}

		if (c == 4) {
			goto done;
		}
	}

done:
	AT91C_BASE_PDC_SSC->PDC_PTCR = AT91C_PDC_RXTDIS;
	Dbprintf("Relay delay was: %u cycles (%u \u03BCs)", delay, (delay/100)*59);
	LEDsoff();
}