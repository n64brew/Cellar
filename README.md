# Cellar

This repository contains a static webpage for N64brew's homebrew repository. It also contains the tools used to generate the website.
</br></br>


### How to use the tools to modify the website

1) Download or compile the tools.
2) Place them in the root of this repository. If you're unsure what that means, just make sure both programs are in the same folder as this README file.
3) You can use the editor GUI (named `cellar-editor`) to take care of editing and compiling the website. This is the recommended route.
4) When finished, recompile the website with `CTRL + R` or by going to `File -> Full Website Recompile`. This will update all the pages, including the index, lists, tags, and content pages.

If you want to edit it manually, just modify any relevant JSON files:
* The list of ROMs are in `roms/roms.json`.
Then, to compile the website, call:
```
./cellar-generator roms
```

If you wish to only compile a single page (faster for previewing), then do:
```
./cellar-generator roms -single category/name
```
where `category` is the category key in the JSON file (AKA `Folder:` in the GUI), and the name is the ROM key in the JSON file (AKA `File:` in the GUI). As a concrete example, the `Pyoro64` page inside the `Ports and Clones` category would be: `portsclones/pyoro64`.
</br></br>


### Submit new ROMs

1) Make a fork of this repo.
2) Use the GUI tool to make relevant changes, or do it manually by editing the JSON file.
3) Compile the whole website and then submit a PR with your changes.
</br></br>


### How to compile the tools

##### Compiler CLI

**Windows**

Simply open the Visual Studio solution in Visual Studio 2019+ and compile it there. If you want XP compatibility, then you will need to use the `Win32 (XP)` solution platform. Otherwise, use `x64`.
</br></br>


**Linux and macOS**

Run `make`. Alternatively use `make DEBUG=1` to compile the project in debug mode.
</br></br>


##### Editor GUI

The tool can be compiled on Windows, Mac, and Linux. To do that, you must first have wxWidgets installed and working. This is outlined in the [wxWidgets website](https://docs.wxwidgets.org/latest/page_port.html), and if that is too confusing, just look at one of my [Azure Pipeline scripts](https://github.com/buu342/N64-Sausage64/blob/master/azure-pipelines.yml).
    
Because my version of wxWidgets is designed with Windows XP compatibility in mind, I use Visual Studio 2019 with the old XP SDK (`v141_xp`) to compile the solution. If you are not interested in supporting XP, then feel free to use a later version of the IDE and SDK. 

Download the [wxWidgets source code ZIP](https://wxwidgets.org/downloads/), and extract it. If you are targeting Windows XP, use wxWidgets 3.2.8, otherwise use the latest. Go to the `build/msw` folder and open the solution file for your Visual Studio version. Since I am using VS 2019, open `wx_vc16.sln` (as Visual Studio 2019 refers to version 16). Once the solution loads, you will need to edit all the projects options to use `/MT` for code generation, as wxWidgets compiles with `/MD` by default. Afterwards, if you want XP compatibility then make sure you set the Platform Toolset to `v141_xp`. After those steps are completed, compile the wxWidgets static libraries using your preferred setup (debug/release 32/64). Once wxWidgets is finished compiling, make sure that the `WXWIN` environment variable is set on your system to point to the wxWidgets directory (if it isn't, do so). If you are able to compile the OpenGL samples (after changing them to `/MT` and their platform toolkit if needed), then you are good to go.

Once wxWidgets is taken care of:
</br></br>


**Windows**

Simply open the Visual Studio solution in Visual Studio 2019+ and compile it there. If you want XP compatibility, then you will need to use the `Win32 (XP)` solution platform. Otherwise, use `x64`.
</br></br>


**Linux and macOS**

Run `make`. Alternatively use `make DEBUG=1` to compile the project in debug mode.
</br></br>