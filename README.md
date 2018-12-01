# Vision-related code for FRC #4241 vision efforts

## Overview
Architecture is a server (cv_cam) for each camera that watches the serial camera output and sends it via a UDP packet to the processor (cv_proc).  Each cv_cam modules passes robot object and visual field location information to cv_proc.

The processor maintains lists per camera, per identified object with timestamps and coordinate-based values for each record.  A time-based sequence of camera records is maintained for each object the cameras report.  A separate thread prunes the lists based on an overall Time-To-Live (TTL) value to remove old records but still allow access to the latest information from each camera as well as an averaged set of values based on the TTL.

Another thread processes the records per identified object (on request) and creates direction commands and calculates distances to the identified object./  This thread will communicate with the cv_robo module.

A third component (cv_robo) will communicate with cv_proc and work with the robot code to steer, drive, and otherwise operate the robot based on visual cues.  Cv_robo will act just like a human-operated joystick and move the robot and actuate the appropriate functions based on visual cues (just like a human operator would).  The actual robot driving logic lives here.

The overall system is designed to operate around a pair of JeVois smart cameras using both the CLI API to control and configure the cameras and the serial output providing object-based information.  Overall, the system is fairly independent of the visual processing strategy used.

## Architecture

Below is a diagram showing the relationship and connectivity of major parts of the vision-procesing system.

<INSERT DIAGRAM>

### Components

#### Sensor Servers

##### Camera Servers

##### Other Sensor Servers

#### Central Processor

#### Robot Driver

The Robot Driver (cv_robo) essentially emulates the actions of a human robot operator.  It queries the cv_proc module for appropriate sensor status to get information useful to detecting potential targets, aligning the robot to interact with them, and establish its orientation with the surrounding field of play.

This will probably be the most complex part of the vision-processing system as it is responsible for mapping this information into the actions a human operator would use to move and actuate the robot.  The programming team will need to work closely with the drive team in order to understand how to effectively move, steer, and otherwise operate the robot.

The interface to the RoboRIO will be through a serial port.  There will need to be some new code resident on the RoboRIO to interpret commands coming from the cv_robo module and interpret them just like human-generated joystick commands for operating the robot.  There are a few key aspects to consider here:

* The command handling module will need to be modified to accept direct commands from the cv_robo module as well as the current joystick interface

* The RoboRIO will need to know when autonomous mode is active or not at any given time

  * Autonomous Start:  Indicate to the cv_robo module that it should start controlling the robot.

    * Until this time, the vision-processing system can be running and actively scanning the robot's surroundings

    * Any potential or incorrect targets detected before the start of autonomous mode can either be explicitly purged from the vision-procssing system once the "Start of Autonomous Mode" signal is given to the cv_robo modules

    * Any potential or incorrect targets detected before the start of autonomous modeAutonomous Active:  Accept command from cv_robo only at this time and ignore other command inputs

  * Autonomous Inactive:  Accept only human-genersted inputs and ignore any comands from the cv_robo module
  
    * Whether or not the cv_robo module nees to be told that autonomous mode is no longer actice is something that can be decided later
    
    * It seems there would be no negative side effects if the vision-processing system continues to operate during the competition and logs objects it sees as well as other sensor inputs as these might be useful for later analysis

### Hardware platform and Environment

### Other ideas and considerations

#### Logging and Tracing

It will be important for each component in the vision-processing system to record information so that post-testing or post-competition, we can analyze how well it worked and potentially improve it for future runs.

This information could include:

* Configuration settings for each component

* Messages sent and received from other components of the vision-processing system

* Key logic decisions impacting the decisions and operations while in autonomous mode

All of this information would need to be timestamped so it can be later coorelated and analyzed to get a complete picture of the operaton of the vision-processing system.  Syncronization of timestamps between differnt components should not be a problem since all of the vision-procssing system will be runnign on a single hardware platform supplying the exact same timebase to all components.

The logging and tracing information should be stored as text rather than binary to allow a programmer to quickly be able to scan through and understand the output without much additional processing.  This doesn't mean that we might not want to create other tools to help us coorelate or otherwise visualize the vision-processing logs to better understand how it is operating.

#### Visual Tracing

In addition to logging vision-processing statuses, events, and activity in the log files, it might be useful to include a set of visible indicators on the robot in order to give resl-time information about what the vision-processing system is doing at that moment in time.  The Raspberry PI B+ platform provides 20+ GPIO pins (**G**eneral **P**urpose **I**nput/**O**utput) which could be used to drive a series of LEDs on the robot indicating information about what is happening.

Each major component could be associated with an appropriate set of LEDs that could indicate appropriate status information such as:

* That module has started and is operating

* 

* 

* 

*

#### Physical Recording

During testing and competition activities, it would be very useful to record the robot from several different locations to capture how it physically controls the robot.  If possible, this should be done in a manner that captures the robot in slow motion for more detailed analysis of the phyical movements of the robot as well as capturing any Visual Tracing indications.

## Key programming concepts to know to understand the code

For maximum portability and control, the bulk of the software is written in C.  There are several concepts that are important to know in order to understand and modify the code.

These include:

### #define

Understanding the #define directive is an important part in creating readable and maintainable code.  This is really part of a while family of directives including things like #ifdef, #ifndef, #else, #endif, and others.  Good use of these directives gives you the ability to change your code with a compile-time directive (like enabling specific deep debugging output or accommodating platforms differences through platform-specific code for example).

They also provide an effective way to define various constants used in your code in a good human-readable way so you'll very often see them used in that capacity.  For example, the cv_proc module is intended to support two (2) cameras and the code takes that into account.  It's obvious that there are some parts of the code that need to be aware of that fact.  You could just use the hard-coded value "2" in these places sort of like this:

```c
int placeToPutStuff[2];
```

In this case, it's hard to know why there this array has 2 elements.  Consider this as an alternative:

```c
#define NUM_OF_CAMERAS

int placeToPutStuff[NUM_OF_CAMERAS];
```

Without even seeing more than that, it's obvious that this is related to number of cameras defined.  Imagine wanting to change the code from supporting 2 cameras to 3 cameras; it's easy to update the NUM_OF_CAMERAS #define and change it from 2 to 3 and have a lot of data structures be updated accordingly.  Otherwise you're left trying to figure out what that "2" means; is it the number of cameras or just something else in the code that there happen to be two of?

In addition, you'll also see them used as a macro for some code that you don't want to put in a separate function but want to be expanded in-line right in the code without repeating it over and over.  With a #define, you can "define" it once and easily incorporate it where needed - the Camera List data structure lock and unlocking code is a good example of this in the cv_proc code.

Some links:

* [C/C++ Preprocessors](https://www.geeksforgeeks.org/cc-preprocessors/)
* [#define Directive (C/C++)](https://docs.microsoft.com/en-us/previous-versions/teas0593(v=vs.140))

As a point of information, the term "preprocessor" refers to the compiler pass that first handles all of the #defines and other #something directives before the code is compiled.  Historically this was done by a separate program called the, wait for it, the "preprocessor" though this behavior is built-into many modern compilers (but usually there is an option to see the "pre-processed" code where you can see the code with all of the preprocessor changes which is very handy for debugging more complicated macro definitions.

### Structures and Typedefs

Structures are a key way in C and C++ to group related variables together.  The combine multiple variables into a data entity that can be stored, passed around in the code, and manipulated in a way that maintains the relationships between the variables there in.

For example, a structure in the vision code could look like this:

```c
struct iSeeYou {
    char thingISee[100];   /  name of the thing I see
    int  x;                // x coordinate in the camera's view
    int  y;                // y coordinate in the camera's view
}
```

This creates a new data type which can be referenced just like a more basic data type - for example, we could declare an array those structures like this:

```c
struct iSeeYou thingsSeen[20];
```

There is a specific syntax to deal with these items - below is an example where we set the X coordinate of the 5th item to 12345:

```c
thingsSeen[4].x = 12345;	// remember, arrays start at 0!
```

Typedefs are a lot like structure definitions but they allow us to actually define a new data type - let's use the above example and create a new data type STUFF_SEEN:

```c
typedef struct {
    char thingISee[100];   /  name of the thing I see
    int  x;                // x coordinate in the camera's view
    int  y;                // y coordinate in the camera's view
} STUFF_SEEN;
```

This looks a lot like the original structure but we can use it much easier in the code - for example, here's a declaration of a variable of this new type:

```c
STUFF_SEEN	hey;
```

More details at:

* [Structures in C](https://www.geeksforgeeks.org/structures-c/)
* [Struct (C programming Language)](https://en.wikipedia.org/wiki/Struct_(C_programming_language)))
* [C - Structures](https://www.tutorialspoint.com/cprogramming/c_structures.htm)


### Pointers

Pointers are a way to efficiently access memory locations corresponding to variables and structures when using C.  Pointers allow efficient access by "pointing" to code to locations of variables and structures rather than passing entire structures around.  The use of pointers also supports more sophisticated and flexible data structures such as linked lists which in some usages are much more flexible and powerful than arrays (as one example).  Understanding the use of pointers and the processing associated with pointer arithmetic is a requirement for almost any non-trivial C programming effort.

Many system and library calls use pointers to pass or return structures so it is very important to have this understanding to make best use of the system call and utility library functions.

Here links to some information sources that can help explain pointers:

* [Introduction to pointers in C](http://www.circuitstoday.com/introduction-to-pointers-in-c)
* [Introduction to pointers in Ci (seems to be a popular title for this topic)](https://www.go4expert.com/articles/introduction-pointers-c-t27959/)
* [Introduction to pointers in C/C++ (video)](https://www.youtube.com/watch?v=h-HBipu_1P0)

### Linked Lists

Linked Lists are a key basic data structure.  Pointers are the easiest (not the only but by far the easiest) way to implement linked lists.  Linked lists provide a flexible mean to connect data records in a way where they can be added, deleted, or reordered without moving large amounts of data by simply changing a few pointers.

Here are a few resources to get you up to speed on linked lists:

* [C Linked List](https://www.zentut.com/c-tutorial/c-linked-list/)
* [Introduction to Linked Lists in C/C++](https://www.codeproject.com/articles/641175/%2fArticles%2f641175%2fAn-Introduction-to-Linked-Lists-in-C-Cplusplus)
* [Linked List Introduction](https://www.geeksforgeeks.org/linked-list-set-1-introduction/)

### System and Library Calls

These are the basic calls to get the operating system to do something for us - open a file, get us some more memory, kill something that's running, etc.  System calls are the most basic functionality the operating system (Linux, Windows, etc). can provide - you get the most control but at times they can be harder to use.  There are environments that have a very minimal (or maybe even no operating system) like an Arduino, a set of pretty basic routines (like the RoboRio) or a full tilt, crankin' it out operating system like Linux (a Raspberry Pi is a good example of that).

Library calls will look very similar and often times cause the operating system to do something as well.  Things that fall into that category are routines like stdio (C) or iostream (C++); these sorts of routines provide an easier to use or more flexible and hide some of the system call complexity (stdio and iostream are perfect examples of that).  They'll also provide functionality that many programs want to use like string manipulation that while they aren't things the operating system cares about ("I'm an 'OPERATING SYSTEM' friend - I don't do string comparison!").

There are a lot of function calls that fall into these areas - 100's at least (at one point I knew pretty much all of them but a lot of water has flowed under the bridge and a lot of routines have been added so that's probably not the case anymore).  So how do you get started?  I'd start by looking at the code and using one of the good "manpage" sites online ("manpage" is short for manual page so you won't sound like you're not hip to the groove that's being laid down).

A good site for Linux manpages is [Man7.org](http://man7.org/linux/man-pages/dir_all_alphabetic.html).

Manpages will have standard set of sections that will you understand what's happening - for system and library calls, you'll typically see:

* **Name**
* **Synopsis** (function name, parameters, and return types - if you know what you are doing, a lot of times this is all you need to jar your memory)
* **Description** (the will provide more information on what it does and what the parameters or options are if it's not totally obvious)
* **Return Value** (what you get back if it works as well as if it doesn't work)
* **See Also** (cross references to other related functions - the page to "open" something probably has a link to the functions to "close" it or otherwise manipulate it so if you just remember one name, you can get connected to the other complimentary functions just about every time)

You'll notice there are numbers involved (we never said there would be no math!); these numbers mean things.  The numbers 2 and 3 are probably the most interesting to software developers as those identify system calls and library functions respectively - these are functions your program can actually call.  Things with number 1 are commands - things you'd type at the Linux command prompt to do something other than calling a function in your program.

Here are a few examples:

* [string(3)](http://man7.org/linux/man-pages/man3/string.3.html) - various C string functions (there are quite a few)
* [cp(1)](http://man7.org/linux/man-pages/man1/cp.1.html) - command to copy files and directories; lots of options in the "Description" section

### Memory Management

Many programming languages relieve the programmer from having to manage memory - that is the need to get memory to put new data and objects into and the need to dispose of it when it's no longer needed (not disposing of unneeded memory is generally considered a bug; at best your application will just keep growing but at worst, you'll overwrite or discard the pointer needed to free memory and create what is considered a "memory leak" - these are not considered a feature).  The languages that do this automatically are typically referred to as supporting *automatic garbage collection*; Java is one of those, C isn't.

In C, you allocate memory using the [malloc](https://linux.die.net/man/3/malloc)() function to get more memory (think of it as shorthand for **m**emory **alloc**ation).  This function will make a certain amount of memory available to the program for use.  This memory can be used to store anything, any type of data, for as long as you need to store or (or at least until your program exits at which point any memory used by it it released so it's not lost *forever* ;).  Malloc() returns a pointer if successful; otherwise it returns 0 (or a NULL pointer in the parlance of C).  This is something your program wants to check for just to make sure something awful hasn't happened.  The vision processing system has it's malloc() call in utility.c and is wrapped by a function called cvAlloc().  If a memory allocation ever fails, it's likely that the situation not just for your program, but potentially for the whole system is, well, grim.  There isn't a lot you would typically do except for exit if that happens (if you really know what you're doing and think you're program can continue, you might want to include logic to try and continue but keep in mind that some other code might want to allocate memory including some library calls which might be much less tolerate of a malloc() failure).

Complimentary function to malloc is [free](https://linux.die.net/man/3/malloc)() which takes a pointer returned by malloc() and takes that memory "back" for re-use by the program potentially as the return from a future malloc() call.

*Important note*:  Malloc() returns a pointer - when you all free(), you want to pass that *exact* pointer value back.

Behind the scenes, there is a memory management routine running that allocates memory for your program in bigger chunks.  This is typically referred to as the *heap*.  The malloc() routine keeps a list of memory addresses it handed out and how big of an area that pointer was associated with.  If you look at the documentation for the calls to malloc(), you'll see it's pretty much an all or nothing interface as you either get a pointer back meaning you got what you asked for or you get back nothing.  Likewise, when you call free() with a pointer value you got from a malloc() call, you're also not passing the size of the amount of memory you're freeing, just a pointer to it.  The free routines looks into a list of pointers that were already handed out, which include how much memory that pointer was pointing to, and marks it internally as being available.  If you pass a different value, you really gum up the works.  In the pro world, we refer to that as *heap corruption* and done enough times (or honestly just once), it messes up the lists this library call uses to track, manage, and return memory to the point where it could lose track of some memory (creating a dreaded *leak*) or even worse, it could give out some memory *twice* causing some very, very, very hard to find find data corruption bugs (imagine you had two variables, **x** and **y** and somehow they ended up overlapping the memory used to store what you thought were two very simple, completely distinct variables and when you updated one, you subtly altered the value of the other in a program section far, far away.  That will be *very* tough to find.

So the pro tip of the day:  make sure you give back what you got, and only what you got, and only once (because free()ing something more than once will also lead to *heap corruption).

Some resources for learning more about C memory management:

* [C - Memory Management](https://www.tutorialspoint.com/cprogramming/c_memory_management.htm)
* [ Memory Management in C Programming](https://www.tutorialcup.com/cprogramming/memory-management.htm)
* [ C Memory Management](https://stackoverflow.com/questions/24891/c-memory-management)


This might all seem like a giant pain but it really isn't; you actually learn to better appreciate how memory is actually managed and used so even in environments with garbage collection, you'll create less garbage (the big downside of automatic garbage collection is that when the environment needs to do it, you're program essentially freezes - not good for situation where you are controlling something in real time; like say, a robot.  Stop processing events on or around your robot at seemingly random times for random amounts of time?  Hello Mr. Wall or have a nice broken part because you sent a command to move something and weren't able to read the limit switch to stop from breaking something on your robot.  Not good.

### Sockets and Networking

### Multi-threaded Programming

### Recursion

* [C - Recursion](https://www.tutorialspoint.com/cprogramming/c_recursion.htm)

### Locks and Mutexes

First off, you need to know how to pronounce "mutex" (syllable break between the "u" and the "t") - check out how "Julia" pronounces it at [Definitions.net](https://www.definitions.net/pronounce/mutex) - listen *ONLY* to Julia, everyone else is pronouncing it, well, *WRONG* (OK, "Oliver" and "Emily" are pretty solid too but skip the US English dudes for sure as they clearly are not programmers).

Mu-tex - OK, now we can move on.



## Conclusion
There is a lot going on in this design and corresponding code.  Depending on your level of experience with C and the Linux environment, this might be the most complicated code you've dealt with to date.  Don't hesitate to search for and read about some of the topics to learn more; also don't hesitate to ask me to help explain something as I'll be glad to do so.
