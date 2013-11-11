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
// Definition of class used to launch a LBS processes (non-server)
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __LBSPROCESSLAUNCH_H__
#define __LBSPROCESSLAUNCH_H__

#include <e32base.h>
#include "tprocessstartparams.h"

/**
@internalTechnology
@released

CBase based Class, presents the launch process of the server side
 */
 class CProcessLaunch : public CBase
	{
public:
	static TInt CreateProcess();
	static TInt CreateProcess(TProcessStartParams& aParams);
	static TInt ProcessLaunch(TProcessStartParams& aParams);
protected:
	static TInt DoCreateProcessL();
	static TInt DoCreateProcessL(TProcessStartParams& aParams);
	static TInt DoProcessLaunch(TProcessStartParams& aParams);
	};
	
#endif // __LBSPROCESSLAUNCH_H__
