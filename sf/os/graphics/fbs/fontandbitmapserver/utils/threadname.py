# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Utility function to append thread names where the Thread Id maps to a known thread/process name.

import re

# Takes as input list <data> which contains OST traces from the SYMBIAN_KERNEL_THREAD_IDENTIFICATION trace group in Symbian
# BTrace Hooks OST dictionary. Returns a list with the same content as <data> with thread names appended.
def addnames(data):
	result = []
	processNames = {}
	threadNames = {}
	rthreadIds = {}
	
	for line in data:		
		pattern = "^.*Thread:Process name assigned;NThread:(.*);DProcess:(.*);Name:(.*)$"
		matched = re.search(pattern, line)
		if matched:
			processNames[matched.group(2)] = matched.group(3)	
			
		pattern = "^.*Thread:Thread created;NThread:(.*);DProcess:(.*);Name:(.*)$"
		matched = re.search(pattern, line)
		if matched:
			threadNames[matched.group(1)] = processNames[matched.group(2)]+":"+matched.group(3)
		
		pattern = "^.*Thread:Thread name assigned;NThread:(.*);DProcess:(.*);Name:(.*)$"
		matched = re.search(pattern, line)
		if matched:
			threadNames[matched.group(1)] = processNames[matched.group(2)]+":"+matched.group(3)

		pattern = "^.*Thread:Thread ID assigned;NThread:(.*);DProcess:(.*);RThread ID:(\d*)$"
		matched = re.search(pattern, line)
		if matched:
			rthreadIds[matched.group(3)] = matched.group(1)
			
		pattern = "(^.*Thread ID:)(\w*)(.*)$"
		matched = re.search(pattern, line)
		if matched:
			threadId = matched.group(2)
			if threadNames.has_key(threadId):
				line = matched.group(1)+threadId+":"+threadNames[threadId]+matched.group(3)
	
		pattern = "(^.*clientThreadId )(\w*)(.*)$"
		matched = re.search(pattern, line)
		if matched:
			rthreadIdHex = matched.group(2)
			rthreadId = hex(rthreadIdHex)
			if rthreadIds.has_key(rthreadId):
				threadId = rthreadIds[rthreadId]
				threadName = threadNames[threadId]
				line = matched.group(1)+rthreadId+":"+threadName+matched.group(3)
				
		pattern = "(^.*threadId )(\w*)(.*)$"
		matched = re.search(pattern, line)
		if matched:
			rthreadIdHex = matched.group(2)
			rthreadId = hex(rthreadIdHex)
			if rthreadIds.has_key(rthreadId):
				threadId = rthreadIds[rthreadId]
				threadName = threadNames[threadId]
				line = matched.group(1)+rthreadId+":"+threadName+matched.group(3)
				
		result.append(line)
				
	return result