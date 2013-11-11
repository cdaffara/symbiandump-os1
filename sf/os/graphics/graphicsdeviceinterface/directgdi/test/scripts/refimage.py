# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#


"""
Reference Image

Class representing test images and results of comparing it against reference images.

"""

import os
import os.path
from string import *
from PIL import Image, ImageChops, ImageOps, ImageStat, ImageFilter
from sets import Set
import shutil

# Relative path for reference images
KRefPath = "\\ref\\"

# Relative path for test images
KTestPath = "\\test\\"

# Compare test with reference images by pixel and pyramid difference; generate diff images
class RefImage:
    # Change the value to tune the passing limit for pyramid diff
    PYRAMID_PASS_LIMIT = 10
    # Change the value to tune the passing limit for pixel diff
    PIXEL_DIFF_PASS_LIMIT = 2

    # These are the types of differences that can be tested.
    PIXEL_DIFF = 1
    DIFF_SCORE = 2
    PYRAMID_DIFF = 3

    # @param aRefFile The reference images
    # @param aTestFile The test images
    # @param aBaseDir The base directory of reference and test images
    # @param aSource The distinctive part of the expected diff image name
    def __init__(self, aRefFile, aTestFile, aBaseDir, aSource):
        self.source     = aSource
        self.refFile    = aRefFile
        self.testFile   = aTestFile
        self.baseDir    = aBaseDir
        self.targetImage  = os.path.basename(aRefFile)
        self.maxDiff      = -1
        self.diffScore    = -1
        self.pyramidDiffs = None
        self.refImageCache  = None
        self.testFileCache  = None
        self.cachedTestFile = None
        self.cachedDiff   = None
        self.diffImages   = None
        self.diffsInUse   = Set([self.PIXEL_DIFF,self.PYRAMID_DIFF])

    # Read in reference images
    def _getImage(self):
        if not self.refImageCache:
            self.refImageCache = Image.open(self.baseDir + KRefPath + self.refFile)
            print "ref image: ", self.refFile
        return self.refImageCache

    # Read in test images
    def _getTestee(self):
        if not self.testFileCache:
            self.testFileCache = Image.open(self.baseDir + KTestPath + self.testFile)
            print "test image: ", self.testFile
        return self.testFileCache  

    # Get absolute value of the difference between test and reference images
    def _getDiff(self):
         self.cachedDiff = ImageChops.difference(self._getImage(), self._getTestee())
         return self.cachedDiff

    # Get pyramid levels of an image.
    # Returns a set of successively low-pass filtered images, resized by 1/2, 1/4, 1/8 respectivly.
    # @param aImage The image as the source to get pyramid levels
    # @return A set of 3 images scaled at 1/2, 1/4, and 1/8
    def _genPyramidLevels(self, aImage):
        # Create a 3X3 convolution kernel.
        # Gaussian image smoothing kernel, approximated by 3x3 convolution filter.
        # A convolution is a weighted average of all the pixels in some neighborhood of a given pixel.
        # The convolution kernel values are the weights for the average.
        kernel = ImageFilter.Kernel((3, 3), [.75, .9, .75, .9, 1, .9, .75, .9, .75])
        source = aImage
        res = []
        while len(res) < 3:
            srcSize = source.size
            # Mirror borders.
            temp = Image.new("RGBA", (srcSize[0]+2, srcSize[1]+2))
            temp.paste(source, (1, 1))
            
            # .crop returns a rectangular region from the current image. Passed: left, upper, right, and lower pixel coordinate.
            # .paste to upper-left corner.
            # left, top, right, bottom
            # Add a one pixel border around the image, so the center of the 3x3 convolution filter starts on the corner pixel of the image. 
            temp.paste(source.crop((1, 0, 1, srcSize[1]-1)), (0, 1))
            temp.paste(source.crop((0, 1, srcSize[1]-1, 1)), (1, 0))
            temp.paste(source.crop((srcSize[0]-2, 0, srcSize[0]-2, srcSize[1]-1)), (srcSize[0]+1, 1))
            temp.paste(source.crop((0, srcSize[1]-2, srcSize[1]-1, srcSize[1]-2)), (1, srcSize[1]+1))
            
            # Resize the filtered image to 0.5 size, via. 2x2 linear interpolation.
            filtered = temp.filter(kernel).crop((1, 1, srcSize[0], srcSize[1])).resize((srcSize[0]/2, srcSize[1]/2), Image.BILINEAR)
            source = filtered
            res.append(filtered)
        return res

    # Compute difference values between test and reference images
    #
    # - Generate mask image (3x3 max/min differences)
    # - Generate pyramid reference images (1/2, 1/4, 1/8 low-pass filtered and scaled).
    # - Generate pyramid test      images (1/2, 1/4, 1/8 low-pass filtered and scaled).
    # - Generate pyramid mask      images (1/2, 1/4, 1/8 low-pass filtered and scaled).
    # - Weight the mask according to level.
    # - For each level:
    #   - Get absolute difference image between reference and test.
    #   - Multiply absolute difference with inverted mask at that level
    # - Take maximum pixel value at each level as the pyramid difference.
    #
    # See: http://www.pythonware.com/library/pil/handbook/index.htm
    #
    def compPyramidDiff(self):
        ref = self._getImage()
        testee = self._getTestee()
        #if testee.size != ref.size:
        #	file.write("WARNING: The reference image has different dimension from the testee image")
        
        # maskImage is the difference between min and max pixels within a 3x3 pixel environment in the reference image.
        maskImage = ImageChops.difference(ref.filter(ImageFilter.MinFilter(3)), ref.filter(ImageFilter.MaxFilter(3)))
  
        # generate low-pass filtered pyramid images.
        refLevels = self._genPyramidLevels(ref)
        refL1 = refLevels[0]
        refL2 = refLevels[1]
        refL3 = refLevels[2]
        testLevels = self._genPyramidLevels(testee)
        testL1 = testLevels[0]
        testL2 = testLevels[1]
        testL3 = testLevels[2]
        maskLevels = self._genPyramidLevels(maskImage)

        # Apply weighting factor to masks at levels 1, 2, and 3.
        maskL1 = Image.eval(maskLevels[0], lambda x: 5*x)
        maskL2 = Image.eval(maskLevels[1], lambda x: 3*x)
        maskL3 = Image.eval(maskLevels[2], lambda x: 2*x)

        # Generate a pixel difference image between reference and test.
        # Multiply the difference image with the inverse of the mask.
        #   Mask inverse (out = MAX - image):
        #     So, areas of regional (3x3) similarity thend to MAX and differences tend to 0x00.
        #   Multiply (out = image1 * image2 / MAX:
        #     Superimposes two images on top of each other. If you multiply an image with a solid black image,
        #     the result is black. If you multiply with a solid white image, the image is unaffected.
        #   This has the effect of accentuating any test/reference differences where there is a small
        #   regional difference in the reference image.
        diffL1 = ImageChops.difference(refL1, testL1)
        diffL1 = ImageChops.multiply(diffL1, ImageChops.invert(maskL1))
        diffL2 = ImageChops.difference(refL2, testL2)
        diffL2 = ImageChops.multiply(diffL2, ImageChops.invert(maskL2))
        diffL3 = ImageChops.difference(refL3, testL3)
        diffL3 = ImageChops.multiply(diffL3, ImageChops.invert(maskL3))
        
        # So now the difference images are a grey-scale image that are brighter where differences
        # between the reference and test images were detected in regions where there was little 
        # variability in the reference image.

        # Get maxima for all bands at each pyramid level, and take the maximum value as the pyramid value.
        # stat.extrema (Get min/max values for each band in the image).

        self.pyramidDiffs = [
            max(map(lambda (x): x[1], ImageStat.Stat(diffL1).extrema)),
            max(map(lambda (x): x[1], ImageStat.Stat(diffL2).extrema)),
            max(map(lambda (x): x[1], ImageStat.Stat(diffL3).extrema))
        ]
        print "self.pyramidDiffs = ", self.pyramidDiffs

    # Compute max diff of pixel difference  
    def compMaxDiff(self):
        self.maxDiff = max(map(lambda (x): x[1], ImageStat.Stat(self._getDiff()).extrema))

    # Compute diff score
    # @param file A log file to store error messages
    def compDiffScore(self, file):
        self.diffScore = 0
        ref = self._getImage()
        testee = self._getTestee()
        if testee.size != ref.size:
            file.write("WARNING: Reference image from source has different dimension than the testee image")
            #raise ValueError("Reference image from source has different dimension than the testee image")
        # If a difference exists...
        if self.maxDiff != 0:      
            # Filter images for min and max pixel (dark and light) values within 5x5 environment.                  
            refMin = ref.filter(ImageFilter.MinFilter(5))
            refMax = ref.filter(ImageFilter.MaxFilter(5))
            testMin = testee.filter(ImageFilter.MinFilter(5))
            testMax = testee.filter(ImageFilter.MaxFilter(5))
            
            # make the min and max filter images a bit darker and lighter, respectively.
            refMin = Image.eval(refMin, lambda x: x - 4)
            refMax = Image.eval(refMax, lambda x: x + 4)
            testMin = Image.eval(testMin, lambda x: x - 4)
            testMax = Image.eval(testMax, lambda x: x + 4)

            refRefHist = ref.histogram()
            testRefHist = testee.histogram()

            # Calculate difference score.
            
            # Check for darkness in reference image.
            # Generate an image of the darkest pixels when comparing the 5x5 max filtered and lightened reference image against the test image.
            # If the pixel colour histogram of the generated image is different from the test image histogram, increase the difference score.
            if (ImageChops.darker(refMax, testee).histogram() != testRefHist):
                self.diffScore += 1
            
            # Check for lightness in reference image.
            if (ImageChops.lighter(refMin, testee).histogram() != testRefHist):
                self.diffScore += 1
            
            # Check for darkness in test image.
            if (ImageChops.darker(testMax, ref).histogram() != refRefHist):
                self.diffScore += 1
            
            #  Check for lightness in test image.
            if (ImageChops.lighter(testMin, ref).histogram() != refRefHist):
                self.diffScore += 1

        print "self.diffScore: ", self.diffScore

    # Generate test results
    # @param file A log file to store error messages
    def pyramidValue (self):
      return self.pyramidDiffs[2]

    def passed(self, file, aThresholdValue):
        if aThresholdValue == -1:
            aThresholdValue = self.PYRAMID_PASS_LIMIT
         
        if self.pyramidDiffs:
            return self.pyramidValue() <= aThresholdValue
        elif self.maxDiff >= 0:
            return self.maxDiff <= self.PIXEL_DIFF_PASS_LIMIT
        elif self.maxDiff < 0:
            warningMsg = "WARNING: Differences were not computed for the test image " + self.testFile + " against its reference image<br>"
            print warningMsg;
            if file: file.write(warningMsg);
            return True
        else:
            assert False
            return False


    # Make diff images
    # @param aDestDir
    def makeDiffImages(self, aDestDir):
        diffBands = list(self._getDiff().split())
        assert (len(diffBands) == 3 or len(diffBands) == 1)
        diffs = {}
        baseDiffName = "Diff_" + self.source +  "_" + self.targetImage
        # Invert the diffs.
        for i in range(len(diffBands)):
        #for i in range(4):
	        diffBands[i] = ImageChops.invert(diffBands[i])

        temp = ["R", "G", "B"]
        for i in range(len(diffBands)):
	        name = temp[i] + baseDiffName
        # Highlight the differing pixels
        if not self.PYRAMID_DIFF in self.diffsInUse and not self.DIFF_SCORE in self.diffsInUse:
           	diffBands[i] = Image.eval(diffBands[i], lambda x: (x / (255 - self.PIXEL_DIFF_PASS_LIMIT)) * 255)
		# Following line commented as we don't need to save bitmaps for the separate R,G or B channels.
        #diffBands[i].save(aDestDir + name, "BMP")
        diffs[temp[i]] = name
            
        if len(diffBands) == 3:
        	rgbDiff = ImageChops.darker(diffBands[0], ImageChops.darker(diffBands[1], diffBands[2]))
        else:
        	rgbDiff = diffBands[0]
            
        rgbDiffName = "RGB" + baseDiffName 
        rgbDiff.save(aDestDir + rgbDiffName, "BMP")
        diffs["RGB"] = rgbDiffName
                    
    	self.diffImages = diffs
    	return diffs


    # Print test results to command line    
    # @param file A log file to store error messages
    def printResult(self, file, aThresholdValue):
        print "test result: ", self.passed(file, aThresholdValue), "maxDiff: ", self.maxDiff

    # Get test results
    # @param file A log file to store error messages
    def getResult(self, file, aThresholdValue):
        return self.passed(file, aThresholdValue);

    # Get current puramid result value.
    def getPyramidResultValue(self):
        if self.pyramidDiffs:
            return self.pyramidValue()
        return 255

    # Get diff images
    def getDiffImages(self):
        assert self.diffImages != None
        return self.diffImages

    # Disable a diff test
    # @param diff Holds either self.PIXEL_DIFF,self.PYRAMID_DIFF or both when the tester wants to disable either or both of the diff tests 
    def disableDiff(self, diff):
        self.diffsInUse.discard(diff)

    # Enabld a diff test
    # @param diff Holds either self.PIXEL_DIFF,self.PYRAMID_DIFF or both when the tester wants to enable either or both of the diff tests
    def enableDiff(self, diff):
        self.diffsInUse.add(diff)

    # Set diffs
    # @param diffs Either self.PIXEL_DIFF,self.PYRAMID_DIFF or both when the tester wants to set either or both of the diff tests
    def setDiffs(self, diffs):
        self.diffsInUse = (diffs)

    # Compute difference according to the values in self.diffsInUse
    # @param file A log file to store error messages
    def computeDifferences(self, file):
        if self.PIXEL_DIFF in self.diffsInUse:
            self.compMaxDiff()
        if self.DIFF_SCORE in self.diffsInUse:
            self.compDiffScore(file)
        if self.PYRAMID_DIFF in self.diffsInUse:
            self.compPyramidDiff()


    
