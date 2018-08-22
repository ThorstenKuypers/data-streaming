
// DataSimulator - simulates iracing sim that is sending data to memory maped file

#include <Windows.h>
#include <iostream>
#include <csignal>

#include "irsdk_server/irsdk_server.h"

double g_SessionTime;
int g_Lap;
bool g_run = true;

void sigint_handler(int p) 
{
	g_run = false;
}

int main(int arc, char**argv)
{
	std::cout << "iRacing SDK Data Simulator" << std::endl;
	std::cout << "  - simulates data send by iRacing sim to memory mapped buffer" << std::endl;
	std::cout << " ! runs endless till killed ! Ctrl+C" << std::endl;

	signal(SIGINT, sigint_handler);

	int sessionNum = 0x12345;
	int playerCarIdx = 0;
	irsdkServer::instance()->regVar((const char*)"SessionTime", &g_SessionTime, irsdk_VarType::irsdk_double, 1, (const char*)"Time elapsed since session start", (const char*)"s", IRSDK_LOG_ALL);
	irsdkServer::instance()->regVar((const char*)"Lap", &g_Lap, irsdk_VarType::irsdk_int, 1, (const char*)"current lap", (const char*)"", IRSDK_LOG_ALL);
	irsdkServer::instance()->regVar((const char*)"SessionNum", &sessionNum, irsdk_VarType::irsdk_int, 1, (const char*)"Session ID", (const char*)"", IRSDK_LOG_LIVE);
	irsdkServer::instance()->regVar((const char*)"PlayerCarIdx", &playerCarIdx, irsdk_VarType::irsdk_int, 1, (const char*)"Player Car Index", (const char*)"", IRSDK_LOG_LIVE);

	g_Lap = 1;
	g_SessionTime = 0;

	while (g_run) {

		if (irsdkServer::instance()->isDiskLoggingEnabled())
			irsdkServer::instance()->toggleDiskLogging();

		if (!irsdkServer::instance()->isHeaderFinalized())
			irsdkServer::instance()->finalizeHeader();

		irsdkServer::instance()->pollSampleVars();

		g_SessionTime += 1.0 / TICKS_PER_SEC;
		DWORD sl = (1.0 / TICKS_PER_SEC) * 1000;
		Sleep(sl);
	}

	system("PAUSE");
	return 0;
}