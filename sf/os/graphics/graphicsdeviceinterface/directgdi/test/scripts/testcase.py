# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
Test Case

Container for test cases and interfaces for executing the cases.

"""
import os
import os.path
import fileinput
import string

from refimage import RefImage
from htmlreport import *

# The threshold value for the failure monitor report
FAILURE_THRESHOLD_MONITOR = 5

# The expected type of images for testing
KImageType = ".bmp"
KImageTypeAlpha = "-alpha.bmp"

# Relative path for reference images
KRefPath = "ref\\"

# Relative path for test images
KTestPath = "test\\"

# A mid name for diff images will be added to make the diff images names
KDiffImageMidName = "diffImg"

# HTML file name for an index page of the test results
KIndexPageName = "testresults.html"
# HTML file name for failed images
KPageForFailedImages = "failed.html"
# HTML file name for passed images
KPageForPassedImages = "passed.html"
KPageForFailedAtImages = "failedAt.html"
# HTML file name for a summary page where passed and failed image information is summarised
KPageForSummary = "summary.html"

# HTML file name for a summary page of the test results for the overnight build results page
KONBSummary = "directgditest_image_results.htm"

class TestCase:
    # Enum of test results
    RESULT_PASSED = 1
    RESULT_FAILED = 2

    def __init__(self, aName, aResultDir, aLogDir, aOnbDir):
        self.name = aName
        self.resultTestImages = []
        self.resultRefImages = []
        self.resultTestImagesAlpha = []
        self.resultRefImagesAlpha = []
        self.resultTestImagesWithAlphaPair = []
        self.resultRefImagesWithAlphaPair = []
        self.resultDir = aResultDir
        self.logDir = aLogDir
        self.onbDir = aOnbDir
        self.testPassedNumber = 0
        self.testFailedNumber = 0
        self.testFailedAtNumber = 0
        self.passedTestImg = []
        self.failedTestImg = []
        self.failedAtTestImg = []
        self.passedRefImg = []
        self.failedRefImg = []
        self.failedAtRefImg = []
        self.thumbnails = {}

    # Read test images to self.resultRefImages
    def readRefImage(self, file):
    # @param file A log file to store error messages
        refimageDir = os.path.join(self.resultDir, KRefPath)
        self.resultRefImages = []
        self.resultRefImagesAlpha = []
        images = os.listdir(refimageDir)
                    
        
        #find bitmaps describing alpha channel first
        for imageFile in images:
            if lower(imageFile[-10:]) == KImageTypeAlpha:
                self.resultRefImagesAlpha.append(imageFile)
                
        #find all bitmaps in directory but those with no alpha pair
        for imageFile in images:
            alphapair = False
            # only process the image with the defined imageType
            if lower(imageFile[-4:]) == KImageType and lower(imageFile[-10:]) != KImageTypeAlpha:
                alpharefimg = self.formatNameAddAlpha(imageFile)
                for imgalph in self.resultRefImagesAlpha:
                    if alpharefimg == imgalph:
                       alphapair = True
                       break
                       
                if alphapair == False:
                   self.resultRefImages.append(imageFile)
                else:
                   self.resultRefImagesWithAlphaPair.append(imageFile)


        if self.resultRefImages == []:
            print("WARNING: No ref images found at " + self.resultDir + KRefPath)
            file.write("warning: No ref images found at: " + self.resultDir + KRefPath + "<br>")

    # Read test images to self.resultTestImages
    # @param file A log file to store error messages
    def readTestImage(self, file):
        testimageDir = os.path.join(self.resultDir, KTestPath)
        self.resultTestImages = []
        self.resultTestImagesAlpha = []
        images = os.listdir(testimageDir)
        
        #find bitmaps describing alpha channel first
        for imageFile in images:
             if lower(imageFile[-10:]) == KImageTypeAlpha:
                self.resultTestImagesAlpha.append(imageFile)
                
        #find all bitmaps in directory but those with no alpha pair
        for imageFile in images:
            alphapair = False
            # only process the image with the defined imageType
            if lower(imageFile[-4:]) == KImageType and lower(imageFile[-10:]) != KImageTypeAlpha:
                alphatestimg = self.formatNameAddAlpha(imageFile)
                for imgalph in self.resultTestImagesAlpha:
                     if alphatestimg == imgalph:
                        alphapair = True
                        break
                        
                if alphapair == False:
                   self.resultTestImages.append(imageFile)
                else:
                   self.resultTestImagesWithAlphaPair.append(imageFile)


        if self.resultTestImages == []:
            print("WARNING: No test images found at " + self.resultDir + KTestPath)
            file.write("warning: No test images found at: " + self.resultDir + KTestPath + "<br>")

    # Remove the prefix of "Bitgdi_" or "Directgdi_" in the image names
    # @param aFileName The image file name
    def formatBaseName(self, aFileName):
        completeName = os.path.basename(aFileName)
        index = completeName.find("_")
        return completeName[index + 1:]
        
    # construct name with -alpha postfix
    # @param aFileName The image file name
    def formatNameAddAlpha(self,aFileName):
        (alphaimgname,alphaimgext) = os.path.splitext(aFileName)
        return alphaimgname + "-alpha" + alphaimgext
        
    # construct name by removing -alpha postfix
    # @param aFileName The image file name
    def formatNameRemoveAlpha(self,aFileName):
        (alphaimgname,alphaimgext) = os.path.splitext(aFileName)
        index = alphaimgname.find("-alpha")
        if index == -1:
           return aFileName
        else:
           return alphaimgname[:index] + alphaimgext

    def processLine(self, aLine):
        return string.split(aLine)

    # Compute the difference of test images against their reference images one by one
    # @param file A log file to store error messages
    def computeDifferences(self, file):

        # Generate list of threshold mappings - 
        # Consisting of base name & threshold values
        thresholds = []
        lineTuple = []
        try:
          for line in fileinput.input("thresholds.txt"):
              lineTuple = self.processLine(line)
              if lineTuple:
                  try:
                      if (lineTuple[0] and (lineTuple[0][0] != '#')):
                          thresholds.append(lineTuple[0])
                          thresholds.append(lineTuple[1])
                          print "Found threshold exception: ", lineTuple[0], " ", lineTuple[1]
                  except Exception:
                      pass
        except Exception:
            pass

        # Create file to recieve information on any test failures.
        # Format:
        # <file name> <pyramid diff failure value>\n
        failuresFile = open ( 'testFailures.txt', 'w' ) 

        # loop through test images
        for imgFile in self.resultTestImages:
            
            # get the test image file basename
            #baseName_test = os.path.basename(imgFile)
            baseName_test = self.formatBaseName(imgFile)

            # only process the image with the defined imageType
            if lower(baseName_test[-4:]) == KImageType:

                # look through reference images
                for refFile in self.resultRefImages:

                    # get the reference image file basename 
                    #baseName_ref = os.path.basename(refFile)
                    baseName_ref = self.formatBaseName(refFile)

                    # only process the image with the defined imageType
                    if lower(baseName_ref[-4:]) == KImageType:

                        # match the file names of a test image with its reference image
                        if baseName_test == baseName_ref:

                            # create a RefImage and compute difference between test and reference images
                            test = RefImage(refFile, imgFile, self.resultDir, KDiffImageMidName)
                            test.computeDifferences(file)
                            test.makeDiffImages(self.resultDir)
                            
                            thresholdValue = -1
                            baseCaseName = baseName_test[:-4]
                            try:
                                thresholdValue = thresholds[thresholds.index(baseCaseName) + 1]
                            except Exception:
                                pass
                            
                            test.printResult(file, int(thresholdValue))
                            result = test.getResult(file, int(thresholdValue))

                            pyramidValue = test.getPyramidResultValue ();
                            if result == True:
                                self.testPassedNumber = self.testPassedNumber + 1
                                self.passedTestImg.append((imgFile, pyramidValue))
                                self.passedRefImg.append((refFile, pyramidValue))
                                
                                # If the test passes but is above and independant threshold
                                # record the test and reference image names for report generation.
                                if (pyramidValue > FAILURE_THRESHOLD_MONITOR):
                                    self.failedAtTestImg.append((imgFile, pyramidValue))
                                    self.failedAtRefImg.append((refFile, pyramidValue))
                                    self.testFailedAtNumber = self.testFailedAtNumber + 1
                                    
                            else:
                                # Output failure information to failures file.
                                strList = []
                                strList.append(baseCaseName)
                                strList.append(`test.pyramidValue()`)
                                strList.append('\n')
                                failuresFile.write(' '.join(strList))
                                print "Test image failure: ", ' '.join(strList)
                                self.testFailedNumber = self.testFailedNumber + 1
                                self.failedTestImg.append((imgFile, pyramidValue))
                                self.failedRefImg.append((refFile, pyramidValue))
        
        # loop through test images with alpha pair
        for imgFile in self.resultTestImagesWithAlphaPair:
            
            # get the test image file basename
            #baseName_test = os.path.basename(imgFile)
            baseName_test = self.formatBaseName(imgFile)

            # only process the image with the defined imageType
            if lower(baseName_test[-4:]) == KImageType:

                # look through reference images
                for refFile in self.resultRefImagesWithAlphaPair:

                    # get the reference image file basename 
                    #baseName_ref = os.path.basename(refFile)
                    baseName_ref = self.formatBaseName(refFile)

                    # only process the image with the defined imageType
                    if lower(baseName_ref[-4:]) == KImageType:

                        # match the file names of a test image with its reference image
                        if baseName_test == baseName_ref:

                            # create a RefImage and compute difference between test and reference images
                            test = RefImage(refFile, imgFile, self.resultDir, KDiffImageMidName)
                            test.computeDifferences(file)
                            test.makeDiffImages(self.resultDir)

                            thresholdValue = -1
                            baseCaseName = baseName_test[:-4]
                            try:
                                thresholdValue = thresholds[thresholds.index(baseCaseName) + 1]
                            except Exception:
                                pass

                            test.printResult(file, int(thresholdValue))
                            result = test.getResult(file, int(thresholdValue))
                            
                            alpharefimg = self.formatNameAddAlpha(refFile)
                            for imgrefalpha in self.resultRefImagesAlpha:
                                if alpharefimg == imgrefalpha:
                                   for imgtestalpha in self.resultTestImagesAlpha:
                                        if self.formatBaseName(imgrefalpha) == self.formatBaseName(imgtestalpha):
                                            testalpha = RefImage(imgrefalpha, imgtestalpha, self.resultDir, KDiffImageMidName)
                                            testalpha.computeDifferences(file)
                                            testalpha.makeDiffImages(self.resultDir)
                                            testalpha.printResult(file, int(thresholdValue))
                                            resultalpha = testalpha.getResult(file, int(thresholdValue))
                                            if resultalpha == False:
                                               result = False
                                            break
                                               
                            pyramidValue = test.getPyramidResultValue ();
                            if result == True:
                                self.testPassedNumber = self.testPassedNumber + 1
                                self.passedTestImg.append((imgFile, pyramidValue))
                                self.passedRefImg.append((refFile, pyramidValue))

                                # If the test passes but is above and independant threshold
                                # record the test and reference image names for report generation.
                                if (pyramidValue > FAILURE_THRESHOLD_MONITOR):
                                    self.failedAtTestImg.append((imgFile, pyramidValue))
                                    self.failedAtRefImg.append((refFile, pyramidValue))
                                    self.testFailedAtNumber = self.testFailedAtNumber + 1

                            else:
                                 # Output failure information to failures file.
                                strList = []
                                strList.append(baseCaseName)
                                strList.append(`test.pyramidValue()`)
                                strList.append('\n')
                                failuresFile.write(' '.join(strList))
                                print "Test image alpha-pair failure: ", ' '.join(strList)
                                self.testFailedNumber = self.testFailedNumber + 1
                                self.failedTestImg.append((imgFile, pyramidValue))
                                self.failedRefImg.append((refFile, pyramidValue))
                                
        failuresFile.close()
        print "All cases: ", self.testPassedNumber + self.testFailedNumber
        print "Failed cases: ", self.testFailedNumber
        print "Passed cases: ", self.testPassedNumber
        print 'Failed at threshold %d cases: %d' % (FAILURE_THRESHOLD_MONITOR, self.testFailedAtNumber)

        # Generate a report of the test results                        
        self.generateReport(os.path.join(self.resultDir, KTestPath), os.path.join(self.resultDir, KRefPath), self.resultDir)

    # Generate a report of test results
    # @param aTestDir The directory of the test images
    # @param aRefDir The directory of the reference images
    # @param diffDir The directory of the diff images
    def generateReport(self, aTestDir, aRefDir, aDiffDir):
        # Write a main index page of the test results
        report = file(self.logDir + KIndexPageName, "wt")
        writeHtmlHeader(report, self.testFailedNumber, self.testPassedNumber, self.testFailedAtNumber, FAILURE_THRESHOLD_MONITOR)
        writeHtmlFooter(report)
        report.close()
        
        #sort tables in alphabetical order
        self.failedTestImg.sort()
        self.failedRefImg.sort()
        self.passedTestImg.sort()
        self.passedRefImg.sort()
        self.failedAtTestImg.sort()
        self.failedAtRefImg.sort()
        
        summary = file(self.logDir + KPageForSummary, "wt")
        writeFailedDetails(summary, self.failedTestImg, self.failedRefImg, aTestDir, aRefDir, aDiffDir, self.logDir)
        writePassedDetails(summary, self.passedTestImg, self.passedRefImg, aTestDir, aRefDir, aDiffDir, self.logDir)                
        writeFailedAtDetails(summary, self.failedAtTestImg, self.failedAtRefImg, aTestDir, aRefDir, aDiffDir, self.logDir, FAILURE_THRESHOLD_MONITOR) 
        
        # Write a html page of the failed images
        failedDetail = file(self.logDir + KPageForFailedImages, "wt")
        writeSimpleHtmlHeader(failedDetail)
        writeFailedDetails(failedDetail, self.failedTestImg, self.failedRefImg, aTestDir, aRefDir, aDiffDir, self.logDir)
        writeHtmlFooter(failedDetail)
        failedDetail.close()
        
        # Write a html page of the passed images
        passedDetail = file(self.logDir + KPageForPassedImages, "wt")
        writeSimpleHtmlHeader(passedDetail)
        writePassedDetails(passedDetail, self.passedTestImg, self.passedRefImg, aTestDir, aRefDir, aDiffDir, self.logDir)
        writeHtmlFooter(passedDetail)
        passedDetail.close()

        # Write a html page of the failed at <thresholdvalue> images.
        failedAtDetail = file(self.logDir + KPageForFailedAtImages, "wt")
        writeSimpleHtmlHeader(failedAtDetail)
        writeFailedAtDetails(failedAtDetail, self.failedAtTestImg, self.failedAtRefImg, aTestDir, aRefDir, aDiffDir, self.logDir, FAILURE_THRESHOLD_MONITOR)
        writeHtmlFooter(failedAtDetail)
        failedAtDetail.close()    
        
        # Write a summary page for the overnight build results
        onb_report = file(self.onbDir + KONBSummary, "wt")
        writeHtmlONB(onb_report, self.testFailedNumber, self.testPassedNumber, self.testFailedAtNumber, FAILURE_THRESHOLD_MONITOR, KIndexPageName) 
        onb_report.close()            
