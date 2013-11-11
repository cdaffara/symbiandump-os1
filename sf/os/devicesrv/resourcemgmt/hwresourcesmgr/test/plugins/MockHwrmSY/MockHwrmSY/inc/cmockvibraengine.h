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
// MockSY engine for the Vibra plugin
// 
//

#ifndef CMOCKVIBRAENGINE_H
#define CMOCKVIBRAENGINE_H

#include "cmockhwrmpluginengine.h"
#include "tmockhwrmplugindata.h"

class CMockHwrmPluginSY;

class CMockVibraEngine : public CMockHwrmPluginEngine
	{
public:
	static CMockVibraEngine* NewL(CMockHwrmPluginSY& aMockHwrmPluginSY);

private:
	CMockVibraEngine(CMockHwrmPluginSY& aMockHwrmPluginSY);
	
protected:
	virtual void LogRequest(TInt aCmdId, const MMockSyEngineData& aData,TInt aResultCode);
	virtual void LogCompletion(TInt aCmdId, const TDesC8& aData,TInt aResultCode);
	virtual void LogExpectError(TInt aCmdId, const MMockSyEngineData& aData,TInt aExpectedCmd,const TDesC8& aExpectedData);	
	virtual void QueueProcessedCompletionL(const TInt aCommandId, const TUint8 aTransId, RBuf8& aBuffer, TDesC8& /*aData*/);
	virtual void FurtherProcessEventL(const TInt aCommandId, TDesC8& aData);	
	
private:
	void DoLogError(TInt aCmdId, const TMockHwrmPluginData& aData,TInt aExpectedCmd,const TMockHwrmPluginData& aExpectedData);
	void DoLogCommandData(TInt aCmdId, const TMockHwrmPluginData& aCmdData, const TDesC& aComment);
	const TDesC& CommandName(TInt aCmdId);
	};
	
#endif
