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


if EXIST \os\mm\mmtestenv\mmtesttools\scripts\secdisp_ini_append.pl goto DABSMODE

:MISTRALMODE
cd %EPOCROOT%..\src\sf\os\mm\mmtestenv\mmtesttools\scripts
goto COMMON

:DABSMODE
cd \os\mm\mmtestenv\mmtesttools\scripts

:COMMON

perl secdisp_ini_append.pl restore secdisp
