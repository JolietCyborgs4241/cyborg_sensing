import libjevois as jevois
import cv2
import numpy as np
import math
from enum import Enum

class ZogTest:
    def __init__(self):

      self.desaturate_output = None
      
      self.__blur_type = BlurType.Box_Blur
      self.__blur_radius = 2

      self.blur_output = None

      self.__find_lines_input = self.blur_output

      self.find_lines_output = None

      self.__filter_lines_lines = self.find_lines_output
      self.__filter_lines_min_length = 50.0
      self.__filter_lines_angle = [45.0, 135.0]

      self.filter_lines_output = None



    def process(self, inframe, outframe):
      
      img = inframe.getCvBGR()


      # make a greyscalew copy of the image to simplify and speed up line detection

      (self.desaturate_output) = self.__desaturate(img)


      # blur the image to help the line finder to find longer lines
      # and not try to build longer lines through a bunch of short segments

      (self.blur_output) = self.__blur(self.desaturate_output, self.__blur_type, self.__blur_radius)
      

      # take the blurred image and find lines

      (self.find_lines_output) = self.__find_lines(self.blur_output)


      # process the lines to leave only those that meet our length and angle criteria
      #
      # right now the minimum line length is fixed at initialization time and is specified
      # in terms of pixels.
      #
      # The original values in this file used a line length of 50 which was proven reasonable
      # based on a trial and error testing with an image resolution of 320X240 pixels.  This means
      # we are using a minimum line length of about 21% of the vertical image resolution.
      #
      # It would be nice to make this resolution-independent; we would do this by calculating a
      # minimum line length based on vertical resolution (say using a figure of 20%). We should
      # be able to get the vertical resolution from the image object which is a numpy array;
      # getting the .ndim and .shape attributes should give us this information.
      #
      # We should be able to get the vertical resolution from the .shape attribute and
      # be able to calculate value passed to the __filter_lines() method.

      (self.filter_lines_output) = self.__filter_lines(self.find_lines_output, self.__filter_lines_min_length, self.__filter_lines_angle)


      # highlight the lines that met the filtering criteria in the camera captured image
      # (original image - not any of the processed copies)
      #
      # white (255,255,255)
      # 1 pixel width (we should make the line width resolution independent as well; 1 pixel
      #                is fine for a horizontal resolution of 320 but at 640, we really need
      #                a width of 2-3 pixels so calculated line width of about 0.05 of the
      #                horizontal resolution would be reasonable)

      for line in self.filter_lines_output:
          cv2.line(img, (line.x1, line.y1), (line.x2, line.y2), (255, 255, 255), 1, 8, 0)
          jevois.sendSerial(str(int(line.length())) + " @ " + str(int(line.angle())))


      # send the original, ful color, unblurred image with the lines highlighted to the USB output

      outframe.sendCv(img)



    @staticmethod
    def __desaturate(src):
        """Converts a color image into shades of gray.
        Args:
            src: A color numpy.ndarray.
        Returns:
            A gray scale numpy.ndarray.
        """

        (a, b, channels) = src.shape
        if(channels == 1):
            return numpy.copy(src)
        elif(channels == 3):
            return cv2.cvtColor(src, cv2.COLOR_BGR2GRAY)
        elif(channels == 4):
                return cv2.cvtColor(src, cv2.COLOR_BGRA2GRAY)
        else:
            raise Exception("Input to desaturate must have 1, 3 or 4 channels")



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



    class Line:

        def __init__(self, x1, y1, x2, y2):
            self.x1 = x1
            self.y1 = y1
            self.x2 = x2
            self.y2 = y2

        def length(self):
            return np.sqrt(pow(self.x2 - self.x1, 2) + pow(self.y2 - self.y1, 2))

        def angle(self):
            return math.degrees(math.atan2(self.y2 - self.y1, self.x2 - self.x1))



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
                tmp = ZogTest.Line(lines[0][i, 0][0], lines[0][i, 0][1],
                            lines[0][i, 0][2], lines[0][i, 0][3])
                output.append(tmp)
        return output



    @staticmethod
    def __filter_lines(inputs, min_length, angle):
        """Filters out lines that do not meet certain criteria.
        Args:
            inputs: A list of Lines.
            min_length: The minimum length that will be kept.
            angle: The minimum and maximum angles in degrees as a list of two numbers.
        Returns:
            A filtered list of Lines.
        """
        outputs = []
        for line in inputs:
            if (line.length() > min_length):
                if ((line.angle() >= angle[0] and line.angle() <= angle[1]) or
                        (line.angle() + 180.0 >= angle[0] and line.angle() + 180.0 <= angle[1])):
                    outputs.append(line)
        return outputs




BlurType = Enum('BlurType', 'Box_Blur Gaussian_Blur Median_Filter Bilateral_Filter')

