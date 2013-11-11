// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __RESTARTSYS_H__
#define __RESTARTSYS_H__

#include <e32base.h>

/**
This class provides static functions to safely restart the system.
@internalComponent
@released
*/
class RestartSys
	{
public:
	IMPORT_C static TInt RestartSystem();
	IMPORT_C static TInt RestartSystem(TInt aStartupMode);
	};

#endif  // __RESTARTSYS_H__
