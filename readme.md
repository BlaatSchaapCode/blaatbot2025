# geblaat

This is the BlaatBot 2025 project.

Back in 2005, the first BlaatBot was written in
Delphi 7. As in 2025 we celebrate the 20th 
anniversary of the BlaatSchaap community, 
we've building yet another IRC bot.

Reading through the IRC specifications, 
starting to build yet another IRC 
implementation. Building a modular approach 
inspired by the BlaatBot2009 design, this
might turn into something more then just a bot.

The bot will be yet another module, and that
module could be replaced by, say, a GUI.
That's the idea of the modular design.

The core will be implemented in C++, but 
modules could be written in another langauge.
Primary, there will be a C and a C++ API/ABI.
But part of the project will be investigating
other possibilities. Can there be Java or
Python modules. The requirement, they must be 
able to receive callbacks from C or C++, and
call into C or C++ code.

To honour the original BlaatBot2005 
implementation, one of the Bot Modules will be
written in Pascal. (Delphi is the name of the
IDE, the langauge is (Object) Pascal)

# Building the project

Note the project is in early development, 
nothing is to be considered stable at this
point in time.

The primary development platform is Linux.
But it will be tested against other Operating
Systems as well. The project builds and runs
under Haiku. A MinGW build will be possible,
but may break from time to time. FreeBSD
has some minor issues. OpenBSD misses modern C++ support. 
NetBSD might work once I figure out how to properly configure it.

## Dependencies

The core has the following dependencies:

* icu-uc 
* nlohmann_json

The networking module libretls

* libretls

