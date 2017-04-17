## OBS SC2 Scene Switcher

[Download](https://github.com/leigholiver/OBS-SC2Switcher/releases/download/0.4/sc2switcher.zip)

Installation: 

64bit: copy `sc2switcher.dll` from the `obs-plugins/64bit` folder in the zip file into `c:/program files (x86)/obs-studio/obs-plugins/64bit` directory

32bit: copy `sc2switcher-32.dll` from the `obs-plugins/32bit` folder in the zip file into `c:/program files (x86)/obs-studio/obs-plugins/32bit` directory 

Usage: Go to Tools -> SC2 Scene Switcher and select a scene to switch to for in game/out of game/in replay. 

#### If you use a separate PC to stream: 
Enter the IP address of your SC2 computer in the SC2 PC IP box.
On your SC2 PC, open the Battle.net launcher, click Options, Game Settings, and under SC2, check 'Additional Command Line Arguments', and enter `-clientapi 6119` into the text box. 

You can check that SC2 is configured correctly by going to `http://[Your SC2 PC IP]:6119/ui` in your browser on the streaming PC. It should look something like:
`{"activeScreens":["ScreenBackgroundSC2/ScreenBackgroundSC2","ScreenReplay/ScreenReplay","ScreenNavigationSC2/ScreenNavigationSC2","ScreenForegroundSC2/ScreenForegroundSC2","ScreenBattlenet/ScreenBattlenet"]}`. 
