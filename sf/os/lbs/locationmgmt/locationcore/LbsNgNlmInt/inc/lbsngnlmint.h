/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#ifndef LBSNGNLMINT_H
#define LBSNGNLMINT_H

#include <e32std.h>
#include <e32property.h>
#include "lbscommoninternaldatatypes.h"

/**
@file
@internalTechnology
@released
*/

//
// Message classes between the Network Location Manager and Network Gateway
//

const TInt32 KMaxLbsNetLocMsgSize = 128;

/** Base class for network location - related messages
*/
NONSHARABLE_CLASS(TLbsNetLocMsgBase)
	{
public:
	enum TLbsNetLocMsgType
		{
		ENetLocMsgUnknown = 0,
		ENetLocMsgNetworkLocationRequest,
		ENetLocMsgNetworkLocationCancel,
		ENetLocMsgNetworkLocationComplete
		};
		
public:
	IMPORT_C TLbsNetLocMsgBase();
	IMPORT_C TLbsNetLocMsgBase(const TLbsNetLocMsgBase& aSrc);
	
	IMPORT_C TLbsNetLocMsgType Type() const;
	IMPORT_C const TLbsNetSessionIdInt& SessionId() const;
	IMPORT_C TInt32 Size() const;

	IMPORT_C TLbsNetLocMsgBase& operator= (const TLbsNetLocMsgBase& aSrc);

protected:
	// Derived classes set their type by using this constructor.
	TLbsNetLocMsgBase(TLbsNetLocMsgType aType, 
					  TInt32 aSize,
					  const TLbsNetSessionIdInt& aSessionId);	

private:
	// All messages have a type and session identifier
	TLbsNetLocMsgType iType;
	TInt32 iSize;
	TLbsNetSessionIdInt iSessionId;
	
protected:	
	// A TUint64 is used to ensure that any objects that use the buffer 
	// are at least 8byte aligned. (The compiler will organise the arrangement
	// of iBuffer within TLbsNetInternalMsgBase to be aligned to the minimum
	// for a TUint64, which is 8bytes.)
	/*TUint64 iBuffer[((256 - (sizeof(TLbsNetLocMsgType) 
					  + sizeof(TLbsNetSessionId))) >> 3)];*/
	};

/** A Network Location Request message class.
*/
NONSHARABLE_CLASS(TLbsNetLocNetworkLocationRequestMsg) : public TLbsNetLocMsgBase
	{
public:
	IMPORT_C TLbsNetLocNetworkLocationRequestMsg(const TLbsNetSessionIdInt& aSessionId,
												 TBool aNewClient,
												 TLbsNetPosRequestQualityInt aQuality);
												 
	IMPORT_C TBool NewClient() const;
	IMPORT_C const TLbsNetPosRequestQualityInt& Quality() const;
	
private:
	TLbsNetLocNetworkLocationRequestMsg();
	
private:
	TBool iNewClient;
	TLbsNetPosRequestQualityInt iQuality;
	};
	
/** A Network Location Cancel message class.
*/	
NONSHARABLE_CLASS(TLbsNetLocNetworkLocationCancelMsg) : public TLbsNetLocMsgBase
	{
public:
	IMPORT_C TLbsNetLocNetworkLocationCancelMsg(const TLbsNetSessionIdInt& aSessionId,
												TInt aReason);
	
	IMPORT_C TInt Reason() const;
	
private:
	TLbsNetLocNetworkLocationCancelMsg();
	
private:
	TInt iReason;
	};
	
/** A Network Location Complete message class.
*/
NONSHARABLE_CLASS(TLbsNetLocNetworkLocationCompleteMsg) : public TLbsNetLocMsgBase
	{
public:
	IMPORT_C TLbsNetLocNetworkLocationCompleteMsg(const TLbsNetSessionIdInt& aSessionId,
												  TInt aReason);
	
	IMPORT_C TInt Reason() const;
	
private:
	TLbsNetLocNetworkLocationCompleteMsg();

private:
	TInt iReason;
	};

//
// Classes that interface between the Network Gateway 
// and Network Location Manager
//

/** Class used to send network location requests to the Network Gateway.
*/
NONSHARABLE_CLASS(RLbsNetworkLocationRequests)
	{
public:
	IMPORT_C static void InitializeL();
	IMPORT_C static void ShutDownL();

	IMPORT_C RLbsNetworkLocationRequests();
	IMPORT_C void OpenL(TUid aModuleId);
	IMPORT_C void Close();

	IMPORT_C TInt GetNetworkLocationRequest(TLbsNetLocMsgBase& aMessage);
	IMPORT_C TInt SetNetworkLocationRequest(const TLbsNetLocMsgBase& aMessage);
	
	IMPORT_C void NotifyNetworkLocationRequestUpdate(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyNetworkLocationRequestUpdate();
	
private:
	RLbsNetworkLocationRequests(const RLbsNetworkLocationRequests&);
	RLbsNetworkLocationRequests& operator=(const RLbsNetworkLocationRequests&);

private:
	RProperty iNetLocRequestProperty;
	};

/** Class used to send the response to network location requests to
the Network Location Manager.

*/
NONSHARABLE_CLASS(RLbsNetworkLocationResponses)
	{
public:
	IMPORT_C static void InitializeL();
	IMPORT_C static void ShutDownL();

	IMPORT_C RLbsNetworkLocationResponses();
	IMPORT_C void OpenL(TUid aModuleId);
	IMPORT_C void Close();

	IMPORT_C TInt GetNetworkLocationResponse(TLbsNetLocMsgBase& aMessage);
	IMPORT_C TInt SetNetworkLocationResponse(const TLbsNetLocMsgBase& aMessage);
	
	IMPORT_C void NotifyNetworkLocationResponseUpdate(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyNetworkLocationResponseUpdate();

private:
	RLbsNetworkLocationResponses(const RLbsNetworkLocationResponses&);
	RLbsNetworkLocationResponses& operator=(const RLbsNetworkLocationResponses&);

private:
	RProperty iNetLocResponseProperty;
	};
		
#endif // LBSNGNLMINT_H
