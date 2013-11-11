import ConfigParser
##import io
import sys
import os
import re
import string
import shutil

active=1
iniFileName='updatecontentandfilenames.ini'

def AddSearchReplacePair(src,dst):
	print "Will replace\t"+src+"  with\t"+dst
	search_replace[src] = dst

def DieAndWait(string):
	print string
	sys.exit()
	
def TranslateString(string):
#	out = string
#	print "\ntranslating: "+string
	for src in search_replace.keys():
		dest = search_replace[src]
#		print "src: "+src+" dest: "+dest
		string = string.replace(src,dest)
#		print "out: "+out
	return string

	
def RenameIfNecessary(root,file):
	thisdest = TranslateString(file)
	if(thisdest != file):
		print "Renaming ["+file+"] to ["+thisdest+"]"
		if active:
			os.rename(os.path.join(root,file),os.path.join(root,thisdest))

def CorrectFile(file):
	# ini file is special
	if(file.upper() == iniFileName.upper()):
		CorrectIniFile(file)
	else:
		if(FileContainsSrc(file)):
			print "Modifying ["+file+"]"
			TranslateFile(file);

def FileContainsSrc(file):
#	print "looking in ["+file+"]"
	f = open(file)
	found = 0
	for line in f:
#		print "[[["+line
		for src in search_replace.keys():
			if(-1 != string.find(line,src)):
				found = 1
#				print "\n'"+src+"' found in "+line+".. so.."
				break
		if(found):
			break
	f.close()
	return found

def TranslateFile(file):
	if (active == 0):
		return
	shutil.copyfile(file,file+".bak")
	i = open(file+".bak")
	o = open(file,"w+")
	for line in i:
		out=TranslateString(line)
		o.write(out)
	i.close()
	o.close()

def CorrectIniFile(file):
	if (active == 0):
		return
	print "Correcting ini file "+file+" .."
	shutil.copyfile(file,file+".bak")
	i = open(file+".bak")
	o = open(file,"w+")
	searchfield=''
	for line in i:
		ms=re.match('^\s*Search\s*=\s*(.+)',line,flags=re.IGNORECASE)
		mr=re.match('^\s*Replace\s*=\s*(.+)',line,flags=re.IGNORECASE)
		if(ms):
			if(searchfield != ''):
				print "Multiple search sections found before a replace. fix config file manually"
				sys.exit()
			searchfield = ms.group(1)
		elif(mr):
			if(searchfield == ''):
				print "Replace section found before search section. fix config file manually"
				sys.exit()
			o.write("Search="+mr.group(1)+"\nReplace=CHANGEME\n")
			searchfield=''
		else:
			o.write(line)
	i.close()
	o.close()




config = ConfigParser.ConfigParser()
config.readfp(open(iniFileName))

print "1. Figuring out search/replace pairs.."

search_replace = {} # empty map

for section in config.sections():
	src = config.get(section,'Search')
	if(src == ""):
		DieAndWait("Error in ini file section '"+section+"': Must specify Search field")
	if(src in search_replace):
		DieAndWait("Error in ini file section '"+section+"': Search field '"+src+"' specified more than once")
	dest = config.get(section,'Replace')
	if(dest == ""):
		DieAndWait("Error in ini file section '"+section+"': Must specify Replace field")
	if(dest == "CHANGEME"):
		DieAndWait("Error in ini file section '"+section+"': Must specify value to replace '"+src+"' with")
	
	AddSearchReplacePair(src,dest)
	
	# deal with upper / lower case variants
	ucsrc = src.upper()
	lcsrc = src.lower()
	lcapsrc = ucsrc[:1]+lcsrc[1:]
	ucdest = dest.upper()
	lcdest = dest.lower()
	lcapdest = ucdest[:1]+lcdest[1:]

	if(ucsrc != src):
		AddSearchReplacePair(ucsrc,ucdest)
	if(lcsrc != src):
		AddSearchReplacePair(lcsrc,lcdest)
	if(lcapsrc != src and lcapsrc != lcsrc):
		AddSearchReplacePair(lcapsrc,lcapdest)

print "-----"
print "2. Scanning for read-only files.."
  
readOnlyFiles=0;
for root, dirs, files in os.walk("",topdown=False):
	for file in files:
#		print "Testing readable  : "+root+file
		try:
			fh = open(os.path.join(root,file),"a+")
			fh.close()
		except:
			print "Read-only file detected: "+os.path.join(root,file)
			readOnlyFiles=1

if(readOnlyFiles):
	print "Can't operate with read-only files present."
	sys.exit()
else:
	print "No read-only files present. Proceeding to search/replace.."


print "-----"
print "3. Renaming files and folders.."

for root, dirs, files in os.walk("",topdown=False):
#	print "ROOT: "+root
	for dir in dirs:
		RenameIfNecessary(root,dir)
	for file in files:
		RenameIfNecessary(root,file)


print "-----"
print "4. Search/replacing in files.."

for root, dirs, files in os.walk("",topdown=False):
	for file in files:
		CorrectFile(os.path.join(root,file))

print "-----"
print "Done."
