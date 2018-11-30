# Vision-related code for FRC #4241 vision efforts

## Overview
Architecture is a server (cv_cam) for each camera that watches the serial camera output and sends it via a UDP packet
to the processor (cv_proc).  Each cv_cam modules passes robot object and visual field location information to cv_proc.

The processor maintains lists per camera, per identified object with timestamps and coordinate-based values for each record.
A time-based sequence of camera records is maintained for each obect the cameras report.  A separate thread prunes the lists
based on an overall Time-To-Live (TTL) value to remove old records but still allow access to the latest information from
each camera as well as an averaged set of values based on the TTL.

Another thread processes the records per identified object (on request) and creates direction commands and calculates
distances to the identified object./  This thread will communicate with the cv_robo module.

A third component (cv_robo) will communicate with cv_proc and work with the robot code to steer, drive, and otherwise
operate the robot based on visual cues.  Cv_robo will act just liike a human-operated joystick and move the robot and
actuate the appropriate functions based on visual cues (just like a human operator would).  The actual robot driving logic
lives here.

The overall system is designed to operate around a pair of JeVois smart cameras using both the CLI API to control and
configure the cameras and the serial output providing object-based information.  Overall, the system is fairly independent
of the visual processing strategy used.

## Architecture


## Connectivity


## Key programming concepts to know to understand the code

For maximum portability and control, the bulk of the software is written in C.  There are several concepts that are important to know in order to understand and
modify the code.

These include:

### #define

Understanding the #define directive is an important part in creating readable and maintainable code.  This is really part of a while family of directives including things like #ifdef, #ifndef, #else, #endif, and others.  Good use of these directives gives you the ability to change your code with a compile-time directive (like enabling specific deep debugging output or accommodating platforms differences through platform-specific code for example).

They also provide an effective way to define various constants used in your code in a good human-readable way so you'll very often see them used in that capacity.  For example, the cv_proc module is intended to support two (2) cameras and the code takes that into account.  It's obvious that there are some parts of the code that need to be aware of that fact.  You could just use the hard-coded value "2" in these places sort of like this:

int placeToPutStuff[2];

In this case, it's hard to know why there this array has 2 elements.  Consider this as an alternative:

#define NUM_OF_CAMERAS

int placeToPutStuff[NUM_OF_CAMERAS];

Without even seeing more than that, it's obvious that this is related to number of cameras defined.  Imagine wanting to change the code from supporting 2 cameras to 3 cameras; it's easy to update the NUM_OF_CAMERAS #define and change it from 2 to 3 and have a lot of data structures be updated accordingly.  Otherwise you're left trying to figure out what that "2" means; is it the number of cameras or just something else in the code that there happen to be two of?

In addition, you'll also see them used as a macro for some code that you don't want to put in a separate function but want to be expanded in-line right in the code without repeating it over and over.  With a #define, you can "define" it once and easily incorporate it where needed - the Camera List data structure lock and unlocking code is a good example of this in the cv_proc code.

Some links:

* [C/C++ Preprocessors](https://www.geeksforgeeks.org/cc-preprocessors/)
* [#define Directive (C/C++)](#define Directive (C/C++))

As a point of information, the term "preprocessor" refers to the compiler pass that first handles all of the #defines and other #something directives before the code is compiled.  Historically this was done by a separate program called the, wait for it, the "preprocessor" though this behavior is built-into many modern compilers (but usually there is an option to see the "pre-processed" code where you can see the code with all of the preprocessor changes which is very handy for debugging more complicated macro definitions.

### Structures and Typedefs

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

### Memory Management

### Sockets and Networking

### Multi-threaded Programming

### Locks and Mutexes


## Conclusion
There is a lot going on in this design and corresponding code.  Depending on your level of experience with C and the Linux environment, this might be the most complicated code you've dealt with to date.  Don't hesitate to search for and read about some of the topics to learn more; also don't hesitate to ask me to help explain something as I'll be glad to do so.


- Chris Herzog

