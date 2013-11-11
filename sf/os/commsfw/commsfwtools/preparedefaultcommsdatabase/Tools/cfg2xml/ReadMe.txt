
CommDB Editor and Utilities
===========================


1. General
----------

This directory contains the CommDB XML Editor and utilities to allow the creation
of CommDB settings files, as input to the CED utility, in an XML format. The editor
as well as the utilities are written in Java and they can be compiled using any
Java Software Development Kit (SDK) Version 1.4 or higher compatible development
environment.

The CommDB XML Editor is part of the com.symbian.commdb package and it is composed
of the following packages:

com.symbian.commdb.ui     - contains all Java classes which implement the GUI components
                            of the editor.

com.symbian.commdb.data   - contains all Java classes which implement the structure of
                            the CommDB database used internally by the editor and the
                            utilities.

com.symbian.commdb.xml    - contains all Java classes which implement XML-related utility
                            classes.

Directory

  com/symbian/commdb/schema<x>/
  
  where <x> is the version of the OS

contains all the XML Schema files which define the required version of CommDB. 

The documentation for the source files is stored in directory:

  com/symbian/commdb/doc/

and it was automatically generated using the javadoc utility. The binary executable Java
files are supplied as JAR files and they are placed in directory:

  bin/

The building process for the CommDB Editor and Utilities can be performed in directory

  group/

by issuing the commands:

  bldmake bldfiles
  abld export

The export procedure copies the Java binary files to directory:

  /epoc32/tools/

and the XML Schema files to directory:

  /epoc32/tools/commdb-schema/<x>schema
  
  where <x> is the version of the OS  i.e 92, 93 ...


2. CommDB XML Editor
--------------------

The CommDB XML Editor executable file is cxmled.jar (executable Java archive) and it can
be executed in two ways:

a. On a desktop environment (Windows, Linux, MacOS X etc) double-click on the cxmled.jar
   file to start the application.

b. From a command line (e.g. a UNIX shell) issue the command:
   
   java -jar cxmled.jar

c. On a Windows computer where the Symbian OS SDK is installed one can simply issue
   the command:

   cxmled

   on a command line shell.

Note that the execution of this application requires the installation of a Java Runtime
Environment (JRE) Version 1.4 or higher on the user's computer (see [4]).


3. Conversion Utility
---------------------

The purpose of the cfg2xml.jar utility is to convert one or more CommDB settings files of 
the CFG format (*.cfg) to the XML format (*.xml). The utility should be executed as:

  java -jar cfg2xml.jar -mode:[file|dir] schema-dir in [out]

where:

-mode:[file|dir] - specifies whether the utility should process
                   a single file (-mode:file) or a directory with
                   some .cfg files (-mode:dir)
schema-dir   - the directory which contains the XML Schema
                   files which define the version of CommDB
                   appropriate for the settings file to convert
in               - The file in the pre-XML format (*.cfg) to convert,
                   if -mode:file is specified, else the directory
                   which contains .cfg files to convert. When operating
                   in directory mode this utility will also process all
                   subdirectories and their contents for .cfg files
out              - The XML settings file (.xml) to generate as the result
                   of the conversion process. This parameter can be
                   supplied only if the utility operates in file mode
                   (-mode:file). If not specified, the converted file
                   has the same name as the original .cfg one

Note that this conversion can also be performed from within the CommDB XML Editor using the
"Import Old Format File" menu item of the "Actions" menu. Then, the XML CommDB settings
file can be created by using the "Export XML File" menu item. However, the cfg2xml.jar utility
can convert a large number of files automatically when executed in directory mode (-mode:dir)
because in that case it converts all .cfg files encountered in the specified directory
as well as all the whole directory tree under this directory.

cfg2xml.bat can be used: 

  cfgxml -mode:[file¦dir] version in out
  
where parameters mean same as above expect:

version	- version of Symbian OS creating database for, i.e 92, 93 ...

if schema directory needs to be configured to point to a different location the batch file must be edited.  
The batch file itself indicates what needs to be added.

example execution:

cfg2xml -mode:file 92 p:\epoc32\plattestserial.cfg P:\epoc32\WINSCW\C\plattestserial.xml

Also note that the execution of this application requires the installation of a Java Runtime
Environment (JRE) Version 1.4 or higher on the user's computer (see [4]).


4. Requirements
---------------

The execution of the CommDB XML Editor and the utilities requires the installation of a
Java Runtime Environment (JRE) Version 1.4 or higher on the user's computer. This can be
obtained as follows:

i)   For PCs running Windows or Linux and for Sun workstations running Solaris,
     it can be downloaded from Sun's Java web site:

     http://java.sun.com/

     Note that PCs with Microsoft's Java Virtual Machine (JVM) cannot execute these
     applications because this JVM is not up to date. Sun's JRE must be downloaded
     and installed from the link given above.

ii)  For Apple computers running MacOS X 10.2 (Jaguar), 10.3 (Panther) or higher, 
     Apple's JRE 1.4 or higher is pre-installed while updates can be obtained from 
     Apple's Developer web site:

     http://developer.apple.com/java/

     When executed on an Apple computer, the CommDB XML Editor uses the Apple Aqua
     look and feel.

iii) On other UNIX platforms the user should visit the web site of the manufacturer in
     order to obtain an appropriate JRE for his system.

Finally, the documentation in directory

  com/symbian/commdb/doc/

is in HTML format and it can be viewed by any web browser on any computer system.


-- 
Panos Asproulis (01/12/2003)
updated Agnelo Vaz (10/08/2006) - New directory structure for mulitversion support
updated Agnelo Vaz (02/08/2006) - Clarified use of cfg2xml.bat due to defect DEF089584, now specify os version