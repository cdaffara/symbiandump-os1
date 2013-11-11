// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll 
*/

#ifndef CMOCKLTSYENGINE_H
#define CMOCKLTSYENGINE_H

#include "cmocksyengine.h"

#include <ctsy/ltsy/mltsydispatchfactory.h>


class CCtsyDispatcherCallback;
class MMockMessHandlerBase;
class CMockCallControlMessHandler;
class CMockPhoneMessHandler;
class CMockSecurityMessHandler;
class CMockPhonebookEnMessHandler;
class CMockCellBroadcastMessHandler;
class CMockPhonebookOnMessHandler;
class CMockPhonebookMessHandler;
class CMockSimMessHandler;
class CMockSmsMessHandler;
class CMockCallControlMultipartyMessHandler;
class CMockSupplementaryServicesMessHandler;
class CMockPacketServicesMessHandler;
class CMockSatMessHandler;

class CMockLtsyEngine : public CMockSYEngine
	{
public:
	static CMockLtsyEngine* NewL(CCtsyDispatcherCallback& aCallback);
	
public:
	~CMockLtsyEngine();
	virtual void DoCompleteEventL(const TMockSYEvent& aEvent);
	virtual TBool CanIgnoreUnexpectedIpc(TInt aCmdId);
 	void LogData(const TDesC& aDesc);
 	
 	TInt ExtFuncL(TInt aFuncUnitId, TInt aInterfaceId,...);
 	TBool IsInterfaceSupported(const TInt aFuncUnitId, TLtsyDispatchInterfaceApiId aDispatchApiId);
 	void CMockLtsyEngine::IsCallbackIndSupported(const TInt /*aFuncUnitId*/, TLtsyDispatchIndIdGroup /*aIdGroup*/, TUint32& aIndIdBitMask);
 	
protected:
	CMockLtsyEngine(CCtsyDispatcherCallback& aCallback);
	void ConstructL();
	
protected:
	virtual void LogRequest(TInt aCmdId, const MMockSyEngineData& aData,TInt aResultCode);
	virtual void LogCompletion(TInt aCmdId, const TDesC8& aData,TInt aResultCode);
	virtual void LogExpectError(TInt aCmdId, const MMockSyEngineData& aData,TInt aExpectedCmd,const TDesC8& aExpectedData, TBool aIsErrorIgnored=EFalse);
  
private:
	MMockMessHandlerBase* RouteRequestUsingFUId(TInt aFuncUnitId);
	MMockMessHandlerBase* RouteRequestUsingApiId(TInt aApiId);
	
	
private: // Unowned
	CCtsyDispatcherCallback& iCompletionCallback;
	
private: // Owned
	CServer2* iMockServer;
	CMockCallControlMessHandler* iMockCallControlMessHandler;
	CMockPhoneMessHandler* iMockPhoneMessHandler;
	CMockSecurityMessHandler* iMockSecurityMessHandler;
	CMockPhonebookEnMessHandler* iMockPhonebookEnMessHandler;
	CMockCellBroadcastMessHandler* iMockCellBroadcastMessHandler;
	CMockPhonebookOnMessHandler* iMockPhonebookOnMessHandler;
	CMockPhonebookMessHandler* iMockPhonebookMessHandler;
	CMockSimMessHandler* iMockSimMessHandler;
	CMockSmsMessHandler* iMockSmsMessHandler;
	CMockCallControlMultipartyMessHandler* iMockCallControlMultipartyMessHandler;
	CMockSupplementaryServicesMessHandler* iMockSupplementaryServicesMessHandler;
	CMockPacketServicesMessHandler* iMockPacketServicesMessHandler;
	CMockSatMessHandler* iMockSatMessHandler;
	
	
	}; // class CMockLtsyEngine
	
#endif // CMOCKLTSYENGINE_H
