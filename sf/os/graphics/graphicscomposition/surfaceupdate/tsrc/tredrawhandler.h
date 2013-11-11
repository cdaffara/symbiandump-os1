// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TREDRAWHANDLER_H__
#define __TREDRAWHANDLER_H__

#include <e32base.h>

class RWsSession;

/**
 * The class will listen all redraw commands associated with any of the test windows.
 * It will ask the windows to update themselves.
 */
class CTRedrawHandler : public CActive
	{
public:
	CTRedrawHandler(RWsSession& aWs);
	virtual ~CTRedrawHandler();
	void Start();
private:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
	RWsSession& iWs;
	};
	
#endif //__TREDRAWHANDLER_H__
