// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__INIT_APPARC_SERVER2_H__)
#define __INIT_APPARC_SERVER2_H__

#include <e32base.h>
#include "StartupCommand.h"

class CStartupUtilProvider;

/** Apparc server must have finished building up the list of app
info before non-native apps can be launched. This class facilitates this sequence of
events by waiting for the list to be built in its Execute method.
@internalComponent
@deprecated
*/
class CInitAppArcServer : public CBase, public MStartupCommand
	{
public:
	static CInitAppArcServer* NewL(CStartupUtilProvider& aProvider);
	static CInitAppArcServer* NewLC(CStartupUtilProvider& aProvider);
	void Execute(TRequestStatus& aStatus); 	// from MStartupCommand
	void Release();

private:
	CInitAppArcServer(CStartupUtilProvider& aProvider);

private:
	CStartupUtilProvider& iProvider;
	};

#endif	// __INIT_APPARC_SERVER2_H__
