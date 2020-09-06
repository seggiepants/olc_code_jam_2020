# The Great Machine Armada
by: seggiepants

8/29/2020 - 9/6/2020

![Screenshot](https://raw.githubusercontent.com/seggiepants/olc_code_jam_2020/master/screenshot.png)
<https://github.com/seggiepants/olc_code_jam_2020>

My entry for the OLC Code Jam 2020.

The theme was The Great Machine. I was going to make a fairly simple side scrolling shmup (like Gradius or R-Type). However, things did not move as quickly as I would have liked. As such, I had to scale it back dramatically to a simple space invaders clone.

## Controls:
* A or Left Arrow - Move the tank left
* D or Right Arrow - Move the tank right
* Space Bar - Fire the tank's canon.
* P - Pause the game
* ESCAPE - Exit to the title (will also exit to the from the main menu OS in the windows version)
* Windows version also allows use of a joystick (first one only) and maps button 1 to fire and button 2 to ESCAPE.

## Technology:
One thing I wanted for this project was to be able to output in WebAssembly as games that can't be played in the web browser are unlikely to be played nearly as much. If I am going to go to all this effort I want to have people play it. I leaned on several libraries/resources/programs including the following:
* Emscripten - Changes C/C++ code to web assembly. <https://emscripten.org/>
* Dear ImGui - Immediate mode GUI library <https://github.com/ocornut/imgui>
* imgui_sdl - Rendering for Dear ImGui using plain SDL2 <https://github.com/Tyyppi77/imgui_sdl>
* SDL2 - Graphics, input, audio library <https://www.libsdl.org/>
* SDL Mixer 2.0 - Sound mixing library for SDL 2 <https://www.libsdl.org/projects/SDL_mixer/>
* SDL Image 2.0 - Image loading library for SDL 2 <https://www.libsdl.org/projects/SDL_image/>
* SDL TTF - True Type Font library for SDL2 <https://www.libsdl.org/projects/SDL_ttf/>
* Nova Font - Used with SDL TTF <http://fontlibrary.org>
* Images from Kenney.nl <https://kenney.nl/>
* BFXR (standalone) for sound effect generation <https://www.bfxr.net/>
* GIMP for image editing <https://www.gimp.org/>
* Visual Studio 2017 Community Edition <https://visualstudio.microsoft.com/>
* Notepad++ <https://notepad-plus-plus.org/>

## Compilation:
### Windows:
You should be able to load (and probably upgrade) the solution file from the repository. You will need to update include and library paths to where SDL2 is located on your machine. I did combine all of the various SDL 2 libraries together into a folder, if you did not you many need to add extra folders. Otherwise it should compile normally in debug and release x64.

### WebAssembly
You will need to have emscripten installed and run the batch file to setup the environment variables, and update the search paths.

The make file is in the src folder. You will need to update the paths for SDL 2 to where things are on your machine. I am not all that great at makefiles, so if you know a way to write it better, please submit a pull request. You should have an obj folder in the src folder and an html folder two levels above. You will also need to have a working copy of make which is not standard on windows. Otherwise just type make. I wouldn't really trust the clean command (just do that manually as needed).

## Problems
If you have problems with the webassembly version of the program but not the win32 version, then debugging is a nightmare. In my case cleaning out the html and obj folders for a clean build straightened things out. I spent a lot of wasted time there. In general emscripten also required a lot of conditional compile tweaks. I also had a problem with the main loop, emscripten didn't like it when I wanted one per scene. Joystick support doesn't work (not really surprising). I also can't get full screen to work (if anyone has a pull request for that I would be happy to see it).

Not sure what I am doing wrong but visual studio sure didn't want to show my source files in proper folders. That was annoying, but not a big waste. In general however, I did spend a lot of time getting visual studio set up. 

Time. Time is always a problem in a game jam. Don't think I could have finished this any faster. It is a good thing I spent the week before the jam trying to get a SDL/emscripten sample project working or I would have been sunk.

Am I missing it or does SDL Mixer not let you change the playback speed of a sound effect. I was going to use it for the invaders march but when I went to add it I couldn't find it. No music because of that.

## Thoughts
I am far more rusty at C/C++ than I thought. C++ slowed me down a lot more than expected. I probably have some bad habits from old C++ compared to new idoms. I did want Windows and Web versions and outside of trying to learn Rust this was the most expedient route. Wanting webassembly also forced me into SDL2 as it is easily supported. Although I probably would have gone for SDL 2 anyway.

I was surprised how nice ImGui worked once integrated. It took quite a while to get working but then things like the next level, and game over popups went surprisingly easy. If I had more time there would have been a lot more menu options. The immediate mode GUI aspect still puzzles me in places. Again, I need more time to look at it.

I think I have a fairly good start on a nice 2D game framework, I think I should probably come back and get it into better shape for future game jams. Animations, sprite sheets, and tile maps would be a good start.