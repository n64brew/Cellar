# Static Site Generator

This folder contains my custom SSG. The tool was written in C++ and uses the [wxWidgets](https://www.wxwidgets.org/) library for a cross-platform GUI. It also uses [JSON for Modern C++](https://github.com/nlohmann/json) and a [fork of MD4C which supports header ID's](https://github.com/tim-gromeyer/MarkdownEdit_md4c). I also modified the MD4C code somewhat to better fit my website's design.

I wrote this SSG specifically for how I wanted to design my website, and mostly as a fun exercise for myself. You can use it for yours, but you might get a lot more out of using one of the already incredibly powerful Static Site Generators out there.
</br></br>


### Compiling

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


### Using the program

When you launch the executable, it will try to load the website in its current working directory. You can change the working directory once the program boots by going to `File -> Open Working Directory` or using the shortcut `CTRL+O`.

The program expects the working directory to have the following folders:
* `projects` - The list of projects
* `blog` - The list of blog pages
* `tags` - The list of tags 
* `template` - The templates to use to generate the other pages

In order to add pages to the list of projects or blog, you need to have categories. You can create categories by going to the projects or blog folder and simply creating a new folder in there. Like that, when you load the generator, the category should be visible in the program tree and you should be able to add new pages by right clicking said category. Pages can be reorderd with a drag and drop.

Clicking on a category or page will bring up the editor on the right side. List of images, URLs, and tags should be comma separated. Dates should be in the format `FullMonthName Day, FullYear`. The page descriptions support markdown, and *some* common unicode characters. Adding support for more will require modifying the `md_sanitize` and `md_unsanitize` function. Unfortunately the latest version of the MD4C library supports all Unicode, but I am using an old version because I want page URL support. Perhaps some day I can consolidate the two, but for now this workaround will be necessary.

Saving using `File -> Save` or `CTRL+S` will cause the program to generate the entire site. To only generate the page you're modifying, click the `Preview` button. The program stores page information in a JSON file which will be located inside the relevant folder.

To generate the site, the tool will use the HTML templates in the `templates` folder. Inside these files, you will spot a bunch of text blocks in the format `_TEMPLATE_X_Y_`, which are keywords that the SSG will find and replace during the page generation. It'll be hard to list all of them here as they're tied to both how my website looks, and sometimes it differs on a page-by-page basis. Frankly, if you want more info about these keywords, just look at the existing templates and at the code itself.