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
@echo off
echo " "
echo "Compilation of CommDB Editor and Utilities"
echo "=========================================="

echo "Compile \com\symbian\commdb\data\ directory"
del ..\com\symbian\commdb\data\*.class
javac -g:none -source 1.4 -target 1.4 -O -classpath ..\ ..\com\symbian\commdb\data\*.java

echo "Compile \com\symbian\commdb\convert\ directory"
del ..\com\symbian\commdb\convert\*.class
javac -g:none -source 1.4 -target 1.4 -O -classpath ..\ ..\com\symbian\commdb\convert\*.java

echo "Compile \com\symbian\commdb\ui\ directory"
del ..\com\symbian\commdb\ui\*.class
javac -g:none -source 1.4 -target 1.4 -O -classpath ..\ ..\com\symbian\commdb\ui\*.java

echo "Compile \com\symbian\commdb\xml\ directory"
del ..\com\symbian\commdb\xml\*.class
javac -g:none -source 1.4 -target 1.4 -O -classpath ..\ ..\com\symbian\commdb\xml\*.java

echo "Generate JAR files"
del *.jar
cd ..
jar cvfm bin\cxmled.jar bin\Manifest-cxmled.mf com\symbian\commdb\data\*.class com\symbian\commdb\ui\*.class com\symbian\commdb\xml\*.class com\symbian\commdb\various\*
jar cvfm bin\cfg2xml.jar bin\Manifest-cfg2xml.mf com\symbian\commdb\data\*.class com\symbian\commdb\convert\*.class com\symbian\commdb\xml\*.class
cd bin