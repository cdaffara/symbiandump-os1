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
// This file contains all the code for the retrieval classes which retrieve variable length 
// buffers from the TSY, such as MBMS monitor service list and MBMS sessions. 
// 
//

/**
 @file
*/

#include "pcktretrieve.h"
#include <pcktcs.h>
#include <etelext.h>

//
//
// Retrieve a list of MBMS monitored service entries 
//
//
EXPORT_C CRetrievePcktMbmsMonitoredServices* CRetrievePcktMbmsMonitoredServices::NewL(RPacketService& aService)
/** Creates an instance of the CActive derived CRetrievePcktMbmsMonitoredServices 
 * class.
 * @param aService An instance of RPacketService over which the list retrieval class operates.
 * @return The newly created CRetrievePcktMbmsMonitoredServices class. 
 * @capability None
 */
	{
	CRetrievePcktMbmsMonitoredServices* self = new(ELeave) CRetrievePcktMbmsMonitoredServices(aService);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CRetrievePcktMbmsMonitoredServices::CRetrievePcktMbmsMonitoredServices(RPacketService& aService) 
:	CAsyncRetrieveWithClientIds(aService.SessionHandle().Handle(), aService.SubSessionHandle()),
	iService(aService)
/**
 * Standard constructor; initialises the data members. 
 */
	{
	}

void CRetrievePcktMbmsMonitoredServices::ConstructL()
/**
 * Part of 2 phase construction, assigns the IPCs to be used 
 * in 2 phase retrieval mechanism.
 */
	{
	iIpcPhase1=EPacketGetMbmsMonitoredServicesPhase1;
	iIpcPhase2=EPacketGetMbmsMonitoredServicesPhase2;
	iIpcCancel=EPacketGetMbmsMonitoredServicesCancel;
	}

EXPORT_C CRetrievePcktMbmsMonitoredServices::~CRetrievePcktMbmsMonitoredServices()
/** 
 * The destructor frees all resources owned by the object, prior to its destruction.
 */
	{
	Cancel();
	delete iResults;
	}

EXPORT_C CPcktMbmsMonitoredServiceList* CRetrievePcktMbmsMonitoredServices::RetrieveListL()
/** 
 * Provides the client with a handle to the appropriate 
 * MBMS service availability list class retrieved. The client will call this function 
 * once Start() has completed.
 * The ownership of the list object is transferred to the client and the client 
 * is responsible for eventual deletion of this object.	 
 *
 * @leave KErrNotFound If there is no valid list object to return to the client.
 * @return A handle to the list class retrieved from the store. If there are no entries in 
 * the list then a handle to an empty list is returned.
 * @capability ReadDeviceData
 */
	{
	if (iResults)
		{
		// Give ownership of list to caller of this method
		CPcktMbmsMonitoredServiceList* ptr=iResults;
		iResults = NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}
	
EXPORT_C void CRetrievePcktMbmsMonitoredServices::Start(TRequestStatus& aReqStatus)
/** Starts the two-phase list retrieval of all entries within the store.
 * 
 * @param aReqStatus On completion, the status of the request; KErrNone if successful, 
 * KErrNotSupported if the phone does not support access to an MBMS service availability list  
 * and KErrNotFound if the list does not exist. If the list is empty then KErrNone
 * is returned.
 * @capability ReadDeviceData
 */
	{
	if (!CompleteIfInUse(aReqStatus))
		{
	   	delete iResults;
		iResults = NULL;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iId,&iId);
		}
	}
		
void CRetrievePcktMbmsMonitoredServices::RestoreListL()
/**
 * Restores a list from a buffer that contains the streamed version of the list
 */
	{
	iResults=CPcktMbmsMonitoredServiceList::NewL();
	iResults->RestoreL(iResultsPtr);
	FreeBuffer();
	}
	
void CRetrievePcktMbmsMonitoredServices::CancelReq(TInt aIpc1, TInt aIpc2)
/**
 *  Cancels previously issued request
 */
	{
	iService.CancelReq(aIpc1, aIpc2);
	}
		
void CRetrievePcktMbmsMonitoredServices::Get(TInt aIpc, TRequestStatus& aReqStatus, TDes8& aDes1, TDes8& aDes2)
/** 
 * Sends a request through the sub-session this object was created with.
 */
	{
	iService.Get(aIpc, aReqStatus, aDes1, aDes2);
	}

CAsyncRetrieveWithClientIds::CAsyncRetrieveWithClientIds(TInt aSessionHandle, TInt aSubSessionHandle)
	{
	iId().iSessionHandle = aSessionHandle;
	iId().iSubSessionHandle = aSubSessionHandle;
	}

EXPORT_C CRetrievePcktMbmsSessionList* CRetrievePcktMbmsSessionList::NewL(RPacketMbmsContext& aPcktMbmsContext, RPacketMbmsContext::CMbmsSession& aSessionIdList)
/**
 * Standard 2 phase constructor.
 * @param aPcktMbmsContext Reference to an open MBMS packet context object.
 * @param aSessionIdList On completion this holds the list of MBMS sessions.
 * @return Pointer instance of the class.
 */
	{
	return new (ELeave)CRetrievePcktMbmsSessionList(aPcktMbmsContext, aSessionIdList);
	}

EXPORT_C void CRetrievePcktMbmsSessionList::Start(TRequestStatus& aReqStatus)
/**
 * Starts the 2 phase retrieval. Assigns the IPCs to be used for the 2-phase retrieval mechanism.
 * @param aReqStatus On return, KErrNone if successful.
 * @capability ReadDeviceData
 */
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		iIpcPhase1=EPacketGetMbmsSessionListPhase1;
		iIpcPhase2=EPacketGetMbmsSessionListPhase2;
		iIpcCancel=EPacketGetMbmsSessionListCancel;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iId,&iId); 
		}
	}
void CRetrievePcktMbmsSessionList::Get(TInt aIpc, TRequestStatus& aReqStatus, TDes8& aDes1, TDes8& aDes2)
/**
 * Issues requested IPC to the ETEL/TSY
 */
	{
	iPcktMbmsContext.Get(aIpc, aReqStatus, aDes1, aDes2);
	}
void CRetrievePcktMbmsSessionList::CancelReq(TInt aIpc1, TInt aIpc2)
/**
 * Cancels previously issued request
 */
	{
	iPcktMbmsContext.CancelReq(aIpc1,aIpc2);
	}
void CRetrievePcktMbmsSessionList::RestoreListL()
/**
 * Overload of base class virtual method; de-serializes data retrieved from TSY to 
 * provided data structure.
 */
	{
	iSessionIdList.InternalizeL(iResultsPtr);
	FreeBuffer();
	}
CRetrievePcktMbmsSessionList::CRetrievePcktMbmsSessionList(RPacketMbmsContext& aPcktMbmsContext,RPacketMbmsContext::CMbmsSession& aSessionIdList)
:iPcktMbmsContext(aPcktMbmsContext),iSessionIdList(aSessionIdList)
/**
 * Standard constructor; initialises data members and the base class
 */	
	{
	iId().iSessionHandle=iPcktMbmsContext.SessionHandle().Handle();
	iId().iSubSessionHandle=iPcktMbmsContext.SubSessionHandle();
	}
	
