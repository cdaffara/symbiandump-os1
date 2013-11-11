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
Html Report Tools

Tools for generating the HTML reports.

"""

import os
import os.path
from string import *

# HTML file name for failed images
KPageForFailedImages = "failed.html"
# HTML file name for passed images
KPageForPassedImages = "passed.html"
KPageForFailedAtImages = "failedAt.html"
# HTML file name for a summary page where passed and failed image information is summarised
KPageForSummary = "summary.html"

# HTML file name for an error message page of the test results
KErrorMessagePageName = "error.html"

# Prefix of the diff image name when the test image passes 
KNoDiffPrefix = "NoDiff_"

# Prefix of the diff image name when the test image fails 
DiffPrefix = "RGBDiff_diffImg_"

# Write html header
# @param file The file object to be accessed
def writeTitle(file):
    file.write("<head>\n")
    file.write("<title> Graphics DirectGdi / BitGdi Test Report</title>\n")
    file.write("</head>\n")

# Write html footer
# @param file The file object to be accessed
def writeHtmlFooter(file):
    file.write('<br/>Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).\n')
    file.write("</body></html>\n")

# Write overall test results
# @param file The file object to be accessed
# @param aNumFailed The number of failed images
# @param aNumPassed The number of passed images
def writeHtmlHeader(file, aNumFailed, aNumPassed, aNumFailedAt, aFailedAtThreshold):
    writeTitle(file)
    file.write("<body>\n")
    file.write("<h1>Graphics DirectGdi / BitGdi Test Report</h1><br/>\n")
    writeLink(file, "All Cases", KPageForSummary, aNumFailed + aNumPassed)
    file.write('Note: Some Asynchronous Interleaving images may be missing due to nature of that test.<br><br>')
    writeLink(file, "Failed Cases", KPageForFailedImages, aNumFailed)
    writeLink(file, "Passed Cases", KPageForPassedImages, aNumPassed)
    failureNotice = 'Failed At Threshold %d Cases'%aFailedAtThreshold
    writeLink(file, failureNotice, KPageForFailedAtImages, aNumFailedAt)
    file.write("<br><h2>")
    file.write('<a href="'+ KErrorMessagePageName +'">' + "Error Messages" + '</a>')
    file.write(" from the tests</h2><br>")
    

# Write simple overall test results for the Overnight Build
# @param file The file object to be accessed
# @param aNumFailed The number of failed images
# @param aNumPassed The number of passed images
def writeHtmlONB(file, aNumFailed, aNumPassed, aNumFailedAt, aFailedAtThreshold, linkFile):
    file.write("<font color=00AFFF>SUMMARY:</font><br>\n")
    total = aNumPassed + aNumFailed
    file.write('<font color=00AF00>ALL = %d</font><br>\n' % total)
    file.write('<font color=00AF00>PASS = %d</font><br>\n' % aNumPassed)
    file.write('<font color=FF0000>FAIL = %d</font><br>\n' % aNumFailed)
    file.write('<font color=FF0000>FAIL AT THRESHOLD %d' % aFailedAtThreshold)
    file.write(' = %d</font><br>\n' % aNumFailedAt)
    file.write("<font color=0000FF>ABORT = 0</font><br>\n")
    file.write("<font color=0000FF>PANIC = 0</font><br>\n")
    file.write("<font color=0000FF>INCONCLUSIVE = 0</font><br>\n")
    file.write("<font color=0000FF>UNKNOWN = 0</font><br>\n")
    file.write("<font color=0000FF>UNEXECUTED = 0</font><br>\n")
    file.write("<font color=0000FF>COMMENTED OUT COMMAND'S = 0</font><br>\n")
    file.write('<a href="directgdi_results\\logs\\' + linkFile + '">Test Results</a><br>\n')

# Write overall test results
# @param file The file object to be accessed
def writeSimpleHtmlHeader(file):
    writeTitle(file)
    file.write("<body>\n")

# Write link pages for test results
# @param file The file object to be accessed
# @param aTitle The text to appear on the page
# @param aPage A URL that the page links to
# @param aNum A number in brackets on the page
def writeLink(file, aTitle, aPage, aNum):
    file.write("<h2>")
    file.write('<span class="listlink">')
    file.write('<a href="'+ aPage +'">' + aTitle + '</a>')
    file.write(' (%d)</span>&nbsp;' % aNum)
    file.write("</h2>")

# Create a html page for all failed images index
# @param file The file object to be accessed
# @param aFailedTestImgs The test images that failed the tests
# @param aFailedRefImgs The reference images referred by the test images
# @param aTestDir The directory of the test images
# @param aRefDir The directory of the reference images
# @param aDiffDir The directory of the diff images
# @param aLogDir The directory of the log files
def writeFailedDetails(file, aFailedTestImgs, aFailedRefImgs, aTestDir, aRefDir, aDiffDir, aLogDir):
    writeTitle(file)
    file.write("<body>\n")
    file.write("<h2>")
    file.write("Failed test images:")
    file.write("</h2>\n")  
    writeSingleFileDetail(file, aFailedTestImgs, aFailedRefImgs, False, aTestDir, aRefDir, aDiffDir, aLogDir)  
    file.write("</body></html>\n") 

# Create a html page for all failed images index
# @param file The file object to be accessed
# @param aFailedTestImgs The test images that failed the tests
# @param aFailedRefImgs The reference images referred by the test images
# @param aTestDir The directory of the test images
# @param aRefDir The directory of the reference images
# @param aDiffDir The directory of the diff images
# @param aLogDir The directory of the log files
def writeFailedAtDetails(file, aFailedAtTestImgs, aFailedAtRefImgs, aTestDir, aRefDir, aDiffDir, aLogDir, aThreshold):
    writeTitle(file)
    file.write("<body>\n")
    file.write("<h2>")
    file.write("Failed At Threshold %d test images:" % aThreshold)
    file.write("</h2>\n")  
    writeSingleFileDetail(file, aFailedAtTestImgs, aFailedAtRefImgs, False, aTestDir, aRefDir, aDiffDir, aLogDir)  
    file.write("</body></html>\n")

# Create a html page for all passed images index
# @param file The file object to be accessed
# @param aPassdTestImgs The images that passed the tests
# @param aPassedRefImgs The reference images referred by the test images
# @param aTestDir The directory of the test images
# @param aRefDir The directory of the reference images
# @param aDiffDir The directory of the diff images
# @param aLogDir The directory of the log files
def writePassedDetails(file, aPassdTestImgs, aPassedRefImgs, aTestDir, aRefDir, aDiffDir, aLogDir):
    writeTitle(file)
    file.write("<body>\n")
    file.write("<h2>")
    file.write("Passed test images:")
    file.write("</h2>\n")
    writeSingleFileDetail(file, aPassdTestImgs, aPassedRefImgs, True, aTestDir, aRefDir, aDiffDir, aLogDir)
    file.write("</body></html>\n")

# Create a html page for each test image (with its reference image and diff image)
# @param file The file object to be accessed
# @param aTestImgs The test images to be processed
# @param aRefImgs The reference images referred by the test images
# @param aIsPass The boolean value of image test result 
# @param aTestDir The directory of the test images
# @param aRefDir The directory of the reference images
# @param aDiffDir The directory of the diff images
# @param aLogDir The directory of the log files
def writeSingleFileDetail(file, aTestImgs, aRefImgs, aIsPass, aTestDir, aRefDir, aDiffDir, aLogDir):
    i = 0
    for testImg in aTestImgs:
        file.write('<a href=%s.html> %s</a> Pyramid diff: %d\n'%(testImg[0], testImg[0], testImg[1]))
        writeDetailedDiffImg(aLogDir, testImg, aRefImgs[i], aIsPass, aTestDir, aRefDir, aDiffDir)
        file.write('<br/>\n')
        i = i + 1

# Show a test, reference and diff images on a html page
# @param aLogDir The directory of the log files
# @param aTestImgs The test images to be processed
# @param aRefImgs The reference images referred by the test images
# @param aIsPass The boolean value of image test result 
# @param aTestDir The directory of the test images
# @param aRefDir The directory of the reference images
# @param aDiffDir The directory of the diff images
def writeDetailedDiffImg(aLogDir, aTestImg, aRefImg, aIsPass, aTestDir, aRefDir, aDiffDir):


    (alphatestimgname,alphatestimgext) = os.path.splitext(aTestImg[0])
    alphatestimg = alphatestimgname + "-alpha" + alphatestimgext
    if os.path.exists(os.path.join(aTestDir,alphatestimg)):
        diff = file((aLogDir + aTestImg[0] + ".html"), "wt")
        writeTitle(diff)
        writeSimpleHtmlHeader(diff)
        
        diff.write('<h2>')
        diff.write("Test " + aTestImg[0])
        diff.write('</h2>\n')
        diff.write('<h2>Pyramid diff: %d </h2>\n' % aTestImg[1])
        # show a test image on the diff page
        diff.write('<h2>Test Image and alpha channel:</h2>\n')
        diff.write(aTestImg[0] + '<br>')
        diff.write('<table border=1><tr>')
        diff.write('<td><img src="../img/test/' + aTestImg[0] + '"> </td>')
        diff.write('<td><img src="../img/test/' + alphatestimg + '"> </td>')
        diff.write('</tr></table>')
        diff.write('<br/>\n')
        
        # show a reference image on the diff page
        diff.write('<h2>Reference Image and alpha channel: </h2>\n')
        diff.write(aRefImg[0] + '<br>')
        diff.write('<table border=1><tr>')
        diff.write('<td><img src="../img/ref/' + aRefImg[0] + '"> </td>')
        
        (alpharefimgname,alpharefimgext) = os.path.splitext(aRefImg[0])
        alpharefimg = alpharefimgname + "-alpha" + alpharefimgext
        
        diff.write('<td><img src="../img/ref/' + alpharefimg + '"> </td>')
        diff.write('</tr></table>')
        diff.write('<br/>\n')
        
        # show a diff image on the diff page
        diff.write('<h2>Diff Image and alpha channel: </h2>\n')
        diff.write('<table border=1><tr>')
        diff.write('<td><img src="../img/' + DiffPrefix + aRefImg[0] + '"> </td>')
        diff.write('<td><img src="../img/' + DiffPrefix + alpharefimg + '"> </td>')
        
        diff.write('</tr></table>')
        diff.write('<br/>\n')
        writeHtmlFooter(diff)
    else:
        diff = file((aLogDir + aTestImg[0] + ".html"), "wt")
        writeTitle(diff)
        writeSimpleHtmlHeader(diff)
        
        diff.write('<h2>')
        diff.write("Test " + aTestImg[0])
        diff.write('</h2>\n')
        diff.write('<h2>Pyramid diff: %d </h2>\n' % aTestImg[1])
        # show a test image on the diff page
        diff.write('<h2>Test Image: </h2>\n')
        diff.write(aTestImg[0] + '<br>')
        diff.write('<table border=1><tr>')
        diff.write('<td><img src="../img/test/' + aTestImg[0] + '"> </td>')
        diff.write('</tr></table>')
        diff.write('<br/>\n')
        
        # show a reference image on the diff page
        diff.write('<h2>Reference Image: </h2>\n')
        diff.write(aRefImg[0] + '<br>')
        diff.write('<table border=1><tr>')
        diff.write('<td><img src="../img/ref/' + aRefImg[0] + '"> </td>')
        diff.write('</tr></table>')
        diff.write('<br/>\n')
        
        # show a diff image on the diff page
        diff.write('<h2>Diff Image: </h2>\n')
        diff.write('<table border=1><tr>')
        diff.write('<td><img src="../img/' + DiffPrefix + aRefImg[0] + '"> </td>')
        diff.write('</tr></table>')
        diff.write('<br/>\n')
        writeHtmlFooter(diff)

def writeErroInfo(file, aErroInfo):
    file.write(aErroInfo)
    
