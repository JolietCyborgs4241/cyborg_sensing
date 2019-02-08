import libjevois as jevois
import cv2
import numpy
import math
from enum import Enum

REFLECTOR_ANG       = 14.5           # +/- from vertical or 90 degrees

VERTICAL_ANG        = 90

LEFT_REFLECTOR_ANG  = (VERTICAL_ANG - REFLECTOR_ANG)
RIGHT_REFLECTOR_ANG = (VERTICAL_ANG + REFLECTOR_ANG)

REFLECTOR_TOLERANCE = 4.0


class Reflectors:
    
    def __init__(self):

        self.__blur_type = BlurType.Box_Blur
        self.__blur_radius = 1.0


        self.__hsv_threshold_hue = [50.0, 75.0]
        self.__hsv_threshold_sat = [100.0, 255.0]
        self.__hsv_threshold_val = [150.0, 255.0]


        # we treat the minimum line length as a percentage of our horizontal
        # resolution as opposed to an absolute pixel length.  This keeps
        # us independent of the resolution.
        
        self.__filter_lines_min_length_percentage = 0.05

        # we filter the lines a little differently than the normally
        # generated code using a "custom" line filtering algorithm.
        # The 2019 FRC challenge uses reflective markers at a 14.5 degree
        # angle (+/-1 degree); we interpret the angles here as the angles
        # we will accept a line off of true vertical (or 90 / 270 degrees)
        # with a little tolerance of our own (we actually accept up to
        # +/-REFLECTOR_TOLERANCE degrees) from the FRC spec because,
        # you know, it's the only way to be sure
        #
        # we look for lines that are within the specified angle range either
        # side of vertical so our filtering routines handles right or left
        # side reflectors
        #
        # it is important to note that a vertical line is either 90 or 270
        # (but could also be -90 or -270)
        # degrees so our limits calculations take that into account

        self.__filter_lines_angles = [(REFLECTOR_ANG - REFLECTOR_TOLERANCE),
                                      (REFLECTOR_ANG + REFLECTOR_TOLERANCE)]


    def process(self, inframe, outframe):


      img = inframe.getCvBGR()

      # get image attributes

      (height, width, ndims) = img.shape


      # blur the image to reduce the noise

      blur_output = self.__blur(img, self.__blur_type, self.__blur_radius)


      # find the green hue of the illuminated reflectors

      hsv_threshold_output = self.__hsv_threshold(blur_output,
                                                  self.__hsv_threshold_hue,
                                                  self.__hsv_threshold_sat,
                                                  self.__hsv_threshold_val)
                                                    

      # we further do a blur on the resulting HSV filtering
      #
      # this is intended to make the job of the line finder easier and
      # to produce better results.  The blurring tends to produce longer
      # singular lines and reduces the tendency to create longer lines
      # out of a lot of little segments.  Longer lines are easy to filter
      # and process because you get a single line along the entire of edge
      # of something like a reflector rather than 10 little segments
      # which are a lot harder to deal with

      blur_output = self.__blur(hsv_threshold_output, self.__blur_type,
                                self.__blur_radius)
                                         
      # outframe.sendCv(blur_output)


      # Find the reflector edges so we can orient on them

      find_lines_output = self.__find_lines(blur_output)


      # uncomment the code below if you want to see the full output from
      # the __find_lines module
      #
      # jevois.sendSerial("vvvvvvvvvv find vvvvvvvvvv\n" + str(int(len(find_lines_output))) + " lines found:")
      #
      # for line in find_lines_output:
      #     jevois.sendSerial(str(line))
      #
      # jevois.sendSerial("^^^^^^^^^^ find ^^^^^^^^^^")


      # normalize all lines to have positive angles for easier evaluation

      normalize_lines_output = self.__normalize_lines(find_lines_output)
      

      # Filter any extraneous lines

      filter_lines_output = self.__filter_lines(normalize_lines_output,
                                                int(width * self.__filter_lines_min_length_percentage),
                                                self.__filter_lines_angles)

      # debug code below
      #
      # jevois.sendSerial(str(len(filter_lines_output)) + " lines returned from filter")


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
      #   hole is 13" below the top inside corner of the reflector
      #
      # working the math gives:
      #
      #  1) difference in pixels between the Y values of each end of the
      #     reflector line corresponds to 5.325"
      #  2) we take the difference between the Y values at and divide
      #     that by 5.325 - that gives us a pixel per inch value at that
      #     distance from the target
      #  3) we take that value and multiply it by 13 to give the number of
      #     pixels covering 13" at our current distance from the target
      #  4) we add that to the Y pixel value of the top end of the line and
      #     we've established the centerline of the hatch
      #
      # We could combine the constants in steps 2) and 3) into one operation
      # by multiplying the pixel difference by 13/5.325 or 2.441 which
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
      #
      # we don't need to worry about mis-matched sets - especially seeing a
      # right-side reflector as the first thing because the line filtering
      # makes sure that our first entry is a left-side reflector (and the
      # second entry is the corresponing right-side reflector)

      if (len(filter_lines_output) > 1):
          hatchCoorY = int((filter_lines_output[0].y2 - filter_lines_output[0].y1) * 2.441 +
                            filter_lines_output[0].y1)

          hatchCoorX = int((filter_lines_output[0].x1 + filter_lines_output[1].x1) / 2)

      # there is debugging code below that can be enabled to
      # highlight the lines that met filtering criteria in the camera captured
      # image (original image - not any of the processed copies)
      #
      # blue-purple (255, 75, 75)
      # pixel width of the line - set to 1% of the horizontal resolution which
      # gives us a 6 pixel wide line @ 640 and 2 pixles @ 320
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

          # jevois.sendSerial("----------------------\n" + str(len(filter_lines_output)) + " lines filtered:")

          for line in filter_lines_output:
              # debug code below to highlight lines in output image
              #
              # cv2.line(img, (line.x1, line.y1), (line.x2, line.y2),
              #          (255, 75, 75), int(float(width) * 0.010), 8, 0)

              jevois.sendSerial(str(width) + " X " + str(height) + ": " + str(line))

          # jevois.sendSerial("----------------------")


          # draw the target crosshair

          crosshairSize = int(float(width) * 0.10)

          cv2.line(img, (hatchCoorX - crosshairSize, hatchCoorY),
                        (hatchCoorX + crosshairSize, hatchCoorY),
                        (50, 255, 255), int(float(width) * 0.010), 8, 0)

          cv2.line(img, (hatchCoorX, hatchCoorY - crosshairSize),
                        (hatchCoorX, hatchCoorY + crosshairSize),
                        (50, 255, 255), int(float(width) * 0.010), 8, 0)

          cv2.circle(img, (hatchCoorX, hatchCoorY), int(crosshairSize/2),
                     (50, 255, 255), int(float(width) * 0.010))

      # more debug - give a warning we couldn't even find a pair of reflectors
      # else:
      #     jevois.sendSerial("Fewer than 2 lines after filtering!");


      # send the original, full color, unblurred image with the lines
      # highlighted to the USB output

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
            return '{},{} -> {},{}: {} pixels @ {} degrees'.format(int(self.x1), int(self.y1),
                                                                   int(self.x2), int(self.y2),
                                                                   int(self.length()),
                                                                   int(self.angle()))

        # comparison routines:
        #
        # a line is "less" than another line based on the value of the x1
        # line coordinate which due to the normalization we applied earlier
        # will be the x coordiante of the "top" of the line
        #
        # this gives us a nice list that we can run through from left to right
        # and helps us to key into the left-most reflector set as our priority
        # target

        def __eq__(self, other):
            return (self.x1 == other.x1)
            
        def __ne__(self, other):
            return (self.x1 != other.x1)
            
        def __lt__(self, other):
            return (self.x1 < other.x1)
            
        def __le__(self, other):
            return (self.x1 <= other.x1)
            
        def __gt__(self, other):
            return (self.x1 > other.x1)
            
        def __ge__(self, other):
            return (self.x1 >= other.x1)


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

        if (lines[0] is not None):
            for i in range(0, len(lines[0])):
                tmp = Reflectors.Line(lines[0][i, 0][0], lines[0][i, 0][1],
                                      lines[0][i, 0][2], lines[0][i, 0][3])
                output.append(tmp)

        return output


    @staticmethod
    def __normalize_lines(input):
        """Make all line angles positive by re-arranging the start and
           end coordinates if needed.
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

        Returns:
            A mormalized list of Lines with all positive angles
        """

        output = []
        
        for line in input:
            if (int(line.angle()) < 0):

                # swap the two endpoints; this won't change the line
                # because it still has the same endpoints but it will
                # change the angle to all be positive which greatly
                # simplifies the comparisons later because we don't have
                # to account for equivalent but complementary angled lines
                #
                # swapping the endpoints has the the effect of adding 180
                # degrees to the line (if it's negative) making it a
                # complimentary, and now positive, angle

                temp    = line.x1
                line.x1 = line.x2
                line.x2 = temp

                temp    = line.y1
                line.y1 = line.y2
                line.y2 = temp

            output.append(line)

        return output


    @staticmethod
    def __filter_lines(inputs, min_length, angle):
        """Filters out lines that do not meet length and angle criteria.
        Args:
            inputs: A list of Lines.
            min_Length: The minimum length (in pixels) that will be kept.
            angle: The minimum and maximum angles in degrees as a list
                   of two numbers; these number represent a range of +/-
                   from 90 degrees either CCW or CW

        Processing:
            We build the list of all lines that meet our length and
            angle criteria - after that, we run through the lines and try to
            find the most edges closest to the center of the bay.  We'll do
            that by sorting the list by x1 coordinates (which X coordinate
            we use probably doesn't matter so long as we are consistent).
            Once they are sorted, scan from "left" to "right" and
            keep only the closest to the center edges of the alignment marks.
            That would be the be the right side of the right-leaning marks
            and the left side of the left-leaning marks

        Returns:
            A filtered list of Lines representing the "inside" edges of the
            reflectors relative to the cargo hatches.  We try to only include
            matched sets - the R/L for a hatch rather than an unpaired
            straggler from an adjacent hatch
        """

        tempOutputs = []
        outputs     = []

        # jevois.sendSerial("__filter_lines:\nMin length: " + str(min_length) + " Angles[] = " + str(angle[0]) + ", " + str(angle[1]) + "\n")
        
        for line in inputs:
            if (line.length() > min_length):

                lineAngle = line.angle()    # no need to keep reinvoking method below
                
                if (lineAngle <= (VERTICAL_ANG - angle[0]) and
                    lineAngle >= (VERTICAL_ANG - angle[1])
                    or
                    lineAngle >= (VERTICAL_ANG + angle[0]) and
                    lineAngle <= (VERTICAL_ANG + angle[1])):

                     tempOutputs.append(line)

        # sort the list - we do it by x1 (which should be the top X coor)

        tempOutputs = sorted(tempOutputs)

        # now let's look through the lines and find the two adjoining r/l edges
        # of the alignment marks; we'll delete the "outside" marks

        sawLeftSide  = False    # last line we saw - non-zero if it was left side
        sawRightSide = False

        prevLine = None

        foundFirstLeft = False
        
        for line in tempOutputs:

            # we already filtered and sorted the lines to meet the angular
            # and length criteria corresponding to the reflectors so
            # can totally key on whether they tilt right or left as
            # our discriminating factor from here on out

            if (line.angle() > 90):               # left side - tilts right

                foundFirstLeft = True
                
                if (sawLeftSide != True):         # no current left side
                    sawRightSide = False
                    sawLeftSide  = True           # keep track of the first
                                                  # edge of a left side
                                                  # reflector
                else:
                    outputs.append(line)          # it's the right-most
                                                  # (inside edge) of a
                                                  # left reflector so save it
                    sawLeftSide = False

            else:                                 # right side - tilts left
                if (foundFirstLeft == True):
                    if (sawRightSide != True):    # no current right side
                        if (sawLeftSide):         # unmatched left so save it
                            outputs.append(prevLine)
                            sawLeftSide = False
                        sawRightSide = True
                        outputs.append(line)      # append this first right side
                    else:
                        sawRightSide = False      # it's the outside of a
                                                  # right side reflector so
                                                  # skip it and don't add it
                                                  # to the list

            # save the most recent line we processed so we can get to it if
            # we need to

            prevLine = line

        return outputs


BlurType = Enum('BlurType', 'Box_Blur Gaussian_Blur Median_Filter Bilateral_Filter')
