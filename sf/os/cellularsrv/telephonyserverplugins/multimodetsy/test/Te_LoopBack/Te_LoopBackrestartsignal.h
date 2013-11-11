// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Te_LoopbackCRestartSignal.H
// 
//

/**
 @file
 @internalComponent 
*/


#ifndef __CRESTART_SIGNAL_H__
#define __CRESTART_SIGNAL_H__

#include <e32base.h>

class CATScriptEng;
class CRestartSignal : public CActive
	{
public:
	static CRestartSignal* NewL(CATScriptEng* aScriptEng);
	CRestartSignal(CATScriptEng* aScriptEng);
	void Start(TRequestStatus** aStatus);
	~CRestartSignal();
	void DoCancel();
	void RunL();
private:
	CATScriptEng* iScriptEng;
	};

#endif
