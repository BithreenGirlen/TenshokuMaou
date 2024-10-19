/*�t�@�C������E�R���\�[���o��*/

#include <Windows.h>
#include <urlmon.h>
#include <atlbase.h>

#include <string>

#include "file_utility.h"

#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "shlwapi.lib")


struct ComInit
{
	HRESULT m_hrComInit;
	ComInit() : m_hrComInit(::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)) {}
	~ComInit() { if (SUCCEEDED(m_hrComInit)) ::CoUninitialize(); }
};

/*-----------------------  �����֐�  -----------------------*/

void WriteMessage(const char* msg);

bool WriteStringToFile(const std::string& data, const char* file_path);

bool GetInternetResourceSize(const char* url, ULARGE_INTEGER& size);
std::string GetFileNameFromUrl(const std::string& url);
std::string GetDirectoryFromUrl(const std::string& url);


/*�`���o��*/
void WriteMessage(const char* msg)
{
	char stamp[16];
	SYSTEMTIME tm;
	::GetLocalTime(&tm);
	sprintf_s(stamp, "%02d:%02d:%02d:%03d ", tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);
	std::string str = stamp + std::string(msg) + "\r\n";
	printf(str.c_str());
}

/*���s�v���Z�X�̊K�w�擾*/
std::string GetBaseFolderPath()
{
	char pzPath[MAX_PATH]{};
	::GetModuleFileNameA(nullptr, pzPath, MAX_PATH);
	std::string::size_type pos = std::string(pzPath).find_last_of("\\/");
	return std::string(pzPath).substr(0, pos) + "\\";
}
/*��ƃt�H���_�쐬*/
std::string CreateWorkFolder(const std::string &strRelativePath)
{
	if (strRelativePath.empty())return std::string();

	std::string strPath = GetBaseFolderPath();
	size_t nRead = 0;
	if (strRelativePath.front() == '\\')++nRead;

	for (; nRead < strRelativePath.size();)
	{
		const char* pPos = strpbrk(&strRelativePath[nRead], "\\/");
		if (pPos == nullptr)
		{
			strPath += strRelativePath.substr(nRead);
			if (strRelativePath.back() != '\\')strPath += "\\";
			::CreateDirectoryA(strPath.c_str(), nullptr);
			break;
		}
		size_t nPos = pPos - &strRelativePath[nRead];
		strPath += strRelativePath.substr(nRead, nPos) + "\\";
		::CreateDirectoryA(strPath.c_str(), nullptr);

		nRead += nPos + 1;
	}

	return strPath;
}
/*����URL�����Ƀt�H���_�쐬*/
std::string CreateFolderBasedOnRelativeUrl(const std::string& strUrl, const std::string& strBaseFolder, int iDepth, bool bBeFilePath)
{
	if (!strUrl.empty())
	{
		std::string strFolder = strBaseFolder;
		size_t nRead = 0;
		int iOccurrence = 0;
		for (;;)
		{
			size_t nPos = strUrl.substr(nRead).find_first_of("\\/");
			if (nPos == std::string::npos)
			{
				if (bBeFilePath)
				{
					strFolder += strUrl.substr(nRead);
				}
				break;
			}
			++iOccurrence;
			if (iOccurrence > iDepth && nPos != 0)
			{
				strFolder += strUrl.substr(nRead, nPos) + "\\";
				::CreateDirectoryA(strFolder.c_str(), nullptr);
			}
			nRead += nPos + 1;
		}

		return strFolder;
	}

	return std::string();
}

/*�t�@�C���̃������W�J*/
char* LoadExistingFile(const char* pzFilePath, unsigned long *ulSize)
{
	HANDLE hFile = ::CreateFileA(pzFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwSize = ::GetFileSize(hFile, nullptr);
		if (dwSize != INVALID_FILE_SIZE)
		{
			char* buffer = static_cast<char*>(malloc(static_cast<size_t>(dwSize + 1ULL)));
			if (buffer != nullptr)
			{
				DWORD dwRead = 0;
				BOOL iRet = ::ReadFile(hFile, buffer, dwSize, &dwRead, nullptr);
				if (iRet)
				{
					::CloseHandle(hFile);
					*(buffer + dwRead) = '\0';

					if (ulSize != nullptr)*ulSize = dwRead;

					return buffer;
				}
				else
				{
					free(buffer);
				}
			}
		}
		::CloseHandle(hFile);
	}

	return nullptr;
}

std::string LoadExistingFileAsString(const char* pzFilePath)
{
	DWORD ulSize = 0;
	char* pBuffer = LoadExistingFile(pzFilePath, &ulSize);
	if (pBuffer != nullptr)
	{
		std::string str;
		str.resize(ulSize);
		memcpy(&str[0], pBuffer, ulSize);

		free(pBuffer);
		return str;
	}

	return std::string();
}
/*�������̃t�@�C���o��*/
bool WriteStringToFile(const std::string& strData, const char* file_path)
{
	BOOL iRet = 0;

	if (file_path != nullptr)
	{
		HANDLE hFile = ::CreateFileA(file_path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			::SetFilePointer(hFile, NULL, nullptr, FILE_END);

			DWORD bytesWrite = 0;
			iRet = ::WriteFile(hFile, strData.data(), static_cast<DWORD>(strData.size()), &bytesWrite, nullptr);
			::CloseHandle(hFile);
		}
	}
	return iRet > 0;
}
/*�ۑ��������b�Z�[�W�o��*/
bool SaveStringToFile(const std::string& strData, const char* pzFilePath)
{
	bool bRet = false;

	if (pzFilePath != nullptr)
	{
		std::string strFileName = GetFileNameFromUrl(pzFilePath);
		if (!strFileName.empty())
		{
			bRet = WriteStringToFile(strData, pzFilePath);
			if (bRet)
			{
				WriteMessage(std::string(strFileName).append(" success.").c_str());
			}
			else
			{
				WriteMessage(std::string(strFileName).append(" failed.").c_str());
			}
		}
	}

	return bRet;
}
/*�t�@�C���L���m�F*/
bool DoesFilePathExist(const char* pzFilePath)
{
	BOOL iRet = ::PathFileExistsA(pzFilePath);
	if (iRet)
	{
		std::string strFileName = GetFileNameFromUrl(pzFilePath);
		WriteMessage(std::string(strFileName).append(" already exists.").c_str());
	}

	return iRet == TRUE;
}

/*�d�q�Ԏ����̑傫���擾*/
bool GetInternetResourceSize(const char* url, ULARGE_INTEGER& size)
{
	ComInit init;
	CComPtr<IStream> pStream;
	HRESULT hr = ::URLOpenBlockingStreamA(nullptr, url, &pStream, 0, nullptr);

	if (hr == S_OK)
	{
		STATSTG stat;
		hr = pStream->Stat(&stat, STATFLAG_DEFAULT);
		if (hr == S_OK)
		{
			size = stat.cbSize;
			return true;
		}
	}
	return false;
}
/*URL����t�@�C�����擾*/
std::string GetFileNameFromUrl(const std::string &url)
{
	size_t pos = url.find_last_of("\\/");
	if (pos != std::string::npos)
	{
		return url.substr(pos + 1);
	}

	return std::string();
}
/*URL����K�w�擾*/
std::string GetDirectoryFromUrl(const std::string &url)
{
	size_t pos = url.find_last_of("\\/");
	if (pos != std::string::npos)
	{
		return url.substr(0, pos + 1);
	}

	return std::string();
}
/*�d�q�Ԏ����̕ۑ�*/
bool SaveInternetResourceToFile(const char* pzUrl, const char* pzFolder, const char* pzFileName, unsigned long nMinFileSize, bool bFolderCreation)
{
	std::string strFileName = pzFileName == nullptr ? GetFileNameFromUrl(pzUrl) : strchr(pzFileName, '/') == nullptr ? pzFileName : GetFileNameFromUrl(pzFileName);
	if (!strFileName.empty())
	{
		std::string strFilePath = pzFolder + strFileName;

		if (!::PathFileExistsA(strFilePath.c_str()))
		{
			ULARGE_INTEGER size{};
			bool bRet = GetInternetResourceSize(pzUrl, size);
			if (bRet)
			{
				if (size.LowPart > nMinFileSize)
				{
					if (bFolderCreation)
					{
						::CreateDirectoryA(pzFolder, nullptr);
					}
					ComInit init;
					HRESULT hr = ::URLDownloadToFileA(nullptr, pzUrl, strFilePath.c_str(), 0, nullptr);
					if (hr == S_OK)
					{
						WriteMessage(std::string(pzUrl).append(" success").c_str());
						return true;
					}
					else
					{
						WriteMessage(std::string(pzUrl).append(" failed").c_str());
					}
				}

			}
		}
		else
		{
			WriteMessage(std::string(strFileName).append(" already exists.").c_str());
			return true;
		}
	}
	else
	{
		WriteMessage("File path invalid.");
	}

	return false;
}
/*�d�q�Ԏ����̃������W�J*/
bool LoadInternetResourceToBuffer(const char* url, char** dst, unsigned long* ulSize)
{
	if (url == nullptr || *dst != nullptr)return false;

	ComInit init;
	CComPtr<IStream> pStream;
	HRESULT hr = ::URLOpenBlockingStreamA(nullptr, url, &pStream, 0, nullptr);

	if (hr == S_OK)
	{
		STATSTG stat;
		hr = pStream->Stat(&stat, STATFLAG_DEFAULT);
		if (hr == S_OK)
		{
			char* buffer = static_cast<char*>(malloc(stat.cbSize.LowPart + 1LL));
			if (buffer != nullptr)
			{
				DWORD dwReadBytes = 0;
				DWORD dwSize = 0;
				for (;;)
				{
					hr = pStream->Read(buffer + dwSize, stat.cbSize.LowPart - dwSize, &dwReadBytes);
					if (FAILED(hr))break;
					dwSize += dwReadBytes;
					if (dwSize >= stat.cbSize.LowPart)break;
				}
				*(buffer + dwSize) = '\0';
				*ulSize = dwSize;
				*dst = buffer;

				return true;
			}
		}
	}

	return false;
}
/*������Ƃ��ēd�q�Ԏ����ǂݍ���*/
std::string LoadInternetResourceAsString(const char* url)
{
	char* pBuffer = nullptr;
	unsigned long ulSize = 0;
	LoadInternetResourceToBuffer(url, &pBuffer, &ulSize);
	if (pBuffer != nullptr)
	{
		std::string str;
		str.resize(ulSize);
		memcpy(&str[0], pBuffer, ulSize);
		free(pBuffer);
		return str;
	}
	return std::string();
}

bool SaveInternetResourceToFileCreatingNestedFolder(const char* pzUrl, const char* pzFileName, const char* pzBaseFolder, int iDepth)
{
	if (pzUrl == nullptr)return false;

	const auto AbsoluteToRelative = [](const std::string& strUrl)
		-> std::string
		{
			const char szKey[] = "https://";

			size_t nPos = strUrl.find(szKey);
			if (nPos == std::string::npos)nPos = 0;
			nPos += sizeof(szKey);
			if (nPos > strUrl.size())return strUrl;

			const char* p = strchr(&strUrl[nPos], '/');
			if (p == nullptr)return strUrl.substr(nPos);

			nPos += p - &strUrl[nPos] + 1;
			return strUrl.substr(nPos);
		};

	std::string strRelativeUrl = AbsoluteToRelative(pzUrl);

	/*
	* �t�@�C���������̊K�w�\�������܂ޏꍇ�̓t�@�C��������Ƀt�H���_����
	*/
	std::string strFolderPath;
	if (pzFileName != nullptr)
	{
		strFolderPath = CreateFolderBasedOnRelativeUrl(pzFileName, pzBaseFolder == nullptr ? GetBaseFolderPath() : pzBaseFolder, iDepth, false);
	}
	else
	{
		strFolderPath = CreateFolderBasedOnRelativeUrl(strRelativeUrl, pzBaseFolder == nullptr ? GetBaseFolderPath() : pzBaseFolder, iDepth, false);
	}

	return SaveInternetResourceToFile(pzUrl, strFolderPath.c_str(), pzFileName, 0, false);
}