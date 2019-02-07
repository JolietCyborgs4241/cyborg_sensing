import sys
import numpy as np
import cv2
import libjevois as jevois
import Reflectors as rf





# there need to be at least 2 command line parameters (the program name counts as one)
#
# the parameter is the name of the static image file used for the image processing

if (len(sys.argv)) < 2 or (sys.argv[1] == "-v" and len(sys.argv) < 3):
    print("Usage: " + sys.argv[0] + ": [image] | -v [video capture]")
    sys.exit(1)

if len(sys.argv) == 2:
    filename = sys.argv[1]
    mode = "image"
else:
    filename = sys.argv[2]
    mode = "video"


window_name = "JeVois Emulator"


# create the output object for the Jevois-targeted code and give it the
# name we'll use to reference the window to display any output it
jv_out = jevois.Jevois_out(window_name)


# create the named window
cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)


# create an instance of the class we plan to run on the Jevois camera
#
# there are two changes from the Jevois environment that need to be made:
#
# 1) at the start of the "process" method there is a line which takes the
#    raw camera output and creates an image.
#
#    this line should be commented out as we pass an image right into the
#    "process" method
#
#    comment out the line below (which is usually the first line in the
#    "process" method:
#
#               img = inframe.getCvBGR()
#
# 2) add a line initializing the "img" variable directly from the
#    "inframe" variable passed to the "process" method.  This variable
#    is used to start the visual process pipeline in the GRIP-generated
#    code and once it is set, the GRIP pipeline will be properly
#    initialized and will run unchanged
#
#    place this line directly below "img = inframe.getCvBGR()" which
#    was commented out above
#
#               img = inframe
#
# at this point, the entire Jevois-targeted code will run on your non-Jevois
# system
#
# any frame which would have been pushed out to the Jevois camera output
# via a call to outframe.sendCv() will be displayed in the
# "Jevois Emulator" window and refreshed with each call
#
# any output sent using the jevois.sendSerial() call will be printed to
# standard output 

reflector = rf.Reflectors()


if mode == "image":

    #read the image
    image = cv2.imread(sys.argv[1], cv2.IMREAD_UNCHANGED)

    reflector.process(image, jv_out)    # we only need to call it once

    cv2.waitKey(0)

else:   # video mode

    video = cv2.VideoCapture(sys.argv[2])

    count = 0

    ret = True

    while (ret == True):

        ret, frame = video.read()

        if ret != True:
            break;

        print(sys.argv[2] + ": frame " + str(count) + " read")

        count += 1

        reflector.process(frame, jv_out)


if mode == "video":

    video.release()


cv2.destroyAllWindows()

sys.exit(0)
