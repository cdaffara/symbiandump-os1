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
# Groups an ASCII TracViewer trace by the contents of a named parameter. 	
#

import fileinput, string, re, sys, threadname

# Utility function to group a list by a given regular expression.
# returns a dictionary indexed by parameter 1 of the passed in pattern.
def groupby(pattern, data):
	r = {}
	for entry in data:
		matched = re.search(pattern, entry)
		if matched:
			r.setdefault(matched.group(1), []).append(entry)
	return r

# Show the usage if the parameters are not as expected
if len(sys.argv) != 3:
	print "Usage: fbsgroup <param|-t> <input>"
	print "Where:"
	print "<param> is a parameter to group by. (-t groups by thread id)"
	print "<input> is the ASCII TraceViewer file to be parsed"
	sys.exit(1)

if sys.argv[1] == "-t":
	pattern = "(Thread ID:0x.*$)"
else:
	pattern = "("+sys.argv[1]+"=*\w*);+"
	
del sys.argv[1]

# Add thread names to the raw trace
rawinput = threadname.addnames(fileinput.input())

# Group by the parameter supplied on the command line...
results = groupby(pattern, rawinput)

for group, entries in results.items():
		print '\n'+group
		
		# Show a count of the number of CFbsBitmap::xxx function calls
		functions = groupby("(CFbsBitmap::\S+:)", entries)
		for name, function in functions.items():
			print "\t%s %s" % (name, len(function))
		
		# Show a count of the number of CFbClient::xxx function calls	
		functions = groupby("(CFbClient::\S+:)", entries)
		for name, function in functions.items():
			print "\t%s %s" % (name, len(function))
		
		# Show a count of the number of RFbsSession::xxx function calls	
		functions = groupby("(RFbsSession::\S+:)", entries)
		for name, function in functions.items():
			print "\t%s %s" % (name, len(function))
		
		# Show the matching entries for this group
		for entry in entries:
			print "\t%s" % entry.strip()

		

		


