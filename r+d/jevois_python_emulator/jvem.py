import sys
import numpy as np
import cv2
import libjevois as jevois
import Reflectors as rf



# to run:
#
#       python3 jvem.py image_file_to_process




# there need to be 2 command line parameters (the program name counts as one)
#
# the parameter is the name of the static image file used for the image processing

if len(sys.argv) < 2:
    print("Usage: " + sys.argv[0] + ": image")
    sys.exit(1)


window_name = "JeVois Emulator"

#read the image
image = cv2.imread(sys.argv[1], cv2.IMREAD_UNCHANGED)

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
#    comment out the line below (whihc is usually the first line in the
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

while True:
    reflector.process(image, jv_out)

# the code below isn't really needed; the only way to exit the emulator right now
# is to interrupt it such as by using a ^C or similar keyboard press
#
# the cleanup done in cv2.destroyAllWindows() will happen automatically in that
# case so not calling it isn't a big deal

cv2.destroyAllWindows()

sys.exit(0)
