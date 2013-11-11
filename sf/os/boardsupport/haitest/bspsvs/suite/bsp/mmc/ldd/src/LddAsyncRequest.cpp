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


#include "mmcldd.h"
#include "d_mmcsdif.h"
#include "LddAsyncRequest.h"

/**
 * DLddAsyncRequest constructor
 *
 * @param TInt aFunctionId - functionId 
 * @param TRequestStatus* aStatus	for signalling upon completion
 * @param MAsyncRequestObserver& aObserver	- observer recieves notification upon completion 
 *
 * @return N/A
 *
 * @leave N/A					
 */
DLddAsyncRequest::DLddAsyncRequest(TInt aFunctionId, TRequestStatus* aStatus, MAsyncRequestObserver& aObserver) 
: iFunctionId(aFunctionId)
, iStatus(aStatus)
, iObserver(aObserver)
, iError(KErrNone) 
	{
	}

/**
 * Destructor 
 *
 * @return N/A
 *
 * @leave	N/A				
 */
DLddAsyncRequest::~DLddAsyncRequest()
	{
	}

/**
 * Get the error code for this request 
 *
 *
 * @return TInt the error code
 *
 * @leave	 N/A				
 */
TInt DLddAsyncRequest::Error() 
	{ 
	return iError; 
	}

/**
 * Get the function Id for this request
 *
 * @return TInt the function Id
 *
 * @leave N/A					
 */
TInt DLddAsyncRequest::FunctionId() 
	{ 
	return iFunctionId; 
	}

/**
 * Get the status of this request 
 *
 *
 * @return TRequestStatus status of the request
 *
 * @leave N/A					 
 */
TRequestStatus* DLddAsyncRequest::Status()
	{
	return iStatus;
	}


///////////////////////////////////////////////////////////////////////
//////////////////// DMmcSessionAsyncRequest //////////////////////////
///////////////////////////////////////////////////////////////////////

/**
 * DMmcSessionAsyncRequest Constructor  
 *
 * @param TInt aFunctionId - functionId 
 * @param TRequestStatus* aStatus	for signalling upon completion
 * @param MAsyncRequestObserver& aObserver	- observer recieves notification upon completion 
 * @param TDynamicDfcQue* aDfcQ - pointer to dfc queue,  	 
 * @param DMMCStack& aStack	stack upon which this request will operate
 * @param TMMCard& aCard multimedia card upon which this request will operate
 *
 * 
 * @return N/A
 *
 * @leave N/A					
 */
DMmcSessionAsyncRequest::DMmcSessionAsyncRequest(TInt aFunctionId, TRequestStatus* aStatus, 
		MAsyncRequestObserver& aObserver, TDynamicDfcQue* aDfcQ, DMMCStack& aStack, TMMCard& aCard)
: DLddAsyncRequest(aFunctionId, aStatus, aObserver)
, iSession(NULL)
, iSessionEndCallBack(DMmcSessionAsyncRequest::SessionEndCallBack, this)
, iSessionEndDfc(DMmcSessionAsyncRequest::SessionEndDfc, this, 1)
, iClientDesPtr(NULL)
, iBlkOffet(0)
, iBuf(NULL)
	{
	TInt bufLen;
	TInt minorBufLen;
	aStack.BufferInfo(iBuf, bufLen, minorBufLen);
	
	iSession = aStack.AllocSession(iSessionEndCallBack);
	iSession->SetStack(&aStack);
	iSession->SetCard(&aCard);

	iSessionEndDfc.SetDfcQ(aDfcQ);
	
	Reset();
	}

/**
 * Destructor
 *
 * @return N/A
 *
 * @leave N/A					
 */
DMmcSessionAsyncRequest::~DMmcSessionAsyncRequest()
	{
	Reset();
	delete iSession;
	}

void DMmcSessionAsyncRequest::Reset()
{
	MMCSDLOG("iSessionEndDfc.Cancel() called");	
	iSessionEndDfc.Cancel();
}


/**
 * Gets the async requests DMMCSession 
 *
 * @return reference to the DMMCSession
 *
 * @leave N/A					
 */
DMMCSession& DMmcSessionAsyncRequest::Session()
	{
	return *iSession;
	}

/**
 * The session end callback (static)- called when and engaged session completes 
 *
 * @param 	TAny* in this case this will be a pointer to the DMmcSessionAsyncRequest 			
 *
 * @return void
 *
 * @leave	N/A				
 */
void DMmcSessionAsyncRequest::SessionEndCallBack(TAny *aPtr)
	{
	((DMmcSessionAsyncRequest*)aPtr)->SessionEndCallBack();
	}


/**
 * Utility function called from static overload SessionEndCallBack(TAny*)
 *
 * @return void
 *
 * @leave		N/A			
 */
void DMmcSessionAsyncRequest::SessionEndCallBack()
	{
	MMCSDLOG("DMmcSessionAsyncRequest::SessionEndCallBack called");
	iError = iSession->EpocErrorCode();

	if (NKern::CurrentContext()==NKern::EInterrupt)
		{
		iSessionEndDfc.Add();
		}
	else
		{
		// Signal request complete using DFC
		if (!iSessionEndDfc.Queued())
			{
			iSessionEndDfc.Enque();
			}
		}
	}


/**
 * Session deferred function call (static)
 *
 * @param 	TAny* in this case this will be a pointer to the DMmcSessionAsyncRequest 			
 *
 * @return void
 *
 * @leave	N/A				
 */
void DMmcSessionAsyncRequest::SessionEndDfc(TAny *aPtr)
	{
	((DMmcSessionAsyncRequest*) aPtr)->SessionEndDfc();
	}


/**
 * Utility function called from static overload SessionEndDfc(TAny*)
 *
 * @return void
 *
 * @leave		N/A			
 */
void DMmcSessionAsyncRequest::SessionEndDfc()
	{
	MMCSDLOG2("DLddAsyncRequest::SessionEndDfc called err(%d)", iError);	
	iObserver.Notify(*this);
	MMCSDLOG("DLddAsyncRequest::SessionEndDfc finished");
	}



/**
 * DBusEventRequest constructor
 *
 * @param TInt aFunctionId - Function ID
 * @param TRequestStatus* aStatus - Request Status
 * @param MAsyncRequestObserver& aObserver - Asynchronous Request observer interface
 * @param TInt aUnit - Socket ID
 *
 * @return N/A
 *
 * @leave N/A
 */
DBusEventRequest::DBusEventRequest(TInt aFunctionId, TRequestStatus* aStatus, MAsyncRequestObserver& aObserver, TInt aUnit,
									TDynamicDfcQue* aDfcQ, DMMCStack& aStack, TMMCard& aCard) 
: DMmcSessionAsyncRequest(aFunctionId, aStatus, aObserver, aDfcQ, aStack, aCard)
, iBusEventCallBack(DBusEventRequest::BusEventCallBack, this)
	{
	iBusEventCallBack.SetSocket(aUnit);
	iBusEventCallBack.Add();
	
	}
/**
 * Destructor
 *
 * @return N/A
 *
 * @leave N/A
 */
DBusEventRequest::~DBusEventRequest()
	{
	iBusEventCallBack.Remove();
	}

/**
 * Callback when request has completed
 *
 * @param TAny* aPtr - Pointer to the bus event request
 * @param TInt aReason - Completion error code
 * @param TAny* a1 - Given parameter
 * @param TAny* a2 - Given parameter
 *
 * @return N/A
 *
 * @leave N/A
 */
void DBusEventRequest::BusEventCallBack(TAny* aPtr, TInt aReason, TAny* a1, TAny* a2)
	{	
	((DBusEventRequest*)aPtr)->BusEventCallBack(aReason, a1, a2);	
	}

/**
 * Callback when request has completed
 *
 * @param TInt aReason - Completion error code
 * @param TAny* a1 - Given parameter
 * @param TAny* a2 - Given parameter
 *
 * @return N/A
 *
 * @leave N/A
 */
void DBusEventRequest::BusEventCallBack(TInt aReason, TAny* a1, TAny* a2)
	{
	
	MMCSDLOG("DBusEventRequest::BusEventCallBack called");
	iError = aReason;
	iBusState = (TPBusState)((TInt) a1);
	iBusError = (TInt) a2;
	MMCSDLOG4("DBusEventRequest::BusEventCallBack iBusState(%d) iBusError(%d) iError(%d)", iBusState, iBusError, iError);
	iObserver.Notify(*this);
	MMCSDLOG("DBusEventRequest::BusEventCallBack finished");
	}
