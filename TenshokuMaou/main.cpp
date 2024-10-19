
#include <string>
#include <vector>

#include "file_utility.h"
#include "text_utility.h"
#include "json_minimal.h"
#include "win_process.h"


std::string GetJsDeclarationValue(const std::string& strText, const char* variableName)
{
	if (variableName == nullptr)return std::string();

	size_t nPos = strText.find(variableName);
	if (nPos == std::string::npos)return std::string();

	nPos = strText.find("=", nPos);
	if (nPos == std::string::npos)return std::string();

	nPos = strText.find("\"", nPos);
	if (nPos == std::string::npos)return std::string();
	++nPos;

	size_t nPos2 = strText.find("\"", nPos);

	return strText.substr(nPos, nPos2 - nPos);
}

std::string LoadConfigUrl()
{
	std::string strIndexUrl = "https://app-dot-king-20240410.an.r.appspot.com/5791070751293440/game/adult";
	std::string strIndexHtml = LoadInternetResourceAsString(strIndexUrl.c_str());
	if (strIndexHtml.empty())return std::string();

	std::vector<std::string> attributes;
	text_utility::ExtractTagAttrbutes(strIndexHtml, "script", "src", attributes);

	for (const auto& attribute : attributes)
	{
		if (attribute.find("__settings__.js") != std::string::npos)
		{
			std::string strScript = LoadInternetResourceAsString(attribute.c_str());
			if (strScript.empty())return std::string();

			return GetJsDeclarationValue(strScript, "window.CONFIG_FILENAME");
		}
	}

	return std::string();
}

void GetConfigFilePaths(std::vector<std::string> &filePaths, std::string &hostUrl)
{
	std::string strConfigUrl = LoadConfigUrl();
	if (strConfigUrl.empty())return;

	size_t nPos = strConfigUrl.rfind("/");
	if (nPos == std::string::npos)return;

	hostUrl = strConfigUrl.substr(0, nPos + 1);

	std::string strConfigFile = LoadInternetResourceAsString(strConfigUrl.c_str());
	if (strConfigFile.empty())return;

	char* p = &strConfigFile[0];
	char* pp1 = nullptr;
	json_minimal::ExtractJsonObject(&p, "assets", &pp1);
	if (pp1 == nullptr)return;

	p = pp1;
	json_minimal::ReadUpToNameEnd(&p);
	json_minimal::ReadUpToNameEnd(&p);
	std::vector<std::string> files;
	for (;;)
	{
		char* pp2 = nullptr;
		json_minimal::ExtractJsonObject(&p, nullptr, &pp2);
		if (pp2 == nullptr)break;
		files.push_back(pp2);
		free(pp2);
	}

	std::vector<char> vBuffer(512, '\0');
	for (auto& file : files)
	{
		bool bRet = json_minimal::GetJsonElementValue(&file[0], "url", vBuffer.data(), vBuffer.size());
		if (!bRet)continue;
		filePaths.push_back(vBuffer.data());
	}
	free(pp1);
}

std::string GetTidyFolderName(const std::string& strFilePath)
{
	size_t nPos1 = strFilePath.rfind('/');
	if (nPos1 == std::string::npos)return std::string();
	++nPos1;

	size_t nPos2 = strFilePath.find('_', nPos1);
	if (nPos2 == std::string::npos)return std::string();

	return strFilePath.substr(nPos1, nPos2 - nPos1);
}

bool DecompressBasisToPng(const std::string& strUrl, const std::string &strFolder)
{
	std::string strBasisFilePath = strFolder + text_utility::TruncateFilePath(strUrl);
	std::string strPngFileName = text_utility::RemoveExtension(strUrl) + ".png";

	std::string strExecutableFilePath = GetBaseFolderPath() + "basisu.exe";

	std::string strCommandLine;
	strCommandLine.reserve(512);
	strCommandLine = "basisu ";
	strCommandLine += strBasisFilePath;
	strCommandLine += " -no_ktx -format_only 1"; /* cTFETC2_RGBA */
	strCommandLine += " -output_path ";
	strCommandLine += strFolder;
	strCommandLine += " -output_file ";
	strCommandLine += strPngFileName;

	return win_process::ExecuteProcessSync(strExecutableFilePath.c_str(), strCommandLine.c_str());
}

int main()
{
	std::vector<std::string> filePaths;
	std::string hostUrl;
	GetConfigFilePaths(filePaths, hostUrl);

	for (const auto& filePath : filePaths)
	{
		const auto IsEndWith = [filePath](const std::string& str)
			-> bool
			{
				if (filePath.size() < str.size()) return false;
				return std::equal(str.rbegin(), str.rend(), filePath.rbegin());
			};

		//if(IsEndWith(".js") || IsEndWith(".wasm"))continue;

		if (filePath.find("_still") == std::string::npos /*&&
			filePath.find("_H0") == std::string::npos*/)continue;

		std::string strFolderName = GetTidyFolderName(filePath);
		std::string strFolderPath = CreateWorkFolder("Resource\\" + strFolderName);

		std::string strUrl = hostUrl + filePath;
		SaveInternetResourceToFile(strUrl.c_str(), strFolderPath.c_str(), nullptr, 0, false);
		if (IsEndWith(".basis"))
		{
			DecompressBasisToPng(strUrl, strFolderPath);
		}
	}
}
