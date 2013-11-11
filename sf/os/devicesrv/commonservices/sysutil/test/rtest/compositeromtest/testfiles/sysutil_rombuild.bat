@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem

::	IMPORTANT NOTE:
::	Before running this batch file, the following steps need to be done:
::	- Copy estartnandcomp.txt in the testfiles folder to epoc32\rom\h4hrp\unistore2
::
::	- Copy quicknand.bat in the testfiles folder to epoc32\rom\h4hrp\
::
::	- Create the nandloader:
::		- get the source code for base_e32 component using CBR.
::		- navigate to ..\cd cedar\generic\base\e32\rombuild\
::		- run the following command:
::			- rom -v=h4hrp -i=armv5 -b=urel -t=nandloader -m=_NAND2
::		- zip this into a sys$rom.zip and copy it on the MMC card.
::		Note: If this doesn't work you can use sys$rom.bin from the testfiles folder.
::
::	Edit the path of the featuredatabase.xml file for each buildrom command below and 
::	then run this batch file.
::
::	After running this batch file, the following steps need to be done:
::	- Copy core.img, rofs1.img, rofs2.img, rofs3.img and sys$rom.zip to an MMC card. If 
::	  you could not create the nandloader copy sys$rom.bin and setupnand.bat from the 
::	  testfiles folder on to the MMC card instead.
::
::	Upon boot-up of the board the device will go in to the NAND LOADER. The following 
::	steps need to be done:
::	- Select 0 to format the NAND Drive.
::
::	- Press 9 to exit the NAND Loader (this is not an option on the menu).
::
::	- To setup the core image and ROFS sections type the following at the command prompt:
::		- quicknand.bat
::	  Note: If you could not create the nandloader you should type the following instead:
::		- D:
::		- setupnand.bat
::
::	- Run compositeromtest.exe




:: BUILDING CORE IMAGE (core.img) AND ROFS SECTION (rofs1.img)
::
call buildrom -D_FULL_DEBUG -D_NAND2 -DSYMBIAN_BAFL_SYSUTIL_ENABLE_VERSION_SETUP_EXE -D_ROFS_SIZE=20000000 -fm=\epoc32\rom\include\featuredatabase.xml h4hrp textshell sysutil.iby compositeromtest.iby rofs1.iby
ren h4hrp_001.textshell.nand.img core.img
ren h4hrp_001.textshell.nand.rofs1.img rofs1.img

:: BUILDING ROFS SECTION 2 (rofs2.img)
::
call buildrom -D_FULL_DEBUG -D_NAND2 -DSYMBIAN_BAFL_SYSUTIL_ENABLE_VERSION_SETUP_EXE -D_ROFS_SIZE=20000000 -fm=\epoc32\rom\include\featuredatabase.xml rofs2.iby
ren .rofs2.img rofs2.img

:: BUILDING ROFS SECTION 3 (rofs3.img)
::
call buildrom -D_FULL_DEBUG -D_NAND2 -DSYMBIAN_BAFL_SYSUTIL_ENABLE_VERSION_SETUP_EXE -D_ROFS_SIZE=20000000 -fm=\epoc32\rom\include\featuredatabase.xml rofs3.iby
ren .rofs3.img rofs3.img
