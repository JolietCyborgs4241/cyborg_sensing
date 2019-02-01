import libjevois as jevois
import cv2
import numpy
import math
from enum import Enum

REFLECTOR_ANG       = 14.5
LEFT_REFLECTOR_ANG  = (90 - REFLECTOR_ANG)
RIGHT_REFLECTOR_ANG = (90 + REFLECTOR_ANG)
REFLECTOR_TOLERANCE = 3.0


class Reflectors:
    
    def __init__(self):

        self.__blur_type = BlurType.Box_Blur
        self.__blur_radius = 5.0

        self.blur_output = None


        self.__hsv_threshold_hue = [50.0, 70.0]
        self.__hsv_threshold_sat = [100.0, 255.0]
        self.__hsv_threshold_val = [140.0, 255.0]

        self.hsv_threshold_output = None


        self.find_lines_output = None


        self.__filter_lines_lines = self.find_lines_output

        # we treat the minimum line length as a percentage of our horizontal
        # resolution as opposed to an absolute pixel length.  This keeps
        # us independent of the resolution.
        #
        # In this case, we use 10% of the horizontal resolution which at
        # 320 pixels gives us 32 pixels
        self.__filter_lines_min_length_percentage = 0.10

        # we filter the lines a little differently than the normally
        # generated code using a "custom" line filtering algorithm.
        # The 2019 FRC challenge uses reflective markers at a 14.5 degree
        # angle (+/-1 degree); we interpret the angles here as the angles
        # we will accept a line off of true vertical (or 90 / 270 degrees)
        # with a little tolerance of our own (we accept up to +/-3.0 degrees)
        # from the FRC spec because, you know, it's the only way to be sure
        #
        # it is important to note that  vertical line is either 90 or 270
        # degrees so our limits calculations take that into account
        self.__filter_lines_angles = [(REFLECTOR_ANG - REFLECTOR_TOLERANCE),
                                      (REFLECTOR_ANG + REFLECTOR_TOLERANCE)]

        self.filter_lines_output = None


    def process(self, inframe, outframe):


      img = inframe.getCvBGR()

      # get image attributes
      (height, width, ndims) = img.shape


      # blur the image to reduce the noise
      (self.blur_output) = self.__blur(img, self.__blur_type,
                                         self.__blur_radius)

      # find the green hue of the illuminated reflectors
      (self.hsv_threshold_output) = self.__hsv_threshold(self.blur_output,
                                                    self.__hsv_threshold_hue,
                                                    self.__hsv_threshold_sat,
                                                    self.__hsv_threshold_val)
      jevois.sendSerial("HSV done")
      outframe.sendCv(self.hsv_threshold_output)


      # Find the reflector edges so we can orient on them
      self.__find_lines_input = self.hsv_threshold_output
      (self.find_lines_output) = self.__find_lines(self.hsv_threshold_output)
      jevois.sendSerial("Lines found")
      jevois.sendSerial(str(int(len(self.find_lines_output))))

      # Filter any extraneous lines
      self.__filter_lines_lines = self.find_lines_output
      (self.filter_lines_output) = self.__filter_lines(self.find_lines_output,
                                                width * self.__filter_lines_min_length_percentage,
                                                self.__filter_lines_angles)
      jevois.sendSerial("Lines filtered")
      jevois.sendSerial(str(int(len(self.filter_lines_output))))


      # CALCULATE THE MID POINT OF THE TWO MARKS
      #
      # FIND THE LEFT MOST MARK AND THEN TRY TO FIND THE FIRST RIGHTMOST MARK
      #
      # SPLIT THE DIFFERENCE AND THEN FIGURE OUT THE HORIZONTAL HASH MARK
      # LOCATION
      #
      # DRAW THEM ON THE IMAGE IN BLUE TWICE THE WIDTH OF THE HIGHLIGHT
      # LINES

      # the lines are now filtered and sorted into pairs of the
      # "inside"-most lines of each reflector with their order from
      # left to write in the processed image.
      #
      # we use this information in a couple of ways:
      #
      # the midpoint of each inside-most x coordinates is the midpoint
      # between the relectors and should be the center of the hatch or
      # cargo hole
      #
      # we also now the height of the reflector and the size of the hatch
      # hole so we can actually calculate the center of the hatch hole.
      # getting things exact is a little bit more trig than we need but
      # we can extrapolate the center from basic parameters we have.
      #
      # - height of the reflective tape is 5.5"
      # - angle is 14.5 degrees off of vertical
      # - difference between the y pixels on the same line represents 5.325"
      #   (basically we can pre-calc all the trigonometry involved)
      # - the top inside corner, the lower corner, is 0.5" below the top edge
      # - the top edge is 12.5" above the center of the hatch hole (this
      #   was derived from the game field specs) so the center of the hatch
      #   hole is 12" below the top inside corner of the reflector
      #
      # working the math gives:
      #
      #  1) difference in pixels between the Y values of each end of the
      #     reflector line corresponds to 5.325"
      #  2) we take the difference between the Y values at and divide
      #     that by 5.325 - that gives us a pixel per inch value at that
      #     distance from the target
      #  3) we take that value and multiply it by 12 to give the number of
      #     pixels covering 12" at our current distance from the target
      #  4) we add that to the Y pixel value of the top end of the line and
      #     we've established the centerline of the hatch
      #
      # We could combine the constants in steps 2) and 3) into one operation
      # by multiplying the pixel difference by 12/5.325 or 2.253 which
      # will get us the pixel offset from the top Y value of the line in
      # one step.
      #
      # We don't need to calculate the location of the vertical
      # center of the hatch for anything right now but it will just show
      # what we could do
      #
      # because we could end up seeing more than one set of alignment
      # reflectors, we give precedence to the left-most set of marks since
      # that's where we're starting our search from

      if (len(self.filter_lines_output)):
          hatchCoorY = int((self.filter_lines_output[0].y2 - self.filter_lines_output[0].y1) * 2.253 +
                            self.filter_lines_output.y2)

          hatchCoorX = int((self.filter_lines_output[0].x1 + self.filter_lines_output[1].x1) / 2)

      # highlight the lines that met filtering criteria in the camera captured
      # image (original image - not any of the processed copies)
      #
      # white (255,255,255)
      # pixel width of the line - set to .5% of the horizontal resolution which
      # gives us a 3 pixel wide line @ 640 and a singe @ 320
      #
      # In addition to highlighting the lines in the original image, we output
      # a formatted string to the console for further processing
      #
      # Format:
      #
      # "Xres X Yres: x1, y1 -> x2, y2 - length @ angle"
      #
      # Example:
      #
      # "320 X 240: 105, 66 -> 112, 114 - 48 @ 80"

          for line in self.filter_lines_output:
              cv2.line(img, (line.x1, line.y1), (line.x2, line.y2), (255, 255, 255),
                       int(float(width) * 0.005), 8, 0)

              jevois.sendSerial(str(width)+" X "+str(height)+": "+
                                str(int(line.x1))+", "+str(int(line.y1))+" -> "+
                                str(int(line.x2))+", "+str(int(line.y2))+" - "+
                                str(int(line.length()))+" @ "+str(int(line.angle())))
      
          crosshairSize = int(float(width) * 0.10)

          cv2.line(img, hatchCoorX - crosshairSize, hatchCoorY, hatchCoorX + crosshairSize, hatchCoorY, (255, 50, 50), int(float(width) * 0.010), 8, 0)

          cv2.line(img, hatchCoorX, hatchCoorY - crosshairSize, hatchCoorX, hatchCoorY + crosshairSize, (255, 50, 50), int(float(width) * 0.010), 8, 0)
      else:
          jevois.sendSerial("No lines found!");
          
      # send the original, full color, unblurred image with the lines highlighted
      # to the USB output

      outframe.sendCv(img)


    @staticmethod
    def __blur(src, type, radius):
        """Softens an image using one of several filters.
        Args:
            src: The source mat (numpy.ndarray).
            type: The blurType to perform represented as an int.
            radius: The radius for the blur as a float.
        Returns:
            A numpy.ndarray that has been blurred.
        """
        if(type is BlurType.Box_Blur):
            ksize = int(2 * round(radius) + 1)
            return cv2.blur(src, (ksize, ksize))
        elif(type is BlurType.Gaussian_Blur):
            ksize = int(6 * round(radius) + 1)
            return cv2.GaussianBlur(src, (ksize, ksize), round(radius))
        elif(type is BlurType.Median_Filter):
            ksize = int(2 * round(radius) + 1)
            return cv2.medianBlur(src, ksize)
        else:
            return cv2.bilateralFilter(src, -1, round(radius), round(radius))

    @staticmethod
    def __hsv_threshold(input, hue, sat, val):
        """Segment an image based on hue, saturation, and value ranges.
        Args:
            input: A BGR numpy.ndarray.
            hue: A list of two numbers that are the min and max hue.
            sat: A list of two numbers that are the min and max saturation.
            lum: A list of two numbers that are the min and max value.
        Returns:
            A black and white numpy.ndarray.
        """
        out = cv2.cvtColor(input, cv2.COLOR_BGR2HSV)
        return cv2.inRange(out, (hue[0], sat[0], val[0]),  (hue[1], sat[1], val[1]))

    class Line:

        def __init__(self, x1, y1, x2, y2):
            self.x1 = x1
            self.y1 = y1
            self.x2 = x2
            self.y2 = y2

        def length(self):
            return numpy.sqrt(pow(self.x2 - self.x1, 2) + pow(self.y2 - self.y1, 2))

        def angle(self):
            return math.degrees(math.atan2(self.y2 - self.y1, self.x2 - self.x1))

        def __repr__(self):
            return '{},{} -> {},{}'.format(self.x1, self.y1, self.x2, self.y2)

        # our compare function sorts on the x1 member
        def __cmp__(self, other):
            if hasattr(other, 'x1'):
                return self.x1.__cmp__(other.x1)


    @staticmethod
    def __find_lines(input):
        """Finds all line segments in an image.
        Args:
            input: A numpy.ndarray.
        Returns:
            A filtered list of Lines.
        """
        detector = cv2.createLineSegmentDetector()
        if (len(input.shape) == 2 or input.shape[2] == 1):
            lines = detector.detect(input)
        else:
            tmp = cv2.cvtColor(input, cv2.COLOR_BGR2GRAY)
            lines = detector.detect(tmp)
        output = []
        if len(lines) != 0:
            for i in range(1, len(lines[0])):
                tmp = Reflectors.Line(lines[0][i, 0][0], lines[0][i, 0][1],
                                lines[0][i, 0][2], lines[0][i, 0][3])
                output.append(tmp)
        return output

    @staticmethod
    def __filter_lines(inputs, min_length, angle):
        """Filters out lines that do not meet certain criteria.
        Args:
            inputs: A list of Lines.
            min_Length: The minimum length (in pixels) that will be kept.
            angle: The minimum and maximum angles in degrees as a list
                   of two numbers; these number represent a range of +/-
                   from 90 degrees either CCW or CW

        Processing:
            Depending on how the lines are found, the start and end of
            the lines and the corresponding angles are not consistent

            The angles are accurate but parallel lines could be reported
            as either 94 or -86 degrees; to keep things consistent, we
            check the angle and if it's negative, we normalize it by
            adding 180 degrees and swap x1, y1 and x2, y2 in the output

            We first build the list of all lines that meet our length and
            angle criteria - after that, we run through the line and try to
            find the most edges closest to the center of the bay.  We'll do
            that by sorting the list by x1 coordinates (which X coordinate
            we use probably doesn't matter so long as we are consistent).
            Once they are sorted, we can scan from "right" to "left" and
            keep only the closest to the center edges of the alignment marks.
            That would be the be the right side of the right-leaning marks
            and the left side of the left-leaning marks

        Returns:
            A filtered list of Lines.
        """

        tempOutputs = []
        outputs     = []

        for line in inputs:
            if (line.length() > min_length):

                lineAngle = line.angle()    # no need to keep reinvoking method

                if (int(lineAngle) < 0):
                    temp    = line.x1
                    line.x1 = line.x2
                    line.x2 = temp

                    temp    = line.y1
                    line.y1 = line.y2
                    line.y2 = temp

                    lineAngle = line.angle()  # recalc the angle

                if (lineAngle >= (90 - angle[0]) and lineAngle <= (90 - angle[1]) or
                     lineAngle >= (90 + angle[0]) and lineAngle <= (90 + angle[1])):

                     tempOutputs.append(line)

        print("Presort: ", tempOutputs)

        # sort the list - we do it by x1 (which should be the bottom X coor)

        tempOutputs = sorted(tempOutputs)

        print("Postsort: ", tempOutputs)

        # now let's look through the lines and find the two adjoining r/l edges
        # of the alignment marks; we'll delete the "outside" marks

        sawLeftSide  = False    # last line we saw - non-zero if it was left side
        sawRightSide = False

        prevLine = None

        for line in tempOutputs:

            # we already filtered the lines to meet the angular criteria
            # of the reflectors so we can totally key on whether the tilt
            # right or left as our discriminating factor

            if (line.angle() > 90):           # left side - tilts right

                if (sawLeftSide):             # no current left side
                    sawRightSide = False
                    sawLeftSide  = True       # keep track of the first left side
                else:
                    outputs.append(line)      # it's the right most side so save it
                    sawLeftSide = False
            else:                             # right side - tilts left
                if (sawRightSide != True):    # no current right side
                    if (sawLeftSide):         # unmatched left so save it
                        outputs.append(prevLine)
                        sawLeftSide = False
                    sawRightSide = True
                    outputs.append(line)      # append this first right side

                # otherwise we just skip any rights after the first one...

            # save the most recent line we processed
            prevLine = line

        return outputs


BlurType = Enum('BlurType', 'Box_Blur Gaussian_Blur Median_Filter Bilateral_Filter')
