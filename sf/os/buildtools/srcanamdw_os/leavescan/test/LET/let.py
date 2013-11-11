#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#!/usr/bin/python
import string
import sys
import os
from os import listdir
from os.path import isdir
import time
from optparse import OptionParser

#config
LEAVESCAN = 'D:\leavescan.exe '

os_type = 'unix'
if sys.platform.lower().startswith('win'):
    os_type = 'windows'

class TestResult:
	def __init__(self):
		self.allOutput=""
		self.lines={}

	def setAllOutput(self,allOuptput):
		self.allOuptput = allOuptput

	def parseLines(self):
		tmp_lines=self.allOuptput.splitlines()
		for tmp_line in tmp_lines:
			line_no = getLeaveScanMsgLine(tmp_line)
			self.lines[line_no]=tmp_line

	def checkResult(self,warnItems):
		result = ''
		ok=0
		for warnItem in warnItems:
			warnLine = warnItem.line
			if not warnItem.semi:
				if self.lines.has_key(warnLine):
					outputLine = self.lines[warnLine]
					for keyword in warnItem.checkExist:
						if outputLine.find(keyword)<0:
							result= result+ 'CHECK:"'+keyword+'" is expected in the output:"'+outputLine+'"\n'
							ok=1
					for keyword in warnItem.checkNoExist:
						if outputLine.find(keyword)>=0:
							result= result+ 'CHECK:"'+keyword+'" is expected not in the output:"'+outputLine+'"\n'
							ok=1
				else:
					for keyword in warnItem.checkExist:
						result= result+ 'CHECK:"'+keyword+'" is expected in the output of line'+str(warnLine)+'\n'
						ok = 1
			else:
				if self.lines.has_key(warnLine):
					outputLine = self.lines[warnLine]
					for keyword in warnItem.checkExist:
						if outputLine.find(keyword)>=0:
							result= result+ 'SEMI:"'+keyword+'" is expected in the output:"'+outputLine+'"\n'
							ok=1
					for keyword in warnItem.checkNoExist:
						if outputLine.find(keyword)<0:
							result= result+ 'SEMI:"'+keyword+'" is expected not in the output:"'+outputLine+'"\n'
							ok=1
				else:
					for keyword in warnItem.checkNoExist:
						result= result+ 'SEMI:"'+keyword+'" is expected in the output of line'+str(warnLine)+'\n'
						ok = 1


		if ok==0:
			return 'OK'
		else:
			return result;

class CheckPoint:
	def __init__(self):
		self.line = -1
		self.checkExist=[]
		self.checkNoExist=[]
		self.semi = False
	def setCheckPoint(self,warnString,semi = False):
		warnString=warnString.strip()
		if len(warnString)<=0:
			return 
		keywordList = warnString.split(',')
		for keyword in keywordList:
			keyword = keyword.strip()
			if len(keyword)>0:
				if keyword[0]=='+':
					self.checkExist.append(keyword[1:])
				elif keyword[0]=='-':
					self.checkNoExist.append(keyword[1:])
				else:
					self.checkExist.append(keyword)
		self.semi = semi

	def printSelf(self):
		printType = 0;
		print 'line:'+ str(self.line)
		if len(self.checkExist)>0:
			print '		checkExist'
			printType +=1;
			for checkWord in self.checkExist:
				print '				'+checkWord
		if len(self.checkNoExist)>0:
			print '		checkNoExist'
			printType +=1;
			for checkWord in self.checkNoExist:
				print '				'+checkWord
		if printType == 0:
			print  '	nothing to check'



class TestcaseConfig:
	def __init__(self):
		self.desc =''
		self.option =''
		self.expect =''
		self.warnItems =[]
		self.author = ''
		self.type = ''
		self.date =''
	def printSelf(self):
		print 'desc:'+self.desc
		print 'option:'+self.option
		print 'author:'+self.author
		print 'type:'+self.type
		print 'date:'+self.date
		print 'expect:'+self.expect
		print 'check:'
		for warnItem in self.warnItems:
			print '	line:'+str(warnItem.line)
			printType = 0;
			if len(warnItem.checkExist)>0:
				print '		checkExist'
				printType +=1;
				for checkWord in warnItem.checkExist:
					print '				'+checkWord
			if len(warnItem.checkNoExist)>0:
				print '		checkNoExist'
				printType +=1;
				for checkWord in warnItem.checkNoExist:
					print '				'+checkWord
			if printType == 0:

				print  '	nothing to check'



def getTestcaseConfig(fileName):
	file = open(fileName,"r")
	fileLine = 0
	testcaseConfig = TestcaseConfig()
	for line in file.readlines():
		if not line.endswith('\\'):
			fileLine += 1
		line = line.strip()
		scommnet = line.find('//')
		if scommnet == 0:
			if (line.find('desc:'))>0:
				desc = line[line.find('desc:')+5:].strip()
				testcaseConfig.desc=desc
			elif line.find('option:')>0:
				option = line[line.find('option:')+7:].strip()
				testcaseConfig.option=option
			elif line.find('expect file:')>0:
				expect = line[line.find('expect file:')+12:].strip()
				#get the content of the file
				data = open(expect,"r")
				for dataline in data.readlines():
					testcaseConfig.expect += dataline
			elif line.find('expect:')>0:
				expect = line[line.find('expect:')+7:].strip()
				testcaseConfig.expect=expect
			elif line.find('author:')>0:
				expect = line[line.find('author:')+7:].strip()
				testcaseConfig.author=expect
			elif line.find('type:')>0:
				expect = line[line.find('type:')+5:].strip()
				testcaseConfig.type=expect
			elif line.find('date:')>0:
				expect = line[line.find('date:')+5:].strip()
				testcaseConfig.date=expect

		if scommnet >=0:
			line =line[scommnet+2:]
			if line.find('check:')>=0:
				line = line[line.find('check:')+6:].strip()
				warnItem = CheckPoint()
				warnItem.setCheckPoint(line)
				warnItem.line = fileLine
				testcaseConfig.warnItems.append(warnItem)
			elif line.find('semi:')>=0:
				line = line[line.find('semi:')+5:].strip()
				warnItem = CheckPoint()
				warnItem.setCheckPoint(line,True)
				warnItem.line = fileLine
				testcaseConfig.warnItems.append(warnItem)

				
	return testcaseConfig

def execute(filename , option):
	commandLine = LEAVESCAN + option + ' ' +filename
	output=''
	for outputLine in os.popen(commandLine).readlines():
		output += outputLine
	return output
	
def analysis(result,testcaseConfig):
	"""
	no use in the version
	for expect keyword
	"""
	if not testcaseConfig.expect=='':
		if testcaseConfig.expect == result:
			return 1;
		else:
			print testcaseConfig.expect
			if DEBUG:
				print '------'
				print result 
				print '========'
			return 0;

	
def excuteCmd(cmdLine):
	i, o = os.popen4(cmdLine)
   	i.close()
    	out = o.read()
    	o.close()
	return out


def getLeaveScanMsgLine(line):
	p1=line.find('(')
	if(p1<0):
		return -2
	p2=line.find(')')
	if(p2<2):
		return -2
	return int(line[p1+1:p2])

def getFormatTime():
	curTime = time.localtime()
	return str(curTime[0])+"_"+str(curTime[1])+"_"+str(curTime[2])+"_"+str(curTime[3])+"_"+str(curTime[4])+"_"+str(curTime[5])

def getCurTime():
	curTime = time.localtime()
	return "'"+str(curTime[0])+"-"+str(curTime[1])+"-"+str(curTime[2])+" "+str(curTime[3])+":"+str(curTime[4])+":"+str(curTime[5])+".000000'"

def getTestcaseName(fileName):
	fileNameList = os.path.splitext(fileName) 
	return fileNameList[len(fileNameList)-2]

def runTest(testFile,logFile,reportFile):
	testcaseConfig = getTestcaseConfig(testFile)
	cmdString = LEAVESCAN+testcaseConfig.option+' '+testFile
	logFile.write(cmdString+'\n')
	startTime = getCurTime()
	result = excuteCmd(cmdString)
	endtime = getCurTime()
	logFile.write(result+'\n')

	lr = TestResult()
	lr.setAllOutput(result)
	lr.parseLines()
	
	result=lr.checkResult(testcaseConfig.warnItems)
	testcaseResult = 'PASS' # for csv file
	if not result=='OK':
		logFile.write('ERROR:'+testFile+'\n')
		logFile.write('DETAIL:'+'\n')
		logFile.write(result)
		testcaseResult = 'FAILTURE'
		print testcaseResult+':'+testFile

	else:
		logFile.write('PASS\n')
	
	reportFile.write(getTestcaseName(testFile)+','+startTime+','+endtime+','+testcaseResult+'\n')
	if result=='OK':
		return 1
	else:
		return 0


if __name__ == '__main__':
	#pass argu
	testFileList=[]
	optParser = OptionParser('let -f file_list -o output_dir')
	optParser.add_option("-f","--file",action = "store",type = "string",dest = "fileListName")
	optParser.add_option("-o","--outputdir",action = "store",type = "string",dest = "outputName")
	optParser.add_option("","--std-in",action = "store_true",dest = "listFromStdin",default =False)
	optParser.add_option("-e","--execute",action = "store",type="string", dest = "exePath", default="")
	options, args = optParser.parse_args(sys.argv[1:])
	#check exe exsitence
	if not options.exePath == "":
		if os.path.exists(options.exePath):
			LEAVESCAN = options.exePath+" "
		else:
			print "WARN-"+options.exePath+" dosen't exist"
			print "WARN-will use default config:"+LEAVESCAN
	if not os.path.exists(LEAVESCAN.strip()):
		print "WARN-please set Leavescan's address"
	else:
		fileListName = options.fileListName
		outputDirName = options.outputName
		if not os.path.exists(outputDirName):
			os.makedirs(outputDirName)
		#get each testcase in the list 
		pwd=os.getcwd()+'\\'
		if options.listFromStdin :
			listFile = sys.stdin
		else:
			listFile = open(fileListName,'r')

		for curFile in listFile.readlines():
			curFile=curFile.strip()
			if(curFile.find('#')==0):
				1+1
			elif os.path.exists(curFile):
				testFileList.append(curFile)
			else:
				print 'WARN-file '+curFile+ 'doesn\'t exist.ignore'
		#other config
		DEBUG = 0;
		totalRun=0;
		totalPass=0;
		logFileName = outputDirName+'\\'+getFormatTime()+'.log'
		reportFileName = outputDirName+'\\'+getFormatTime()+'.csv'
		logFile = open(logFileName,'w')
		reportFile = open(reportFileName,'w')
		reportFile.write('TestCaseId,StartTime,EndTime,Result\n')

		for testFileName in testFileList:
		#test it
			totalRun+=1;
			totalPass+=runTest(testFileName,logFile,reportFile)
		#summary
		print 'RSLT-Total:'+str(totalRun)+';Pass:'+str(totalPass)
		#environment
		logFile.close()
		reportFile.close()
	

