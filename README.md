## OBS SC2Switcher

## [Download](https://github.com/leigholiver/OBS-SC2Switcher/releases/latest/)

## Installation: 
### Windows
(0.96 and below): [Install the dependencies](https://www.microsoft.com/en-us/download/details.aspx?id=40784)

64bit: copy `sc2switcher.dll` from the zip file into `c:/program files (x86)/obs-studio/obs-plugins/64bit` directory

32bit: copy `sc2switcher-32.dll` from the  zip file into `c:/program files (x86)/obs-studio/obs-plugins/32bit` directory 

### Linux
Copy `sc2switcher.so` from the zip file into your obs-plugins directory. For me this is `/usr/lib/obs-plugins/` but this may vary depending on your distro

## Usage: 
In the Tools menu, click on SC2Switcher. 

### Scene Switcher
- Choose a scene to switch to in different menus/games 
- All are optional and will fall back to the in game/out of game scenes
- The observing scene requires you to have entered your username in the 'usernames' tab

### Score Tracker
- Enter the name of the text source you are using for your scores
- The text source will be updated with your score (mostly) automatically
- If you play against a random player the plugin will ask you for their race
- If you are neither player or both players (ie barcodes), the plugin will ask which player you were
- There is a small chance that these notifications will take focus over sc2. If this is an issue you can untick 'Popups Enabled' and these will be ignored. You can use the buttons to update the score manually 


### Game Webhook 
- When entering or leaving a game, the plugin will send a request to the specified url with information about the game for use in other applications
- For an example of how this could be used you could check out [Ladderbet](https://github.com/leigholiver/ladderbet/), an automated twitch chat betting bot

```
event: 'enter' or 'exit',
displayTime: ~,
players: [
	{
		'name': ~,
		'type': ~,
		'race': 'Terr', 'Zerg' or 'Prot',
		'result': 'Victory' or 'Defeat',
		'isme': 'true' or 'false',
	},
]
scores: {
	'Terr', 'Zerg' or 'Prot': {
		"Victory": ~,
		"Defeat": ~
	},
}
```
- Known issue: Scores may not update until the start of the next game if you change the score manually or come across one of the situations mentioned in the Score Tracker section

## If you use a separate PC to stream: 
Enter the IP address of your SC2 computer in the SC2 PC IP box.
On your SC2 PC, open the Battle.net launcher, click Options, Game Settings, and under SC2, check 'Additional Command Line Arguments', and enter `-clientapi 6119` into the text box. 

You can check that SC2 is configured correctly by going to `http://[Your SC2 PC IP]:6119/ui` in your browser on the streaming PC. It should look something like:
`{"activeScreens":["ScreenBackgroundSC2/ScreenBackgroundSC2","ScreenReplay/ScreenReplay","ScreenNavigationSC2/ScreenNavigationSC2","ScreenForegroundSC2/ScreenForegroundSC2","ScreenBattlenet/ScreenBattlenet"]}`. 

## Building from source:
Make sure you have a version of obs-studio building properly [(instructions are here)](https://github.com/jp9000/obs-studio/wiki/Install-Instructions).

Clone this repository into `[OBS Source Directory]/UI/frontend-plugins/SC2Switcher`

Add the line `add_subdirectory(SC2Switcher)` to `[OBS Source Directory]/UI/frontend-plugins/CMakeLists.txt`

Run CMake again and hit Configure, Generate, then Open Project

