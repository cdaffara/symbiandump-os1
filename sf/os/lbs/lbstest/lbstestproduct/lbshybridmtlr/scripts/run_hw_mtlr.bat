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

md c:\logs
md c:\logs\lbs
md c:\testdata
md c:\testdata\configs
copy z:\testdata\configs\ctagpshybridmoduledatasource.ini c:\testdata\configs\ctagpshybridmoduledatasource.ini
copy z:\testdata\configs\lbstestconfig.txt c:\testdata\configs\lbstestconfig.txt
copy z:\testdata\configs\lbs_mtlr.ini c:\testdata\configs\lbs_mtlr.ini
testexecute z:\testdata\scripts\uebased_mtlr.script
copy c:\logs e:\logs
