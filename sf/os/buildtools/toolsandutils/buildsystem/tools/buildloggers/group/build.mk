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

SRCDIR = ..\src
JARDIR = ..\..\..\bin\java
PKGDIR = com\symbian\ant
JAVAC = $(JAVA_HOME)\bin\javac -cp $(ANT_HOME)\lib\ant.jar

JARFILE = $(JARDIR)\symbianant.jar

ALL: $(JARFILE)

$(SRCDIR)\$(PKGDIR)\AbldWhatLogger.class: $(SRCDIR)\$(PKGDIR)\AbldWhatLogger.java
	$(JAVAC) $<
	
$(SRCDIR)\$(PKGDIR)\ScanLogger.class: $(SRCDIR)\$(PKGDIR)\ScanLogger.java
	$(JAVAC) $<
	
$(JARFILE): $(SRCDIR)\$(PKGDIR)\AbldWhatLogger.class $(SRCDIR)\$(PKGDIR)\ScanLogger.class
	$(JAVA_HOME)\bin\jar -cf $@ -C $(SRCDIR) $(PKGDIR)\AbldWhatLogger.class -C $(SRCDIR) $(PKGDIR)\ScanLogger.class

CLEAN:
	-del $(SRCDIR)\$(PKGDIR)\*.class
	-del $(JARFILE)