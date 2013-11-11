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
// MockSY engine for Hwrm PowerState plugin
// 
//

#ifndef CMOCKPOWERSTATEENGINE_H
#define CMOCKPOWERSTATEENGINE_H

#include "cmocksyengine.h"

class CMockPowerStateSY;

class CMockPowerStateEngine : public CMockSYEngine
	{
public:
	static CMockPowerStateEngine* NewL(CMockPowerStateSY& aMockPowerStateSY);

	virtual void DoCompleteEventL(const TMockSYEvent& aEvent);

protected:
	CMockPowerStateEngine(CMockPowerStateSY& aMockHwrmPluginSY);
	
protected:
//	virtual void LogRequest(TInt aCmdId, const MMockSyEngineData& aData,TInt aResultCode);
//	virtual void LogCompletion(TInt aCmdId, const TDesC8& aData,TInt aResultCode);
//	virtual void LogExpectError(TInt aCmdId, const MMockSyEngineData& aData,TInt aExpectedCmd,const TDesC8& aExpectedData);	
	
private:
	CMockPowerStateSY& iMockPowerStateSY;
	};
	
#endif
