#ifndef FILE_UTILITY_H_
#define FILE_UTILITY_H_

#include <string>


/*ファイル操作系*/

std::string GetBaseFolderPath();
std::string CreateWorkFolder(const std::string& strRelativePath);
std::string CreateFolderBasedOnRelativeUrl(const std::string& strUrl, const std::string& strBaseFolder, int iDepth, bool bBeFilePath);

std::string LoadExistingFileAsString(const char* pzFilePath);
bool SaveStringToFile(const std::string& strData, const char* pzFilePath);
bool DoesFilePathExist(const char* pzFilePath);

/*電子網系*/
bool SaveInternetResourceToFile(const char* pzUrl, const char* pzFolder, const char* pzFileName, unsigned long nMinFileSize, bool bFolderCreation);
std::string LoadInternetResourceAsString(const char* url);

bool SaveInternetResourceToFileCreatingNestedFolder(const char* pzUrl, const char* pzFileName, const char* pzBaseFolder, int iDepth);



#endif //FILE_UTILITY_H_
