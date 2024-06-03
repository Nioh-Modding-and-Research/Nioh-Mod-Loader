#pragma once
class config
{
public:
	static bool enableConsole;
	static bool enableFileOverride;
	static bool logFileLoading;
	static bool extendedPathLength;
	static std::map<std::string, std::string> fileOverrides;
	static std::string ModsPath;

	static bool init();
};

