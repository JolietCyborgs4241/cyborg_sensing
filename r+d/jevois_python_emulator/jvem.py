import sys
import numpy as np
import cv2
import libjevois as jevois
import Reflectors as rf



def usage():
    print("Usage: " + sys.argv[0] + ": [-h] | [-s static_image_file] | [-v video_capture_file] | [-c camera_id]")


# initialize and document a few key variables

MODE_STATIC_FILE = "static"
MODE_VIDEO_FILE  = "video"
MODE_CAMERA      = "camera"

WINDOW_NAME = "JeVois Emulator"

mode         = ""    # will be either "static", "video", or "camera"

static_file  = ""    # static image source file

video_source = ""    # video source - file or camera


# check and setup operational parameters off of command line
#
# I swear, one more command line option and we'll move to getopt

if len(sys.argv) == 2 and sys.argv[1] == "-h":

    usage()
    sys.exit(0)     # exit with 0 since -h is a valid option


if sys.argv[1] == "-s" and len(sys.argv) == 3:

    mode = MODE_STATIC_FILE
    static_file = sys.argv[2]

elif sys.argv[1] == "-v" and len(sys.argv) == 3:

    mode = MODE_VIDEO_FILE
    video_source = sys.argv[2]

elif sys.argv[1] == "-c" and len(sys.argv) == 3:

    mode = MODE_CAMERA
    video_source = int(sys.argv[2])

else:   # unknown command line option!!

    print("Error: " + sys.argv[0] + ": invalid or missing command line parameters - use -h for help")
    usage()
    sys.exit(1)


print("mode = '" + mode + "'")

# create the output object for the Jevois-targeted code and give it the
# name we'll use to reference the window to display any output it

jv_out = jevois.Jevois_out(WINDOW_NAME)


# create the named window

cv2.namedWindow(WINDOW_NAME, cv2.WINDOW_NORMAL)


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


if mode == MODE_STATIC_FILE:    # static file processing

    #read the image
    image = cv2.imread(static_file, cv2.IMREAD_UNCHANGED)

    reflector.process(image, jv_out)    # we only need to call it once

    cv2.waitKey(0)

elif mode == MODE_VIDEO_FILE or mode == MODE_CAMERA:   # video file processing

    video = cv2.VideoCapture(video_source)

    count = 0

    ret = True

    while (ret == True):

        ret, frame = video.read()

        if ret != True:
            break;

        print(sys.argv[2] + ": frame " + str(count) + " read")

        count += 1

        reflector.process(frame, jv_out)

else:   # what mode is this???

    print(sys.argv[0] + ": error: unknown mode '" + mode +"'")
    sys.exit(1)


# cleanup as needed

if mode == MODE_VIDEO_FILE or mode == MODE_CAMERA:

    video.release()


cv2.destroyAllWindows()

sys.exit(0)
