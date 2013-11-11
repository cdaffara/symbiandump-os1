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
@rem tfont.bat
@rem

echo off
cl /P /EP tmisc1.gd
..\rel\fnttran tmisc1.i tmisc1.gdr
cl /P /EP tmisc2.gd
..\rel\fnttran tmisc2.i tmisc2.gdr
cl /P /EP tcrop.gd
..\rel\fnttran tcrop.i tcrop.gdr
cl /P /EP tuncrop.gd
..\rel\fnttran tuncrop.i tuncrop.gdr
fc /B tcrop.gdr tuncrop.gdr
cl /P /EP tfail.gd
..\rel\fnttran tfail.i tfail.gdr
cl /P /EP snow.gd
..\rel\fnttran snow.i snow.gdr
del *.i
