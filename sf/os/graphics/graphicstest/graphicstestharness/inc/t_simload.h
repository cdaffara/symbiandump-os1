// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @test
*/

#ifndef __T_SIM_LOAD__H__
#define __T_SIM_LOAD__H__

#include <e32std.h>

const TInt KNumSimLoadApps = 3;

_LIT(KSimLoadApp,          "z:\\sys\\bin\\t_simloadapp%d.exe");
_LIT(KSimLoadConfigFile,   "c:\\graphics\\tsimloadconfig.ini");
_LIT(KConfigSimLoadValue,  "SimLoadValue");
_LIT(KConfigSimLoadType,   "SimLoadType");
_LIT(KConfigSimLoadStatic, "KSimLoadStatic");
_LIT(KConfigSimLoadSpiked, "KSimLoadSpiked");

#endif //__T_SIM_LOAD_APP_H__

