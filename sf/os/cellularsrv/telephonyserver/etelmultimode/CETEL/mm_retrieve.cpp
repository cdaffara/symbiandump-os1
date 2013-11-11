// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains all the code for the retrieval classes defined in the api which
// retrieve variable length buffers from the TSY, such as Sms messages or list of detected
// networks
// 
//

/**
 @file
*/

// Multimode header files
#include <etelmm.h>
#include <mmretrieve.h>
#include <etelext.h>
#include <e32base.h>
#include "mm_hold.h"
	

EXPORT_C void CAsyncRetrieveVariableLengthBuffer::Start(TRequestStatus& /*aReqStatus*/, TDes8* /*aPhase1Request*/, TDes8* /*aPhase2Request*/)
	{
	PanicClient(EEtelPanicInvalidRequestType);
	}

EXPORT_C void CAsyncRetrieveVariableLengthBuffer::DoCancel()
/**
 * This method cancels the active phase of the retrieval
 * It is called from the CActive::Cancel() method
 * 
 * @capability ReadDeviceData
 * @capability NetworkServices
*/
	{
	}

EXPORT_C void CAsyncRetrieveVariableLengthBuffer::RunL()
/**
 * This method processes the completion of each asynchronous phase of the retrieval
 * If phase 1 is now complete, it starts phase 2
 * If phase 2 is now complete, it complete's the client's request
 *  
 * @capability None
*/
	{
	}

EXPORT_C TBool CAsyncRetrieveVariableLengthBuffer::CompleteIfInUse(TRequestStatus& /*aReqStatus*/)
	{
	return EFalse;
	}


EXPORT_C void CAsyncRetrieveVariableLengthBuffer::FreeBuffer()
/**
 * This method deallocates the buffer used to hold the retrieved data
 *  
 * @capability None
*/
	{
	}

/***********************************************************************************/
//
// base class for buffer retrieval which require client ids to be sent to tsy
//
/***********************************************************************************/


CAsyncRetrieveWithClientIds::CAsyncRetrieveWithClientIds(TInt aSessionHandle, TInt aSubSessionHandle)
	{
	iId().iSessionHandle = aSessionHandle;
	iId().iSubSessionHandle = aSubSessionHandle;
	}

/***********************************************************************************/
//
// base class for list retrieval from RMobilePhoneStore
//
/***********************************************************************************/

CAsyncRetrieveStoreList::CAsyncRetrieveStoreList(RMobilePhoneStore& aStore) : 
	CAsyncRetrieveWithClientIds(aStore.SessionHandle().Handle(), aStore.SubSessionHandle()), 
	iStore(aStore)
/** Construction of the list.

@param aStore The Mobile phone store class that is associated with this list. */
	{
	iIpcPhase1=EMobilePhoneStoreReadAllPhase1;
	iIpcPhase2=EMobilePhoneStoreReadAllPhase2;
	iIpcCancel=EMobilePhoneStoreReadAllCancel;
	}
	
CAsyncRetrieveStoreList::~CAsyncRetrieveStoreList()
 	{	
 	Cancel();
 	}

void CAsyncRetrieveStoreList::Get(TInt aIpc, TRequestStatus& aReqStatus, TDes8& aDes1, TDes8& aDes2)
/** This function member uses the Get IPC request within RTelSubSessionBase to 
send one of the asynchronous phase requests to ETel server.

@param aIpc The IPC value of the asynchronous phase request.
@param aReqStatus On return, the request status for the asynchronous phase 
request.
@param aDes1 The first parameter for the asynchronous phase request.
@param aDes2 The second parameter for the asynchronous phase request. */
	{
	iStore.Get(aIpc,aReqStatus,aDes1,aDes2);
	}

void CAsyncRetrieveStoreList::CancelReq(TInt aIpc1, TInt aIpc2)
/** This function member cancels a previously issued Get() requests.

@param aIpc1 The IPC value of the cancel request.
@param aIpc2 The IPC value of the original asynchronous phase request. */
	{
	iStore.CancelReq(aIpc1,aIpc2);
	}

/***********************************************************************************/
//
// base class for list retrieval from RMobilePhone
//
/***********************************************************************************/

EXPORT_C CAsyncRetrievePhoneList::CAsyncRetrievePhoneList(RMobilePhone& aPhone) : 
	CAsyncRetrieveWithClientIds(aPhone.SessionHandle().Handle(), aPhone.SubSessionHandle()),
	iPhone(aPhone) 
/** Contruct the list for RMobilePhone aPhone.

@param aPhone The phone class that will be used to retrieve the lists. */
	{
	}

EXPORT_C CAsyncRetrievePhoneList::~CAsyncRetrievePhoneList()
 	{
 	Cancel();
 	}

EXPORT_C void CAsyncRetrievePhoneList::Get(TInt aIpc, TRequestStatus& aReqStatus, TDes8& aDes1, TDes8& aDes2)
/** This function member uses the Get IPC request within RTelSubsessionBase to 
send one of the asynchronous phase requests to ETel server.

@param aIpc The IPC value of the asynchronous phase request.
@param aReqStatus On return, the request status for the asynchronous phase 
request.
@param aDes1 The first parameter for the asynchronous phase request.
@param aDes2 The second parameter for the asynchronous phase request. 
*/
	{
	iPhone.Get(aIpc,aReqStatus,aDes1,aDes2);
	}

EXPORT_C void CAsyncRetrievePhoneList::CancelReq(TInt aIpc1, TInt aIpc2)
/** This function member uses the Cancel IPC request within RTelSubsessionBase 
to send a cancel for one of the asynchronous phases to ETel server.

@param aIpc1 The IPC value of the cancel request.
@param aIpc2 The IPC value of the original asynchronous phase request. 

@capability NetworkServices
@capability ReadDeviceData
*/
	{
	iPhone.CancelReq(aIpc1,aIpc2);
	}


/***********************************************************************************/
//
// Retrieve a list of detected networks
//
/***********************************************************************************/

EXPORT_C CRetrieveMobilePhoneDetectedNetworks* CRetrieveMobilePhoneDetectedNetworks::NewL(RMobilePhone& aPhone)
/** This function member creates an instance of the CRetrieveMobilePhoneDetectedNetworks 
class. 

@param aPhone A reference to their RMobilePhone sub-session as it is this 
sub-session that the list retrieval class will operate on.
@return The newly created CRetrieveMobilePhoneDetectedNetworks class. 
@capability None
*/
	{
	CRetrieveMobilePhoneDetectedNetworks* r=new(ELeave) CRetrieveMobilePhoneDetectedNetworks(aPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhoneDetectedNetworks::CRetrieveMobilePhoneDetectedNetworks(RMobilePhone& aPhone) 
:	CAsyncRetrievePhoneList(aPhone), iListExtensionId(KETelExtMultimodeV1)
	{
	}

void CRetrieveMobilePhoneDetectedNetworks::ConstructL()
	{
		iIpcPhase1=EMobilePhoneGetDetectedNetworksPhase1;
		iIpcPhase2=EMobilePhoneGetDetectedNetworksPhase2;
		iIpcCancel=EMobilePhoneGetDetectedNetworksCancel;
	}

EXPORT_C CRetrieveMobilePhoneDetectedNetworks::~CRetrieveMobilePhoneDetectedNetworks()
/** The destructor frees all resources owned by the object, prior to its destruction. */
	{
	delete iResults;
	delete iResultsV2;
	delete iResultsV5;
	delete iResultsV8;

	}

EXPORT_C CMobilePhoneNetworkList* CRetrieveMobilePhoneDetectedNetworks::RetrieveListL()
/** This function member provides the client with a handle to the list class retrieved 
from the phone. The client will call this function member once Start() has 
completed.

When the client calls this function member the ownership of the CMobilePhoneNetworkList 
object will transfer to the client, so the client is then responsible for 
eventual deletion of this object. 

@leave KErrNotFound If there is no valid list object to return to the client. 
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability None
*/
	{
	if (iResults)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneNetworkList* ptr=iResults;
		iResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}

EXPORT_C CMobilePhoneNetworkListV2* CRetrieveMobilePhoneDetectedNetworks::RetrieveListV2L()
/** This function member provides the client with a handle to the CMobilePhoneNetworkListV2 
list class retrieved from the phone. The client will call this function member once 
StartV2() has completed.

When the client calls this function member the ownership of the CMobilePhoneNetworkListV2 
object will transfer to the client, so the client is then responsible for 
eventual deletion of this object. 

@leave KErrNotFound If there is no valid list object to return to the client. 
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability None
*/
	{
	if (iResultsV2)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneNetworkListV2* ptr=iResultsV2;
		iResultsV2=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}

EXPORT_C CMobilePhoneNetworkListV5* CRetrieveMobilePhoneDetectedNetworks::RetrieveListV5L()
/** This function member provides the client with a handle to the CMobilePhoneNetworkListV5
list class retrieved from the phone. The client will call this function member once 
StartV5() has completed.

When the client calls this function member the ownership of the CMobilePhoneNetworkListV5 
object will transfer to the client, so the client is then responsible for 
eventual deletion of this object. 

@leave KErrNotFound If there is no valid list object to return to the client. 
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability None
*/
	{
	if (iResultsV5)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneNetworkListV5* ptr=iResultsV5;
		iResultsV5=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}

EXPORT_C CMobilePhoneNetworkListV8* CRetrieveMobilePhoneDetectedNetworks::RetrieveListV8L()
/** This function member provides the client with a handle to the CMobilePhoneNetworkListV8
list class retrieved from the phone. The client will call this function member once 
StartV8() has completed.

When the client calls this function member the ownership of the CMobilePhoneNetworkListV8 
object will transfer to the client, so the client is then responsible for 
eventual deletion of this object. 

@leave KErrNotFound If there is no valid list object to return to the client. 
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability None
*/
	{
	if (iResultsV8)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneNetworkListV8* ptr=iResultsV8;
		iResultsV8=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}

EXPORT_C void CRetrieveMobilePhoneDetectedNetworks::Start(TRequestStatus& aReqStatus)
/** This function member starts the two-phase list retrieval of detected networks.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to a detected network 
store and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned. 
@capability NetworkServices
@capability ReadDeviceData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		iIpcPhase1=EMobilePhoneGetDetectedNetworksPhase1;
		iIpcPhase2=EMobilePhoneGetDetectedNetworksPhase2;
		iIpcCancel=EMobilePhoneGetDetectedNetworksCancel;
		iListExtensionId = KETelExtMultimodeV1;

		delete iResults;
		iResults = NULL;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iId,&iId);
		}
	}

EXPORT_C void CRetrieveMobilePhoneDetectedNetworks::StartV2(TRequestStatus& aReqStatus)
/** This function member starts the two-phase list retrieval of detected networks.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to a detected network 
store and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned. 
@capability NetworkServices
@capability ReadDeviceData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		iIpcPhase1=EMobilePhoneGetDetectedNetworksV2Phase1;
		iIpcPhase2=EMobilePhoneGetDetectedNetworksV2Phase2;
		iIpcCancel=EMobilePhoneGetDetectedNetworksV2Cancel;
		iListExtensionId = KETelExtMultimodeV2;
		
		delete iResultsV2;
		iResultsV2 = NULL;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iId,&iId);
		}
	}

EXPORT_C void CRetrieveMobilePhoneDetectedNetworks::StartV5(TRequestStatus& aReqStatus)
/** This function member starts the two-phase list retrieval of detected networks.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to a detected network 
store and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned. 
@capability NetworkServices
@capability ReadDeviceData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		iIpcPhase1=EMobilePhoneGetDetectedNetworksV5Phase1;
		iIpcPhase2=EMobilePhoneGetDetectedNetworksV5Phase2;
		iIpcCancel=EMobilePhoneGetDetectedNetworksV5Cancel;
		iListExtensionId = KEtelExtMultimodeV5;
		
		delete iResultsV5;
		iResultsV5 = NULL;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iId,&iId);
		}
	}

EXPORT_C void CRetrieveMobilePhoneDetectedNetworks::StartV8(TRequestStatus& aReqStatus)
/** This function member starts the two-phase list retrieval of detected networks.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to a detected network 
store and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned. 
@capability NetworkServices
@capability ReadDeviceData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		iIpcPhase1=EMobilePhoneGetDetectedNetworksV8Phase1;
		iIpcPhase2=EMobilePhoneGetDetectedNetworksV8Phase2;
		iIpcCancel=EMobilePhoneGetDetectedNetworksV8Cancel;
		iListExtensionId = KEtelExtMultimodeV8;
		
		delete iResultsV8;
		iResultsV8 = NULL;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iId,&iId);
		}
	}

void CRetrieveMobilePhoneDetectedNetworks::RestoreListL()
/**
 * This method restores a list from a buffer that contains the streamed version of the list
 */
	{
	if(iListExtensionId == KETelExtMultimodeV1)
		{	
		iResults=CMobilePhoneNetworkList::NewL();
		iResults->RestoreL(iResultsPtr);
		FreeBuffer();
		}
	else if(iListExtensionId == KETelExtMultimodeV2)
		{
		iResultsV2=CMobilePhoneNetworkListV2::NewL();
		iResultsV2->RestoreL(iResultsPtr);
		FreeBuffer();
		}
	else if(iListExtensionId == KEtelExtMultimodeV5)
		{
		iResultsV5=CMobilePhoneNetworkListV5::NewL();
		iResultsV5->RestoreL(iResultsPtr);
		FreeBuffer();
		}
	else if(iListExtensionId == KEtelExtMultimodeV8)
		{
		iResultsV8=CMobilePhoneNetworkListV8::NewL();
		iResultsV8->RestoreL(iResultsPtr);
		FreeBuffer();
		}
	else
		{
		User::Leave(KErrNotFound);
		}

	}


/***********************************************************************************/
//
// Retrieve a list of call forwarding status/info
//
/***********************************************************************************/

EXPORT_C CRetrieveMobilePhoneCFList* CRetrieveMobilePhoneCFList::NewL(RMobilePhone& aPhone)
/** This member function creates an instance of the CRetrieveMobilePhoneCFList 
class.

@param aPhone A reference to the RMobilePhone sub-session as it is this sub-session 
that the list retrieval class will operate on.
@return A pointer to the newly created CRetrieveMobilePhoneCFList object. 
*/
	{
	CRetrieveMobilePhoneCFList* r=new(ELeave) CRetrieveMobilePhoneCFList(aPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhoneCFList::CRetrieveMobilePhoneCFList(RMobilePhone& aPhone) : 
	CAsyncRetrievePhoneList(aPhone)
	{
	}

void CRetrieveMobilePhoneCFList::ConstructL()
	{
	iIpcPhase1=EMobilePhoneGetCallForwardingStatusPhase1;
	iIpcPhase2=EMobilePhoneGetCallForwardingStatusPhase2;
	iIpcCancel=EMobilePhoneGetCallForwardingStatusCancel;	
	}

EXPORT_C CRetrieveMobilePhoneCFList::~CRetrieveMobilePhoneCFList()
/** The destructor frees all resources owned by the object, prior to its destruction. */
	{
	delete iResults;
	}

EXPORT_C CMobilePhoneCFList* CRetrieveMobilePhoneCFList::RetrieveListL()
/** This member function provides the client with a handle to the list class retrieved 
from the phone. The client will call this member function once Start() has 
completed. The retrieved list will consist of an entry per basic service group 
per specified CF condition. When the client calls this member function the 
ownership of the CMobilePhoneCFList object will transfer to the client, so 
the client is then responsible for eventual deletion of this object.

@leave KErrNotFound If there is no valid list object to return to the client. 
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
*/
	{
	if (iResults)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneCFList* ptr=iResults;
		iResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}

EXPORT_C void CRetrieveMobilePhoneCFList::Start(TRequestStatus& aReqStatus, 
												RMobilePhone::TMobilePhoneCFCondition aCondition, 
												RMobilePhone::TMobileInfoLocation aLocation)
/**
 * This method starts the retrieval of the call forwarding status list
 *
 * @deprecated 7.0s.
 * Deprecated, other CRetrieveMobilePhoneCFList::Start overload should be used instead
 * @capability NetworkServices 
 * @capability ReadDeviceData
 */
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		delete iResults;
		iResults = NULL;

		iCFRequestData().iCondition=aCondition;
		iCFRequestData().iLocation=aLocation;
		iCFRequestData().iClient=iId();
		iCFRequestData().iServiceGroup=RMobilePhone::EAllServices;

		CAsyncRetrievePhoneList::Start(aReqStatus,&iCFRequestData,&iId);
		}
	}
	
EXPORT_C void CRetrieveMobilePhoneCFList::Start(TRequestStatus& aReqStatus, 
												RMobilePhone::TMobilePhoneCFCondition aCondition, 
												RMobilePhone::TMobileService aServiceGroup,
												RMobilePhone::TMobileInfoLocation aLocation
												)
/**
 * This method starts the retrieval of the call forwarding status list. 
 * If the list is empty then KErrNone is returned. 
 * @capability NetworkServices 
 * @capability ReadDeviceData
 */
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		
		delete iResults;
		iResults = NULL;

		iCFRequestData().iCondition=aCondition;
		iCFRequestData().iLocation=aLocation;
		iCFRequestData().iClient=iId();
		iCFRequestData().iServiceGroup=aServiceGroup;

		CAsyncRetrievePhoneList::Start(aReqStatus,&iCFRequestData,&iId);
		}
	}
		
void CRetrieveMobilePhoneCFList::RestoreListL()
/**
 * This method restores a list from a buffer that contains the streamed version of the list
 */
	{
	iResults=CMobilePhoneCFList::NewL();
	iResults->RestoreL(iResultsPtr);
	FreeBuffer();
	}

/***********************************************************************************/
//
// Retrieve a list of call barring status/info
//
/***********************************************************************************/

EXPORT_C CRetrieveMobilePhoneCBList* CRetrieveMobilePhoneCBList::NewL(RMobilePhone& aPhone)
/** This member function creates an instance of the CRetrieveMobilePhoneCBList 
class.

@param aPhone A reference to the RMobilePhone sub-session, as it is this sub-session 
that the list retrieval class will operate on.
@return A pointer to the newly created CRetrieveMobilePhoneCBList object. 
@capability None
*/
	{
	CRetrieveMobilePhoneCBList* r=new(ELeave) CRetrieveMobilePhoneCBList(aPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhoneCBList::CRetrieveMobilePhoneCBList(RMobilePhone& aPhone) : 
	CAsyncRetrievePhoneList(aPhone), iListExtensionId(KETelExtMultimodeV1)
	{
	}

void CRetrieveMobilePhoneCBList::ConstructL()
	{
	}

EXPORT_C CRetrieveMobilePhoneCBList::~CRetrieveMobilePhoneCBList()
/** The destructor frees all resources owned by the object, prior to its destruction. */
	{
	delete iResults;
	}

EXPORT_C CMobilePhoneCBList* CRetrieveMobilePhoneCBList::RetrieveListL()
/** This member function provides the client with a handle to the list class retrieved 
from the phone.

The client will call this member function once Start() has completed. The 
retrieved list will consist of an entry per basic service group per specified 
CB condition.

When the client calls this member function the ownership of the CMobilePhoneCBList 
object will transfer to the client, so the client is then responsible for 
eventual deletion of this object.

@leave KErrNotFound If there is no valid list object to return to the client.
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability ReadDeviceData
*/
	{
	if (iResults)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneCBList* ptr=iResults;
		iResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}



EXPORT_C void CRetrieveMobilePhoneCBList::Start(TRequestStatus& aReqStatus, 
												RMobilePhone::TMobilePhoneCBCondition aCondition, 
												RMobilePhone::TMobileInfoLocation aLocation)
/** This member function starts the two-phase list retrieval. The client also specifies 
the call barring condition(s) that it wants to interrogate and from where 
this information should be retrieved. The client then waits for its request 
to be completed.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to a CB list store and 
KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned.
@param aCondition The barring conditions to interrogate.
@param aLocation Specifies from where this information should be retrieved. 
This parameters defaults to: RMobilePhone::EInfoLocationCachePreferred.
@capability NetworkServices
@capability ReadDeviceData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		
		iIpcPhase1 = EMobilePhoneGetBarringStatusPhase1;
		iIpcPhase2 = EMobilePhoneGetBarringStatusPhase2;
		iIpcCancel = EMobilePhoneGetBarringStatusCancel;
		iListExtensionId = KETelExtMultimodeV1;
		
		delete iResults;
		iResults = NULL;

		iCBRequestData().iCondition=aCondition;
		iCBRequestData().iLocation=aLocation;
		iCBRequestData().iClient=iId();
		
		CAsyncRetrievePhoneList::Start(aReqStatus,&iCBRequestData,&iId);
		}
	}
		
		
void CRetrieveMobilePhoneCBList::RestoreListL()
/**
 * This method restores a list from a buffer that contains the streamed version of the list
 */
	{
	if(iListExtensionId == KETelExtMultimodeV1)
		{
	iResults=CMobilePhoneCBList::NewL();
	iResults->RestoreL(iResultsPtr);
	FreeBuffer();
	}
	else
		User::Leave(KErrNotFound);
	}

/***********************************************************************************/
//
// Retrieve a list of call waiting status/info
//
/***********************************************************************************/

EXPORT_C CRetrieveMobilePhoneCWList* CRetrieveMobilePhoneCWList::NewL(RMobilePhone& aPhone)
/** This function member creates an instance of the CRetrieveMobilePhoneCWList 
class.

@param aPhone A reference to the RMobilePhone sub-session as this is the sub-session 
that the list retrieval class will operate on.
@return A pointer to the newly created CRetrieveMobilePhoneCWList object. 
@capability None
*/
	{
	CRetrieveMobilePhoneCWList* r=new(ELeave) CRetrieveMobilePhoneCWList(aPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhoneCWList::CRetrieveMobilePhoneCWList(RMobilePhone& aPhone) : 
	CAsyncRetrievePhoneList(aPhone)
	{
	}

void CRetrieveMobilePhoneCWList::ConstructL()
	{
	iIpcPhase1=EMobilePhoneGetWaitingStatusPhase1;
	iIpcPhase2=EMobilePhoneGetWaitingStatusPhase2;
	iIpcCancel=EMobilePhoneGetWaitingStatusCancel;
	}

EXPORT_C CRetrieveMobilePhoneCWList::~CRetrieveMobilePhoneCWList()
/** The destructor frees all resources owned by the object, prior to its destruction. */
	{
	delete iResults;
	}

EXPORT_C CMobilePhoneCWList* CRetrieveMobilePhoneCWList::RetrieveListL()
/** This function member provides the client with a handle to the list class retrieved 
from the phone. The client will call this function member once Start() has 
completed.

When the client calls this function member the ownership of the CMobilePhoneCWList 
object will transfer to the client, so the client is then responsible for 
eventual deletion of this object.

@leave KErrNotFound If there is no valid list object to return to the client
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability ReadDeviceData
*/
	{
	if (iResults)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneCWList* ptr=iResults;
		iResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}

EXPORT_C void CRetrieveMobilePhoneCWList::Start(TRequestStatus& aReqStatus, 
												RMobilePhone::TMobileInfoLocation aLocation)
/** This function member starts the two-phase list retrieval.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to a call waiting list 
store and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned.
@param aLocation Specifies from where this information should be retrieved. 
Defaults to RMobilePhone::EInfoLocationCachePreferred
@capability NetworkServices
@capability ReadDeviceData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		delete iResults;
		iResults = NULL;

		iCWRequestData().iLocation=aLocation;
		iCWRequestData().iClient=iId();
			
		CAsyncRetrievePhoneList::Start(aReqStatus,&iCWRequestData,&iId);
		}
	}
		
void CRetrieveMobilePhoneCWList::RestoreListL()
/**
 * This method restores a list from a buffer that contains the streamed version of the list
 */
	{
	iResults=CMobilePhoneCWList::NewL();
	iResults->RestoreL(iResultsPtr);
	FreeBuffer();
	}


/***********************************************************************************/
//
// Retrieve a list of call completion Requests
//
/***********************************************************************************/

EXPORT_C CRetrieveMobilePhoneCcbsList* CRetrieveMobilePhoneCcbsList::NewL(RMobilePhone& aPhone)
/** This member function creates an instance of the CRetrieveMobilePhoneCcbsList 
class.

@param aPhone A reference to the RMobilePhone sub-session, as it is this sub-session 
that the list retrieval class will operate on.
@return A pointer to the newly created CRetrieveMobilePhoneCcbsList object. 
@capability None
*/
	{
	CRetrieveMobilePhoneCcbsList* r=new(ELeave) CRetrieveMobilePhoneCcbsList(aPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhoneCcbsList::CRetrieveMobilePhoneCcbsList(RMobilePhone& aPhone) : 
	CAsyncRetrievePhoneList(aPhone)
	{
	}

void CRetrieveMobilePhoneCcbsList::ConstructL()
	{
	iIpcPhase1=EMobilePhoneGetCompletionRequestsPhase1;
	iIpcPhase2=EMobilePhoneGetCompletionRequestsPhase2;
	iIpcCancel=EMobilePhoneGetCompletionRequestsCancel;	
	}

EXPORT_C CRetrieveMobilePhoneCcbsList::~CRetrieveMobilePhoneCcbsList()
/** The destructor frees all resources owned by the object, prior to its destruction. */
	{
	delete iResults;
	}

EXPORT_C CMobilePhoneCcbsList* CRetrieveMobilePhoneCcbsList::RetrieveListL()
/** This member function provides the client with a handle to the list class retrieved 
from the phone. The client will call this member function once Start() has 
completed.

When the client calls this member function the ownership of the CMobilePhoneCcbsList 
object will transfer to the client, so the client is then responsible for 
eventual deletion of this object.

@leave KErrNotFound If there is no valid list object to return to the client.
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability ReadDeviceData
*/
	{
	if (iResults)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneCcbsList* ptr=iResults;
		iResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}

EXPORT_C void CRetrieveMobilePhoneCcbsList::Start(TRequestStatus& aReqStatus)
/** This member function starts the two-phase list retrieval.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to a CCBS list store 
and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned. 
@capability NetworkServices
@capability ReadDeviceData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		delete iResults;
		iResults = NULL;

		CAsyncRetrievePhoneList::Start(aReqStatus,&iId,&iId);
		}
	}
		
void CRetrieveMobilePhoneCcbsList::RestoreListL()
/**
 * This method restores a list from a buffer that contains the streamed version of the list
 */
	{
	iResults=CMobilePhoneCcbsList::NewL();
	iResults->RestoreL(iResultsPtr);
	FreeBuffer();
	}

/***********************************************************************************/
//
// Retrieve a list of SMS messages from a phone-side SMS store
//
/***********************************************************************************/

EXPORT_C CRetrieveMobilePhoneSmsList* CRetrieveMobilePhoneSmsList::NewL(RMobilePhoneStore& aStore, RMobileSmsStore::TMobileSmsEntryExtensionId aType)
/** This function member creates an instance of the CActive derived CRetrieveMobilePhoneSmsList 
class.

@param aStore An instance of RMobileSmsStore as this store that the list retrieval 
class will operate on.
@param aType Specifies whether GSM or CDMA message store entries will be retrieved.
@return The newly created CRetrieveMobilePhoneSmsList class. 
@capability None
*/
	{
	CRetrieveMobilePhoneSmsList* r=new(ELeave) CRetrieveMobilePhoneSmsList(aStore, aType);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhoneSmsList::CRetrieveMobilePhoneSmsList(RMobilePhoneStore& aStore, RMobileSmsStore::TMobileSmsEntryExtensionId aType) 
:	CAsyncRetrieveStoreList(aStore), iEntryType(aType)
	{
	}

void CRetrieveMobilePhoneSmsList::ConstructL()
	{
	}

EXPORT_C CRetrieveMobilePhoneSmsList::~CRetrieveMobilePhoneSmsList()
/** The destructor frees all resources owned by the object, prior to its destruction. */
	{
	delete iGsmResults;
	delete iCdmaResults;
	}

EXPORT_C CMobilePhoneGsmSmsList* CRetrieveMobilePhoneSmsList::RetrieveGsmListL()
/** These function members will provide the client with a handle to the appropriate 
GSM list class retrieved from the store.

The client will call this function member once Start() has completed.

When the client calls this function member the ownership of the list object 
will transfer to the client, so the client is then responsible for eventual 
deletion of this object. 

@leave KErrNotFound If there is no valid list object to return to the client.
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability ReadUserData
*/
	{
	if ((iEntryType==RMobileSmsStore::KETelMobileGsmSmsEntryV1) && iGsmResults)
		{
		// Give ownership of GSM SMS list to caller of this method
		CMobilePhoneGsmSmsList* ptr=iGsmResults;
		iGsmResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}
	
EXPORT_C CMobilePhoneCdmaSmsList* CRetrieveMobilePhoneSmsList::RetrieveCdmaListL()
/** These function members will provide the client with a handle to the appropriate 
CDMA list class retrieved from the store.

The client will call this function member once Start() has completed.

When the client calls this function member the ownership of the list object 
will transfer to the client, so the client is then responsible for eventual 
deletion of this object. 

@leave KErrNotFound If there is no valid list object to return to the client. 
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability ReadUserData
*/
	{
	if ((iEntryType==RMobileSmsStore::KETelMobileCdmaSmsEntryV1) && iCdmaResults)
		{
		// Give ownership of CDMA SMS list to caller of this method
		CMobilePhoneCdmaSmsList* ptr=iCdmaResults;
		iCdmaResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}
	
EXPORT_C void CRetrieveMobilePhoneSmsList::Start(TRequestStatus& aReqStatus)
/** This function member starts the two-phase list retrieval of all entries within 
the store.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to an SMS list store 
and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned.
@capability ReadUserData
*/
	{
	InitiateRequest(aReqStatus,EFalse,KValueNotUsed,KValueNotUsed);
	}
	
EXPORT_C void CRetrieveMobilePhoneSmsList::StartBatch(TRequestStatus& aReqStatus, TInt aStartIndex, TInt aBatchSize)
/** This function member starts the two-phase list retrieval of a batch of entries.

The client must supply the index (i.e. slot location) from which to start 
the batch retrieval and the number of entries to retrieve in that batch. Please 
note, that if the client specifies aStartIndex=X and aBatchSize=Y, the TSY 
should look in slot X to start with and then continue looking in slots until 
Y used entries are found (or less if the end of the store is reached). When 
the client receives the batch, the slot index attribute of the last entry 
in the list is not necessarily equal to aStartIndex+aBatchSize due to the 
possibility of empty slots. 

@param aReqStatus On return, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to an emergency number 
store and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned.
@param aStartIndex The slot where the TSY will start looking.
@param aBatchSize The number of entries the TSY will try to get. 
@capability ReadUserData
*/
	{
	InitiateRequest(aReqStatus,ETrue,aStartIndex,aBatchSize);
	}

void CRetrieveMobilePhoneSmsList::InitiateRequest(TRequestStatus& aReqStatus, TBool aBatch, TInt aStartIndex, TInt aBatchSize)
/**
 * This method uses the base class's generic Start() method to start either reading 
 * the whole list or just a batch of entries
 *
 * @param aReqStatus returns the result code after the asynchronous call completes
 * @param aBatch Specifies whether a batch or all entries is being read
 * @param aStartIndex Specifies the first index of the batch
 * @param aBatchSize Specifies how many entries to read after the start
 */
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		delete iGsmResults;
		iGsmResults = NULL;
		delete iCdmaResults;
		iCdmaResults = NULL;

		iRequestData().iClient=iId();
		iRequestData().iEntryType = iEntryType;
		iRequestData().iBatchRequest = aBatch;
		iRequestData().iStartIndex = aStartIndex;
		iRequestData().iBatchSize = aBatchSize;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iRequestData,&iId);
		}
	}

void CRetrieveMobilePhoneSmsList::RestoreListL()
/**
 * This method restores a list from a buffer that contains the streamed version of the list
 */
	{
	if (iEntryType==RMobileSmsStore::KETelMobileGsmSmsEntryV1)
		{
		iGsmResults=CMobilePhoneGsmSmsList::NewL();
		iGsmResults->RestoreL(iResultsPtr);
		}
	else
		{
		iCdmaResults=CMobilePhoneCdmaSmsList::NewL();
		iCdmaResults->RestoreL(iResultsPtr);
		}
	FreeBuffer();
	}

/***********************************************************************************/
//
// Retrieve a list of SMS parameters from a phone-side SMSP store
//
/***********************************************************************************/

EXPORT_C CRetrieveMobilePhoneSmspList* CRetrieveMobilePhoneSmspList::NewL(RMobileSmsMessaging& aMessaging)
/** This member function creates an instance of the CActive derived CRetrieveMobilePhoneCbmiList 
class.

@param aMessaging An instance of RMobileSmsMessaging as it is this messaging 
sub-session that the list retrieval class will operate on.
@return The newly created CRetrieveMobilePhoneSmspList class. 
@capability None
*/
	{
	CRetrieveMobilePhoneSmspList* r=new(ELeave) CRetrieveMobilePhoneSmspList(aMessaging);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhoneSmspList::CRetrieveMobilePhoneSmspList(RMobileSmsMessaging& aMessaging) 
:	CAsyncRetrieveWithClientIds(aMessaging.SessionHandle().Handle(), aMessaging.SubSessionHandle()),
	iMessaging(aMessaging)
	{
	}

void CRetrieveMobilePhoneSmspList::ConstructL()
	{
	iIpcPhase1=EMobileSmsMessagingGetSmspListPhase1;
	iIpcPhase2=EMobileSmsMessagingGetSmspListPhase2;
	iIpcCancel=EMobileSmsMessagingGetSmspListCancel;
	}

EXPORT_C CRetrieveMobilePhoneSmspList::~CRetrieveMobilePhoneSmspList()
/** The destructor frees all resources owned by the object, prior to its destruction. */
	{
	Cancel();
	delete iResults;
	}

EXPORT_C CMobilePhoneSmspList* CRetrieveMobilePhoneSmspList::RetrieveListL()
/** This member function provides the client with a handle to the list class retrieved 
from the store. The client will call this member function once Start() has 
completed.

When the client calls this member function the ownership of the CMobilePhoneSmspList 
object will transfer to the client, so the client is then responsible for 
eventual deletion of this object.

@leave KErrNotFound If there is no valid list object to return to the client.
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability ReadDeviceData
*/
	{
	if (iResults)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneSmspList* ptr=iResults;
		iResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}
		
void CRetrieveMobilePhoneSmspList::RestoreListL()
/**
 * This method restores a list from a buffer that contains the streamed version of the list
 */
	{
	iResults=CMobilePhoneSmspList::NewL();
	iResults->RestoreL(iResultsPtr);
	FreeBuffer();
	}

EXPORT_C void CRetrieveMobilePhoneSmspList::Start(TRequestStatus& aReqStatus)
/** This member function starts the two-phase list retrieval. 

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to a SMSP list store 
and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned.
@capability ReadDeviceData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		delete iResults;
		iResults = NULL;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iId,&iId);
		}
	}

void CRetrieveMobilePhoneSmspList::Get(TInt aIpc, TRequestStatus& aReqStatus, TDes8& aDes1, TDes8& aDes2)
/**
 * This method uses the Get IPC request within RTelSubsessionBase
 * to send one of the asynchronous phase requests to ETel server
 *
 * @param aIpc The IPC value of the asynchronous phase request
 * @param aReqStatus The request status for the asynchronous phase request
 * @param aDes1 The first parameter for the asynchronous phase request
 * @param aDes2 The second parameter for the asynchronous phase request
 */
	{
	iMessaging.Get(aIpc,aReqStatus,aDes1,aDes2);
	}

void CRetrieveMobilePhoneSmspList::CancelReq(TInt aIpc1, TInt aIpc2)
/**
 * This method uses the Cancel IPC request within RTelSubsessionBase
 * to send a cancel for one of the asynchronous phases to ETel server
 *
 * @param aIpc1 The IPC value of the cancel request
 * @param aIpc2 The IPC value of the original asynchronous phase request
 */
	{
	iMessaging.CancelReq(aIpc1,aIpc2);
	}

/***********************************************************************************/
//
// Retrieve a list of Broadcast ID entries from the SIM or phone
//
/***********************************************************************************/

EXPORT_C CRetrieveMobilePhoneBroadcastIdList* CRetrieveMobilePhoneBroadcastIdList::NewL(RMobileBroadcastMessaging& aMessaging)
/** This member function creates an instance of the CRetrieveMobilePhoneBroadcastIdList 
class.

@param aMessaging An instance of RMobileBroadcastMessaging as it is this messaging 
sub-session that the list retrieval class will operate on.
@return The newly created CRetrieveMobilePhoneBroadcastIdList. 
@capability None
*/
	{
	CRetrieveMobilePhoneBroadcastIdList* r=new(ELeave) CRetrieveMobilePhoneBroadcastIdList(aMessaging);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhoneBroadcastIdList::CRetrieveMobilePhoneBroadcastIdList(RMobileBroadcastMessaging& aMessaging) : 
	CAsyncRetrieveWithClientIds(aMessaging.SessionHandle().Handle(), aMessaging.SubSessionHandle()),
	iMessaging(aMessaging)
	{
	}

void CRetrieveMobilePhoneBroadcastIdList::ConstructL()
	{
	iIpcPhase1=EMobileBroadcastMessagingGetIdListPhase1;
	iIpcPhase2=EMobileBroadcastMessagingGetIdListPhase2;
	iIpcCancel=EMobileBroadcastMessagingGetIdListCancel;
	}

EXPORT_C CRetrieveMobilePhoneBroadcastIdList::~CRetrieveMobilePhoneBroadcastIdList()
/** The destructor frees all resources owned by the object, prior to its destruction. */
	{
	Cancel();
	delete iResults;
	}

EXPORT_C CMobilePhoneBroadcastIdList* CRetrieveMobilePhoneBroadcastIdList::RetrieveListL()
/** This member function provides the client with a handle to the list class retrieved 
from the store. The client will call this member function once Start() has 
completed with KErrNone.

When the client calls this member function the ownership of the CMobilePhoneBroadcastIdList 
object will transfer to the client, so the client is then responsible for 
eventual deletion of this object.

@leave KErrNotFound If there is no valid list object to return to the client.
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability ReadUserData
*/
	{
	if (iResults)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneBroadcastIdList* ptr=iResults;
		iResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}

EXPORT_C void CRetrieveMobilePhoneBroadcastIdList::Start(TRequestStatus& aReqStatus, RMobileBroadcastMessaging::TMobileBroadcastIdType aIdType)
/** This member function starts the two-phase list retrieval.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to a phone book broadcast 
id list store and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned.
@param aIdType Specifies whether to retrieve the GSM/WCDMA list of CBMI entries 
or the CDMA list of Service Category entries. 
@capability ReadUserData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		delete iResults;
		iResults = NULL;

		iBroadcastIdRequestData().iIdType=aIdType;
		iBroadcastIdRequestData().iClient=iId();
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iBroadcastIdRequestData,&iId);
		}
	}
		
void CRetrieveMobilePhoneBroadcastIdList::RestoreListL()
/**
 * This method restores a list from a buffer that contains the streamed version of the list
 */
	{
	iResults=CMobilePhoneBroadcastIdList::NewL();
	iResults->RestoreL(iResultsPtr);
	FreeBuffer();
	}

void CRetrieveMobilePhoneBroadcastIdList::Get(TInt aIpc, TRequestStatus& aReqStatus, TDes8& aDes1, TDes8& aDes2)
/**
 * This method uses the Get IPC request within RTelSubsessionBase
 * to send one of the asynchronous phase requests to ETel server
 *
 * @param aIpc The IPC value of the asynchronous phase request
 * @param aReqStatus The request status for the asynchronous phase request
 * @param aDes1 The first parameter for the asynchronous phase request
 * @param aDes2 The second parameter for the asynchronous phase request
 */
	{
	iMessaging.Get(aIpc,aReqStatus,aDes1,aDes2);
	}

void CRetrieveMobilePhoneBroadcastIdList::CancelReq(TInt aIpc1, TInt aIpc2)
/**
 * This method uses the Cancel IPC request within RTelSubsessionBase
 * to send a cancel for one of the asynchronous phases to ETel server
 *
 * @param aIpc1 The IPC value of the cancel request
 * @param aIpc2 The IPC value of the original asynchronous phase request
 */
	{
	iMessaging.CancelReq(aIpc1,aIpc2);
	}

/***********************************************************************************/
//
// Retrieve a list of entries from a phone-side NAM store
// The list contains entries of type RMobileNamStore::TMobileNamEntryV1
//
/***********************************************************************************/

EXPORT_C CRetrieveMobilePhoneNamList* CRetrieveMobilePhoneNamList::NewL(RMobilePhoneStore& aStore)
/** This member function member creates an instance of the CRetrieveMobilePhoneNamList 
class.

@param aStore An instance of RMobileNamStore as it is this store that the 
list retrieval class will operate on.
@return The newly created CRetrieveMobilePhoneNamList class. 
@capability None
*/
	{
	CRetrieveMobilePhoneNamList* r=new(ELeave) CRetrieveMobilePhoneNamList(aStore);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhoneNamList::CRetrieveMobilePhoneNamList(RMobilePhoneStore& aStore) : 
	CAsyncRetrieveStoreList(aStore), iListExtensionId(KETelExtMultimodeV1)
	{
	}

void CRetrieveMobilePhoneNamList::ConstructL()
	{
	iRequestData().iClient=iId();
	}

EXPORT_C CRetrieveMobilePhoneNamList::~CRetrieveMobilePhoneNamList()
/** The destructor frees all resources owned by the object, prior to its destruction. */
	{
	delete iResults;
	delete iResultsV4;
	}

EXPORT_C CMobilePhoneNamList* CRetrieveMobilePhoneNamList::RetrieveListL()
/** This function member provides the client with a handle to the list class retrieved 
from the store. The client will call this function member once Start() has 
completed.

When the client calls this function member the ownership of the CMobilePhoneNamList 
object will transfer to the client, so the client is then responsible for 
eventual deletion of this object. 

@leave KErrNotFound if there is no valid list object to return to the client. 
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability ReadDeviceData
*/
	{
	if (iResults)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneNamList* ptr=iResults;
		iResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}
		
EXPORT_C CMobilePhoneNamListV4* CRetrieveMobilePhoneNamList::RetrieveListV4L()
/** This function member provides the client with a handle to the list class retrieved 
from the store. The client will call this function member once Start() has 
completed.

When the client calls this function member the ownership of the CMobilePhoneNamListV4 
object will transfer to the client, so the client is then responsible for 
eventual deletion of this object. 

@leave KErrNotFound if there is no valid list object to return to the client. 
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability ReadDeviceData
*/
	{
	if (iResultsV4)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneNamListV4* ptr=iResultsV4;
		iResultsV4=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}
		
void CRetrieveMobilePhoneNamList::RestoreListL()
/**
 * This method restores a list from a buffer that contains the streamed version of the list
 */
	{
	if(iListExtensionId == KETelExtMultimodeV1)
		{	
		iResults=CMobilePhoneNamList::NewL();
		iResults->RestoreL(iResultsPtr);
		FreeBuffer();
		}
	else if(iListExtensionId == KETelExtMultimodeV4)
		{
		iResultsV4=CMobilePhoneNamListV4::NewL();
		iResultsV4->RestoreL(iResultsPtr);
		FreeBuffer();
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}

EXPORT_C void CRetrieveMobilePhoneNamList::Start(TRequestStatus& aReqStatus, TInt aNamId)
/** This function member starts the two-phase list retrieval.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to an NAM list store 
and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned.
@param aNamId The identity of the NAM that the client wishes to read. 
@capability ReadDeviceData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		iIpcPhase1=EMobilePhoneStoreReadAllPhase1;
		iIpcPhase2=EMobilePhoneStoreReadAllPhase2;
		iIpcCancel=EMobilePhoneStoreReadAllCancel;
		iListExtensionId = KETelExtMultimodeV1;

		delete iResults;
		iResults = NULL;

		iRequestData().iNamId = aNamId;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iRequestData,&iId);
		}
	}

EXPORT_C void CRetrieveMobilePhoneNamList::StartV4(TRequestStatus& aReqStatus, TInt aNamId)
/** This function member starts the two-phase list retrieval.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to an NAM list store 
and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned.
@param aNamId The identity of the NAM that the client wishes to read. 
@capability ReadDeviceData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		iIpcPhase1=EMobilePhoneStoreReadAllV4Phase1;
		iIpcPhase2=EMobilePhoneStoreReadAllV4Phase2;
		iIpcCancel=EMobilePhoneStoreReadAllV4Cancel;
		iListExtensionId = KETelExtMultimodeV4;

		delete iResultsV4;
		iResultsV4 = NULL;

		iRequestData().iNamId = aNamId;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iRequestData,&iId);
		}
	}


/***********************************************************************************/
//
// Retrieve a list of entries from a phone-side Own Number store
//
/***********************************************************************************/

EXPORT_C CRetrieveMobilePhoneONList* CRetrieveMobilePhoneONList::NewL(RMobilePhoneStore& aStore)
/** This function member creates an instance of the CActive derived CRetrieveMobilePhoneONList 
class.

@param aStore An instance of RMobileONStore as this store that the list retrieval 
class will operate on.
@return The newly created CRetrieveMobilePhoneONList class. 
@capability None
*/
	{
	CRetrieveMobilePhoneONList* r=new(ELeave) CRetrieveMobilePhoneONList(aStore);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhoneONList::CRetrieveMobilePhoneONList(RMobilePhoneStore& aStore) : 
	CAsyncRetrieveStoreList(aStore)
	{
	}

void CRetrieveMobilePhoneONList::ConstructL()
	{
	}

EXPORT_C CRetrieveMobilePhoneONList::~CRetrieveMobilePhoneONList()
/** The destructor frees all resources owned by the object, prior to its destruction. */
	{
	delete iResults;
	}

EXPORT_C CMobilePhoneONList* CRetrieveMobilePhoneONList::RetrieveListL()
/** This function member provides the client with a handle to the list class retrieved 
from the store. The client will call this function member once Start() has 
completed.

When the client calls this function member the ownership of the CMobilePhoneONList 
object will transfer to the client, so the client is then responsible for 
eventual deletion of this object.

@leave KErrNotFound if there is no valid list object to return to the client. 
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability ReadUserData
*/
	{
	if (iResults)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneONList* ptr=iResults;
		iResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}
		
void CRetrieveMobilePhoneONList::RestoreListL()
/**
 * This method restores a list from a buffer that contains the streamed version of the list
 */
	{
	iResults=CMobilePhoneONList::NewL();
	iResults->RestoreL(iResultsPtr);
	FreeBuffer();
	}

EXPORT_C void CRetrieveMobilePhoneONList::Start(TRequestStatus& aReqStatus)
/** This function member starts the two-phase retrieval of all own numbers.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support a access to an own number store 
and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned.
@capability ReadUserData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		delete iResults;
		iResults = NULL;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iId,&iId);
		}
	}

/***********************************************************************************/
//
// Retrieve a list of entries from a phone-side Emergency Number store
//
/***********************************************************************************/

EXPORT_C CRetrieveMobilePhoneENList* CRetrieveMobilePhoneENList::NewL(RMobilePhoneStore& aStore)
/** This function member creates an instance of the CActive derived CRetrieveMobilePhoneENList 
class.

@param aStore An instance of RMobileENStore as it this store that the list 
retrieval class will operate on.
@return The newly created CRetrieveMobilePhoneENList class. 
@capability None
*/
	{
	CRetrieveMobilePhoneENList* r=new(ELeave) CRetrieveMobilePhoneENList(aStore);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhoneENList::CRetrieveMobilePhoneENList(RMobilePhoneStore& aStore) : 
	CAsyncRetrieveStoreList(aStore)
	{
	}

void CRetrieveMobilePhoneENList::ConstructL()
	{
	}

EXPORT_C CRetrieveMobilePhoneENList::~CRetrieveMobilePhoneENList()
/** The destructor frees all resources owned by the object, prior to its destruction. */
	{
	delete iResults;
	}

EXPORT_C CMobilePhoneENList* CRetrieveMobilePhoneENList::RetrieveListL()
/** This function member provides the client with a handle to the list class retrieved 
from the store. The client will call this function member once Start() has 
completed.

When the client calls this function member the ownership of the CMobilePhoneENList 
object will transfer to the client, so the client is then responsible for 
eventual deletion of this object.

@leave KErrNotFound if there is no valid list object to return to the client. 
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability ReadUserData
*/
	{
	if (iResults)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneENList* ptr=iResults;
		iResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}
		
void CRetrieveMobilePhoneENList::RestoreListL()
/**
 * This method restores a list from a buffer that contains the streamed version of the list
 */
	{
	iResults=CMobilePhoneENList::NewL();
	iResults->RestoreL(iResultsPtr);
	FreeBuffer();
	}

EXPORT_C void CRetrieveMobilePhoneENList::Start(TRequestStatus& aReqStatus)
/** This function member starts the two-phase retrieval of all emergency numbers.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to an emergency number 
store and KErrNotFound if the store does not exist. If the list is empty then KErrNone
is returned.
@capability ReadUserData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		delete iResults;
		iResults = NULL;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iId,&iId);
		}
	}

/***********************************************************************************/
//
// Retrieve a list of stored network entries 
//
/***********************************************************************************/

EXPORT_C CRetrieveMobilePhonePreferredNetworks* CRetrieveMobilePhonePreferredNetworks::NewL(RMobilePhone& aPhone)
/**
Creates an instance of the CActive derived CRetrieveMobilePhonePrefferredNetworks class

@param aPhone Reference to clients RMobilePhone sub-session. 
@return A pointer to the newly creasted CRetrieveMobilePhonePreferredNetworks object.
 
@capability None
*/
	{
	CRetrieveMobilePhonePreferredNetworks* r=new(ELeave) CRetrieveMobilePhonePreferredNetworks(aPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhonePreferredNetworks::CRetrieveMobilePhonePreferredNetworks(RMobilePhone& aPhone) : 
	CAsyncRetrievePhoneList(aPhone)
	{
	}

void CRetrieveMobilePhonePreferredNetworks::ConstructL()
	{
	iIpcPhase1=EMobilePhoneGetPreferredNetworksPhase1;
	iIpcPhase2=EMobilePhoneGetPreferredNetworksPhase2;
	iIpcCancel=EMobilePhoneGetPreferredNetworksCancel;
	}
	
EXPORT_C CRetrieveMobilePhonePreferredNetworks::~CRetrieveMobilePhonePreferredNetworks()
/**
Destructor
*/
		{
		delete iResults;
	}
	
EXPORT_C CMobilePhoneStoredNetworkList* CRetrieveMobilePhonePreferredNetworks::RetrieveListL()
/**
Returns a pointer to the retrieved list.

@return CMobilePhoneStoredNetworkList* Pointer to the detected networks list.
If there are no entries in the list then a handle to an empty list is returned.

@capability None
*/
	{
	if (iResults)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneStoredNetworkList* ptr=iResults;
		iResults = NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}

EXPORT_C void CRetrieveMobilePhonePreferredNetworks::Start(TRequestStatus& aReqStatus)
/**
Starts the retrieval of the stored networks list.

@param aReqStatus Returns the result code after the asynchronous call completes.
If the list is empty then KErrNone is returned.
 
@capability ReadDeviceData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		delete iResults;
		iResults = NULL;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iId,&iId);
		}
	}


void CRetrieveMobilePhonePreferredNetworks::RestoreListL()
/**
Restores a list from a buffer that contains the streamed version of the list
 */
	{
	iResults=CMobilePhoneStoredNetworkList::NewL();
	iResults->RestoreL(iResultsPtr);
	FreeBuffer();
	}

//
//
// class for retrieval of authorization data
//
/***********************************************************************************/
/** standard constructor, initialises data members and the base class*/
CAsyncRetrieveAuthorizationInfo::CAsyncRetrieveAuthorizationInfo(RMobilePhone& aPhone,RMobilePhone::CImsAuthorizationInfoV5& aAuthInfo)
: iPhone(aPhone),iAuthInfo(aAuthInfo)
	{
	iId().iSessionHandle=aPhone.SessionHandle().Handle();
	iId().iSubSessionHandle=aPhone.SubSessionHandle();
	iIpcPhase1=EMobilePhoneAuthorizationInfoPhase1;
	iIpcPhase2=EMobilePhoneAuthorizationInfoPhase2;
	iIpcCancel=EMobilePhoneAuthorizationInfoCancel;
	iAuthInfoValid=FALSE;
	}
/** overload of base class virtual method, deserialize data retrieved from TSY to 
provided data structure*/
void CAsyncRetrieveAuthorizationInfo::RestoreListL()
	{
	iAuthInfo.InternalizeL(iResultsPtr);
	iAuthInfoValid=TRUE;
	FreeBuffer();
	}
/** Issues requested IPC to the ETEL/TSY */	
void CAsyncRetrieveAuthorizationInfo::Get(TInt aIpc, TRequestStatus& aReqStatus, TDes8& aDes1, TDes8& aDes2)
	{
	iPhone.Get(aIpc,aReqStatus,aDes1,aDes2);
	}
/** Cancels previously issued request*/
void CAsyncRetrieveAuthorizationInfo::CancelReq(TInt aIpc1, TInt aIpc2)
	{
	iPhone.CancelReq(aIpc1,aIpc2);
	}

/** 
Indicates whether iAuthInfo data member is valid - data from ISIM/USIM retrieved 

@return Whether the authorization information is valid.
*/ 
EXPORT_C TBool CAsyncRetrieveAuthorizationInfo::AuthInfoValid() const
	{
	return iAuthInfoValid;
	}

/** 
Starts the 2 phase retrieval.

@param aReqStatus On return, KErrNone if successful.
@capability ReadDeviceData
*/
EXPORT_C void CAsyncRetrieveAuthorizationInfo::Start(TRequestStatus& aReqStatus)
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		iAuthInfoValid=FALSE;
		iIpcPhase1=EMobilePhoneAuthorizationInfoPhase1;
		iIpcPhase2=EMobilePhoneAuthorizationInfoPhase2;
		iIpcCancel=EMobilePhoneAuthorizationInfoCancel;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iId,&iId);
		}
	}


/** 
Standard 2 phase constructor.

@param aPhone Reference to an open phone object.
@param anAuthInfo On completion will contains the IMS authorization information.
@return Pointer instance.of class.
*/
EXPORT_C CAsyncRetrieveAuthorizationInfo* CAsyncRetrieveAuthorizationInfo::NewL(RMobilePhone& aPhone,RMobilePhone::CImsAuthorizationInfoV5& aAuthInfo)
	{
	return new (ELeave)CAsyncRetrieveAuthorizationInfo(aPhone,aAuthInfo);
	}


//
//
// Class for two-phased submission and retrieval of EAP packets
//
//

/**
Creates a new two-phase retrieval object for sending an EAP packet to
a DF_EAP and receiving its response.

@param aScEap The sub-session handle that will be used by the returned
              object, for submitting requests to the server.
@param aEapRequestPckt The EAP request packet to send to the smart card
                       application's (and aScEap's) DF_EAP.
@return Pointer to a new CAsyncSmartCardEapAuthentication object for
        use in one EAP request/response.
*/
EXPORT_C CAsyncSmartCardEapAuthentication* CAsyncSmartCardEapAuthentication::NewL(RMobileSmartCardEap& aScEap, const TDesC8& aEapRequestPckt)
	{
	CAsyncSmartCardEapAuthentication* self = new(ELeave) CAsyncSmartCardEapAuthentication(aScEap);
	CleanupStack::PushL(self);
	self->ConstructL(aEapRequestPckt);
	CleanupStack::Pop();
	return self;
	}

/**
Default constructor.  Initialises the IPC values to be used in the two-
phase retrieval process.

@param aScEap The sub-session handle that will be used by the returned
              object, for submitting requests to the server.
*/
CAsyncSmartCardEapAuthentication::CAsyncSmartCardEapAuthentication(RMobileSmartCardEap& aScEap)
: iSmartCardEap(aScEap)
	{
	iIpcPhase1 = EMobileSmartCardEapAuthenticationPhase1;
	iIpcPhase2 = EMobileSmartCardEapAuthenticationPhase2;
	iIpcCancel = EMobileSmartCardEapAuthenticationCancel;
	}

/**
Default destructor.
*/
EXPORT_C CAsyncSmartCardEapAuthentication::~CAsyncSmartCardEapAuthentication()
	{
	delete iReqData;
	}

/**
Second phase constructor.  Ensures memory can be allocated on the
client-side for storing a copy of the EAP request packet.

@param aEapRequestPckt The EAP request packet to send to the smart card
                       application's (and aScEap's) DF_EAP.  This will
                       be copied to this object's member data.
*/
void CAsyncSmartCardEapAuthentication::ConstructL(const TDesC8& aEapRequestPckt)
	{
	iReqData = RMobileSmartCardEap::CEapAuthenticateRequestDataV6::NewL();
	iReqData->SetEapReqPacketL(aEapRequestPckt);
	}

/**
Starts the two-phase retrieval process.  The process only completes
when both phases of retrieval complete (or in case of error/failure/
cancellation).

@param aReqStatus Returns the result code after the asynchronous call
                  completes.  Request completion with KErrBadHandle
                  means the RMobileSmartCardEap sub-session handle,
                  given in the NewL(), has not been initialised
                  (RMobileSmartCardEap::InitialiseEapMethod()).
                  Completion with KErrInUse means the handle instance
                  is not the current owner of the DF_EAP.

@see RMobileSmartCardEap::InitialiseEapMethod()
*/
EXPORT_C void CAsyncSmartCardEapAuthentication::Start(TRequestStatus& aReqStatus)
	{
	if (iSmartCardEap.iSemaphore.Handle() == 0)
		{
		TRequestStatus* status1 = &aReqStatus;
		User::RequestComplete(status1, KErrBadHandle);
		return;
		}

	if (!iSmartCardEap.iOwnsEapMethodLock)
		{
		TRequestStatus* status2 = &aReqStatus;
		User::RequestComplete(status2, KErrInUse);
		return;
		}

	TRAPD(err, iReqData->ExternalizeL(iSmartCardEap.iMmPtrHolder->iReqAuthData));
	if (err != KErrNone)
		{
		TRequestStatus* status3 = &aReqStatus;
		User::RequestComplete(status3, err);
		return;
		}

	TPtr8& ptr1 = iSmartCardEap.iMmPtrHolder->iReqAuthData;

	CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus, &ptr1, &ptr1);
	}

/**
Used to retrieve a descriptor pointing to the response data from the
DF_EAP.

@return Reference to the descriptor of the retrieved response data.  If
        this method is called prior to a successful completion of
        CAsyncSmartCardEapAuthentication::Start(), the descriptor will
        point to NULL.
*/
EXPORT_C TDesC8& CAsyncSmartCardEapAuthentication::RetrieveEapResponse()
	{
	return iResultsPtr;
	}

/**
Cancels the request specified through the sub-session this object was
created with.
*/
void CAsyncSmartCardEapAuthentication::CancelReq(TInt aIpc1, TInt aIpc2)
	{
	iSmartCardEap.CancelReq(aIpc1, aIpc2);
	}

/**
Sends a request through the sub-session this object was created with.
*/
void CAsyncSmartCardEapAuthentication::Get(TInt aIpc, TRequestStatus& aReqStatus, TDes8& aDes1, TDes8& aDes2)
	{
	iSmartCardEap.Get(aIpc, aReqStatus, aDes1, aDes2);
	}

//
//
// Class for two-phased retrieval of GBA NAF and MBMS MSK Authentication List
//
//

EXPORT_C CRetrieveMobilePhoneAuthenticationIds* CRetrieveMobilePhoneAuthenticationIds::NewL(RMobilePhone& aPhone)
/** This function member creates an instance of the CActive derived CRetrieveMobilePhoneAuthenticationIds 
class.

@param aPhone An instance of RMobilePhone as this store that the list retrieval 
class will operate on.
@return The newly created CRetrieveMobilePhoneAuthenticationIds class. 
@capability None
*/
	{
	CRetrieveMobilePhoneAuthenticationIds* r=new(ELeave) CRetrieveMobilePhoneAuthenticationIds(aPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhoneAuthenticationIds::CRetrieveMobilePhoneAuthenticationIds(RMobilePhone& aPhone)
:	CAsyncRetrievePhoneList(aPhone) 
	{
	}

void CRetrieveMobilePhoneAuthenticationIds::ConstructL()
	{
	iIpcPhase1=EMobilePhoneAuthenticationListPhase1;
	iIpcPhase2=EMobilePhoneAuthenticationListPhase2;
	iIpcCancel=EMobilePhoneAuthenticationListCancel;
	}


EXPORT_C CRetrieveMobilePhoneAuthenticationIds::~CRetrieveMobilePhoneAuthenticationIds()
/** The destructor frees all resources owned by the object, prior to its destruction.
 */
	{
	delete iGbaNafIdResults;
	delete iMbmsMskIdResults;
	}

EXPORT_C CMobilePhoneGbaNafIdList* CRetrieveMobilePhoneAuthenticationIds::RetrieveGbaListL()
/** These function members will provide the client with a handle to the appropriate 
GbaNaf list class retrieved from the Authentication.

The client will call this function member once Start() has completed.

When the client calls this function member the ownership of the list object 
will transfer to the client, so the client is then responsible for eventual 
deletion of this object. 

@leave KErrNotFound If there is no valid list object to return to the client.
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability ReadDeviceData
*/
	{
	if ((iListType==RMobilePhone::EGbaNafIdList) && iGbaNafIdResults)
		{
		// Give ownership of GbaNaf list to caller of this method
		CMobilePhoneGbaNafIdList* ptr=iGbaNafIdResults;
		iGbaNafIdResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}
	
EXPORT_C CMobilePhoneMbmsMskIdList* CRetrieveMobilePhoneAuthenticationIds::RetrieveMbmsListL()
/** These function members will provide the client with a handle to the appropriate 
MBMS MSK ID list class retrieved from the Authentication.

The client will call this function member once Start() has completed.

When the client calls this function member the ownership of the list object 
will transfer to the client, so the client is then responsible for eventual 
deletion of this object. 

@leave KErrNotFound If there is no valid list object to return to the client.
@return A handle to the list class retrieved from the store. If there are no entries in 
the list then a handle to an empty list is returned.
@capability ReadDeviceData
*/
	{
	if ((iListType==RMobilePhone::EMbmsMskIdList) && iMbmsMskIdResults)
		{
		// Give ownership of MbmsMsk list to caller of this method
		CMobilePhoneMbmsMskIdList* ptr=iMbmsMskIdResults;
		iMbmsMskIdResults=NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}	

EXPORT_C void CRetrieveMobilePhoneAuthenticationIds::Start(TRequestStatus& aReqStatus,const TDesC8& aData)
/** This function member starts the two-phase list retrieval of all entries within 
the store.

@param aReqStatus On completion, the status of the request; KErrNone if successful, 
KErrNotSupported if the phone does not support access to an Gba/Mbms list  
and KErrNotFound if the list does not exist. If the list is empty then KErrNone
is returned.
@capability ReadDeviceData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		TPckgBuf<RMobilePhone::TAuthInfoV8> commandId;
     	commandId.Copy(aData);
     	iListType=commandId().iData;
     	
     	if(iListType == RMobilePhone::EGbaNafIdList || iListType == RMobilePhone::EMbmsMskIdList)
     		{
     		delete iGbaNafIdResults;
			iGbaNafIdResults = NULL;
			delete iMbmsMskIdResults;
			iMbmsMskIdResults = NULL;
			
			iAuthRequestData().iClient = iId();
			iAuthRequestData().iAuthInfo=commandId();
			CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iAuthRequestData,&iId);
			}
		else
			{
			TRequestStatus* status = &aReqStatus;
			User::RequestComplete(status, KErrNotSupported);	
			}
	
		}
	}
		
void CRetrieveMobilePhoneAuthenticationIds::RestoreListL()
/**
 * This method restores a list from a buffer that contains the streamed version of the list
 */
	{
	if (iListType==RMobilePhone::EGbaNafIdList)
		{
		iGbaNafIdResults=CMobilePhoneGbaNafIdList::NewL();
		iGbaNafIdResults->RestoreL(iResultsPtr);
		}
		
	else if(iListType==RMobilePhone::EMbmsMskIdList)
		{
		iMbmsMskIdResults=CMobilePhoneMbmsMskIdList::NewL();
		iMbmsMskIdResults->RestoreL(iResultsPtr);
		}
	else
		{
		User::Leave(KErrNotSupported);
		}	
		
	FreeBuffer();
	}
	
//
//
// Class for two-phased retrieval of Wlan SIDs list
//
//

EXPORT_C CRetrieveMobilePhonePreferredWlanSIDs* CRetrieveMobilePhonePreferredWlanSIDs::NewL(RMobilePhone& aPhone)
/**
Creates an instance of the CActive derived CRetrieveMobilePhonePreferredWlanSIDs class

@param aPhone Reference to clients RMobilePhone sub-session. 
@return A pointer to the newly created CRetrieveMobilePhonePreferredWlanSIDs object.
 
@capability None
*/
	{
	CRetrieveMobilePhonePreferredWlanSIDs* r=new(ELeave) CRetrieveMobilePhonePreferredWlanSIDs(aPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}	

CRetrieveMobilePhonePreferredWlanSIDs::CRetrieveMobilePhonePreferredWlanSIDs(RMobilePhone& aPhone) : 
	CAsyncRetrievePhoneList(aPhone)
	{
	}

void CRetrieveMobilePhonePreferredWlanSIDs::ConstructL()
	{
	iIpcPhase1=EMobilePhoneGetPreferredWlanSIDsPhase1;
	iIpcPhase2=EMobilePhoneGetPreferredWlanSIDsPhase2;
	iIpcCancel=EMobilePhoneGetPreferredWlanSIDsCancel;
	}
	
EXPORT_C CRetrieveMobilePhonePreferredWlanSIDs::~CRetrieveMobilePhonePreferredWlanSIDs()
/**
Destructor
*/
	{
	delete iResults;
	}
	
EXPORT_C CMobilePhoneStoredWlanSIDList* CRetrieveMobilePhonePreferredWlanSIDs::RetrieveListL()
/**
Returns a pointer to the retrieved list.

@leave KErrNotFound If there is no valid list object to return to the client. 
@return CMobilePhoneStoredWlanSIDList Pointer to the Wlan Specific identifier list(user/Operator controlled).
If there are no entries in the list then a handle to an empty list is returned.

@capability None
*/
	{
	if (iResults)
		{
		// Give ownership of list to caller of this method
		CMobilePhoneStoredWlanSIDList* ptr=iResults;
		iResults = NULL;
		return ptr;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}

EXPORT_C void CRetrieveMobilePhonePreferredWlanSIDs::Start(TRequestStatus& aReqStatus)
/**
Starts the retrieval of the stored Wlan Specific identifier list.

@param aReqStatus Returns the result code after the asynchronous call completes.
If the list is empty then KErrNone is returned.
 
@capability ReadDeviceData
*/
	{
	if (!CompleteIfInUse(aReqStatus))
		{
		delete iResults;
		iResults = NULL;
		CAsyncRetrieveVariableLengthBufferV2::Start(aReqStatus,&iId,&iId);
		}
	}

void CRetrieveMobilePhonePreferredWlanSIDs::RestoreListL()
/**
Restores a list from a buffer that contains the streamed version of the list
 */
	{
	iResults=CMobilePhoneStoredWlanSIDList::NewL();
	iResults->RestoreL(iResultsPtr);
	FreeBuffer();
	}
