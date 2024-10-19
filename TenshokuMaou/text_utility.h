#ifndef TEXT_UTILITY_H_
#define TEXT_UTILITY_H_

#include <string>
#include <vector>

namespace text_utility
{
	void TextToLines(const std::string& strText, std::vector<std::string>& lines);
	void SplitTextBySeparator(const std::string& strText, const char cSeparator, std::vector<std::string>& splits);

	void ReplaceAll(std::wstring& wstrText, const std::wstring& wstrOld, const std::wstring& wstrNew);

	std::wstring ExtractDirectory(const std::wstring& wstrFilePath, size_t nLevel = 1);

	std::string TruncateFilePath(const std::string& strRelativePath);
	std::string RemoveExtension(const std::string& strFileName);

	void ExtractTagAttrbutes(const std::string& strText, const char* tagName, const char* attributeName, std::vector<std::string>& attributes);
}

#endif // !TEXT_UTILITY_H_
