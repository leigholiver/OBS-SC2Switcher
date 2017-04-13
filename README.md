## OBS-SC2Switcher

[Download](https://github.com/leigholiver/OBS-SC2Switcher/releases/download/0.3/sc2switcher.dll)

Installation: Move sc2switcher.dll into your `obs-studio/obs-plugins/64bit` directory 

#### If you use a separate PC to stream: 
Enter the IP address of your SC2 computer in the SC2 PC IP box.
On your SC2 PC, open the Battle.net launcher, click Options, Game Settings, and under SC2, check 'Additional Command Line Arguments', and enter `-clientapi 6119` into the text box. 

You can check that SC2 is configured correctly by going to `http://[Your SC2 PC IP]:6119/ui` in your browser on the streaming PC. It should look something like:
`{"activeScreens":["ScreenBackgroundSC2/ScreenBackgroundSC2","ScreenReplay/ScreenReplay","ScreenNavigationSC2/ScreenNavigationSC2","ScreenForegroundSC2/ScreenForegroundSC2","ScreenBattlenet/ScreenBattlenet"]}`. 
