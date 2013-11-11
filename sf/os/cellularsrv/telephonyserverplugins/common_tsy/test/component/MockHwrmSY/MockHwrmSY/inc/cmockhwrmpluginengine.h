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
// MockSY engine for Hwrm plugin (Light and Vibra)
// 
//

#ifndef CMOCKHWRMPLUGINENGINE_H
#define CMOCKHWRMPLUGINENGINE_H

#include "cmocksyengine.h"

class CMockHwrmPluginSY;

class CMockHwrmPluginEngine : public CMockSYEngine
	{
public:
	virtual void DoQueueEventL(TMockSYEvent::TEventType aType, TInt aCmdId, HBufC8* aData, 
		TInt aResultCode, TBool aLeave, TInt aDelay);

	TInt ExecuteProcessCommandL(const TInt aCommandId, const TUint8 aTransId, TDesC8& aData);
	TInt ExecuteCancelCommandL(const TUint8 aTransId, const TInt aCommandId);

	virtual void DoCompleteEventL(const TMockSYEvent& aEvent);

protected:
	CMockHwrmPluginEngine(CMockHwrmPluginSY& aMockHwrmPluginSY);
	
	void QueueProcessedEventL(const TInt aCommandId, const TUint8 aTransId, TDesC8& aData);
	virtual void Reset();

protected:
	virtual void LogRequest(TInt aCmdId, const MMockSyEngineData& aData,TInt aResultCode);
	virtual void LogCompletion(TInt aCmdId, const TDesC8& aData,TInt aResultCode);
	virtual void LogExpectError(TInt aCmdId, const MMockSyEngineData& aData,TInt aExpectedCmd,const TDesC8& aExpectedData);	
	virtual void QueueProcessedCompletionL(const TInt aCommandId, const TUint8 aTransId, RBuf8& aBuffer, TDesC8& aData)=0;
	virtual void FurtherProcessEventL(const TInt aCommandId, TDesC8& aData)=0;
private:
	TInt iLatestTransId;
	TInt iNumStarted; //Number of processcommand received
	CMockHwrmPluginSY& iMockHwrmPluginSY;
	};
	
#endif
