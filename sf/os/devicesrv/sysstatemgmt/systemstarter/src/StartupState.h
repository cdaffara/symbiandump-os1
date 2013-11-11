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

#if !defined(__STARTUPSTATE_H__)
#define __STARTUPSTATE_H__

#include <e32base.h>

class MStartupStateInfo;
class MStartupCommand;

/** Action class controlling individual startup states.
@internalComponent
@deprecated
*/
class CStartupState : public CBase
	{
public:
	static CStartupState* NewL(const MStartupStateInfo& aInfo);
	static CStartupState* NewLC(const MStartupStateInfo& aInfo);
	
public:
	void Start();

private:
	CStartupState(const MStartupStateInfo& aInfo);
	
	void DoCommand(MStartupCommand& aCommand);
	
private:
	const MStartupStateInfo& iStateInfo;
	};

#endif	// __STARTUPSTATE_H__
