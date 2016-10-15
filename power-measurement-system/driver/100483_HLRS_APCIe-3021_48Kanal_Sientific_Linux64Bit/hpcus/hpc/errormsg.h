#ifndef ERRORMSG_H_
#define ERRORMSG_H_

const char* msg_CMD_xpci3xxx_TestInterrupt(int nb);

const char* msg_CMD_xpci3xxx_InitAnalogInputSequence(int nb);
const char* msg_CMD_xpci3xxx_ReleaseAnalogInputSequence(int nb);
const char* msg_CMD_xpci3xxx_StartAnalogInputSequence(int nb);
const char* msg_CMD_xpci3xxx_StopAnalogInputSequence(int nb);

const char* msg_CMD_xpci3xxx_InitAnalogInputAutoRefresh(int nb);
const char* msg_CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh(int nb);
const char* msg_CMD_xpci3xxx_StartAnalogInputAutoRefresh(int nb);
const char* msg_CMD_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter(int nb);
const char* msg_CMD_xpci3xxx_StopAnalogInputAutoRefresh(int nb);
const char* msg_CMD_xpci3xxx_EnableDisableAnalogInputHardwareTrigger(int nb);

const char* msg_CMD_xpci3xxx_ReadAnalogOutputBit(int nb);
const char* msg_CMD_xpci3xxx_InitAnalogOutput(int nb);
const char* msg_CMD_xpci3xxx_WriteAnalogOutputValue(int nb);

const char* msg_CMD_xpci3xxx_SetTTLPortConfiguration(int nb);

const char* msg_CMD_xpci3xxx_SetDigitalOutputMemoryOn(int nb);
const char* msg_CMD_xpci3xxx_Set32DigitalOutputsOff(int nb);
const char* msg_CMD_xpci3xxx_Set32DigitalOutputsOn(int nb);
const char* msg_CMD_xpci3xxx_Read32DigitalInputs(int nb);
const char* msg_CMD_xpci3xxx_Get32DigitalOutputStatus(int nb);

const char* msg_CMD_xpci3xxx_InitTimer(int nb);
const char* msg_CMD_xpci3xxx_EnableDisableTimerInterrupt(int nb);
const char* msg_CMD_xpci3xxx_EnableDisableTimerHardwareTrigger(int nb);
const char* msg_CMD_xpci3xxx_StartTimer(int nb);
const char* msg_CMD_xpci3xxx_StopTimer(int nb);
const char* msg_CMD_xpci3xxx_ReleaseTimer(int nb);

const char* msg_CMD_xpci3xxx_OUTPORTDW(int nb);
const char* msg_CMD_xpci3xxx_INPORTDW(int nb);

#endif /*ERRORMSG_H_*/
