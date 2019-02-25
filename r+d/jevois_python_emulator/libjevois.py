# jevois emulation class
#
# this lets us run code intended to execute on the JeVois camera
# from the command line
#
# support here is minimal - depending on the camera-targeted code, this
# will need to be extended

import cv2
import numpy as np


def sendSerial(string):

    print(string)


class Jevois_in:

    def __init__(self, image):
        self.image = image

    def getCvBGR(self):
        return self.image


    def shape(self):
        return self.image.shape


class Jevois_out:

    def __init__(self, name):
        self.window_name = name

    def sendCv(self, image):
        cv2.imshow(self.window_name, image)
        cv2.waitKey(1)      # keep the GUI running








