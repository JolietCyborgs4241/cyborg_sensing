# JeVois Python Emulator Environment

## What it does

The JeVois Emulator (*jvem*) provides a way to run python code targeted for a JeVois camefa on most any system which can run Python3 and has the approrpriate python modules such as *cv2* and *numpy* installed.

This provides a richer and more productive environment to develop and test this code while still making it easy to move to the JeVois for deployment.  With only a simple two line change, your code is ready to run off of the JeVois camera.

Hopefully you'll find this useful and a good way to increase your productivity.

## Adding your custom class to *jvem*

You will need to have your class in the same directory as **jvem.py** and import it into the jvem

              import your_class
              
Where appropriate in jvem.py, you'll need to instantiate your class.  The reference copy of **jvem.py** in the **cyborgs_sensing** GitHub repository includes **Reflectors** as the Jevois-targeted class to execute; change this as appropriate for your class.

## Adapting your custom code to *jvem*

There are two changes from the JeVois environment that need to be made to your custom class:

1) At the start of the "**process**" method in your custom class there is a line which takes the  raw camera output and creates an image object from it (actually a numpy array).  This line should be commented out as we pass an image directly into the "**process**" method from *jvem*.

Comment out the line below (which is usually the first line in the "**process**" method):

               # img = inframe.getCvBGR()

 2) Add a line initializing the "**img**" variable directly from the "**inframe**" variable passed to the "**process**" method.  This variable is used to start the visual process pipeline in the GRIP-generated and once it is set, the GRIP pipeline will be properly
initialized and will run unchanged.  Place this line directly below "**img =  inframe.getCvBGR()***" which was commented out above: 

               img = inframe

 At this point, the entire Jevois-targeted code will run on your non-Jevois system.

## Jevois-specific calls
 Any frame which would have been pushed out to the Jevois camera output via a call to **outframe.sendCv()** will be displayed in the window titled "Jevois Emulator"  and refreshed with each call.

 Any output sent using the **jevois.sendSerial()** call will be printed to
 standard output in the window the python interpreter is running.


## How to run it

There are three modes in which the *jvem* can be run:

1. using a static image file (your custom class is called once with the single image as input)

1. using a video capture file such as an MPEG (your customer class is called once per frame in the video capture file)

1. using a camera (a frame is captured from the camera and your class is called with this frame - once your class returns, a new frame is captured and the process repeats)


Each operational mode has a cooresponding command linbe option as shown below:

* python3 jvem.py -s static_image_file

* python3 jvem.py -v video_capture_file

* python3 jvem.py -c camera_id

If you are using a static image, the results fill be displayed (based on what your class passed to the **outframe.sendCv()** call and pressing any key will exit *jvem*.

If you are using a video capture file, the last frame will be displayed and held until you press any key which will exit *jvem*.

If you are using live camera capture, you will need to interrupt the python interpreter (typically using the ^C key) in the window in which it is running; this is the window you started *jvem* in and not the window displaying the video output.



