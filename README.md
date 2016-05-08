# The Homebrew Launcher U

This is a WiiU homebrew how permit listing homebrew application and run them (like the Homebrew Channel of the Wii, by Team Twiizer).

#### Usage

To use the Homebrew Launcher U (or HBLU, for short) you must copy homebrew_launcher.elf into SD:/wiiu/apps/homebrew_launcher/homebrew_launcher.elf, and run the installer throught your WiiU browser.

Every apps who will be listed are in a directory named wiiu/apps/homebrew_name/homebrew_name.elf. Here is an example:

- sdmc:/
  - wiiu/
    - apps/
     - homebrew_launcher/
        - homebrew_launcher.elf
        - meta.xml
        - icon.png
     - loadiine_gx2/
       - loadiine_gx2.elf
       - meta.xml
       - icon.png
     - ddd/
       - ddd.elf
       - meta.xml
       - icon.png
     - ftpiiu/
       - ftpiiu.elf
       - meta.xml
       - icon.png

#### ELF and XML

ELF (executable and linkable format) can be linked using [dekitPPC](http://devkitpro.org/wiki/Getting_Started).

Here is a XML example:

    <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    <app version="1">
    <name>name</name>
    <coder>coder<coder>
    <version>app version</version>
    <release_date>app release date</release_date>
    <short_description>short description</short_description> 
    <long_description>long description</long_description> 
    </app>

#### Building

To build the main application devkitPPC is required as well as some additionally libraries. If not yet done export the path of devkitPPC and devkitPRO to the evironment variables DEVKITPRO and DEVKITPPC. All remaining is to enter the main application path and enter "make". You should get a homebrew_launcher.elf and a homebrew__launcher_dbg.elf in the main path.

To compile the installer application enter the "installer" path on the source code and type "make".
