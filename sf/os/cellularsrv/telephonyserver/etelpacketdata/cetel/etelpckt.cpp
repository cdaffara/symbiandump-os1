// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ETel Multimode Packet Data API
// GPRS Release 97/98, CDMAOne, CDMA2000, Release 99 and Release 4.
// 
// Copyright (c) Symbian Software Ltd. 2002-2009.  All rights reserved.
// 
//

#include <et_clsvr.h>
#include <etelext.h>

// Packet Data header files
#include "pcktptr.h"
#include <pcktcs.h>
#include "pcktlist.h"
#include "etelpckt.h"


// Used by ETel to instruct the TSY to create a name for the newly opened object
_LIT(KETelNewContextName, "::");

TPacketBase::TPacketBase()
	{}

EXPORT_C TInt TPacketBase::ExtensionId() const
/**
	Returns the value of iExtensionId for the associated class.
	e.g. iExtensionId=KETelExtPcktV2 for all the V2 classes.

	@return The protected data member iExtensionId, which specifies the API version number.

*/
	{
	return iExtensionId;
	}

/***********************************************************************************/
//
// RPacketContext
//
/***********************************************************************************/

EXPORT_C RPacketContext::RPacketContext()
	:iEtelPacketContextPtrHolder(NULL)

/** 
	Standard constructor. 
*/
	{
	}

EXPORT_C void RPacketContext::ConstructL()
/**
	This function creates an instance of CPacketContextPtrHolder and assigns it to the
	iEtelPacketContextPtrHolder pointer.
 
*/
	{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iEtelPacketContextPtrHolder = CPacketContextPtrHolder::NewL(CEtelPacketPtrHolder::EMaxNumPacketContextPtrSlots, CEtelPacketPtrHolder::EMaxNumPacketContextPtrCSlots);
	}

EXPORT_C void RPacketContext::Destruct()
/**
	This function deletes and NULLs the iEtelPacketContextPtrHolder pointer.
 
*/
	{
	delete iEtelPacketContextPtrHolder;
	iEtelPacketContextPtrHolder = NULL;
	}

EXPORT_C TInt RPacketContext::OpenNewContext(RPacketService& aPacketNetwork,TDes& aContextName)
// This function may be called by the client application.
/**
	Creates a primary PDP context (i.e. RPacketContext) on an existing RPacketService
	connection.

	There is a global mutex around this method and the RPacketService::OpenNewSecondaryContext()
	method. There is a potential problem with an Open function that makes an initial "set-up"
	function call - another client could also call OpenNewSecondaryContext() or
	OpenNewContext() at the same time, and the wrong client's OpenNewObjectL() will reach the
	TSY first. This would mean clients would receive handles to the wrong context!
	To avoid this, the API code for both OpenNewContext() and OpenNewSecondaryContext() must
	have a shared global mutex around it, so only one client can call either of these two
	functions at any one time.
	
	The TSY will return KErrUmtsMaxNumOfContextExceededByPhone if the number of PDP contexts
	supported by the phone is exceeded. The TSY will return KErrUmtsMaxNumOfContextExceededByNetwork
	if the number of PDP contexts supported by the network is exceeded.

	An opened RPacketContext must be closed explicitly by calling Close() to
	prevent a resource (memory) leak.

	@param aPacketNetwork An RPacketContext may only be opened from an existing RPacketService
	subsession. A client application must therefore pass a reference to their previously
	instantiated RPacketService object.

	@param aContextName When the new context is created, the TSY will assign it a unique name
	and pass this name back to the client using this parameter. The client can use this name to
	uniquely identify the particular context.
	@return KErrNone if successful, a system-wide error code if not.

@capability None
*/
	{
	RSessionBase* session=&aPacketNetwork.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle)); // client has no existing session with ETel

	TRAPD(ret,ConstructL());
	if (ret)
		return ret;

	TInt subSessionHandle=aPacketNetwork.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=0,PanicClient(EEtelPanicNullHandle)); // client has no existing sub-session!
	TPtrC name(KETelNewContextName); // necessary so that server knows to ask TSY for new name

	SetSessionHandle(*session);
	
	aPacketNetwork.iMutex.Wait();					// wait for access to the mutex	



	TIpcArgs args(&name,&aContextName,subSessionHandle);
	ret= CreateSubSession(*session,EEtelOpenFromSubSession,args);
	
	aPacketNetwork.iMutex.Signal();					// Unblock the mutex

	if (ret)
		Destruct();

	return ret;
	}

EXPORT_C TInt RPacketContext::OpenExistingContext(RPacketService& aPacketNetwork, const TDesC& aContextName)
// This function may be called by the client application.
/**
	Opens a handle on an existing context (RPacketContext) object, either primary or secondary,
	identified by a name.
	
	KErrNotFound is returned if the object does not exist.

	An opened RPacketContext must be closed explicitly by calling Close() to
	prevent a resource (memory) leak.
	
	@param aPacketNetwork This identifies the packet network (RPacketService) the particular
	context to be opened belongs. All existing contexts must have an RPacketService parent.
	
	@param aContextName This uniquely identifies to ETel which existing RPacketContext object
	the client wants to open. This name was previously assigned by the TSY when the specified
	RPacketContext was created, using OpenNewContext().
	
	@return KErrNone if successful, KErrNotFound if the object does not exist.

@capability None
*/
	{
	RSessionBase* session=&aPacketNetwork.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}
	TInt subSessionHandle=aPacketNetwork.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=0,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(aContextName.Length()!=0,PanicClient(KErrBadName));

	SetSessionHandle(*session);
	TIpcArgs args(&aContextName,TIpcArgs::ENothing,subSessionHandle);
	ret = CreateSubSession(*session,EEtelOpenByNameFromSubSession,args);
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RPacketContext::Close()
// This function may be called by the client application.
/** Closes the client's current sub-session with ETel. Any outstanding requests
	the client may have with ETel (notifications, for example) will be automatically
	destroyed by ETel.
	
	An opened RPacketContext must be closed explicitly by calling Close() to
	prevent a resource (memory) leak. 
@capability None
*/
	{
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C void RPacketContext::SetConfig(TRequestStatus& aStatus, const TDesC8& aConfig) const
// This function may be called by the client application.
/** 
	Passes to the TSY all the parameters necessary to configure the particular 
	context. The parameters are passed via a packaged TContextConfigGPRS/
	TContextConfigCDMA/TContextConfigR99_R4/RPacketMbmsContext::TContextConfigMbmsV1
	object depending on the current network mode.
	
	This is an asynchronous function.
	
	In the case of this function call, the value for each TContextConfigXXX parameter will be 
	input by the client and probably obtained from CommDB. To determine how to correctly unpack 
	the descriptor inside the TSY, the TSY must first cast the descriptor to a 
	TPacketDataConfigBase pointer and check whether ExtensionId() returns a KConfigGPRS, 
	KConfigCDMA,KConfigRel99Rel4 or KConfigMBMS. The TSY will then cast the descriptor to the correct 
	TPacketDataConfigBase-derived class.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketContextSetConfig) to cancel 
	a previously placed asynchronous SetConfig() request.
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide 
	error codes.
	@param aConfig A TContextConfigXXX object packaged as a TPckg<TContextConfigXXX>. 

@capability WriteDeviceData
*/
	{
	Set(EPacketContextSetConfig, aStatus, aConfig);
	}

EXPORT_C void RPacketContext::GetConfig(TRequestStatus& aStatus, TDes8& aConfig) const
// This function may be called by the client application.
/** 
	Gets the configuration of the current context. 
	
	This is an asynchronous function.
	
	In the case of this function call, the value for each TContextConfigXXX parameter will 
	be input by the TSY.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketContextGetConfig) to cancel 
	a previously placed asynchronous GetConfig() request.
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide 
	error codes.
	@param aConfig On completion, a TContextConfigXXX context configuration object packaged 
	as a TPckg<TContextConfigXXX>.  

@capability ReadDeviceData
*/
	{
	Get(EPacketContextGetConfig, aStatus, aConfig);
	}

EXPORT_C void RPacketContext::NotifyConfigChanged(TRequestStatus& aStatus, TDes8& aConfig) const
// This function may be called by the client application.
// At the RPacketContext level, there are only minor differences between GPRS and UMTS contexts.
/**
	Allows a client to be notified whenever a change in the current context is 
	detected and gets the updated configuration.
	
	This is an asynchronous function.
	
	In the case of this function call, the value for each parameter will be input by the TSY.
	
	It should be noted that once the notification completes, the client must then re-post the 
	notification if they wish to continue receiving the notification.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketContextNotifyConfigChanged) 
	to cancel a previously placed asynchronous NotifyConfigChanged() request.
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide 
	error codes.
	@param aConfig On completion, a TContextConfigXXX context configuration object packaged 
	as a TPckg<TContextConfigXXX>.

@capability ReadDeviceData
*/
	{
	Get(EPacketContextNotifyConfigChanged, aStatus, aConfig);
	}

EXPORT_C void RPacketContext::Activate(TRequestStatus& aStatus) const
// This function may be called by the client application.
/**
	Activates a context that has been previously configured using SetConfig(). 
	Once this function completes, the context will remain in an 'Activating' state 
	until a layer 2 connection (e.g.: PPP or SLIP) has been successfully negotiated 
	between the phone and the packet network gateway. Once the link layer has 
	been negotiated, the context can be considered to be 'Activated'.
	
	This is an asynchronous function.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketContextActivate) to cancel 
	a previously placed asynchronous Activate() request.
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide 
	error codes.

@capability NetworkServices
*/
	{
	Blank(EPacketContextActivate,aStatus);
	}

EXPORT_C void RPacketContext::Deactivate(TRequestStatus& aStatus) const
// This function may be called by the client application.
/**
	Deactivates a context previously configured on the phone.
	
	This is an asynchronous function.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketContextDeactivate) to cancel 
	a previously placed asynchronous Deactivate() request.
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide
	error codes.

@capability NetworkServices
*/
	{
	Blank(EPacketContextDeactivate, aStatus);
	}

EXPORT_C void RPacketContext::Delete(TRequestStatus& aStatus) const
/** Deletes a context previously configured on the phone.
	
	This asynchronous function may be called by the client application.
	
	Although the context is deleted from the TSY (and/or ME), the client's 
	RPacketContext/RPacketMbmsContext subsession remains. 
	To remove this, a client must call Close(). 
	
	This is an asynchronous function.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketContextDelete) to cancel 
	a previously placed asynchronous Delete() request.
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide 
	error codes.

@capability WriteDeviceData
*/
	{
	Blank(EPacketContextDelete, aStatus);
	}

EXPORT_C void RPacketContext::LoanCommPort(TRequestStatus& aStatus, RCall::TCommPort& aDataPort) const
/** 
	Provides a client with details of the comm port to be used for sending/receiving 
	user data in version 1 of the API. This comm port is used by Network Interfaces, NIFs, 
	(for example: PPP.NIF) to send and receive data. In a 2-box device this port would generally 
	be the same as that used by the TSY to set up and control the connection. 
	In a 1-box device this port would generally be different to the port used 
	by the TSY to set up and control the connection.
	
	This asynchronous function may be called by the client application.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketContextLoanCommPort) to 
	cancel a previously placed asynchronous LoanCommPort() request.
	
	@deprecated Superceded in v7.0S
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide 
	error codes.
	@param aDataPort A reference to a TCommPort class containing information on 
	the port, including its name and the associated CSY used to access it. 

@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iEtelPacketContextPtrHolder->Set(CEtelPacketPtrHolder::ESlotDataPort,aDataPort);

	Get(EPacketContextLoanCommPort,aStatus,ptr1);
	}

EXPORT_C void RPacketContext::RecoverCommPort(TRequestStatus& aStatus) const
/**
	Allows a client, who previously called LoanCommPort(), to inform ETel and hence 
	the TSY that it has finished with the comm port, in version 1 of the API.
	
	This asynchronous function may be called by the client application.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketContextRecoverCommPort) 
	to cancel a previously placed asynchronous RecoverCommPort() request.
	
	@deprecated Superceded in v7.0S.
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide 
	error codes.

@capability NetworkServices
*/
	{
	Blank(EPacketContextRecoverCommPort, aStatus);
	}

EXPORT_C TInt RPacketContext::GetStatus(TContextStatus& aContextStatus) const
/**
	Allows a client to get the current status of the context.
	
	This synchronous function may be called by the client application.
	
	@param aContextStatus On return, the current status of the context.
	@return KErrNone if successful, a system-wide error code if not.

@capability None
*/
	{
	TPckg<TContextStatus> ptr1(aContextStatus);
	return Get(EPacketContextGetStatus, ptr1);
	}

EXPORT_C void RPacketContext::NotifyStatusChange(TRequestStatus& aStatus,TContextStatus& aContextStatus)
// This function may be called by the client application.
/**
	Allows a client to receive a notification whenever a change in the status of
	the context is detected and gets the new context status.
	
	This is an asynchronous function.
	
	Once the notification completes, the client must re-post the notification 
	if they wish to continue receiving further status change notifications.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketContextNotifyStatusChange) 
	to cancel a previously placed asynchronous NotifyStatusChange() request.

	@param aStatus KErrNone if successful, otherwise another of the system-wide 
	error codes.
	@param aContextStatus On completion, the new context status.

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iEtelPacketContextPtrHolder->Set(CEtelPacketPtrHolder::ESlotContextStatus,aContextStatus);

	Get(EPacketContextNotifyStatusChange,aStatus,ptr1);
	}	

EXPORT_C TInt RPacketContext::GetProfileName(TName& aQoSProfile) const
/**
	Allows a client to retrieve the name of the Quality of Service profile 
	(RPacketQoS) associated with the context.
	
	This is an synchronous function.
	
	@param aQoSProfile On return, a reference to the name of the existing QoS 
	profile. If no profile has been defined then this name will be NULL.
	@return KErrNone if successful, a system-wide error code if not.

@capability None
*/
	{
	TPckg<TName> ptr1(aQoSProfile);
	return Get(EPacketContextGetProfileName, ptr1); 
	}

EXPORT_C TInt RPacketContext::GetDataVolumeTransferred(TDataVolume& aVolume) const
/**
	Allows a client to get details on the current amount of data transmitted and 
	received over the airlink since the context was activated, in version 1 of the API.
	
	This synchronous method may be called by the client application.
	
	@deprecated Use asynchronous version from v7.0S.
	@param aVolume On return, a reference to TDataVolume with the current amount 
	of data transmitted and received.
	@return KErrNone if successful, a system-wide error code if not.

@capability ReadUserData
*/
	{
	TPckg<TDataVolume> ptr1(aVolume);
	return Get(EPacketContextGetDataVolumeTransferred, ptr1);
	}

EXPORT_C void RPacketContext::GetDataVolumeTransferred(TRequestStatus& aStatus, TDataVolume& aVolume) const
//This function may be called by the client.
/**
	Allows a client to get details on the current amount of data transmitted and received over the airlink 
	since the context was activated, for version 2 of this API onwards.
	
	This is an asynchronous function. Note that the synchronous variant of this method is deprecated.
	
	Introduced in v7.0S.
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide error codes.
	@param aVolume On return, a reference to TDataVolume with the current amount of data transmitted and 
	received.

@capability ReadUserData
*/
	{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1 = iEtelPacketContextPtrHolder->Set(CEtelPacketPtrHolder::ESlotGetDataVolumeTransferred, aVolume);
	
	Get(EPacketContextGetDataVolumeTransferred, aStatus, ptr1);
	}

EXPORT_C void RPacketContext::NotifyDataTransferred(TRequestStatus& aStatus, TDataVolume& aVolume, TUint aRcvdGranularity,TUint aSentGranularity) const
// This function may be called by the client application.
/**
	Allows a client to be notified of a change in the volume of data transmitted 
	and received over the airlink since the context was activated and gets the 
	new volume.
	
	The notification will only complete when the change in volume increases by 
	a client-specified amount (granularity). The default granularity for each 
	direction (rx/tx) is 4KB. 
	
	This is an asynchronous function.

	Once the notification completes, the client must then re-post the notification 
	if they wish to continue receiving further notifications.

	Use RTelSubSessionBase::CancelAsyncRequest(EPacketContextNotifyDataTransferred) 
	to cancel a previously placed asynchronous NotifyDataTransferred() request.

	@param aStatus KErrNone if successful, otherwise another of the system-wide 
	error codes.
	@param aVolume On return, a reference to the current amount of data transmitted 
	and received.
	@param aRcvdGranularity The granularity for the volume of received data.
	@param aSentGranularity The granularity for the volume of transmitted data.

@capability ReadUserData
*/
	{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iEtelPacketContextPtrHolder->iNotifyDataTransferRequest.iRcvdGranularity=aRcvdGranularity;
	iEtelPacketContextPtrHolder->iNotifyDataTransferRequest.iSentGranularity=aSentGranularity;

	TPtr8& ptr1=iEtelPacketContextPtrHolder->Set(CEtelPacketPtrHolder::ESlotDataVolume,aVolume);
	TPtr8& ptr2=iEtelPacketContextPtrHolder->Set(CEtelPacketPtrHolder::ESlotGranularity,iEtelPacketContextPtrHolder->iNotifyDataTransferRequest);

	Get(EPacketContextNotifyDataTransferred, aStatus, ptr1, ptr2);
	}

EXPORT_C void RPacketContext::GetConnectionSpeed(TRequestStatus& aStatus, TUint& aRate) const
// This function may be called by the client application.
/**
	Gets the current connection speed.
	
	This is an asynchronous function.

	Use RTelSubSessionBase::CancelAsyncRequest(EPacketContextGetConnectionSpeed) 
	to cancel a previously placed asynchronous GetConnectionSpeed() request.

	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide 
	error codes.
	@param aRate On return, the current connection bandwidth in bits per second. 

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1 = iEtelPacketContextPtrHolder->Set(CEtelPacketPtrHolder::ESlotGetSpeed, aRate);
	Get(EPacketContextGetConnectionSpeed, aStatus, ptr1);
	}

EXPORT_C void RPacketContext::NotifyConnectionSpeedChange(TRequestStatus& aStatus, TUint& aRate) const
//This function may be called by the client.
/**
	Allows a client to be notified whenever the current connection speed changes 
	and gets the current connection speed.
	
	This is an asynchronous function.
	
	Once the notification completes, the client must then re-post the notification 
	if they wish to continue receiving further notifications.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketContextNotifyConnectionSpeedChange) 
	to cancel a previously placed asynchronous NotifyConnectionSpeedChange() request.
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide 
	error codes.
	@param aRate On return, the current connection bandwidth in bits per second.

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1=iEtelPacketContextPtrHolder->Set(CEtelPacketPtrHolder::ESlotNotifySpeed, aRate);
	Get(EPacketContextNotifyConnectionSpeedChange, aStatus, ptr1);
	}

EXPORT_C void RPacketContext::GetConnectionInfo(TRequestStatus& aStatus, TDes8& aInfo) const
// This function may be called by the client.
/**
	Gets the current connection information.
	
	This is an asynchronous function.
	
	In the case of this function call, the members of class TConnectionInfoV1 or any later
	derived versions supplied by the client will be returned, eg. TConnectionInfoVx.

	Use RTelSubSessionBase::CancelAsyncRequest(EPacketContextGetConnectionInfo) 
	to cancel a previously placed asynchronous GetConnectionInfo() request.

	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide 
	error codes.
	@param aInfo TConnectionInfoV1 or any later derived versions supplied by the client packaged
	as a TPckg<TConnectionInfoVx>.

	N.B. only supported if telephony was built with SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW macro
@capability None
*/
	{
	Get(EPacketContextGetConnectionInfo, aStatus, aInfo);
	}
	
EXPORT_C void RPacketContext::NotifyConnectionInfoChange(TRequestStatus& aStatus, TDes8& aInfo) const
// This function may be called by the client.
/**
	Allows a client to be notified whenever the connection information changes 
	and gets the updated connection information.
	
	This is an asynchronous function.
	
	Once the notification completes, the client must then re-post the notification 
	if they wish to continue receiving further notifications.

	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide 
	error codes.
	@param aInfo TConnectionInfoV1 or any later derived versions supplied by the client packaged
	as a TPckg<TConnectionInfoVx>.

	N.B. only supported if telephony was built with SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW macro
@capability None
*/
	{
	Get(EPacketContextNotifyConnectionInfoChange, aStatus, aInfo);
	}		

EXPORT_C TInt RPacketContext::GetLastErrorCause(TInt& aError) const
/**
	Gets the most recent error to have occurred. This may be used, for instance,
	to retrieve which error caused PPP to close down prematurely.
	
	This function may be called by the client.
	
	@param aError On return, the last error received or generated by the TSY.
	@return KErrNone if successful, a system-wide error code if not.

@capability None
*/
	{
	TPckg<TInt> ptr1(aError);
	return Get(EPacketContextGetLastErrorCause, ptr1);
	}

EXPORT_C void RPacketContext::InitialiseContext(TRequestStatus& aStatus, TDes8& aDataChannelV2) const
//This function may be called by the client.
/**
	Enables asynchronous resource reservation for a new primary or secondary context at the creation phase,
	since version 2 of the API.
	
	This is an asynchronous function.
	
	Supersedes the LoanCommPort() and RecoverCommPort() methods, which are now deprecated.
	This new method enables asynchronous resource reservation for a new primary or
	secondary context at the creation phase.
	
	The InitialiseContext() method still contains the information that was previously in the
	LoanCommPort() method, i.e. details of the comm port to be used for sending/receiving user
	data. It however has an additional member which will return any additional information
	generated by context creation.
	
	Introduced in v7.0S
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide
	error codes.
	@param aDataChannelV2 Reference to a TDataChannelV2 class packaged inside a TPckg<> class.
	The TDataChannelV2 class contains the members of the struct used previously by the
	deprecated LoanCommPort() method as well as an additional member which will now return
	technical information generated when a context is created, for instance, a context name or
	id. This name/id is different from that assigned by the TSY when the OpenNewContext() or
	OpenNewSecondaryContext() methods are called.

@capability NetworkServices
*/
 	{
	Get(EPacketContextInitialiseContext, aStatus, aDataChannelV2);
	}

EXPORT_C void RPacketContext::EnumeratePacketFilters(TRequestStatus& aStatus, TInt& aCount) const
/**
	This asynchronous method may be called by the client application.
	It returns the number of defined packet filters in the Traffic Flow Template (TFT) associated
	with this context since version 2 of the API.
	Supported from v7.0S.
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide error codes.
	@param aCount Number of packet filters in the TFT.

@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1 = iEtelPacketContextPtrHolder->Set(CEtelPacketPtrHolder::ESlotEnumeratePacketFilters, aCount);
	Get(EPacketContextEnumeratePacketFilters, aStatus, ptr1);
	}

EXPORT_C void RPacketContext::GetPacketFilterInfo(TRequestStatus& aStatus, TInt aIndex, TDes8& aPacketFilterInfo) const
/**
	This asynchronous method may be called by the client application.
	Retrieves information about the packet filter specified by the aIndex parameter since version 2 of this API.
	It is usual to use this function in conjunction with the EnumeratePacketFilters() method,
	which returns the current number of defined packet filters in the TFT in its 'aCount' argument.
	
	Supported from v7.0S
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide error codes.
	
	@param aIndex Specifies packet filter for which information is to be retrieved.
	The valid range for aIndex will be between 0 and (n-1) where n is the number returned in
	aCount. If an invalid index is specified the method will return KErrArgument in its aStatus.
	
	@param aPacketFilterInfo	Reference to a TPacketFilterV2 class packaged inside a TPckg<> class.

@capability ReadDeviceData
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iEtelPacketContextPtrHolder->iGetPacketFilterInfoIndex = aIndex;
	TPtr8& ptr1=iEtelPacketContextPtrHolder->Set(CEtelPacketPtrHolder::ESlotGetPacketFilterInfo,iEtelPacketContextPtrHolder->iGetPacketFilterInfoIndex);

	Get(EPacketContextGetPacketFilterInfo, aStatus, ptr1, aPacketFilterInfo);
	}

EXPORT_C void RPacketContext::AddPacketFilter(TRequestStatus& aStatus, const TDesC8& aPacketFilterInfo) const
/**
	This asychronous method may be called by the client application.
	It sets up a packet filter in the TFT belonging to this context with the ID specified
	in the packet filter information since version 2 of this API.
	Supported from v7.0S .
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide error codes.
	@param aPacketFilterInfo	Reference to a TPacketFilterV2 class packaged inside a TPckg<> class.

@capability WriteDeviceData
@capability NetworkServices
*/
	{
	Set(EPacketContextAddPacketFilter, aStatus, aPacketFilterInfo);
	}

EXPORT_C void RPacketContext::RemovePacketFilter(TRequestStatus& aStatus, TInt aId) const
/**
	This asynchronous method may be called by the client application.
	Removes a packet filter from the TFT belonging to this context, since version 2 of the API.
	
	Supported from v7.0S
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide error codes.
	@param aId Specifies the packet filter.

@capability WriteDeviceData
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iEtelPacketContextPtrHolder->iRemovePacketFilterId = aId;
	TPtr8& ptr1=iEtelPacketContextPtrHolder->Set(CEtelPacketPtrHolder::ESlotRemovePacketFilter,iEtelPacketContextPtrHolder->iRemovePacketFilterId);

	Set(EPacketContextRemovePacketFilter, aStatus, ptr1);
	}

EXPORT_C void RPacketContext::ModifyActiveContext(TRequestStatus& aStatus) const
/**
	This asynchronous method may be called by the client application.
	It invokes an MS-initiated  modification of an active context in the network since version 2 of the API.
	The client must previously have altered either the requested QoS profile of the context
	and/or the TFT whilst the context was active before calling this method.
	
	Supported from v7.0S
	
	@param aStatus On return, KErrNone if successful, otherwise another of the system-wide error codes.

@capability NetworkServices
*/
	{
	Blank(EPacketContextModifyActiveContext, aStatus);
	}

EXPORT_C TInt RPacketContext::OpenNewSecondaryContext(RPacketService& aPacketService, const TDesC& aExistingContextName, TDes& aNewContextName)
// This function may be called by the client application.
/**
	Creates a new secondary context from an existing context (either primary or secondary)
	since version 2 of the API.
	
	The configuration of the existing context is used by the new one so the client need not
	call the RPacketContext::SetConfig() method.
	
	Immediately after the creation of the newly created secondary context, a client must -
	a) ensure that they specify the requested QoS using an RPacketQoS object;
	b) add a packet filter to the secondary object. This assumes that there already exists a
	primary or secondary context for this NIF without a TFT. If this is not the case, it is
	not mandatory that a packet filter is added to this new secondary context before activation.
	The TSY will return KErrNotSupported if it does not support secondary contexts.
	
	The TSY will return KErrUmtsMaxNumOfContextExceededByPhone if the number of PDP contexts
	supported by the phone is exceeded. The TSY will return KErrUmtsMaxNumOfContextExceededByNetwork
	if the number of PDP contexts supported by the network is exceeded.
	
	There is a global mutex around this method and the RPacketService::OpenNewContext() method.
	There is a potential thread timing issue with an Open function that makes an initial "set-up"
	function call - another client could also call OpenNewSecondaryContext() or OpenNewContext()
	at the same time, and the wrong client's OpenNewObjectL() will reach the TSY first. This would
	mean clients would receive handles to the wrong context! To avoid this, the API code for
	both OpenNewContext() and OpenNewSecondaryContext() must have a shared global mutex around
	it, so only one client can call either of these two functions at any one time.
	
	@param aPacketService An RPacketContext may only be opened from an existing RPacketService subsession.
	A client application must therefore pass a reference to their previously instantiated RPacketService object.
	
	@param aExistingContextName : Name of the existing context which is to spawn a secondary
	context.
	
	@param aNewContextName : New Secondary context name is assigned by the TSY.

@capability None
*/
	{
	RSessionBase* session=&aPacketService.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}
	TInt subSessionHandle=aPacketService.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=0,PanicClient(EEtelPanicNullHandle));

	TPtrC name(KETelNewContextName);	// The server knows to ask the TSY for a new name.

	SetSessionHandle(*session);

	aPacketService.iMutex.Wait();					// wait for access to the mutex
	
	TInt retVal=aPacketService.PrepareOpenSecondary(aExistingContextName);

	if (retVal)
		{		
		aPacketService.iMutex.Signal();				// Unblock the mutex
		return retVal;
		}
	TIpcArgs args(&name,&aNewContextName,subSessionHandle);
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);
		
	aPacketService.iMutex.Signal();					// Unblock the mutex
	
	if (ret)
		Destruct();

	return ret;
	}


EXPORT_C RPacketContext::TContextConfigGPRS::TContextConfigGPRS()
: TPacketDataConfigBase(), iPdpType(EPdpTypeIPv4), iAccessPointName(NULL), iPdpAddress(NULL),
  iPdpCompression(0), iAnonymousAccessReqd(ENotRequired), iUseEdge(EFalse),
  iProtocolConfigOption(), iNWIContext(EFalse)
 /**
	Standard constructor. Sets the member data values to the defaults.
	This class contains member data compatible with parameters required for configuring a
	context on the GPRS and UMTS packet networks. 
	
	The iExtensionId parameter, set automatically by ETel, is used by the TSY when unpacking this 
	class from a TPckg<> to indicate it is to be unpacked as a TContextConfigGPRS class.
 */
	{
	iExtensionId = KConfigGPRS;
	};
	
EXPORT_C RPacketContext::TContextConfigR99_R4::TContextConfigR99_R4()
: TPacketDataConfigBase(), iPdpType(EPdpTypeIPv4), iAccessPointName(NULL), iPdpAddress(NULL), 
  iUseEdge(EFalse), iProtocolConfigOption(), iNWIContext(EFalse), iPFI(EBestEffort)
/**
	Standard constructor. Sets the member data values to the defaults.
	
	This class contains member data compatible with parameters required for configuring a 
	context on the R99/R4 packet networks.	
	The iExtensionId parameter, set automatically by ETel, is used by the TSY when unpacking this 
	class from a TPckg<> to indicate it is to be unpacked as a TContextConfigR99_R4 class.
*/
	{
	iExtensionId = KConfigRel99Rel4;
	};
	
EXPORT_C RPacketContext::TContextConfigCDMA::TContextConfigCDMA()
: TPacketDataConfigBase(), iServiceOption(KLowSpeedData), iProtocolType(EPdpTypePPP)
/**
	Standard constructor. Sets all the member data values to the defaults.
	 
	The iExtensionId parameter, set automatically by ETel, is used by the TSY when unpacking this 
	class from a TPckg<> to indicate it is to be unpacked as a TContextConfigCDMA class.
	The Service option is set to KLowSpeedData (Default: Assume IWF/PDSN supports 2G service
	option. (exact value of "LowSpeedData" is to be decided by TSY implementation)). 
	The Protocol type is set to EPdpTypePPP (Default: Assume IWF/PDSN supports PPP link layer)
 */
	{
	iExtensionId = KConfigCDMA;
	};

EXPORT_C RPacketContext::TConnectionInfoV1::TConnectionInfoV1()
: TConnectionInfoBase(), iHSDPACategory(0), iHSUPACategory(0)
/**
	Standard constructor. Sets the member data values to the defaults.
	
	This class contains miscellaneous information about packet data connection.

	The iExtensionId parameter, set automatically by ETel, is used by the TSY 
	when unpacking this class from a TPckg<> to indicate it is to be unpacked 
	as a TConnectionInfoV1 class.
*/
	{
	iExtensionId = KConnectionInfoV1;
	iValid = 0;
	};

EXPORT_C void RPacketContext::GetDnsInfo(TRequestStatus& aStatus, TDes8& aDnsInfo) const
/**
	Extracts DNS server names from the context since version 2 of the API.
	
	Supported from v7.0S.
	
	@param aStatus TRequestStatus object with which error code is returned
	@param aDnsInfo Packaged TDnsInfoV2 object.
	
	GetDNSInfo() is needed when the client uses dynamic IPv4 PDP addresses. 
	After the context is activated the client needs to know the actual primary 
	and secondary DNS addresses to use when sending packet data. As the network 
	may have supplied these addresses there is a need for an API method to get 
	these addresses from the TSY.
 
@capability ReadDeviceData
*/
	{
		__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle)); 
		Get(EPacketContextGetDNSInfo, aStatus, aDnsInfo);
	};


EXPORT_C RPacketContext::TDnsInfoV2::TDnsInfoV2()
: TPacketBase(), iPrimaryDns(0), iSecondaryDns(0)
/**
	Standard constructor since version 2 of the API.
	
	@see KETelExtPcktV2
*/
	{
	iExtensionId = KETelExtPcktV2;
	}

EXPORT_C RPacketContext::TProtocolConfigOptionV2::TProtocolConfigOptionV2()
: TPacketBase(), iChallenge(0), iResponse(0), iId(0), iDnsAddresses(),
  iMiscBuffer(0)
/**
	Constructor for TProtocolConfigOptionV2 class

	This class will hold authentication data encapsulated in TAuthInfo as well as further data that 
	may be required for CHAP protocol authentication, such a challenge and response
*/
	{
	iExtensionId = KETelExtPcktV2;

	//
	// Initialise the TAuthInfo structure...
	//
	iAuthInfo.iProtocol = EProtocolNone;
	iAuthInfo.iUsername.Zero();
	iAuthInfo.iPassword.Zero();
	};

EXPORT_C RPacketContext::TDataChannelV2::TDataChannelV2()
: TPacketBase(), iCsy(NULL), iPort(NULL), iChannelId(NULL)
/**
	Standard constructor since version 2 of the API.
	
	The member data are initialized to NULL. The client should set 
	the data to valid values. Note that the iCsy and iPort data 
	members are deprecated.
*/
	{
	iExtensionId = KETelExtPcktV2;
	};

EXPORT_C RPacketContext::TPacketFilterV2::TPacketFilterV2()
: TPacketBase(), iId(0), iEvaluationPrecedenceIndex(0), iProtocolNumberOrNextHeader(0), iSrcPortMin(0),
  iSrcPortMax(0), iDestPortMin(0), iDestPortMax(0), iIPSecSPI(0), iTOSorTrafficClass(0),
  iFlowLabel(0)
/**
	Standard constructor since version 2 of the API.
	
	All the member data are initialized to zero. The client should set the required parameters
	to valid values.
	
	Some of the parameters are optional while others are mutually exclusive. Only the Packet 
	filter identifier (iId) and the precedence index (iEvaluationPrecedenceIndex) are 
	mandatory.
*/
	{
	iExtensionId = KETelExtPcktV2;
	TInt i;							//< loop variable

	// Initialize the Domain Source address and Domain Subnet Mask
	for (i=0; i<KIPAddressSize; i++)				
		{
		iSrcAddr[i] = 0;			//< Optional parameter. Zero value implies Not applicable	
		iSrcAddrSubnetMask[i] = 0;	//< Optional parameter. Zero value implies Not applicable
		}
	};

EXPORT_C void RPacketContext::CreateNewTFT(TRequestStatus& aStatus, const TInt aSize) const
/**
	This asynchronous method may be called by the client application. 
	Creates a new Traffic Flow Template.
	
	@param aStatus TRequestStatus reference, enables asynchronous function call.
	@param aSize Number of filters that the TFT contains.

@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iEtelPacketContextPtrHolder->iCreateTFTSize = aSize;
		
	TPtr8& ptr1=iEtelPacketContextPtrHolder->Set(CEtelPacketPtrHolder::ESlotCreateTFT, iEtelPacketContextPtrHolder->iCreateTFTSize);
		
	Set(EPacketContextCreateNewTFT, aStatus, ptr1);	
	}
	
EXPORT_C void RPacketContext::DeleteTFT(TRequestStatus& aStatus) const
/**
	This asynchronous method may be called by the client application. 
	Deletes the existing traffic flow template (including all filters) for this context object.  
	
	@param aStatus TRequestStatus reference, enables asynchronous function call.

@capability NetworkServices
*/
	{		
	Blank(EPacketContextDeleteTFT, aStatus);	
	}

EXPORT_C RPacketContext::TPacketFilterV3::TPacketFilterV3(TPacketFilterType aType)
		:iType(aType), iAllowedDataBitmap(0), iPresentDataBitmap(0)
/**
	Standard Constructor: Version 3 of the API
	
	iType is set to aType which is passed as parameter and can be of the type EPacketFilterTypeOne,
	EPacketFilterTypeTwo or EPacketFilterTypeThree.
	iPresentDataBitmap is initialised to zero.
	iAllowedDataBitmap is set to a valid combination of TPacketFilterV3DataBitmapPos according to
	aType.
	
	@param aType TPacketFilterType - Can take any of the following values: EPacketFilterTypeOne,
	EPacketFilterTypeTwo or EPacketFilterTypeThree.
*/
	{
	iExtensionId = KETelExtPcktV3;

	iAllowedDataBitmap = EPacketFilterSrcAddr | EPacketFilterSrcAddrSubnetMask;
	switch (iType)
		{
		case EPacketFilterTypeOne:
			{			
			iAllowedDataBitmap|=
				(
				EPacketFilterProtocolNumberOrNextHeader |
				EPacketFilterSrcPortMin | EPacketFilterSrcPortMax | 
				EPacketFilterDestPortMin | EPacketFilterDestPortMax |
				EPacketFilterTOSorTrafficClass
				);
			break;
			}
		case EPacketFilterTypeTwo:
			{
			iAllowedDataBitmap|=
				(			
			 	EPacketFilterProtocolNumberOrNextHeader |				
			 	EPacketFilterIPSecSPI |
				EPacketFilterTOSorTrafficClass 
				);
			break;
			}
		case EPacketFilterTypeThree:
			{
			iAllowedDataBitmap|=
				(				 
				EPacketFilterTOSorTrafficClass |
				EPacketFilterFlowLabel 
				);
			break;
			}
		default:
		 	break;
		}
	}
	
EXPORT_C RPacketContext::TPacketFilterV3::TPacketFilterV3()
		:iType(EPacketFilterTypeUnknown), iAllowedDataBitmap(0), iPresentDataBitmap(0)
/**
	Default Constructor
	
	iType is set to EPacketFilterTypeUnknown
*/
	{
		iExtensionId = KETelExtPcktV3;
	}

EXPORT_C TBool RPacketContext::TPacketFilterV3::SetSrcAddr(TIPAddress &aSrcAddr)
/**
	This method may be called by the client application. 
	Sets the iSrcAddr to a SrcAddr only if a valid combination of bitmap is present.
	
	@param aSrcAddr TIPAddress value to be set.

*/
	{
	if(!(EPacketFilterSrcAddr & iAllowedDataBitmap))
		return EFalse;
	Mem::Copy(iSrcAddr,aSrcAddr,sizeof(TIPAddress));
	iPresentDataBitmap |= EPacketFilterSrcAddr;
	return ETrue;
	}
		
EXPORT_C TBool RPacketContext::TPacketFilterV3::SetSrcAddrSubnetMask(TIPAddress &aSrcAddrSubnetMask)				
/**
	This method may be called by the client application. 
	Sets the iSrcAddrSubnetMask to aSrcAddrSubnetMask only if a valid combination of bitmap is present.
	
	@param aSrcAddrSubnetMask TIPAddress value to be set.

*/
	{
	if(!(EPacketFilterSrcAddrSubnetMask &	iAllowedDataBitmap))
		return EFalse;
	Mem::Copy(iSrcAddrSubnetMask,aSrcAddrSubnetMask,sizeof(TIPAddress));
	iPresentDataBitmap |= EPacketFilterSrcAddrSubnetMask;
	return ETrue;
	}

EXPORT_C TBool RPacketContext::TPacketFilterV3::SetProtocolNumberOrNextHeader(TInt aProtocolNumberOrNextHeader)
/**
	This method may be called by the client application. 
	Sets the iProtocolNumberOrNextHeader to aProtocolNumberOrNextHeader only if a valid combination of bitmap is present.
	
	@param aProtocolNumberOrNextHeader TInt value to be set.

*/
	{
	if(!(EPacketFilterProtocolNumberOrNextHeader & iAllowedDataBitmap))
		return EFalse;
	iProtocolNumberOrNextHeader=aProtocolNumberOrNextHeader;	
	iPresentDataBitmap |= EPacketFilterProtocolNumberOrNextHeader;
	return ETrue;
	}
		
EXPORT_C TBool RPacketContext::TPacketFilterV3::SetSrcPortMin(TInt aSrcPortMin)					 		
/**
	This method may be called by the client application. 
	Sets the iSrcPortMin to aSrcPortMin only if a valid combination of bitmap is present.
	
	@param aSrcPortMin TInt value to be set.

*/
	{
	if(!(EPacketFilterSrcPortMin & iAllowedDataBitmap))
		return EFalse;
	iSrcPortMin=aSrcPortMin;
	iPresentDataBitmap |= EPacketFilterSrcPortMin;
	return ETrue;
	}

EXPORT_C TBool RPacketContext::TPacketFilterV3::SetSrcPortMax(TInt aSrcPortMax)
/**
	This method may be called by the client application. 
	Sets the iSrcPortMax to aSrcPortMax only if a valid combination of bitmap is present.
	
	@param aSrcPortMax TInt value to be set.

*/
	{
	if(!(EPacketFilterSrcPortMax & iAllowedDataBitmap))
		return EFalse;
	iSrcPortMax=aSrcPortMax;
	iPresentDataBitmap |= EPacketFilterSrcPortMax;
	return ETrue;
	}
			
EXPORT_C TBool RPacketContext::TPacketFilterV3::SetDestPortMin(TInt aDestPortMin)
/**
	This method may be called by the client application. 
	Sets the iDestPortMin to aDestPortMin only if a valid combination of bitmap is present.
	
	@param aDestPortMin TInt value to be set.

*/
	{
	if(!(EPacketFilterDestPortMin & iAllowedDataBitmap))
		return EFalse;
	iDestPortMin=aDestPortMin;
	iPresentDataBitmap |= EPacketFilterDestPortMin;
	return ETrue;
	}

EXPORT_C TBool RPacketContext::TPacketFilterV3::SetDestPortMax(TInt aDestPortMax)
/**
	This method may be called by the client application. 
	Sets the iDestPortMax to aDestPortMax only if a valid combination of bitmap is present.
	
	@param aDestPortMax TInt value to be set.

*/					
	{
	if(!(EPacketFilterDestPortMax &	iAllowedDataBitmap))
		return EFalse;
	iDestPortMax=aDestPortMax;
	iPresentDataBitmap |= EPacketFilterDestPortMax;
	return ETrue;
	}
		 	
EXPORT_C TBool RPacketContext::TPacketFilterV3::SetSPI(TUint32 aIPSecSPI)	
/**
	This method may be called by the client application. 
	Sets the iIPSecSPI to aIPSecSPI only if a valid combination of bitmap is present.
	
	@param aIPSecSPI TUint32 value to be set.

*/			
	{
	if(!(EPacketFilterIPSecSPI &	iAllowedDataBitmap))
		return EFalse;
	iIPSecSPI=aIPSecSPI;
	iPresentDataBitmap |= EPacketFilterIPSecSPI;
	return ETrue;
	}
	
EXPORT_C TBool RPacketContext::TPacketFilterV3::SetTOSorTrafficClass(TUint16 aTOSorTrafficClass)
/**
	This method may be called by the client application. 
	Sets the iTOSorTrafficClass to aTOSorTrafficClass only if a valid combination of bitmap is present.
	
	@param aTOSorTrafficClass TUint16 value to be set.

*/
	{
	if(!(EPacketFilterTOSorTrafficClass &	iAllowedDataBitmap))
		return EFalse;
	iTOSorTrafficClass=aTOSorTrafficClass;
	iPresentDataBitmap |= EPacketFilterTOSorTrafficClass;
	return ETrue;
	}

EXPORT_C TBool RPacketContext::TPacketFilterV3::SetFlowLabel(TUint32 aFlowLabel)
/**
	This method may be called by the client application.
	Sets the iFlowLabel to aFlowLabel only if a valid combination of bitmap is present.

	@param aFlowLabel TUint32 value to be set.

*/
	{
	if(!(EPacketFilterFlowLabel &	iAllowedDataBitmap))
		return EFalse;
	iFlowLabel=aFlowLabel;
	iPresentDataBitmap|=EPacketFilterFlowLabel ;
	return ETrue;
	}

RPacketContext::CTFTMediaAuthorizationV3::CTFTMediaAuthorizationV3()
{
	iExtensionId = KETelExtPcktV3;
}

EXPORT_C RPacketContext::CTFTMediaAuthorizationV3* RPacketContext::CTFTMediaAuthorizationV3::NewL()
{
	CTFTMediaAuthorizationV3* self = new(ELeave) CTFTMediaAuthorizationV3;
	return self;
}

EXPORT_C RPacketContext::CTFTMediaAuthorizationV3::~CTFTMediaAuthorizationV3()
{
	iFlowIds.Reset();
	iFlowIds.Close();
}

EXPORT_C void RPacketContext::CTFTMediaAuthorizationV3::ExternalizeL(HBufC8*& aBuffer) const
{
/**
Serialize data to the buffer.
If aBuffer points to already allocated memory, client has to be aware that it won't have
access to this location anymore as method reallocates memory and updates aBuffer accordingly.
Therefore, value passed in should not be freed upon completion of the method neither put onto
the cleanup stack before method is invoked.

After completion of the API, it's clients responsibility to control life scope
of allocated memory passed back via aBuffer.

@param aBuffer Heap-based buffer which will contain the serialized data.
*/
	TUint uintLen=sizeof(TUint);
	TUint uintLen16 = sizeof(TUint16);
	TUint size= uintLen/* extension id*/ + uintLen /* Authorization Token length*/ + iAuthorizationToken.Length();
	TUint count=iFlowIds.Count();
	size+=uintLen; /** number of iFlowIds elements*/
	size+= count*(uintLen16+uintLen16);

	if(aBuffer)
		{
	 	delete aBuffer;
	 	aBuffer=NULL;
	 	}
	aBuffer=HBufC8::NewL(size);
	aBuffer->Des().SetLength(size);

	TInt cursor(0);

	Mem::Copy(const_cast<TUint8*>(aBuffer->Des().Ptr()+cursor),(const TUint8*)(&iExtensionId),uintLen);
	cursor+=uintLen;

	TUint len=iAuthorizationToken.Length();
	Mem::Copy(const_cast<TUint8*>(aBuffer->Des().Ptr()+cursor),&len,uintLen);
	cursor+=uintLen;
	Mem::Copy(const_cast<TUint8*>(aBuffer->Des().Ptr()+cursor),iAuthorizationToken.Ptr(),len);
	cursor+=len;

	Mem::Copy(const_cast<TUint8*>(aBuffer->Des().Ptr()+cursor),(const TUint8* )(&count),uintLen);
	cursor+=uintLen;

	for(TUint i=0;i<count;i++)
		{
		Mem::Copy((TUint16*)(aBuffer->Des().Ptr()+cursor),(const TUint16* )(&iFlowIds[i].iMediaComponentNumber),uintLen16);
		cursor+=uintLen16;

		Mem::Copy((TUint16*)(aBuffer->Des().Ptr()+cursor),(const TUint16* )(&iFlowIds[i].iIPFlowNumber),uintLen16);
		cursor+=uintLen16;
		}

}

EXPORT_C void RPacketContext::CTFTMediaAuthorizationV3::InternalizeL(TDes8& aBuffer)
{
	iFlowIds.Reset();

	TUint uintLen=sizeof(TUint);
	TInt cursor=0;

	if(aBuffer.Length() < cursor + uintLen)
		User::Leave(KErrOverflow);	

	Mem::Copy(&iExtensionId,aBuffer.Ptr(),uintLen);
	cursor+=uintLen; 
	
	if(aBuffer.Length() < cursor + uintLen)
		User::Leave(KErrOverflow);

	TInt len(0);
	Mem::Copy(&len,aBuffer.Ptr()+cursor,uintLen);
	cursor+=uintLen;
		
	if(aBuffer.Length() < cursor + len)
		User::Leave(KErrOverflow);
	iAuthorizationToken.Copy(aBuffer.MidTPtr(cursor,len));
	cursor+=len;


	if(aBuffer.Length() < cursor + uintLen)
		User::Leave(KErrOverflow);
	TUint count(0);
	Mem::Copy(&count,aBuffer.Ptr()+cursor,uintLen);
	cursor+=uintLen;

	RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier flowIdentifier;
	
	TUint uintLen16 = sizeof(TUint16);
	for(TUint i=0;i<count;i++)
		{
		if(aBuffer.Length() < cursor + uintLen16 + uintLen16)
			User::Leave(KErrOverflow);
		Mem::Copy(&flowIdentifier.iMediaComponentNumber,aBuffer.Ptr()+cursor,uintLen16);
		cursor+=uintLen16;
		Mem::Copy(&flowIdentifier.iIPFlowNumber,aBuffer.Ptr()+cursor,uintLen16);
		cursor+=uintLen16;
		
		iFlowIds.Append(flowIdentifier);
		}
}
	
EXPORT_C TUint RPacketContext::CTFTMediaAuthorizationV3::ExtensionId() const
{
	return iExtensionId;	
}

EXPORT_C void RPacketContext::AddMediaAuthorizationL(TRequestStatus& aStatus, CTFTMediaAuthorizationV3& aMediaAuthorization) const
/**
	Adds a new set of Authorization Token and Flow Identifiers to the packet context object.
	This asynchronous method may be called by the client application. 
	
	@param aStatus TRequestStatus reference, enables asynchronous function call.
	@param aMediaAuthorization a reference to a CTFTMediaAuthorizationV3 object containing the data that is to be added.

@capability NetworkServices
@capability WriteDeviceData
*/
{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle)); 
	
	delete iEtelPacketContextPtrHolder->iStoreNetworkBuf;
	iEtelPacketContextPtrHolder->iStoreNetworkBuf = NULL;
		
	aMediaAuthorization.ExternalizeL(iEtelPacketContextPtrHolder->iStoreNetworkBuf);
	
	(iEtelPacketContextPtrHolder->iStoreNetworkPtr).Set((iEtelPacketContextPtrHolder->iStoreNetworkBuf)->Des());
	
	Set(EPacketAddMediaAuthorization,aStatus,iEtelPacketContextPtrHolder->iStoreNetworkPtr);	
	
}

EXPORT_C void RPacketContext::RemoveMediaAuthorization(TRequestStatus& aStatus, TAuthorizationToken& aAuthorizationToken) const
/**
	Removes the Authorization Token and Flow Identifiers set from the packet context object. This is identified by aAuthorizationToken.
	This asynchronous method may be called by the client application. 
		
	@param aStatus TRequestStatus reference, enables asynchronous function call.
	@param aAuthorizationToken identifier to the Authorization Token that is to be removed.

@capability NetworkServices
@capability WriteDeviceData
*/
{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iEtelPacketContextPtrHolder->Set(CEtelPacketPtrHolder::ESlotRemoveMediaAuthorization,aAuthorizationToken);

	Set(EPacketRemoveMediaAuthorization, aStatus, ptr1);

}

EXPORT_C RPacketContext::TContextConfig_R5::TContextConfig_R5()
: TContextConfigR99_R4(), iPdpHeaderCompression(ENoHeaderCompression), iPdpDataCompression(ENoDataCompression)
/**
	Standard constructor. Sets the member data values to the defaults.
	
	This class contains member data compatible with parameters required for configuring a 
	context on the R5 packet networks.	
*/
	{
	iExtensionId = KConfigRel5;
	};
	
/***********************************************************************************/
//
// RPacketService
//
/***********************************************************************************/

EXPORT_C RPacketService::RPacketService()
	:iEtelPacketPtrHolder(NULL)
/** Standard constructor. */
	{
	}

EXPORT_C void RPacketService::ConstructL()
/**
	ConstructL for the RPacketService class
 
@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iEtelPacketPtrHolder = CPacketPtrHolder::NewL(CEtelPacketPtrHolder::EMaxNumPacketPtrSlots, CEtelPacketPtrHolder::EMaxNumPacketPtrCSlots);
	}

EXPORT_C void RPacketService::Destruct()
/**
	Destruct() method for the RPacketService class

@capability None
*/
	{
	delete iEtelPacketPtrHolder;
	iEtelPacketPtrHolder = NULL;
	}	

EXPORT_C TInt RPacketService::Open(RPhone& aPhone)
// This function may be called by the client.
/**
	Creates a RPacketService subsession from an existing RPhone session.
	
	An opened RPacketService must be closed explicitily by calling Close() to
	prevent a resource (memory) leak.
	
	@param aPhone A reference to an existing RPhone session the client has previously
	created.
	@return KErrNone if successful. 

@capability None
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=NULL,PanicClient(EEtelPanicNullHandle));

	TRAPD(ret,ConstructL());
	if (ret)
		return ret;

	SetSessionHandle(*session);
	TIpcArgs args(&KPacketName,TIpcArgs::ENothing,subSessionHandle);
	ret= CreateSubSession(*session,EEtelOpenFromSubSession,args);		
	
	if (ret)
		{		
		Destruct();
		return ret;
		}

// initialise synchronisation mutex in EKA2
		TInt mutexHandle= SendReceive(EEtelGlobalKernelObjectHandle);
		if(mutexHandle <=0)
		{
		 Destruct();
		 return KErrBadHandle;
		}
		ret= iMutex.SetReturnedHandle(mutexHandle);
		if( ret != KErrNone)
			Destruct();

	return ret;	
	}

EXPORT_C void RPacketService::Close()
// This function may be called by the client.
/**
	Closes the RPacketService subsession.
	
	An opened RPacketService must be closed explicitily by calling Close() to
	prevent a resource (memory) leak.

@capability None
*/
	{
	iMutex.Close();	
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C void RPacketService::NotifyContextAdded(TRequestStatus& aStatus, TDes& aContextId) const
//This function may be called by the client.
/**
	Notifies the client whenever a new context (RPacketContext) is configured 
	on the phone by a client using RPacketContext::SetConfig(). 
	
	This is an asynchronous function.
	
	Once the notification completes, the client must then re-post the notification 
	if they wish to continue receiving further notifications.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketNotifyContextAdded) to cancel 
	a previously placed asynchronous NotifyContextAdded() request.
	
	@param aStatus On completion, KErrNone if successful, a system-wide error 
	code if not.
	@param aContextId On completion, the name of the newly added context. The name 
	is assigned by the TSY and uniquely identifies the particular context.

@capability None
*/
	{
	Get(EPacketNotifyContextAdded,aStatus,aContextId);
	}

EXPORT_C void RPacketService::Attach(TRequestStatus& aStatus) const
// This function may be called by the client.
/**
	Causes the phone to attempt an attach to the packet network, if it is not already 
	attached.
	
	This is an asynchronous function since attachment may take anything up to one 
	minute in the worst case scenario.
	
	Attachment may take anything up to one minute in the worst case scenario.
	
	It is not often necessary to use this function as there is a separate setting 
	to tell the phone either to attach at start-up, or to attach when a context 
	is created. It is intended that this function only be utilized if the user 
	has an option to choose exactly when to attach and detach.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketAttach) to cancel a previously 
	placed asynchronous Attach() request.
	
	@param aStatus On completion, KErrNone if successful, KErrAlreadyExists if 
	the phone was already attached to a packet network, KErrPacketNetworkFailure 
	if the phone cannot attach.

@capability NetworkServices
*/
	{
	Blank(EPacketAttach,aStatus);
	}

EXPORT_C void RPacketService::Detach(TRequestStatus& aStatus) const
//This function may be called by the client.
/**
	Causes the phone to attempt to detach from the packet network, if it is not already.
	
	This is an asynchronous function.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketDetach) to cancel a previously 
	placed asynchronous Detach() request.
	
	@param aStatus On completion, KErrNone if successful, KErrAlreadyExists if 
	the phone was already detached, KErrInUse if at least one context is active.

@capability NetworkServices
*/
	{
	Blank(EPacketDetach, aStatus);
	}

EXPORT_C TInt RPacketService::GetStatus(TStatus& aPacketStatus) const
// This function may be called by the client.
/**
	Gets the status of the current packet service.
	
	This is a synchronous function.
	
	@param aPacketStatus On return, the current packet service status.
	@return KErrNone if successful, otherwise another of the system wide error codes.

@capability None
*/
	{
	TPckg<TStatus> ptr1(aPacketStatus);
	return Get(EPacketGetStatus, ptr1);
	}

EXPORT_C void RPacketService::NotifyStatusChange(TRequestStatus& aStatus,TStatus& aPacketStatus) const
//This function may be called by the client.
/**
	Allows a client to be notified of a change in the status of the connection 
	to the packet service.
	
	This is an asynchronous function.
	
	Once the notification completes, the client must then re-post the notification 
	if they wish to continue receiving further notifications.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketNotifyStatusChange) to cancel 
	a previously placed asynchronous NotifyStatusChange() request.
	
	@param aStatus On completion, KErrNone if successful, otherwise another of the system wide error codes.
	@param aPacketStatus On completion, the new packet status 

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotPacketStatus,aPacketStatus);

	Get(EPacketNotifyStatusChange,aStatus,ptr1);
	}

EXPORT_C void RPacketService::NotifyContextActivationRequested(TRequestStatus& aStatus, TDes8& aContextParameters) const
/**
	Completes when the phone receives a request from the packet network to activate 
	a PDP context.
	
	This is an asynchronous function.
	
	Depending on the type of network, the aContextParameters may contain the PDP 
	type requested, the PDP address for this context, and possibly the Access 
	Point Name (APN) of the gateway to connect to.
	
	To determine how to correctly unpack the aContextParameters descriptor inside the 
	TSY, the TSY must first cast the descriptor to a TPacketDataConfigBase pointer and 
	check whether ExtensionId() returns KConfigGPRS, KConfigCDMA or KConfigRel99Rel4. 
	The TSY will then cast the descriptor to the correct TPacketDataConfigBase-derived 
	class.
	
	Once the notification completes, the client must then re-post the notification 
	if they wish to continue receiving further notifications.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketNotifyContextActivationRequested) 
	to cancel a previously placed asynchronous NotifyContextActivationRequested() 
	request.
	
	@param aStatus On completion, KErrNone if successful, or a system-wide error 
	code if not successful.
	@param aContextParameters A TContextConfigXXX context configuration object packaged 
	as a TPckg<TContextConfigXXX>.

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	Get(EPacketNotifyContextActivationRequested, aStatus, aContextParameters);
	}

EXPORT_C void RPacketService::RejectActivationRequest(TRequestStatus& aStatus) const
/**
	Causes the phone to reject the network's request for a PDP context activation.
	
	This is an asynchronous function.
	
	There is no corresponding "accept" message for the phone to send. To accept, 
	the phone should simply begin to activate a PDP context with the appropriate 
	IP address before the network's timer expires. On GPRS, this timer last between 
	5 and 8 seconds, but may effectively last between around 25 and 40 seconds, 
	as the specifications state that the activation request should be re-sent 
	up to five times.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketRejectActivationRequest) 
	to cancel a previously placed asynchronous RejectActivationRequest() request.
	
	@param aStatus On completion, KErrNone if successful, or a system-wide error 
	code if not successful.

@capability NetworkServices
*/
	{
	Blank(EPacketRejectActivationRequest, aStatus);
	}

EXPORT_C void RPacketService::EnumerateContexts(TRequestStatus& aStatus, TInt& aCount, TInt& aMaxAllowed) const
//This function may be called by the client.
/**
	Retrieves both the number of opened contexts in the TSY, whether active or inactive, and 
	the maximum possible number of contexts.
	
	This is an asynchronous function.

	Allows a client to determine whether they can open a new context.
	
	The maximum number of possible contexts is not the maximum number of simultaneously 
	active contexts, but the total number allowed. (The maximum possible number 
	of simultaneously active contexts is currently limited to 1). Clients are 
	thus able to determine whether they can open a new context or not.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketEnumerateContexts) to cancel 
	a previously placed asynchronous EnumerateContexts() request.
	
	@param aStatus On completion, KErrNone if successful, or a system-wide error 
	code if not successful.
	@param aCount On completion, the number of opened contexts existing in the TSY.
	@param aMaxAllowed On completion, the maximum number of opened contexts that 
	are allowed in the TSY, or -1 if the maximum number of opened contexts is 
	not available.

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotEnumerateCount,aCount);
	TPtr8& ptr2=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotEnumerateMaxCount,aMaxAllowed);
	Get(EPacketEnumerateContexts, aStatus, ptr1, ptr2);
	}

EXPORT_C void RPacketService::GetContextInfo(TRequestStatus& aStatus,TInt aIndex,TContextInfo& aInfo) const
/**
	Retrieves information about a particular context.
	
	It is usual to use this function in conjunction with the EnumerateContexts() 
	function, which returns the current number of opened contexts.
	
	This is an asynchronous function.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketGetContextInfo) to cancel 
	a previously placed asynchronous GetContextInfo() request.
	
	@param aStatus On completion, KErrNone if successful, or KErrArgument if the aIndex is invalid. 
	@param aIndex The index of the context to get. The valid range for aIndex will be between 0 and 
	(n-1) where n is the number of open contexts returned by EnumerateContexts().
	@param aInfo On completion, the context information.

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iEtelPacketPtrHolder->iGetContextInfoIndex = aIndex;
	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotContextInfoIndex,iEtelPacketPtrHolder->iGetContextInfoIndex);
	TPtr8& ptr2=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotContextInfo,aInfo);

	Get(EPacketGetContextInfo,aStatus, ptr1, ptr2);
	}

EXPORT_C void RPacketService::GetNtwkRegStatus(TRequestStatus& aStatus, TRegistrationStatus& aRegistrationStatus) const
// This function may be called by the client.

/**
	Retrieves the current registration status of the phone on the packet network.
	
	This is an asynchronous function.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketGetNtwkRegStatus) to cancel 
	a previously placed asynchronous GetNtwkRegStatus() request.
	
	@param aStatus On completion, KErrNone if successful, or a system-wide error 
	code if not successful.
	@param aRegistrationStatus On completion, the current registration status of 
	the	phone on the packet network.

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotGetNtwkReg,aRegistrationStatus);
	Get(EPacketGetNtwkRegStatus, aStatus, ptr1);
	}

EXPORT_C void RPacketService::NotifyChangeOfNtwkRegStatus(TRequestStatus& aStatus,TRegistrationStatus& aRegistrationStatus) const
// This asynchronous function may be called by the client.
/**
	Allows a client to be notified whenever a change in the status of the packet 
	network registration is detected.
	
	This is an asynchronous function.
	
	Once the notification completes, the client must then re-post the notification 
	if they wish to continue receiving further notifications.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketNotifyChangeOfNtwkRegStatus) 
	to cancel a previously placed asynchronous NotifyChangeOfNtwkRegStatus() request.
	
	@param aStatus On completion, KErrNone if successful, or a system-wide error 
	code if not successful.
	@param aRegistrationStatus On completion, the new registration status of the 
	phone on the packet network. 

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotRegStatus,aRegistrationStatus);

	Get(EPacketNotifyChangeOfNtwkRegStatus,aStatus,ptr1);
	}

EXPORT_C void RPacketService::GetMSClass(TRequestStatus& aStatus, TMSClass& aCurrentClass, TMSClass& aMaxClass) const
/**
	Gets both the current class and the highest class of the mobile phone.
	
	This is an asynchronous function.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketGetMSClass) to cancel a 
	previously placed asynchronous GetMSClass() request.
	
	@param aStatus On completion, KErrNone if successful, or another of the system-wide error 
	codes.
	@param aCurrentClass On completion, the current class of the phone.
	@param aMaxClass On completion, the highest possible class the phone can emulate.

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotCurrentMsClass,aCurrentClass);
	TPtr8& ptr2=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotMaxMsClass,aMaxClass);

	Get(EPacketGetMSClass, aStatus, ptr1, ptr2);
	}

EXPORT_C void RPacketService::SetMSClass(TRequestStatus& aStatus, TMSClass aClass) const
/** 
	Sets the current packet class of the mobile station. This should never be higher 
	than the maximum packet class of the mobile station. 
	
	It is expected that this function will be used to force a ClassSuspensionRequired 
	mobile into a Class C mode of operation: either packet or circuit-switched only. 
	This is useful, for instance, in a situation where the user is billed for incoming 
	circuit-switched calls (a practice of U.S. network operators) and does not wish to 
	be available for incoming calls for long periods of time, but does want to keep the 
	packet side of the phone always on and always connected.
	
	This is an asynchronous function.
	
	Once the notification completes, the client must then re-post the notification 
	if they wish to continue receiving further notifications.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketSetMSClass) to cancel a 
	previously placed asynchronous SetMSClass() request.
	
	@param aStatus On completion, KErrNone if successful, orKErrTooBig if the 
	requested class is higher than the maximum class supported by the phone.
	@param aClass The new packet class for the mobile station.
	
@capability NetworkServices
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iEtelPacketPtrHolder->iMSClass = aClass;
	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotSetMsClass,iEtelPacketPtrHolder->iMSClass);

	Set(EPacketSetMSClass, aStatus, ptr1);
	}

EXPORT_C void RPacketService::NotifyMSClassChange(TRequestStatus& aStatus, TMSClass& aNewClass) const
/**
	Completes when there is a change in the current GPRS class of the mobile station.
	
	This is an asynchronous function.
	
	Once the notification completes, the client must then re-post the notification 
	if they wish to continue receiving further notifications.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketNotifyMSClassChange) to 
	cancel a previously placed asynchronous NotifyMSClassChange() request.
	
	@param aStatus On completion, KErrNone if successful, otherwise another of the system
	wide error codes.
	@param aNewClass On completion, the new packet class.
	
@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotNtfMsClass,aNewClass);

	Get(EPacketNotifyMSClassChange, aStatus, ptr1);
	}

EXPORT_C TInt RPacketService::GetStaticCaps(TUint& aCaps, RPacketContext::TProtocolType aPdpType) const
/**
	Retrieves those capabilities of the TSY/phone that are static.
	
	This is a synchronous function.
	
	@param aCaps On return, a bitmask of the TStaticMiscCaps static capabilities.
	@param aPdpType The static parameters may be different depending on the PdpType 
	supported on the phone. Hence the client must pass in the current PdpType 
	for which they wish to receive static capabilities.
	@return KErrNone if successful, a system-wide error code if not. 
	
@capability None
*/
	{
	TPckg<TUint> ptr1(aCaps);
	TPckg<RPacketContext::TProtocolType> ptr2(aPdpType);
	return Get(EPacketGetStaticCaps, ptr1, ptr2);
	}

EXPORT_C TInt RPacketService::GetDynamicCaps(TDynamicCapsFlags& aCaps) const
/**
	Retrieves those capabilities of the TSY/phone that are dynamic.
	
	This is a synchronous function.
	
	@param aCaps On return, a bitmask of the RPacketService::TDynamicCaps dynamic capabilities.
	@return KErrNone if successful, a system-wide error code if not. 
	
@capability None
*/
	{
	TPckg<TDynamicCapsFlags> ptr1(aCaps);
	return Get(EPacketGetDynamicCaps, ptr1);
	}

EXPORT_C void RPacketService::NotifyDynamicCapsChange(TRequestStatus& aStatus, TDynamicCapsFlags& aCaps) const
/**
	Completes when the dynamic packet data capabilities change.
	
	This is an asynchronous function.
	
	Once the notification completes, the client must then re-post the notification 
	if they wish to continue receiving further notifications.
	
	A common usage of the dynamic capabilities functions would be for GetDynamicCaps() to be 
	called first to retrieve the current set of capabilities, and then the NotifyDynamicCapsChange()
	request to be posted to be kept informed of any changes.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketNotifyDynamicCapsChange) 
	to cancel a previously placed asynchronous NotifyDynamicCapsChange() request.
	
	@param aStatus On completion, KErrNone if successful, a system-wide error 
	code if not.
	@param aCaps On completion, a bitmask of the new RPacketService::TDynamicCaps dynamic capabilities.

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotDynamicCaps,aCaps);

	Get(EPacketNotifyDynamicCapsChange,aStatus,ptr1);
	}

EXPORT_C void RPacketService::SetPreferredBearer(TRequestStatus& aStatus, TPreferredBearer aBearer) const
/** 
	Sets the preferred bearer for the Mobile Terminal to use when as default when 
	dropping from Class B operation to Class C. This could happen when the Mobile 
	Terminal moves into a Routing Area of cells or new GPRS network which operates 
	in Network Mode III. (Class C operation requires that the Mobile Terminal 
	is attached to either GSM or GPRS, not both.)
	
	Should the Mobile Terminal be a Class C only device, this function is used 
	simply to switch bearers. If the required bearer is GSM, the Mobile Terminal 
	will immediately attempt to attach to the GSM service. If GPRS, the Mobile 
	Terminal may either immediately attempt to attach to the GPRS service or wait 
	until context activation to attach, depending on the current attach mode as 
	defined by TAttachMode.
	
	This is an asynchronous function.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketSetPrefBearer) to cancel 
	a previously placed asynchronous SetPreferredBearer() request.
	
	@param aStatus On completion, KErrNone if successful, a system-wide error 
	code if not.
	@param aBearer The new preferred bearer. 

@capability NetworkServices
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iEtelPacketPtrHolder->iPrefBearer = aBearer;
	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotSetPrefBearer,iEtelPacketPtrHolder->iPrefBearer);

	Set(EPacketSetPrefBearer,aStatus, ptr1);
	}

EXPORT_C TInt RPacketService::GetPreferredBearer(TPreferredBearer& aBearer) const
/**
	Note that the synchronous variant of this method is deprecated. The asynchronous variant instead should be used.
	 
	Retrieves the currently selected preferred bearer to attach to when in Class C 
	operation. The TSY should cache this value, so the function should return quickly 
	hence there is no need for an asynchronous version. 
	
	@deprecated  The synchronous variant of this method is deprecated. The asynchronous variant instead should be used.
	@param aBearer On return will contain the selected preferred bearer.
	@return Result code. KErrNone if successful. 

@capability None
*/
	{
	TPckg<TPreferredBearer> ptr1(aBearer);
	return Get(EPacketGetPrefBearer, ptr1);
	}
	
EXPORT_C void RPacketService::GetPreferredBearer(TRequestStatus& aStatus, TPreferredBearer& aBearer) const
/**
	This asynchronous function retrieves the currently selected preferred bearer to attach to 
	when in Class C operation. 
	 
	@param aStatus A reference to a TRequestStatus which on completion will contain the result code.
	@param aBearer On completion will contain the selected preferred bearer. 

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotGetPrefBearer,aBearer);
	
	Get(EPacketGetPrefBearer, aStatus, ptr1);
	}

EXPORT_C TInt RPacketService::SetAttachMode(TAttachMode aMode) const
/**
	Sets when the attachment to the GPRS network should occur, in version 1 of the API. 
	
	In either mode, the manual Attach() and Detach() functions will force the attach or detach 
	respectively, as long as it is a valid action.
	
	@deprecated The asynchronous version should be used from v7.0S.
	
	@param aMode The attach mode.
	@return KErrNone if successful, a system-wide error code if not.

@capability WriteDeviceData
*/
	{
	TPckg<TAttachMode> ptr1(aMode);
	return Set(EPacketSetAttachMode, ptr1);
	}

EXPORT_C void RPacketService::SetAttachMode(TRequestStatus& aStatus, TAttachMode aMode) const
/**
	This asynchronous method may be called by the client. It has superseded the synchronous
	variant of this method.
	
	It sets when the 'attach' to the packet network should occur.
	
	@param aStatus A reference to a TRequestStatus.
	@param aMode This parameter contains instructions for this method: RPacketService::EAttachWhenPossible
	and RPacketService::EAttachWhenNeeded.
 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iEtelPacketPtrHolder->iAttachMode = aMode;
	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotSetAttachMode,iEtelPacketPtrHolder->iAttachMode);

	Set(EPacketSetAttachMode, aStatus, ptr1);
	}

EXPORT_C TInt RPacketService::GetAttachMode(TAttachMode& aMode) const
/**
	Retrieves the current mode of GPRS Attach mode, in version 1 of the API. 
	
	@deprecated The asynchronous version should be used from v7.0S.
	
	@param aMode On return, the current attachment mode.
	@return KErrNone if successful, a system-wide error code if not.

@capability None
*/
	{
	TPckg<TAttachMode> ptr1(aMode);
	return Get(EPacketGetAttachMode, ptr1);
	}

EXPORT_C void RPacketService::GetAttachMode(TRequestStatus& aStatus, TAttachMode& aMode) const
/**
	This asynchronous method may be called by the client. It has superseded the synchronous
	variant of this method.
	It will retrieve the current mode of the packet network attach operation.
	
	@param aStatus This is a reference to aTRequestStatus.
	@param aMode Has 2 values: RPacketService::EAttachWhenPossible and RPacketService::EAttachWhenNeeded.

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1 = iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotGetAttachMode, aMode);
	
	Get(EPacketGetAttachMode, aStatus, ptr1);
	}
	
EXPORT_C void RPacketService::NotifyAttachModeChange(TRequestStatus& aStatus, TAttachMode& aMode) const
/**
	Notifies the client when there is a change in the current mode of the packet network attach operation. 
	 
	@param aStatus This is a reference to a TRequestStatus.
	@param aMode Has 2 values: RPacketService::EAttachWhenPossible and RPacketService::EAttachWhenNeeded.

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1 = iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotNotifyAttachModeChange, aMode);
	
	Get(EPacketNotifyAttachModeChange, aStatus, ptr1);
	}

EXPORT_C TInt RPacketService::SetDefaultContextParams(const TDesC8& aPckg) const
//This function may be called by the client.
/** 
	Sets the default context parameters in the phone, in version 1 of the API. 
	The default context parameters are the parameters which the phone will use if no 
	context parameters have been explicitly set up via either the ETel API or AT 
	commands from an external PC.
	
	The actual context parameters are set up in the RPacketContext::TContextConfigGPRS 
	or RPacketContext::TContextConfigCDMA structure. 
	
	To determine how to correctly unpack the descriptor inside the TSY, the TSY must 
	first cast the descriptor to a TPacketDataConfigBase pointer and check whether 
	ExtensionId() returns KConfigGPRS, KConfigCDMA or KConfigRel99Rel4. The TSY will then 
	cast the descriptor to the correct TPacketDataConfigBase-derived class.
	
	@deprecated The asynchronous version should be used from v7.0S.
	
	@param aPckg A TContextConfigXXX context configuration class packaged as a 
	TPckg<TContextConfigXXX>.  
	@return KErrNone if successful, or a system-wide error code if not.

@capability WriteDeviceData
*/
	{
	return Set(EPacketSetDefaultContextParams, aPckg);
	}

EXPORT_C void RPacketService::SetDefaultContextParams(TRequestStatus& aStatus, const TDesC8& aPckg) const
/**
	This asynchronous method may be called by the client. It has superseded the synchronous
	variant of this method.

	It will set the default context parameters in the ME. These are the parameters the ME will
	use if no explicit context parameters have previously been set up.

	@param aStatus A reference to a TRequestStatus.
	@param aPckg A TContextConfigGPRS or TContextConfigCDMA context configuration class packaged 
	inside a TPckg<TContextConfigGPRS> or TPckg<TContextConfigCDMA> class. 
	
	To determine how to correctly unpack the descriptor inside the TSY, the TSY must first cast 
	The descriptor to a TPacketDataConfigBase pointer and check the iExtensionId. 
	Depending on the iExtensionId, the TSY will then cast the descriptor to the correct 
	TPacketDataConfigBase-derived class.

@capability WriteDeviceData
*/
	{
	Set(EPacketSetDefaultContextParams, aStatus, aPckg);
	}

EXPORT_C TInt RPacketService::GetDefaultContextParams(TDes8& aPckg) const
/**
	Retrieves the default context parameters that are stored in the phone, in version 1 of the API.
	
	@deprecated The asynchronous version should be used from v7.0S.
	
	@param aPckg On return, a RPacketContext::TContextConfigGPRS or RPacketContext::TContextConfigCDMA 
	context configuration class packaged inside a TPckg<TContextConfigGPRS> or 
	TPckg<TContextConfigCDMA> class. To determine how to correctly unpack the 
	descriptor inside the TSY, the TSY must first cast the descriptor to a TPacketDataConfigBase 
	pointer and check whether ExtensionId() returns KConfigGPRS, KConfigCDMA or KConfigRel99Rel4.
	@return KErrNone if successful, or a system-wide error code if not.

@capability ReadDeviceData
*/
	{
	return Get(EPacketGetDefaultContextParams, aPckg);
	}

EXPORT_C void RPacketService::GetDefaultContextParams(TRequestStatus& aStatus, TDes8& aPckg) const
/**
	This asynchronous method may be called by the client. It has superseded the synchronous
	variant of this method.
	
	It will retrieve the default context parameters that are stored in the ME.
	
	@param aStatus This is a reference to a TRequestStatus.
	@param aPckg On completion contains the packaged default context parameters. aPckg should be
	a TPckg of a TPacketDataConfigBase derived class.

	@see RPacketContext::TContextConfigCDMA
	@see RPacketContext::TContextConfigGPRS
	@see RPacketContext::TContextConfigR99_R4
	@see RPacketContext::TContextConfig_R5

@capability ReadDeviceData
*/
	{
	Get(EPacketGetDefaultContextParams, aStatus, aPckg);
	}

EXPORT_C void RPacketService::GetCurrentReleaseMode(TRequestStatus& aStatus, TPacketReleaseMode& aReleaseMode) const
/**
	This asynchronous method may be called by the client application.
	It returns the current packet network release mode(Release97/98, Release 99 or Release 4).
	
	@param aStatus This is a reference to a TRequestStatus.
	@param aNetworkMode On completion contains the retrieved release mode.

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1 = iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotGetCurrentReleaseMode, aReleaseMode);
	
	Get(EPacketGetCurrentReleaseMode, aStatus, ptr1);
	}

EXPORT_C void RPacketService::NotifyReleaseModeChange(TRequestStatus& aStatus, TPacketReleaseMode& aReleaseMode) const
/**
	This asynchronous notification function may be called by the client.
	
	It completes if the packet release mode of the network changes.
	
	@param aStatus This is a reference to a TRequestStatus.
	@param aReleaseMode This will contain the new mode (Release97/98, Release 99 or Release 4)

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotNotifyModeChange, aReleaseMode);

	Get(EPacketNotifyReleaseModeChange,aStatus, ptr1);
	}

EXPORT_C void RPacketService::EnumerateNifs(TRequestStatus& aStatus, TInt& aCount) const
/**
	This asynchronous method may be called by the client application.
	
	It returns the number of defined packet network interfaces in the parameter aCount. 
	One network interface (NIF) exists per local IP address, but there may be several PDP contexts
	per NIF, i.e. one primary and several secondary contexts, all sharing the IP address and other
	configuration information such as PDP type and APN. 
	
	@param aCount Number of defined packet network interfaces

@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotEnumerateNifs,aCount);

	Get(EPacketEnumerateNifs, aStatus, ptr1);
	}

EXPORT_C void RPacketService::GetNifInfo(TRequestStatus& aStatus, TInt aIndex, TDes8& aNifInfoV2) const
/**
	This asynchronous method may be called by the client application.
	It retrieves information about the network interface specified by the aIndex parameter.
	It is usual to use this function in conjunction with the EnumerateNifs() method, which
	returns the current number of defined packet Network interfaces in its 'aCount' argument. 
	
	@param aStatus TRequestStatus reference, enables asynchronous function call

	@param aIndex Specifies network interface for which information is to be retrieved.
	The valid range for aIndex will be between 0 and (n-1) where n is the number returned in
	aCount. If an invalid index is specified the method will return KErrArgument in its aStatus.
	
	@param aNifInfoV2 Reference to a TNifInfoV2 class packaged inside a TPckg<> class. Contains the
	returned Network Interface information
 
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iEtelPacketPtrHolder->iGetNifInfoIndex = aIndex;
	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotGetNifInfo,iEtelPacketPtrHolder->iGetNifInfoIndex);

	Get(EPacketGetNifInfo,aStatus, ptr1, aNifInfoV2);
	}

EXPORT_C void RPacketService::EnumerateContextsInNif(TRequestStatus& aStatus, const TDesC& aExistingContextName, TInt& aCount) const
/**
	This asynchronous method may be called by the client application.
	
	It retrieves the number of contexts sharing a particular network interface, including the 
	primary context (if it still exists) and the context specified in aExistingContextName.
	
	@param aStatus TRequestStatus reference, enables asynchronous function call
	@param aExistingContextName The name of any context belonging to the network interface (NIF)
	@param aCount Contains the number of contexts sharing a NIF.

@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotEnumerateContextsInNif,aCount); 

	SetAndGet(EPacketEnumerateContextsInNif, aStatus, ptr1, aExistingContextName); 
	}

EXPORT_C void RPacketService::GetContextNameInNif(TRequestStatus& aStatus, const TDesC& aExistingContextName, TInt aIndex, TDes& aContextName) const
/**
	This asynchronous method may be called by the client application.
	
	It retrieves the name of the context specified by aIndex for a particular network interface,
	which is determined by aExistingContextName
	
	@param aStatus TRequestStatus reference, enables asynchronous function call
	@param aExistingContextName The name of any context belonging to the network interface (NIF)
	@param aIndex Integer between 0 and (n-1) where n is the number of existing contexts 
	sharing this network interface (as provided by EnumerateContextsInNif() method).
	@param aContextName The name of the context specified by aIndex is returned in this parameter.

@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iEtelPacketPtrHolder->iGetContextNameInNif.iIndex=aIndex;
	iEtelPacketPtrHolder->iGetContextNameInNif.iExistingContextName=aExistingContextName;
	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotGetContextNameInNif,iEtelPacketPtrHolder->iGetContextNameInNif); 
	
	Get(EPacketGetContextNameInNif, aStatus, ptr1, aContextName); 
	}

TInt RPacketService::PrepareOpenSecondary(const TDesC& aOriginalContextName)
/**
	This is an internal API method. It is not intended for direct client access.
	It is called from RPacketContext::OpenNewSecondaryContext().
	
	It passes the name of the context (which is to spawn a secondary context) to the TSY.
	This alerts the TSY to the fact that the next context creation call is going to be
	for a secondary context.
	
	@param aOriginalContextName Name of the original context that this secondary context is 
	spawned from.
	@return KErrNone if successful, KErrNotSupported if the TSY does not support
	secondary contexts.
 */
	{
	__ASSERT_ALWAYS(aOriginalContextName.Length()!=0,PanicClient(KErrBadName));
	return Set(EPacketPrepareOpenSecondary, aOriginalContextName);
	}

EXPORT_C RPacketService::TNifInfoV2::TNifInfoV2()
: TPacketBase(), iContextName(NULL), iNumberOfContexts(0), iNifStatus(RPacketContext::EStatusUnknown),
  iPdpAddress(NULL), iContextType(RPacketService::EUnspecified)
/**
	Constructor for TNifInfoV2 class
	
	This class contains network information (NIF) parameters.
	The member data are initialized to zero, NULL, or unknown. The client should set the 
	parameters to valid values.
*/
	{
	iExtensionId = KETelExtPcktV2;
	};
	
EXPORT_C void RPacketService::DeactivateNIF(TRequestStatus& aStatus, const TDesC& aContextName) const
/**
	This asynchronous method may be called by the client application.
	
	Invokes a packet network deactivation of all associated contexts sharing the same network interaface (NIF)
	with the specified context (aContextName argument) using the tear down indication when requesting the deactivation 
	operation for the specified context. 
 
	@param aStatus TRequestStatus reference, enables asynchronous function call.
	@param aContextName Name of Context to be deactivated.

@capability NetworkServices
*/
	{
	Set(EPacketDeactivateNIF, aStatus, aContextName);
	}
	
TPacketType::TPacketType()
	{}

EXPORT_C TInt TPacketType::ExtensionId() const
/**
 * This method returns the packet API extension number of the class
 *
 * @return TInt An integer that will indicate the version of the type
 * @capability None
*/
	{
	return iExtensionId;
	}	
	
void TPacketType::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the packet type from a stream 
 *
 * @param aStream The read stream containing the packet type
 */
	{
	iExtensionId=aStream.ReadInt32L();	
	}

void TPacketType::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the packet type into a stream 
 *
 * @param aStream The write stream that will contain the packet type
 */
	{
	aStream.WriteInt32L(iExtensionId);
	}

EXPORT_C RPacketService::TMbmsServiceAvailabilityV1::TMbmsServiceAvailabilityV1() 
	{
	iExtensionId=KETelExtPcktV1;
	}		

void RPacketService::TMbmsServiceAvailabilityV1::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the MBMS Service availability parameters from a stream 
 *
 * @param aStream The read stream containing the MBMS Service availability parameters
 */
	{
	TPacketType::InternalizeL(aStream);
	iTmgi.SetServiceId(STATIC_CAST(TUint,aStream.ReadUint32L()));
	iTmgi.SetMCC(STATIC_CAST(TUint16,aStream.ReadUint32L()));
	iTmgi.SetMNC(STATIC_CAST(TUint16,aStream.ReadUint32L()));
	iMbmsServiceMode = STATIC_CAST(TMbmsServiceMode,aStream.ReadUint32L());
	iMbmsAvailabilityStatus = STATIC_CAST(TMbmsAvailabilityStatus,aStream.ReadUint32L());
	iMbmsAccessBearer = STATIC_CAST(TMbmsScope,aStream.ReadUint32L());
	}

void RPacketService::TMbmsServiceAvailabilityV1::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the MBMS Service availability parameters into a stream 
 *
 * @param aStream The write stream that will contain the MBMS Service availability parameters 
 */
	{
	TPacketType::ExternalizeL(aStream);
	
	aStream.WriteInt32L(iTmgi.GetServiceId());
	aStream.WriteInt32L(iTmgi.GetMCC());
	aStream.WriteInt32L(iTmgi.GetMNC());
	aStream.WriteInt32L(iMbmsServiceMode);
	aStream.WriteInt32L(iMbmsAvailabilityStatus);
	aStream.WriteInt32L(iMbmsAccessBearer);
	}

EXPORT_C void RPacketService::NotifyMbmsServiceAvailabilityChange(TRequestStatus& aStatus) const
/**
	Notifies the client that changes have been made to the MBMS Service availability list. 
	When it completes, it does not return the new version of the list so the client will have to 
	retrieve the list again if interested in the new list.

	@param aReqStatus returns the result code after the asynchronous call completes.

@capability None
*/
	{
	Blank(EPacketNotifyMbmsServiceAvailabilityChange,aStatus);
	}
	
EXPORT_C void RPacketService::UpdateMbmsMonitorServiceListL(TRequestStatus& aStatus,const TMbmsAction aAction, CPcktMbmsMonitoredServiceList* aLists) const
/**
	Updates the MBMS monitor service availability list with the entries as per the action mentioned in second parameter.
	The list, which is supplied by the client in an instance of CRetrievePcktMbmsServiceAvailability, should not
	contain more entries than can be stored.
	This method will store all the mbms service availability list entries in the CPcktMbmsMonitoredServiceList
	into the packet storage.
	   
	@param aReqStatus returns the result code after the asynchronous call completes.
			Completes with KErrNotFound, if all the supplied entries are invalid for removal.
			Completes with KErrMbmsImpreciseServiceEntries, if some of the valid entries cannot be removed.
	@param aAction Holds the MBMS action(add, remove or remove all).
	@param aLists List of Mbms service availability to be stored phone-side.

@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	delete iEtelPacketPtrHolder->iStoreMbmsBuf;
	iEtelPacketPtrHolder->iStoreMbmsBuf = NULL;
	if(aAction == ERemoveAllEntries)
		{
		iEtelPacketPtrHolder->iMbmsAction = aAction;
		TPtrC8& ptr1=iEtelPacketPtrHolder->SetC(CEtelPacketPtrHolder::ESlotUpdateMbmsMonitorServiceListL,iEtelPacketPtrHolder->iMbmsAction);
		Set(EPacketUpdateMbmsMonitorServiceList,aStatus,ptr1);
		}
	else 
		{
		__ASSERT_ALWAYS(aLists!=NULL,PanicClient(EEtelPanicNullHandle));
		iEtelPacketPtrHolder->iStoreMbmsBuf=aLists->StoreLC();
		CleanupStack::Pop();
		iEtelPacketPtrHolder->iMbmsAction = aAction;
		TPtrC8& ptr1=iEtelPacketPtrHolder->SetC(CEtelPacketPtrHolder::ESlotUpdateMbmsMonitorServiceListL,iEtelPacketPtrHolder->iMbmsAction);
		(iEtelPacketPtrHolder->iStoreMbmsPtr).Set((iEtelPacketPtrHolder->iStoreMbmsBuf)->Ptr(0));
		Set(EPacketUpdateMbmsMonitorServiceList,aStatus,ptr1,iEtelPacketPtrHolder->iStoreMbmsPtr);
		}
	}

EXPORT_C void RPacketService::NotifyMbmsNetworkServiceStatusChange(TRequestStatus& aStatus, TMbmsNetworkServiceStatus& aServiceStatus) const
/**
	Allows a client to be notified whenever a change in the MBMS network service status identified.
	There can be 3 states that MBMS feature availability could return,
	Unknown - when UE is not GMM Attached.
	Supported - When UE is GMM Attached and it is known that cell supports MBMS.
	Not Supported - When UE is GMM Attached and it is known that cell does not support MBMS.

	This is an asynchronous function.
	
	Once the notification completes, the client must re-post the notification 
	if the client wish to continue receiving further notifications.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketNotifyMbmsNetworkServiceStatusChange) 
	to cancel a previously placed asynchronous NotifyMbmsNetworkServiceStatusChange() request.
	
	@param aStatus On completion, KErrNone if successful, or a system-wide error 
	code if not successful.
	@param aServiceStatus On completion, the current mbms network service status of
	the	phone.

@capability ReadDeviceData
*/	
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotNotifyMbmsNetworkServiceStatusChange,aServiceStatus);

	Get(EPacketNotifyMbmsNetworkServiceStatusChange,aStatus,ptr1);
	}
		
EXPORT_C void RPacketService::GetMbmsNetworkServiceStatus(TRequestStatus& aStatus,TBool aAttemptAttach, TMbmsNetworkServiceStatus& aServiceStatus) const
/**
	Retrieves the MBMS network status of the phone. The indication of cell support for MBMS feature 
	is only known from GMM Attach response from network or during Routing Area update.
	A consequence of this is that it is required for the UE to be in GMM Attach mode in order 
	for either of these messages to be received. 
	If the UE is not in GMM ATTACH then it is unable to determine MBMS network support.

	There can be 3 states that MBMS feature availability could return,
	Unknown - when UE is not GMM Attached.
	Supported - When UE is GMM Attached and it is known that cell supports MBMS.
	Not Supported - When UE is GMM Attached and it is know that cell does not support MBMS.

	This is an asynchronous function.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EPacketGetMbmsNetworkServiceStatus) to cancel 
	a previously placed asynchronous GetMbmsNetworkServiceStatus() request.
	
	@param aStatus On completion, KErrNone if successful, or a system-wide error 
	code if not successful.
	@param aAttemptAttach On completion, the MBMS attach information,if it is true 
	then GMM attached is perfomed, else any of the states is returned.
	@param aServiceStatus On completion, the current mbms network service status of
	the	phone.
		
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iEtelPacketPtrHolder->iMbmsAttemptAttach = aAttemptAttach;
	TPtr8& ptr1 = iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotGetMbmsNetworkServiceStatus, iEtelPacketPtrHolder->iMbmsAttemptAttach);
	TPtr8& ptr2 = iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotGetMbmsNetworkServiceStatus2, aServiceStatus);

	Get(EPacketGetMbmsNetworkServiceStatus, aStatus, ptr1,ptr2);
	}
	
EXPORT_C void RPacketService::EnumerateMbmsMonitorServiceList(TRequestStatus& aStatus, TInt& aCount, TInt& aMaxAllowed) const
/**
	Retrieves both, the number of services in the Mbms Monitor service list in the TSY, and 
	the maximum possible number of Mbms Monitor services.
	
	This is an asynchronous function.

	Use RTelSubSessionBase::CancelAsyncRequest(EPacketEnumerateMbmsMonitorServiceList) to cancel 
	a previously placed asynchronous EnumerateMbmsMonitorServiceList() request.
	
	@param aStatus On completion, KErrNone if successful, or a system-wide error 
	code if not successful.
	@param aCount On completion, the number of Mbms Monitor services existing in the TSY.
	@param aMaxAllowed On completion, the maximum number of Mbms Monitor services that 
	are allowed in the TSY.

@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotEnumerateMbmsMonitorCount,aCount);
	TPtr8& ptr2=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotEnumerateMbmsMonitorMaxCount,aMaxAllowed);
	Get(EPacketEnumerateMbmsMonitorServiceList, aStatus, ptr1, ptr2);
	}
	
EXPORT_C void RPacketService::EnumerateMbmsActiveServiceList(TRequestStatus& aStatus, TInt& aCount, TInt& aMaxAllowed) const
/**
	Retrieves both, the number of services in the Mbms Active service list in the TSY, and 
	the maximum possible number of Mbms Active services.
	
	This is an asynchronous function.

	Use RTelSubSessionBase::CancelAsyncRequest(EPacketEnumerateMbmsActiveServiceList) to cancel 
	a previously placed asynchronous EnumerateMbmsActiveServiceList() request.
	
	@param aStatus On completion, KErrNone if successful, or a system-wide error 
	code if not successful.
	@param aCount On completion, the number of Mbms Active services existing in the TSY.
	@param aMaxAllowed On completion, the maximum number of Mbms Active services that 
	are allowed in the TSY.

@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iEtelPacketPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotEnumerateMbmsActiveCount,aCount);
	TPtr8& ptr2=iEtelPacketPtrHolder->Set(CEtelPacketPtrHolder::ESlotEnumerateMbmsActiveMaxCount,aMaxAllowed);
	Get(EPacketEnumerateMbmsActiveServiceList, aStatus, ptr1, ptr2);
	}	
/***********************************************************************************/
//
// RPacketMbmsContext
//
/***********************************************************************************/
	
EXPORT_C RPacketMbmsContext::RPacketMbmsContext()
/** 
	Standard constructor. 
*/
	{
	}
	
EXPORT_C RPacketMbmsContext::TContextConfigMbmsV1::TContextConfigMbmsV1()
: TPacketDataConfigBase()
 /**
	Standard constructor. Sets the member data values to the defaults.
	This class contains member data compatible with parameters required for configuring a
	MBMS Broadcast context. 
	
	The iExtensionId parameter, set automatically by ETel, is used by the TSY when unpacking this 
	class from a TPckg<> to indicate it is to be unpacked as a TContextConfigMbmsV1 class.
 */
	{
	iExtensionId = KConfigMBMS;
	};

EXPORT_C TInt RPacketMbmsContext::OpenNewContext(RPacketService& aPacketNetwork,TDes& aContextName)
/**
	Creates a MBMS context (i.e. RPacketMbmsContext) on an existing RPacketService
	connection.
		
	The TSY will return KErrUmtsMaxNumOfContextExceededByPhone if the number of MBMS contexts
	supported by the phone is exceeded. The TSY will return KErrUmtsMaxNumOfContextExceededByNetwork
	if the number of MBMS contexts supported by the network is exceeded.

	An opened RPacketMbmsContext must be closed explicitly by calling Close() to
	prevent a resource (memory) leak.

	@param aPacketNetwork An RPacketMbmsContext may only be opened from an existing RPacketService
	subsession. A client application must therefore pass a reference to their previously
	instantiated RPacketService object.

	@param aContextName Set as the hint for the TSY to identify the Mbms Context.
	@return KErrNone if successful, a system-wide error code if not.

@capability None
*/
	{
	RSessionBase* session=&aPacketNetwork.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle)); // client has no existing session with ETel

	TRAPD(ret,ConstructL());
	if (ret)
		{
		return ret;
		}

	TInt subSessionHandle=aPacketNetwork.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=0,PanicClient(EEtelPanicNullHandle)); // client has no existing sub-session!
	TPtrC name(KETelNewContextName); // necessary so that server knows to ask TSY for new name

	SetSessionHandle(*session);
	
	aContextName = KMBMSContextName; //set this as the hint for the TSY to use to create correct(MBMS) object

   	TIpcArgs args(&name,&aContextName,subSessionHandle);
	ret= CreateSubSession(*session,EEtelOpenFromSubSession,args);
	
	if (ret)
		{
		Destruct();
		}
		
	return ret;
	}

EXPORT_C void RPacketMbmsContext::UpdateMbmsSessionList(TRequestStatus& aStatus,const TMbmsAction aAction, const TMbmsSessionId aSessionId) const	
/** 
	Provides MBMS client the facility to add a new session to be received or remove
	an existing session to/from the mbms services table.
	
	@param aStatus On completion, KErrNone if successful.
			Completes with KErrNotFound, if the supplied session is invalid for removal. 
			In the above case the action would be ERemoveEntries.
			Completes with KErrMbmsImpreciseServiceEntries, if some of the valid sessions cannot be removed.
			In the above case the action would be ERemoveAllEntries.
			Or a system-wide error code if not successful.
	@param aAction The session action information which can be either add,remove or removeall.
	@param aSessionId The MBMS session identifier which need to be received or removed.

@capability WriteDeviceData
*/	
	{
	__ASSERT_ALWAYS(iEtelPacketContextPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iEtelPacketContextPtrHolder->iMbmsAction = aAction;
	TPtrC8& ptr1=iEtelPacketContextPtrHolder->SetC(CEtelPacketPtrHolder::ESlotUpdateMbmsSessionList,iEtelPacketContextPtrHolder->iMbmsAction);
	iEtelPacketContextPtrHolder->iSessionId = aSessionId;
	TPtrC8& ptr2=iEtelPacketContextPtrHolder->SetC(CEtelPacketPtrHolder::ESlotUpdateMbmsSessionList2,iEtelPacketContextPtrHolder->iSessionId);

	Set(EPacketContextUpdateMbmsSessionList, aStatus, ptr1,ptr2);
	}
	
EXPORT_C void RPacketMbmsContext::Close()
/** 
	Closes the client's current sub-session with ETel. Any outstanding requests
	the client may have with ETel (notifications, for example) will be automatically
	destroyed.
	
	An opened RPacketMbmsContext must be closed explicitly by calling Close() to
	prevent a resource (memory) leak. 
	
@capability None
*/
	{
	CloseSubSession(EEtelClose);
	Destruct();
	}	

EXPORT_C RPacketMbmsContext::CMbmsSession* RPacketMbmsContext::CMbmsSession::NewL()
/**
 * Creates an instance of CMbmsSession
 */
	{
	CMbmsSession *self = new(ELeave)CMbmsSession;
	return self;
	}

EXPORT_C void RPacketMbmsContext::CMbmsSession::InternalizeL(const TDesC8& aBuffer)
/**
 * This method internalizes the CMbmsSession from the aBuffer.
 * @param	aBuffer	Holds the streamed data of CMbmsSession type.
 */
	{
	iSessionIdList.Reset();
	TUint uintLen=sizeof(TUint);
	TInt cursor=0;
	if(aBuffer.Length() < cursor + uintLen)
		User::Leave(KErrOverflow);	
	Mem::Copy(&iExtensionId,aBuffer.Ptr(),uintLen);
	cursor+=uintLen; 
	if(aBuffer.Length() < cursor + uintLen)
		User::Leave(KErrOverflow);
	TUint count(0);
	Mem::Copy(&count,aBuffer.Ptr()+cursor,uintLen);
	cursor+=uintLen;
	TUint session=0;
	for(TUint i=0;i<count;i++)
		{
		if(aBuffer.Length() < cursor + uintLen)
			User::Leave(KErrOverflow);
		Mem::Copy(&session,aBuffer.Ptr()+cursor,uintLen);
		cursor+=uintLen;
		iSessionIdList.Append(session);
		}
	}
	
EXPORT_C void RPacketMbmsContext::CMbmsSession::ExternalizeL(HBufC8*& aBuffer)
/**
 * This method externalizes the CMbmsSession into the aBuffer.
 * @param	aBuffer	Holds the streamed data of CMbmsSession type.
 */
	{
	TUint uintLen=sizeof(TUint);
	TUint size = uintLen; //ExtensionId
	TUint count=iSessionIdList.Count();
	size += uintLen; //Number of iSessionIdList elements
	size += count*(uintLen);
	if(aBuffer)
		{
	 	delete aBuffer;
	 	aBuffer=NULL;
	 	}
	aBuffer=HBufC8::NewL(size);
	aBuffer->Des().SetLength(size);
	TInt cursor(0);
	Mem::Copy(const_cast<TUint8*>(aBuffer->Des().Ptr()+cursor),(const TUint8*)(&iExtensionId),uintLen);
	cursor+=uintLen;
	Mem::Copy(const_cast<TUint8*>(aBuffer->Des().Ptr()+cursor),(const TUint8* )(&count),uintLen);
	cursor+=uintLen;
	for(TUint i=0;i<count;i++)
		{
		Mem::Copy((TUint8*)(aBuffer->Des().Ptr()+cursor),(const TUint8* )(&iSessionIdList[i]),uintLen);
		cursor+=uintLen;
		}
	}

EXPORT_C TUint RPacketMbmsContext::CMbmsSession::ExtensionId() const
/**
 * Returns the value of iExtensionId for the associated class.
 * e.g. iExtensionId=KETelExtPcktV1 for all the V1 classes.
 *
 * @return The protected data member iExtensionId, which specifies the API version number.
 */
	{
	return iExtensionId;
	}

RPacketMbmsContext::CMbmsSession::CMbmsSession()
/**
 * Standard constructor.
 */  
	{
	iExtensionId = KETelExtPcktV1;
	}

EXPORT_C RPacketMbmsContext::CMbmsSession::~CMbmsSession()
/**
 * Standard destructor
 */
	{
	iSessionIdList.Reset();
	iSessionIdList.Close();
	}

