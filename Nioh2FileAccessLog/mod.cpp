#include "pch.h"
#include "mod.h"
#include <filesystem>
using namespace std;

std::map<std::string, std::string> mod::fileOverrides;

bool mod::init() 
{
	toml::table config;
	std::string fileString = "";

	printf("[DebugLog] Loading Mods...\n");

	if (!std::filesystem::exists("..\\mods")) {
		printf("[DebugLog] Could not find mods path\n");
	}

	for (const auto& entry : std::filesystem::recursive_directory_iterator("..\\mods")) 
	{
		if (std::filesystem::path(entry.path()).extension() != ".toml")
			continue;

		printf("[DebugLog] processing: %S\n", entry.path().filename().c_str());

		try
		{
			std::ifstream file(entry.path().string());
			std::string str;
			while (std::getline(file, str))
				fileString += str + "\n";
			config = toml::parse(fileString);
		}
		catch (std::exception& exception)
		{
			char text[1024];
			sprintf_s(text, "Failed to parse %S:\n%s", entry.path().c_str(), exception.what());
			MessageBoxA(nullptr, text, "File Access Log", MB_OK | MB_ICONERROR);
		}

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
	}

	return true;
}
