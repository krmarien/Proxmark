//-----------------------------------------------------------------------------
// High frequency relay commands
//-----------------------------------------------------------------------------

#ifndef CMDRELAY_H__
#define CMDRELAY_H__

int CmdRelayMaster(const char *Cmd);
int CmdRelaySlave(const char *Cmd);
int CmdRelayDelay(const char *Cmd);

int CmdRelay(const char *Cmd);

#endif
