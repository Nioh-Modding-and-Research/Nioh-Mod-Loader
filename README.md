# Nioh-Mod-Loader
 WIP code hook mod loader for Nioh 1 & 2
 The main focus of this mod loader is support for Nioh 2 but due to how similar the engines are I'll support both games when possible.

### Requirements
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases), you'll want the one labeled "x64".

### Installation
- install the Ultimate ASI Loader dinput8.dll in the "Nioh2" folder next to the exe
- if the "plugins" folder doesn't exist in your "Nioh2" folder make one
- add the "Nioh2FileAccessLog.asi" and "config.toml" into your plugins folder

### Usage
The Mod loader contains a few options in the config.toml in the plugins folder
| Option | type | Info | Nioh 1 | Nioh 2 |
| ------ | ---- | ---- | ------ | ------ |
| Enable_Console | bool | Enables showing the console | [x] | [x] |
| Enable_FileOverides | bool | Enables loose files to be loaded over archive files | [x] | [x] |
| Log_File_Loading | bool | Prints loaded file names to the console as the file is loaded | [x] | [x] |
| ModsPath | string | Path relative to the game exe to search for mod tomls | [x] | [x] |

#### File Overrides
in the FileOverrides section of the toml archive files can be overwittern.  
Overrides are defined by adding the original hashed file name as the key and the new file path as the value.  
example:  
```
[FileOverrides]
70EA2C86B93C63A66C8EC33787920BC8C2C74E5E69037EC4C2D82C2E28B5A99B = "data\\character\\CN_MIYOSHINO\\CN_MIYOSHINO.pg1m"
```

#### Mod Tomls
Mods can provide their own toml files instead of have to relay on the main config.toml but their functionality only supports file overrides.  
file overrides are done in the exact same way as shown above however keep in mind as of the moment there's no handling of conflicting mods.  
whichever mod get's loaded first will get the priority, this may be improved in the future.  
Mod tomls don't need to have a specific file name, as long as they are a .toml and inside the set mods folder they should get picked up.

### Credits
[DeathChaos25](https://github.com/DeathChaos25) - help setting up detours and toml 
