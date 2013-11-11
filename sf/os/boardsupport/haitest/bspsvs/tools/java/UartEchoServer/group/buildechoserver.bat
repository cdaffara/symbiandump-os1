@rem
@rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

javac ..\src\UartEchoServer.java -classpath ..\lib\RXTXcomm.jar;..\lib\commons-cli-1.1.jar -verbose >Log.txt 2>&1
xcopy /R /Y ..\src\UartEchoServer*.class ..\bin\
del /F /Q ..\src\*.class
xcopy /R /Y ..\lib\*.dll ..\bin\
