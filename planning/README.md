# FRC #4241 Software Planning Proposal

Original draft by: Chris Herzog

## Overview & Goals

This document is intended to be a first pass project plan for the software effort for the #4241 team.

The goal here is to put together a project breakdown to help divide the software activities into some parallel efforts.

The basic schedule is based on the milestones developed by Mr. Farias:

* Strategize - by first part of the first week
    * Mechanism and Chassis decisions based on this effort
* Prototype designs - by end of 2nd week
* Competition manufacturing and assembly - by end of 4th week
* Robot Testing and Debugging - by end of 5th week
* Final full-up Robot Testing and Adjustment - by end of 6th week

Once the "competition robot" has been completed, additional testing and adjustment will continue using the non-competition clone robot.

## Challenges

As everyone is aware, the time is short and the deadlines immovable - the  competitions will happen whether we are ready or not so we'd best be ready.

We potentially have a significant-sized software team; the challenge will be to manage this team so that people are doing productive work - for this we need:

* Clear assignments
    * What needs to be done
    * When it needs to be done by
    * How we can tell if we're done with some effort
* Good communications
    * Whose working on what
    * How they are doing
    * Helping as appropriate


## Task Breakdown

### Basic Robot Control

This is an area we should have some experience in.  While I have not focused heavily in this area, we can likely do things here in a clear step-by-step way:

* Establish basic communications to the robot
* Establish communications back to the dashboard
* Basic control handling (it's enough at this stage to just make sure that we can handle all of the various control inputs))
    * Can we make the robot easier to control or more reliable through software?
        * Implement traction control or acceleration curves to make the robot respond more consistently and predictably and be a more stable and controllable platform (as one example)?
* Map the control inputs to various robot actions
    * We can simulate various actions and systems to get some early feedback on the control software
    
The key to any successful software effort is incremental change - change only a few things at a time.  This minimizes risk and keeps things from getting out of hand.  It's stressful and hard to debug a thousand lines of new code; it's a lot easier to debug just those last fifty lines you added.  Repeat that a few times and you're in great shape without having lost your mind.

### Autonomous Control

This is a somewhat new, or at least different from in the past, effort.  We're trying to integrate cameras and other sensors into a comprehensive system to make the robot aware of its surroundings and have the ability to interact with them in a meaningful (and hopefully massive point scoring) way.

Right now, my thoughts are to implement the robot autonomous control on a separate processing system based on something like a Raspberry Pi.  This will let us develop this functionality independently, and in parallel with the more mainline robot control software

Things we need to do:

* Understand the parameters of the competition and our robots capabilities (this comes a lot from the Strategy and Prototyping efforts)
* Research on how we can best use vision
    * What vision processing module works best?
        * Maybe even more than one?
    * Do we need to tune or configure it for best utilization?
* What other sensors are important?
* How do we map these sensors into robot control algorithms?
    * What do we care about?
    * What sensors are important?
    * What decisions will we make based on those sensors?
    * How will that information map into actual robot control commands?
* Tie the autonomous system into the mainline robot control
   * Enter and leave autonomous mode
   * Is there anything we can do with the autonomous system to help during human-controlled operation?
       * What if we had the ability for a human controller to get a robot close to or roughly aligned with something and then had the autonomous system do the final robot positioning and actuation
       * This could eliminate the need to try to precisely align the robot or perform some precision placement all the way across the competition field where it's hard to see or visualize exactly where the robot is or where it's positioned relative to other items on the competition field
   * Build the additional sensors we need
        * I designed and had fabricated the Printed Circuit Boards (PCBs) for several new sensors and devices
        * I build singular prototypes to check out the circuit boards
        * I'd like folks to take on the build and testing of the remaindder of the sensors w need - enough to equip a main robot, a test robot, and have some loose sensors as spares and for other development needs
        * Design and print enclosures for the sensors
               - They use a common PCB layout for shared components and connections
               - We need to design the enclosures
               - Check them out to make sure they fit
               - Produce enough to package all of the sensors needed to equip the robots and have some spares

##Dependencies and Prerequisites

The software team is going to need a few things to work well:

Basic robot control:

* A chassis that is the same as what we'll be using (this comes out of the Strategy efforts)
    * Early on while we're setting basic communications and control, almost any chassis will be suitable
* Information about various mechanisms and manipulators the robots is going to have (this comes out of the Strategy and Prototyping efforts)
    * Even if the robot isn't done, we should aim to start developing and testing code as early as possible
    * We can build simulators for what the robot will have so that we can start and validating writing code
        * We'll have a motor that activates X by running one direction or the other
        * We'll have a limit switch Y that will indicate that we've reached the end of travel in some direction
        * We'll have sensor Z that will indicate that the shooter is loaded (or whatever)
        * The intent is that once we know, we can almost certainly cobble together something that we could manually mess around with to test our software early (which is key; we'll almost certainly have to make some adjustments once we're running on th real robot but we can eliminate a lot of "stupid" mistakes even before we see the real robot)
    * We can create control code to have the drive team be able to "drive" the robot in some manner very early and get feedback for both the mechanical and software implementations
    
Autonomous control:
 
 * Information on the chassis
 * Information on the robot systems
 * Information on the playfield layout, objects, targets, and conditions
 * Determination of our basic strategy and planned capabilities
 * Decisions on the sensor package the robot needs (this will come largely from the Strategy and Prototyping efforts)
 * Feedback towards the Design and Prototyping efforts; "if we had this, it would be easier to do that" or "it would be easier if we included the capability to do this" for example

## How we do things downtown

"How we do things downtown".

This has nothing to do with downtown as a place but refers to how we do things "for real".  In the future, you want to do something that you'll get paid for; hopefully paid for very well.  Being able to organize and plan things is the way you progress from being someone that contributes to an effort to someone that leads the effort.  Someone that makes a difference.  Someone that other people will say "we couldn't have done it without them" sort of statements about.

*That's where you want to be*.

The funny thing is that it isn't super hard.  You don't need to have a Harvard MBA or something like that but you need to pay attention to details.  You need to have a plan.  You need to work your plan.  Please be aware that "working your plan" doesn't mean having a plan and slavishly following it straight to disaster.  It means having a plan, using it as a tool to check your progress and validate your ideas, and here's the cool part, adjusting it to reflect reality as you go along.

Monkeys can follow rudimentary instructions but won't win a FIRST competition.  Smart people learn, adjust, change, evolve, and repeat this process to create the basis for better outcomes.

Set a goal, get moving, adjust as you go along to reflect reality; that's "how we do things downtown".

### Communications

This is going to be key.

Speaking for myself, I can only be there one or two days a week and on weekends.  We should make use of Discord to share information, ask and answer questions, and generally share what's happening across the team.  If needed, we can create additional channels to split things into more focused discussions.

#### Time is ever the tyrant*

As you may have noticed, the most precious and most limited resource we have is time; whether it's some individual team members time or the relentlessly advancing calendar.

Staying touch and up to date so we can help each other make the most of this fleeting resource is one of the best things we can do. 

*I thought Shakespeare said that but I can't find any evidence to support that.  Maybe I made it up myself - cool.

### Source Code Control

We're going to have multiple people working in multiple areas - all at the same time.  Having a mechanism to handle this and stop us from stepping on each others various body parts is important.  I've put the work I've done to date in regards to vision and autonomy under GitHub in this repository; I'd like to suggest we do the same for all our digital efforts.

Even beyond that, I'd like to strongly, as in **VERY STRONGLY**,  suggest we do that for all digital assets (nd if things aren't digital, let's look at making them that way so we easily archive them in that manner).

This includes:

* Software source code (this is the no brainer)
* Schematics and wiring diagrams
    * We could use an actual schematic capture program to be super cool
    * or we could just draw it up and take a picture
        * The value is in documenting, capturing, and archiving this information
* CAD files
    * Robot parts
    * 3D printed parts
* Any sorts of specialize information which might include:
    * Assembly instructions
    * Robot tuning information
    * Driving and operating instructions
    * Packing lists
    *Pre and post-competition checklists
* **Anything we want to capture so we can remember it, improve it, share it, or build off of in the future**

* **You might think you're a genius (and maybe you are); *prove* it by sharing your incredible insight with the next iteration of the team to make *them* better **

Every team leader should be making sure their sub team is capturing their information for future teams - this is how we do things downtown - this is how we improve ourselves.

#### Repositories

Having someone in charge of the various repositories is a good idea.  Rather than having everyone just change repositories in a willy-nilly fashion, I'll suggest that each team or sub-team has a person who can handle "pull" requests (change request to a GitHub repository) and do a quick review and accept them into the repository (it's a very easy to do thing)

### Code Reviews

From the software perspective, having other people look at and review your code is a good idea.  In fact, in my professional life, I require it (and this is for a team of the smartest people I've ever worked with).  Having other people review your code before putting it into the robot in an official way is an easy way to spot obvious errors you might have missed, get feedback that might improve your code, and in general, catch *oops* sorts of mistakes as soon as possible.

It can be done using actual software tools that force you to go through a review process (probably overkill) or it could just be as simple as a comment in your pull request ("Reviewed it with Christian and Ben").

Boom - done.

In general, having a review process for anything that changes is a good way to double check peoples work and even to share information and knowledge so other team members have the opportunity to grow.

