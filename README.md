# Research for FRC #4241 sensing and autonomous operation efforts

## Introduction

One of the key aspects to creating a solution for autonomous control (or for any sort of complex system really) is to develop a strategy to manage, coorelate, and analyze information from a number of different sources.

The goal here is to give the robot a sense of situational awareness potentially including:

* Location

* Distances from other objects

* Orientation to other objects and general playfield

With this information, the robot has the potential to be able to perform non-trivial high-level tasks without human intervention.  This is a continous process sometimes refered to as an "*OODA loop*":

* **O** - Observe (what is around the robot?)

* **O** - Orient (how is the robot oriented in regards to its observed surroundings?)

* **D** - Decide (what should the robot do?  As in *now*; turn, move, lift, drop, shoot, etc.?  This probably is and should be part of a bigger goal.)

* **A** - Act (how should the robot instantiate its decision?)

* Repeat...

Each of these is a discrete step in the autononmous process and is repeated over and over with updated inputs and goals as appropriate.   This is not an easy to reach goal.

One of the first challenges is the collection and management of this informaton which may be coming from a significant number of individual sensors and other data sources.  Maintaining the timeliness of this information is key - much of the robots actions will be driven by very near term observations of its surroundings, likely limited to a few seconds of history (at most, maybe even less).  To this end, the core of the autonomous system is the database of sensed information and providing a set of ways to analyze, normalize (if needed), and make this raw or conditioned information available to the robot control logic (which could reside on or off the RoboRio).

This a longer term and more comprehensive approach - it might yield payoffs in the short term (I feel we can use the information even for a simple robot control scenario) but it also provides an architecture for a longer term and more sophisticated method of awareness and subsequent control.

The architecture is designed around a series of functional blocks which should give the ability to modify, replace, and re-work substantial components with minimal impacts on other parts of the system.  Each component can be thought of as having a specific responsibility with connections to and from other components over which data and commands move between functional blocks.  Looking at the system in that way can make it easier to understand the overall approach without having to dig into the details of implementation of any individual component.

**Note:**  This is an evolving document and is trying to be maintained as the thinking about this approach is evolving as well.  You might see repetition or even contradictions; excuse the first but please point out the second.

## Overview
Architecture is a set of servers collecting information (such as cv_cam for each camera that watches the serial camera output) and sending the sensor data via a UDP packet to the centralized database (cv_db).  In the case of a camera, each cv_cam modules passes robot object and visual field location information to cv_db as well as some identifying information tying this visual information to a specific camera.  Similar processing happens for other sensors (like ultrasonic rangers for example).

The database maintains lists per sensor with some extra structure for cameras (such as names of identified object with visual coordinate-based values for each record)  Each record is timestamped to sub-millisecond precision so we know precisely when it arrived and was stored.  In the case of cameras, this creates a time-based sequence of camera records maintained for each object the cameras report; other sensors are logged in the same way.  A separate thread prunes the lists based on an overall Time-To-Live (TTL) value to remove old records but still allow access to the latest information from each camera (and potentially other sensors) as well as an averaged set of values based on the TTL.

Another thread responds to querys about the data base to provide up-to-date information about the spatial environment around the robot.  Queries could include:

* Does any camera see the "ball" object?

* How far away from an obstacle is the front sensor of the robot?

 This thread will typically communicate with the cv_robo module.

A third component (cv_robo) will communicate with cv_db and work with the robot code to steer, drive, and otherwise operate the robot based on visual cues.  Cv_robo will act just like a human-operated joystick and move the robot and actuate the appropriate functions based on visual cues (just like a human operator would).  The actual robot driving logic lives here.

The overall system is designed to operate around at least a pair of JeVois smart cameras using both the CLI API to control and configure the cameras and the serial output providing object-based information.  Overall, the system is fairly independent of the visual processing strategy used so we can delay choosing a visual processing algorithm until we understand the challenge and can even it change it while wr're working on the roboto or even between events or runs if we are feeling especially bold and confident).  There may be other sensors use to augment the robot's perception of its environment and its orientation to the field of play and the contents thereof.   

## Architecture

Below is a diagram showing the relationship and connectivity of major parts of the vision-processing system.  Keep in mind that this can grow to be more than just vision; the goal is autonomous operation of the robot and while it's believed that vision is going to play a key role in this, it's also very likely that the robot is going to end up growing some sort of "sensor package" to help it get the job done.
 
 ![Cyborg-vision Architectural Diagram](https://github.com/cgzog/cyborg_vision/blob/master/cv_arch_diagram.png "Cyborg-vision Architectural Diagram")

### Components

#### Device Servers

These are small, special purpose processes that in general watch a sensor, get it's current readings (either on a regular interval or whenever the sensor sends it out depending on the behavior of the sensor).

They take these values, add some identification information, and send it to the CV Database for logging.

##### Camera Servers

##### Other Sensor Servers

The design does not preclude having more sensor servers which can monitor individual or sets of sensors and pass that information to cv_db.  This could include different sensor types such as ultrasonic, light or others sensor types.

Each sensor type can have its value stored in same time-tracked manner as the cameras making the most recent values available to the Robot Driver module for consideration in cotrolling the robot.  The format of the data from each sensor type can be defined at the time that sensor is incorporated taking into account the type and richness of the information available from that sensor type.  Each data record will need to include the appropriate sensor identification information so that each specific sensor instance can be mapped into the physical space surrounding the robot.

The diagram includes a 9-way combination sensor including:

* 3-axis accelerometer

* 3-axis magnetometer

* 3 axis rotation

This information will be logged in the same manner as any other sensor with the hope of being able to recover actual robot movments and forces post-practive and post-competition.

It may also send measurement to cv_db to make them available to the robot controlling logic.

#### Database

The Cyborg-Vision Database (cv_db) gets readings from various sensor servers and saves their values.  In addition to saving the values, it also regularly "prunes" the database to remove older values (typically referred to as a TTL, or **T**ime-**t**o-**L**ive).  Whenever it finds values older than the TTL limit, it removes these from the database.  Think of it terms of keeping only the most recent sensor history; who cares what a camera saw 30 seconds ago?  The robot has probably moved, turned, or something else has changed; we mostly want to know what's happening right now (or at least within only the last few seconds).  This database is essentially an enhanced "Last Value" cache ("Last Few Values" essentially) and will be retained fully in memory (so it will be lost when the robot powers down).  It will however log all incoming data messages and queries for later reference.

Processes that want to find out about the conditions around the robot (whether visual, ranging, etc.) can query cv_db and get the status of a specific sensor, a set of sensors, or potentially even an average sensor reading across the values available.

Here is a high level diagram showing how the various structures that correspond to the different sensors are stored and how the data is related to it's associated sensor (which generally provides the context for the data in terms of where this data applies in terms of it's relatonship with the robot:

 ![Cyborg-vision Sensor Data Storage Diagram](https://github.com/cgzog/cyborg_vision/blob/master/sensor_db_structure.png "Cyborg-vision Sensor Data Storage Diagram")

#### Robot Driver

The Robot Driver (cv_robo) essentially emulates the actions of a human robot operator.  It queries the cv_proc module for appropriate sensor status to get information useful to detecting potential targets, aligning the robot to interact with them, and establish its orientation with the surrounding field of play.

This will probably be the most complex part of the vision-processing system as it is responsible for mapping this information into the actions a human operator would use to move and actuate the robot.  The programming team will need to work closely with the drive team in order to understand how to effectively move, steer, and otherwise operate the robot.

The interface to the RoboRIO will be through a serial port.  There will need to be some new code resident on the RoboRIO to interpret commands coming from the cv_robo module and interpret them just like human-generated joystick commands for operating the robot.  There are a few key aspects to consider here:

* The command handling module will need to be modified to accept direct commands from the cv_robo module as well as the current joystick interface

* The RoboRIO will need to know when autonomous mode is active or not at any given time

  * Autonomous Start:  Indicate to the cv_robo module that it should start controlling the robot.

    * Until this time, the vision-processing system can be running and actively scanning the robot's surroundings

    * Any potential or incorrect targets detected before the start of autonomous mode can either be explicitly purged from the vision-processing system once the "Start of Autonomous Mode" signal is given to the cv_robo modules

    * Any potential or incorrect targets detected before the start of autonomous modeAutonomous Active:  Accept command from cv_robo only at this time and ignore other command inputs

  * Autonomous Inactive:  Accept only human-generated inputs and ignore any commands from the cv_robo module
  
    * Whether or not the cv_robo module needs to be told that autonomous mode is no longer active is something that can be decided later
    
    * It seems there would be no negative side effects if the vision-processing system continues to operate during the competition and logs objects it sees as well as other sensor inputs as these might be useful for later analysis

    * One big advantage for keeping the autonomous system active is the ability to provide what could be considered "augmented control" - the ability to have some smaller sets of operations happen autonomously even during human-controlled operation.  As an example, there could be an operational mode where the robot advances on a target object or location until it is some sensed distance away from it, and then automatically perform some operation (like drop or pick up something).  The advantage for a capability like this is it could make some potential operations more reliable by eliminating the need to specific robot positioning or interactions by having the robot itself handle the final, close-in details of some interaction.  Another example could be align the robot with a target, get a distance measurement, and then adjust it's operation with minima human operation (shooting a ball towards a target is an example of this; the robot can be coarsely aligned by the human operation who can initiate a series of operations where the robot does a more fine grained alignment and adjusts the power of it's action in order to accomodate the distance to the target.

##### Robot Driver Fallback Approach

If we can't get the logic to control the robot to the level we like in the Robot Driver module, we can fallback to making the sensor data available directly to the Robo Rio.  In this case, the autonomous system is reduced to a sensor database that provides sets of sensor data to the main robot controller.

#### Visual Status Display

This module, cv_status, drives a series of human-visible LEDs on the robot that display the status of different components of the visual-processing system.  This will provide a way for the components in the autonomous system to report various status and states via RGB LEDs driven by the cv_status server (and supporting hardware driver) providing immediate visual feedback on the operation of the autonomous system.

This can provide visual feedback to an observer as to the status and operation of the visual-processing system.  All of the different components can send messages to cv_status to control their respective indicators.

##### Visual Status Display Information

To display different types of status indications, we have two+ basic tools at our disposal:

* Whether an LED is on or off

* What color it is if it is on

* What brightness it is when it is on (I consider this the "+" part; it can be very hard to distinguish brightness levels via the human eye to a point where an observer could clearly differentiate brightness of an LED under a range of differnt or even varying lighting conditions so let's leave this one off the table and stick with on/off and color for now)

In regards to types of status, there are two types of status that are most valuable to display

* Is something running?

* If it's running, what state is it in?

In addition to the status, we want to be able to distinguish the status for sifferent system components in a clear way; using dedicated LED(s) for specific system components would help there.  Consider something like:

* First LED on the left is the interface to the RoboRio

* First LED on the right is the sensor database

* The next two LEDs in from the right are the camera servers

* and so on...

If something important enough to warrant displaying on the status bar isn't running, we probably want to know, in a very clear way.  We want to know if something is expected to be running is actually running and even more importantly, we want to know if something that is expected to be running **ISN'T** running.

So how do we do that?  How do we have something that *isn't* running *tell* us that it *isn't* running.  Well, we don't do that directly (because we can't).  What we can do is use a concept called a *timeout*.  A *timeout* is a counter that runs down and if the counter runs out, something happens.  Sometimes this running down is deliberate (like an alarm) but sometimes it's intended to b a fail safe part of the system design.  Some types of train locomotives have a periodic alarm that goes off which requires the locomotive engineer to hit a button to show they are still alert; together with the alarm is a timeout which if it expires without the engineer hitting the button, the train's brakes are automatically applied.  Hitting the button resets the timeout and prevents its ultimate action from occurring.

We can do the same with the visual status display.  In this case, we'll light all of the LEDs red unless we get an explicit update from the corresponding part of the system to change it to another color.  This will start all the LEDs as red until something happens to change them (we'll discuss how things get changed below).  So what about the *timeout* above?  That is an important part of the entire status display because we'll use a timeout to set a LED back to red to indicate that we've not received a status update from some component within an allotted timeout interval since the last update - we can use a simple value like 1 second provided that we think every component in the system will communicate with the status display server at least that often.  I would estimate that the system would be procesing dozens of sensor readings each second and if each sensor reading updated a corresponding status LED, each system component would be talking to the status display server much more frequently than that.  In this case, if we haven't heard from some system component in 1 second, it's probably dead and no longer working.

We can and will make this configurable so that we specify a reasonable value which doesn't cause too many false alarms.

##### LED Hardware

We can use a strip of [WS2811](https://randomnerdtutorials.com/guide-for-ws2812b-addressable-rgb-led-strip-with-arduino/) RGB LEDs.  These LEDs can display a full mix of RGB colors (Red-Green-Blue) at different intensities using only three wires: power (+5vdc), ground, and one signal wire.  There are a number of good libraries like [FastLED](https://github.com/FastLED) that will let us drive it from an Arduino.  We can drive an arbitrarily long strip of LEDs so long as we have enough power for each LED (about 50mA each at full brightness so a strip of 8 will draw about 400mA max).

For now, let's consider 8 LEDs and see how well it maps to what we want to display.

##### LED Status Encoding

Here are some proposed LED status indications that will help us determine the number of LEDs we might want and what they might indicate

* LED - Sensor Database up and running
    * Green - running

* LED - Sensor Database Status
    * Yellow - sensor data being received
    * Blue - sensor data being queried
    * Green - mix of both statuses
    
* LED - RoboRio Interface up and running
    * Green - running
    
* LED - RoboRio Interface to Sensor Database communications
    * Yellow - sensor data being received
    * Blue - sensor data being queried
    * Green - mix of both statuses

* LED - RoboRio Interface to RoboRio communications
    * Yellow - commands being sent to RoboRio
    * Blue - responses from RoboRio
    * Green - mix of both statuses
    
* LED - Main Sensor Server Status
    * Yellow - sensor data being received
    * Blue - sensor data being sent to database
    * Green - mix of both statuses

* LED - Camera Sensor Server Status
    * Yellow - camera data being received
    * Blue - camera data being sent to database
    * Green - mix of both statuses
    
* LED - Autonomous system active
    * Green - autonomous system in control of robot
    * Off - not lit when not in autonomous mode
    
This covers 8 LEDs.  We could stop at that or potentially include a larger number, like 10 LEDs into the robot status display for future expansion (**NEWS FLASH**:  the hardware is going to have 10 LEDs on it and the communications protocol between the Raspberry Pi and the LED driver module will accomodate 16 LEDs).

Suggestions here are welcome.

### Hardware platform and Environment

### Other ideas and considerations

#### Logging and Tracing

It will be important for each component in the vision-processing system to record information so that post-testing or post-competition, we can analyze how well it worked and potentially improve it for future runs.

This information could include:

* Configuration settings for each component

* Messages sent and received from other components of the vision-processing system

* Key logic decisions impacting the decisions and operations while in autonomous mode

All of this information would need to be timestamped so it can be later correlated and analyzed to get a complete picture of the operation of the vision-processing system.  Syncronization of timestamps between different components should not be a problem since all of the vision-processing system will be running on a single hardware platform supplying the exact same timebase to all components.

The logging and tracing information should be stored as text rather than binary to allow a programmer to quickly be able to scan through and understand the output without much additional processing.  This doesn't mean that we might not want to create other tools to help us correlate or otherwise visualize the vision-processing logs to better understand how it is operating.

There are a few key considerations to make this work well:

* We need make sure that the log survives the robot powering off at the end of the competition (unless we can maintain power to the hardware supporting the log-supporting device from before the practice or competiion starts until we access the log)

* If we will lose power, we need to ensure that the log records are not damaged by the log-supporting device losing power.  We can take some steps to try and minimize the chances of this and try to make things record log information as synchronously as we can.

We need to extend this to the Robo Rio-based control code as well.  If possible, we should timestamp and log each command input from the human operators for later review and use as test data for a further robot testing and debugging.

##### Logging Formats

The most obvious information we want to log is the the sensor data.  We could probably get away with just logging it at the central database but we should take a strong look at whether we can afford to it at the individual sensor level (or more specifically the server for that sensor on the Raspberry Pi).  If we can timestamp the data when it leaves a sensor processor as well as when it arrives at the database, we'll not only have two copies of the data but we'll also have the information that tells us the latency of the sensor data as it moves through the system.  Latency is the time difference from between the information being sent and the time it is received; as an example from the physical world, the latency of First Class mail is usually 2 days from when it arrives at the sending Post Office to the destination within the continental United States.  With timestamps at both ends of the data write to the database from the sensor server, we'll have the ability to figure out how long it takes sensor data to move through our system and make adjustments, if we feel we need to.

Different parts of the system might log their information a litle differently; for instance, we might want to log sensor data in a way that saves it in the same sort of form that a sensor generates it in.  Other aspects of the system might log their information differently such as logging any queries made to the database and what the results are (which potentially might involve several records being returned).  The key is that the information is as "raw" or "native" as practical so we diagnose whether there are issues with the sensor or otehr data staring right from the sensor or coming into play through other, more subsequent, processing steps.

Each component should log it's information to a separate file and should include enough information in each record so that we identify the component that wrote it and when that specific record was generated.

Here is a potential log record format as well as an example record of sensor data received by the central database:

name: ts id direction data

xyz_server: 1544748781.810069 db i "C ball 3 100 250 75 75"

This could be decoded like this:

* "name" - name of the executable that generated this log record; argv[0] in C/C++ terms

* "ts" - time stamp in seconds and microseconds from the epoch (epoch in terms of Lunix is seconds since midnight, Jan 1, 1970, UTC - the timestamp shown in the example above is the actual system time at the time of writing this line).  It's important that the microsecond component get printed out with 6 digits including leading 0s and trailing 0s as appropriate; there is a huge difference between "123.456" and "123.000456" for example and we need the correct representation

* "db" - produced by the central sensor database (each component has some sort of identifying code so that if we merge all of the log records, we can still identify the originating component)

* "direction" - this could indicate "i" for incoming (being recieved) or "o" for outgoing (being sent to something else) records

* "data" - this is dependent on the component, sensor, or other aspect of the data and will likely vary depending on the type of data being logged.  What is important is that we can idetify the type for later analysis whether implicilty in the data itself (like this example shows it is a "camera" record) or explicitly where needed (by adding something in addition to the data).

  * "C" - camera record

  * "ball" - sensor ID

  * "3" - sensor subID

  * "100 250 75 75" - "ball" object coordinates and bounding box (this part of the record is specific to the type of sensor being reported and might include more or less data depending on the sensor)

Currently defined sensor record types are:

* **C** - "camera" sensor - C ID subID X Y W H

* **R** - "ranger" sensor - R ID subID R

* **G** - 3-axis Accelerometer sensor - G ID subID x y z

* **O** - 3-axis Roll sensor - O ID subID x y z

* **M** - 3-axis Magnetic orientation sensor - M ID subID x y z

It's important to note that the sensor record is all about the *type* of the sensor and not the technology involved in sensing that, well, sense of the robot.  We could use an electronic roll sensor just as well as we could strap a freshman into the robot so long as they report "180! 180! 180!" when the robot rolls upside down.  My preference would be for the electronic sensor but we do have a budget to work within and underclassman may be the better economic choice for some specific sensing scenarios.  More realistically, we could try different ranging technologies or implementations such as sonar or LIDAR and swap a sensor based on one of those technologied for another one without any changes to the code so long as they return the same style record for their data.  So long as they return the same record type, none of the software needs to be changed or even cares about the sensor swap.

#### Visual Tracing

In addition to logging vision-processing statuses, events, and activity in the log files, it might be useful to include a set of visible indicators on the robot in order to give real-time information about what the vision-processing system is doing at that moment in time.  The Raspberry PI B+ platform provides 20+ GPIO pins (**G**eneral **P**urpose **I**nput/**O**utput) which could be used to drive a series of LEDs on the robot indicating information about what is happening.

Each major component could be associated with an appropriate set of LEDs that could indicate appropriate status information such as:

* That module has started and is operating

* Specific processing indications like certain read, write, or computational events

* Heartbeats indicating some component is still running.

#### Physical Recording

During testing and competition activities, it would be very useful to record the robot from several different locations to capture how it physically controls the robot.  If possible, this should be done in a manner that captures the robot in slow motion for more detailed analysis of the physical movements of the robot as well as capturing any Visual Tracing indications.

#### Post-practice and post-competition procedures

We should include as part of the post practice or competition checklist the task of downloading, saving, and uploading the logs.  This will make sure that we have them available and saved and also allow a remote team member to review and work with the on-site team members to potentially help understand a problem or other anomoly.

This probably should be one of the very first things we do even before fixing a broken or damaged robot; it will only take a few moments (we'll need to work out our procedures to they'll be fast, safe for the logged data, and well documented).

The example is a Formula One racing team; data is one of the most valuable to things to get from the car each time it goes out and gets taken care of before anyone even starts wrenching on it.

#### General Autonomous Hardware Philosophy

There will be a lot of moving parts here - software and hardware.  To give us the ability to have reasonable spares on hand and the ability to put those spares into play quickly and reliably, we need to take a little care as to how the system gets configured and connected.  Ideally we will want a system implementation that will allow us to setup and validate major devices back at the shop and be able to replace a failing component with no or minimal (and super clear and super documented) configuration.

If it takes a bunch of software programming or at worst, actual changes to the code for something, in order to get it configured, the likelyhood of an error or mistake under pressure goes *way* up.  Once we validate something, we need to be able to bag it and stash it as a spare without messing around with it any more than we absolutely have to.

That said, we'll want to make sure that we bring the right equipment to be able to maintain and work on the autonomous components if needed during a competition.  In those sorts of high-pressure situations, we will need to have well documented and practiced procedures to make sure we stay in control, don't make things worse through not knowing what code something is running or what it's configuration should be, don't lose changes or improvements, and similar considerations.

**At the end of the day, it's not just what you do but how you do it so that you can do it again - reliably and repeatably!**

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

So the pro tip of the day:  make sure you give back what you got, and only what you got, and only once (because free()ing something more than once will also lead to *heap corruption* - whihc is not good and will usually result in the program crashing or corrupting data, usually in very subtle, very non-obvious, and very hard to find ways).

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
