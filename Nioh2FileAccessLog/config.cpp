#include "pch.h"
#include "config.h"
using namespace std;

bool config::enableConsole;
bool config::enableFileOverride;
bool config::logFileLoading;
bool config::extendedPathLength;
std::map<std::string, std::string> config::fileOverrides;
std::string config::ModsPath;

bool config::init()
{
	toml::table config;
	std::string fileString = "";

	try
	{
		std::ifstream file("config.toml");
		std::string str;
		while (std::getline(file, str))
			fileString += str + "\n";
		config = toml::parse(fileString);
	}
	catch (std::exception& exception)
	{
		char text[1024];
		sprintf_s(text, "Failed to parse config.toml:\n%s", exception.what());
		MessageBoxA(nullptr, text, "Nioh Mod Loader", MB_OK | MB_ICONERROR);
	}

	enableConsole = config["General"]["Enable_Console"].value_or(true);
	enableFileOverride = config["General"]["Enable_FileOverides"].value_or(true);
	logFileLoading = config["General"]["Log_File_Loading"].value_or(false);
	extendedPathLength = config["Experimental"]["Extended_Path_Length"].value_or(false);
	ModsPath = config["General"]["ModsPath"].value_or("mods");

	auto table = config["FileOverrides"];
	if (table.as_table()) 
	{
		for (auto [key, value] : *table.as_table())
		{
			std::string k = key.str().data();
			std::string v = value.value_or("");

			if (v == "")
			{
				printf("[FileReplaceLog] replacement file path for \"%s\" is empty\n", k.c_str());
				continue;
			}

			fileOverrides.insert({ k, v });
		}
	}

	return true;
}