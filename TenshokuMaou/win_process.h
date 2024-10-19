#ifndef WIN_PROCESS_H_
#define WIN_PROCESS_H_

namespace win_process
{
	bool ExecuteBatchSync(const char* pzFilePath);
	bool ExecuteProcessSync(const char* pzFilePath, const char* pzCommandLine);
}

#endif // WIN_PROCESS_H_
