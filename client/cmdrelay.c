//-----------------------------------------------------------------------------
// High frequency Relay commands
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "util.h"
#include "data.h"
#include "proxmark3.h"
#include "ui.h"
#include "cmdparser.h"
#include "common.h"
#include "cmdmain.h"
#include "cmdrelay.h"

static int CmdHelp(const char *Cmd);

int CmdRelayMaster(const char *Cmd) {
	UsbCommand c = {CMD_RELAY_MASTER, {0, 0, 0}};

	SendCommand(&c);
	return 0;
}

int CmdRelaySlave(const char *Cmd) {
	UsbCommand c = {CMD_RELAY_SLAVE, {0, 0, 0}};


	SendCommand(&c);
	return 0;
}

int CmdRelayDelay(const char *Cmd) {
	UsbCommand c = {CMD_RELAY_DELAY, {0, 0, 0}};

	SendCommand(&c);
	return 0;
}

static command_t CommandTable[] = 
{
  {"help",   CmdHelp,         1, "This help"},
  {"master", CmdRelayMaster,  0, "The master: send data to other proxmark"},
  {"slave",  CmdRelaySlave,   0, "The slave: receive data from other proxmark"},
  {"delay",  CmdRelayDelay,   0, "Read the measured delay"},
  {NULL, NULL, 0, NULL}
};

int CmdHelp(const char *Cmd)
{
  CmdsHelp(CommandTable);
  return 0;
}

int CmdRelay(const char *Cmd)
{
  CmdsParse(CommandTable, Cmd);
  return 0; 
}
