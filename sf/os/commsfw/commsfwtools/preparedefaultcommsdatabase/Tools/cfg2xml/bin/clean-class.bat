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
echo "Clean .class files of CommDB Editor and Utilities"
echo "================================================="

echo "Clean \com\symbian\commdb\data\ directory"
del ..\com\symbian\commdb\data\*.class

echo "Clean \com\symbian\commdb\convert\ directory"
del ..\com\symbian\commdb\convert\*.class

echo "Clean \com\symbian\commdb\ui\ directory"
del ..\com\symbian\commdb\ui\*.class

echo "Clean \com\symbian\commdb\xml\ directory"
del ..\com\symbian\commdb\xml\*.class
