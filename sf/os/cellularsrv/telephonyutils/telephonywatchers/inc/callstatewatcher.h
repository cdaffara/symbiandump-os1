// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CALLSTATEWATCHER_H__
#define __CALLSTATEWATCHER_H__

// System includes
#include <e32base.h>


class CCallStateWatcher : public CActive
/**
@internalComponent
*/
	{
public:
	~CCallStateWatcher();
	CCallStateWatcher(RMobilePhone& aPhone, TInt aPriority = CActive::EPriorityStandard);
	
	TInt StartCallStateMonitor();

private:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	TInt GetCallState();

private:
	RMobilePhone& iPhone;
	RLine iLine;
	RMobileCall iCall;
	RMobileCall::TMobileCallStatus iCallState;
	};


#endif
