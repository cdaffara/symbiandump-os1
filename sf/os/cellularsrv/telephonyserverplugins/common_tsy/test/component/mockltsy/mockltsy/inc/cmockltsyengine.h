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
//

#ifndef CMOCKLTSYENGINE_H
#define CMOCKLTSYENGINE_H

#include "cmocksyengine.h"

class CMmMessageRouter;
class CMmDataPackage;

class CMockLtsyEngine : public CMockSYEngine
	{
public:
	static CMockLtsyEngine* NewL(CMmMessageRouter* aMsgRouter);
public:
	
	virtual void DoCompleteEventL(const TMockSYEvent& aEvent);
	virtual TBool CanIgnoreUnexpectedIpc(TInt aCmdId);
 	void LogData(const TDesC& aDesc);
 	
protected:
	CMockLtsyEngine(CMmMessageRouter* aMsgRouter);
	
protected:
	virtual void LogRequest(TInt aCmdId, const MMockSyEngineData& aData,TInt aResultCode);
	virtual void LogCompletion(TInt aCmdId, const TDesC8& aData,TInt aResultCode);
	virtual void LogExpectError(TInt aCmdId, const MMockSyEngineData& aData,TInt aExpectedCmd,const TDesC8& aExpectedData, TBool aIsErrorIgnored=EFalse);	
	
private:
	CMmMessageRouter* iMsgRouter;
	};
	
#endif
