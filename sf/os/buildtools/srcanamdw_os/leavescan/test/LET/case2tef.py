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
import re
import time
import sys
import csv

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

class CheckPoint:
	def __init__(self):
		self.line = -1
		self.checkExist=[]
		self.checkNoExist=[]
	def setCheckPoint(self,warnString):
		warnString=warnString.strip()
		if len(warnString)<=0:
			return 
		keywordList = warnString.split(',')
		for keyword in keywordList:
			keyword = keyword.strip()
			if keyword[0]=='+':
				self.checkExist.append(keyword[1:])
			elif keyword[0]=='-':
				self.checkNoExist.append(keyword[1:])
			else:
				self.checkExist.append(keyword)

	def toString(self):
		printType = 0;
		res=''
		res = res+'line:'+ str(self.line)+'\n'
		if len(self.checkExist)>0:
			res =res + '		checkExist\n'
			printType +=1;
			for checkWord in self.checkExist:
				res =res+ '				'+checkWord+'\n'
		if len(self.checkNoExist)>0:
			res =res+ '		checkNoExist\n'
			printType +=1;
			for checkWord in self.checkNoExist:
				res=res+ '				'+checkWord+'\n'
		if printType == 0:
			res=res+  '	nothing to check\n'
		return res

def get_case_name(value):
	while value.find('\\\\') != -1:
        	value = value.replace('\\\\', '\\')
        	continue
	while value.find('//') != -1:
		value = value.replace('//', '\\')
        	continue
        while value.find('/') != -1:
            	value = value.replace('/', '\\')
            	continue
	pos = value.rfind('\\')
	value =  value[pos+1:]
	pos = value.find('.')
	return value[:pos]

def get_testcase_config(fileName):
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
	return testcaseConfig


def comment_string(value):
	lines = value.splitlines()
	result = ''
	for line in lines:
		if line.find('@SYM')==0:
			result += '//! ' + line+'\n'
		else:	
			result += '//!                           ' + line+'\n'
	return result

if __name__ == '__main__':
	argv = sys.argv[1:]
	if len(argv) >=1:
		input_file_name = argv[0]
	else:
		print 'case2tef file_list_file'
	if input_file_name == '-':
		input_file = sys.stdin 
	elif os.path.exists(input_file_name) and os.path.isfile(input_file_name):
		input_file = open(input_file_name)
	else:
		input_file = None
	if not input_file == None:
		lines = input_file.readlines()
		for line in lines:
			while line.find('\n') != -1:
        			line = line.replace('\n', '')
        			continue

			testcase_config =get_testcase_config(line)
			case_name = get_case_name(line)
			res = 'START_TESTCASE                '+case_name+'\n'
			body = '@SYMTestCaseID            '    +case_name+'\n'
			body+= '@SYMTestCaseDesc          '    +testcase_config.desc+'\n'
			body+= '@SYMTestPriority          '    +'HIGH'+'\n'
			body+= '@SYMTestActions           '    +'run LeaveScan command line'
			if not testcase_config.option=='':
				body+=' with option:'+testcase_config.option
			body+='\n'
			body+= '@SYMTestExpectedResults   '    +'Check the follwing:'+'\n'
			if not testcase_config.expect=='':
				body+='Output is:\n'
				body+=testcase_config.expect+'\n'
			if len(testcase_config.warnItems)>0:
				body+='check the keyword of output\n'
				for warnItem in testcase_config.warnItems:
					body+= warnItem.toString()
			body+= ''
			body+= '@SYMTestType              '    +testcase_config.type+'\n'
			body+= '@SYMCreationDate          '    +testcase_config.date+'\n'
			body+= '@SYMAuthor                '    +testcase_config.author+'\n'
			res += comment_string(body)
			res += 'END_TESTCASE                  '+case_name+'\n'
			res += '\n'
			print res




