/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef LDDASYNCREQUEST_H_
#define LDDASYNCREQUEST_H_

#include <mmc.h>


class DLddAsyncRequest ;

/**
 * DLddAsyncRequest observer interface
 *
 */
class MAsyncRequestObserver
	{
public:
	virtual void Notify(DLddAsyncRequest& aRequest) = 0; 
	};

/**
 * Wraps a TRequestStatus along with a function (command) id
 *
 */
class DLddAsyncRequest 
	{
public:
	DLddAsyncRequest(TInt aFunctionId, TRequestStatus* aStatus, MAsyncRequestObserver& aObserver);
	virtual ~DLddAsyncRequest();
	TRequestStatus* Status();
	TInt Error();
	TInt FunctionId();
	
protected:
	TInt iFunctionId;	
	TRequestStatus* iStatus;	
	MAsyncRequestObserver& iObserver;
	
	TInt iError;
	};

/**
 * Extends DLddAsyncRequest to add DMMCSession async functionality
 *
 */
class DMmcSessionAsyncRequest : public DLddAsyncRequest 
	{
public:	
	DMmcSessionAsyncRequest(TInt aFunctionId, TRequestStatus* aStatus, 
			MAsyncRequestObserver& aObserver, TDynamicDfcQue* aDfcQ, DMMCStack& aStack, TMMCard& aCard);
	~DMmcSessionAsyncRequest();
	DMMCSession& Session();	
public:
	static void SessionEndCallBack(TAny *aPtr);
    static void SessionEndDfc(TAny *aPtr);
	void Reset();

private:
	DMMCSession* iSession;
    TMMCCallBack iSessionEndCallBack;
	TDfc iSessionEndDfc;
	
public:
	TAny* iClientDesPtr;
	TUint32 iBlkOffet;
	TUint8* iBuf;

private:
	void SessionEndCallBack();
	void SessionEndDfc();
	};


/**
 * Extends DLddAsyncRequest to add TPBusCallBack (DMMCSocket) async functionality
 * used to handle callbacks for Bus events
 *
 */
class DBusEventRequest : public DMmcSessionAsyncRequest
	{
public:
	DBusEventRequest(TInt aFunctionId, TRequestStatus* aStatus, MAsyncRequestObserver& aObserver, TInt aUnit,
						TDynamicDfcQue* aDfcQ, DMMCStack& aStack, TMMCard& aCard);
	~DBusEventRequest();
	
	TPBusState BusState() const { return (TPBusState) iBusState; }
	TInt BusError() const { return iBusError; }
public:
	static void BusEventCallBack(TAny* aPtr, TInt aReason, TAny* a1, TAny* a2);
private:
	TPBusCallBack	iBusEventCallBack;
private:
	void BusEventCallBack(TInt aReason, TAny* a1, TAny* a2);
	TPBusState iBusState;
	TInt iBusError;	
	};

#endif /*LDDASYNCREQUEST_H_*/
