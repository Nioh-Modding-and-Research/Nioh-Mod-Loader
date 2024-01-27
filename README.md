# Nioh2-Mod-Loader
 WIP code hook mod loader for Nioh 2

### Requirements
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases), you'll want the one labeled "x64".

### Installation
- install the Ultimate ASI Loader dinput8.dll in the "Nioh2" folder next to the exe
- if the "plugins" folder doesn't exist in your "Nioh2" folder make one
-add the "Nioh2FileAccessLog.asi" and "config.toml" into your plugins folder

### Usage
The Mod loader contains a few options in the config.toml in the plugins folder
| Option | type | Info |
| -------- | ------ | -----|
| Enable_Console | bool | Enables showing the console |
| Enable_FileOverides | bool | Enables loose files to be loaded over archive files |
| Log_File_Loading | bool | Prints loaded file names to the console as the file is loaded |

#### File Overrides
in the FileOverrides section of the toml archive files can be overwittern.  
Overrides are defined by adding the original hashed file name as the key and the new file path as the value.  
example:
`70EA2C86B93C63A66C8EC33787920BC8C2C74E5E69037EC4C2D82C2E28B5A99B = "data\\character\\CN_MIYOSHINO\\CN_MIYOSHINO.pg1m"`


### Credits
[DeathChaos25](https://github.com/DeathChaos25) - help setting up detours and toml 