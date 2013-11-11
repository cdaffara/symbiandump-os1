// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the implementation of the Similator TSY MBMS Packet Context functionality.  
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimMbmsPacketContextTraces.h"
#endif

#include "CSimPhone.h"
#include "CSimMbmsPacketContext.h"
#include "CSimPacketQoS.h"
#include "utils.h"
#include <e32math.h>
#include "CSimPacketService.h"
#include "CSimContextHelper.h"

CSimMbmsPacketContext* CSimMbmsPacketContext::NewL(CSimPhone* aPhone, CSimPacketService* aPacketService, const TDesC& aContextName)
/**
* Standard two phase constructor
*
* @param aPacketService Pointer to the Packet Service object (CSimPacketService)
* @param aContextName name  for this packet context
* @return CSimMbmsPacketContext pointer to the packet context object created.
* @leave Leaves if no memory or object is not created for any reason.
*/
	{
	CSimMbmsPacketContext* packetContext = new(ELeave) CSimMbmsPacketContext(aPhone, aPacketService, aContextName);
	CleanupStack::PushL(packetContext);
	packetContext->ConstructL();
	CleanupStack::Pop();
	return packetContext;
	}

CSimMbmsPacketContext::CSimMbmsPacketContext(CSimPhone* aPhone, CSimPacketService* aPacketService, const TName& aContextName)
	:CSimPacketContext(aPhone,aPacketService,aContextName),iContextName(aContextName), iContextType(TPacketDataConfigBase::KConfigMBMS),
	iPhone(aPhone),iPacketService(aPacketService)
/**
* Trivial Constructor.  Initialises all the data members
*
* @param aPacketService Pointer to the Packet Service object (CSimPacketService)
* @param aContextName name  for this packet context
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_CTOR_1, "CSimMbmsPacketContext: Entered constructor");
	iNotifyConfigMBMS.iNotifyPending = EFalse;
	iNotifyStatusChange.iNotifyPending = EFalse;
	}

void CSimMbmsPacketContext::ConstructL()
/**
* Second phase of the 2-phase constructor.
* Constructs all the member data and retrieves all the data from the config file specific to this class.
*
* @leave Leaves no memory or any data member does not construct for any reason.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_CONSTRUCTL_1, "CSimMbmsPacketContext: Entered constructL function");
	CSimPacketContext::ConstructL();
	
	iMbmsSetConfigTimer = CSimTimer::NewL(iPhone);
	iMbmsContextTimer = CSimTimer::NewL(iPhone);
	iMbmsUpdateSessionTimer = CSimTimer::NewL(iPhone);
	iMbmsContextInitTimer = CSimTimer::NewL(iPhone);
	iMbmsGetSessionTimer = CSimTimer::NewL(iPhone);
	
	iMbmsContextConfigParams = new (ELeave) CArrayFixFlat<TMbmsConfigParams>(KNumberofConfigs);
	iSessionIdList = new (ELeave) CArrayFixFlat<TUint>(1);
	iMbmsSessionList = new CArrayPtrFlat<CPcktListReadAllAttempt>(1);
		
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	/* Reading the MBMS Context configuration parameters */
	TUint count=CfgFile()->ItemCount(KMBMSBroadcastMonitorList); //< Retrieves the Count of MBMS related information

	for(TInt i=0;i<count;i++)
		{
		item = CfgFile()->Item(KMBMSBroadcastMonitorList,i); //< Retrieves the MBMS related information

		TInt mnc =0; 
		TInt mcc =0;
		TInt serviceId =0;
		TInt accessBearer=0;
		TInt serviceMode =0;
		TInt activateCode =0;
		TInt servicePriority=0;
		if(!item)
			break;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,mcc);
		if(ret!=KErrNone)
			{
			break;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,mnc);
		if(ret!=KErrNone)
			{
			break;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,serviceId);
		if(ret!=KErrNone)
			{
			break;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,accessBearer);
		if(ret!=KErrNone)
			{
			break;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,serviceMode);
		if(ret!=KErrNone)
			{
			break;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,activateCode);
		if(ret!=KErrNone)
			{
			break;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,servicePriority);
		if(ret!=KErrNone)
			{
			break;
			}

		TMbmsConfigParams mbmsConfigParams;

		mbmsConfigParams.iMCC = mcc;
		mbmsConfigParams.iMNC = mnc;
		mbmsConfigParams.iServiceId = serviceId;
		mbmsConfigParams.iAccessBearer = reinterpret_cast<TMbmsScope&> (accessBearer);
		mbmsConfigParams.iServiceMode = reinterpret_cast<TMbmsServiceMode&> (serviceMode);
		mbmsConfigParams.iServicePriority = reinterpret_cast<TMbmsServicePriority&> (servicePriority);
		mbmsConfigParams.iActivateCode = activateCode;

		iMbmsContextConfigParams->AppendL(mbmsConfigParams);
		}
	
	//iSessionIdList->AppendL(iNumOfSessionId);
	iSessionIdList->InsertL(0,iNumOfSessionId);
	
	/* Reading the list of session ids mentioned in the configuration file*/
	/*item = CfgFile()->Item(KMBMSSessionIdList,0); //< Retrieves the MBMS related information
	
	if(item != NULL)
		{
		TInt sessionId =0;
		TInt numOfSessions =0;
		
		ret = CTestConfig::GetElement(item->Value(),KStdDelimiter,0,numOfSessions);
		iSessionIdList->AppendL(numOfSessions);
		
		for(TInt j=1;j<=numOfSessions;j++)
			{
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,j,sessionId);
			if(ret!=KErrNone)
				{
				break;
				}
			else
				{
				iSessionIdList->AppendL(sessionId);
				}
			}
		}*/
	}

void CSimMbmsPacketContext::Init()
/*
 * Initialisation function; Not used in the CSimMbmsPacketContext class.
 * but should be implemented as it is a pure virtual function of MTelObjectTSY. 
 */
	{}
	
CSimMbmsPacketContext::~CSimMbmsPacketContext()
/**
* Trivial Destructor
* Closes all CObject type objects and destroys all other objects created in the ConstructL()
*
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_DTOR_1, "CSimMbmsPacketContext: Entered destructor");

	if (iMbmsSetConfigTimer != NULL)
		{
		delete iMbmsSetConfigTimer;
		}

	if(iMbmsContextTimer != NULL)
		{
		delete iMbmsContextTimer;
		}

	if(iMbmsUpdateSessionTimer != NULL)
		{	
		delete iMbmsUpdateSessionTimer;
		}

	if(iMbmsContextInitTimer)
		{
		delete iMbmsContextInitTimer;
		}

	if(iMbmsGetSessionTimer)
		{
		delete iMbmsGetSessionTimer;
		}
	
	if(iMbmsContextConfigParams !=NULL)
		{
		iMbmsContextConfigParams->Delete(0,iMbmsContextConfigParams->Count());
		delete iMbmsContextConfigParams;
		}

	if(iSessionIdList != NULL)
		{
		iSessionIdList->Delete(0,iSessionIdList->Count());
		delete iSessionIdList;
		}

	if(iMbmsSessionList)
		{
		iMbmsSessionList->ResetAndDestroy();
		}
	delete iMbmsSessionList;

	iPacketService->DecrementMbmsContextCount();
	}

CTelObject* CSimMbmsPacketContext::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * No Qos settings for the MBMS broadcast context 
 * This is not supported and if called, will leave.
 *
 * @param aName name of the QoS object to be opened
 * @leave Leaves with KErrNotSupported if the name is not as expected.
 * @ return NULL.
 */
	{	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_OPENNEWOBJECTBYNAMEL_1, "Unexpected call to CSimMbmsPacketContext: OpenNewObjectByName");
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CSimMbmsPacketContext::OpenNewObjectL(TDes& /*aNewName*/)
/**
 * No Qos settings for the MBMS broadcast context 
 * This is not supported and if called, will leave.
 *
 * @param aNewName new name of the object created
 * @return NULL
 * @leave Leaves if out of memory.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_OPENNEWOBJECTL_1, "Unexpected call to CSimMbmsPacketContext: OpenNewObjectL");
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSimMbmsPacketContext::ReqModeL(const TInt aIpc)
/**
* ReqModeL is called from the server's CTelObject::ReqAnalyserL
* in order to check the type of request it has.
* 
* @param aIpc the ipc number that identifies the client request
* @return CTelObject::TReqMode The request mode to be used for this request
* @leave Leaves if not supported by this tsy 
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_REQMODEL_1, "CSimMbmsPacketContext: ReqModeL");
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
		case EPacketContextGetConfig:
		case EPacketContextSetConfig:
		case EPacketContextActivate:
		case EPacketContextDeactivate:
		case EPacketContextDelete:
		case EPacketContextGetStatus:
		case EPacketContextUpdateMbmsSessionList:
		case EPacketContextInitialiseContext:
		case EPacketGetMbmsSessionListPhase1:
		case EPacketGetMbmsSessionListPhase2:
		case EPacketContextGetLastErrorCause:
			break;
						
		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
		case EPacketContextNotifyDataTransferred:
			ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
			break;
		default:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_REQMODEL_2, "CSimMbmsPacketContext: ReqModeL error, unknown IPC");
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_REQMODEL_3, "CSimMbmsPacketContext: ReqModeL sending the request to CSimPacketContext");
			ret = CSimPacketContext::ReqModeL(aIpc);
			break;
		}
	return ret;
	}

TInt CSimMbmsPacketContext::RegisterNotification(const TInt aIpc)
/**
* RegisterNotification is called when the server recognises that this notification
* is being posted for the first time on this sub-session object.
* 
* It enables the TSY to "turn on" any regular notification messages that it may receive 
* from the phone
*
* @param aIpc the ipc number that identifies the client request
* @return err KErrNone if fine
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_REGISTERNOTIFICATION_1, "CSimMbmsPacketContext: RegisterNotification called");
	switch (aIpc)
		{
		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
		case EPacketContextNotifyDataTransferred:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_REGISTERNOTIFICATION_2, "CSimMbmsPacketContext: RegisterNotification");
			return KErrNone;
		default:
			// Unknown or invalid IPC
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_REGISTERNOTIFICATION_3, "CSimMbmsPacketContext: Register error, unknown IPC");
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_REGISTERNOTIFICATION_4, "CSimMbmsPacketContext: Register sending the request to CSimPacketContext");
			return CSimPacketContext::RegisterNotification(aIpc);
		}
	}

TInt CSimMbmsPacketContext::DeregisterNotification(const TInt aIpc)
/**
* DeregisterNotification is called when the server recognises that this notification
* will not be posted again because the last client to have a handle on this sub-session
* object has just closed the handle.
*
* It enables the TSY to "turn off" any regular notification messages that it may 
* receive from the phone
*
* @param aIpc the ipc number that identifies the client request
* @return err KErrNone if fine
*/
	{	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_DEREGISTERNOTIFICATION_1, "CSimMbmsPacketContext: DeregisterNotification called");
	switch (aIpc)
		{
		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
		case EPacketContextNotifyDataTransferred:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_DEREGISTERNOTIFICATION_2, "CSimMbmsPacketContext: DeregisterNotification");
			return KErrNone;
		default:
			// Unknown or invalid IPC
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_DEREGISTERNOTIFICATION_3, "CSimMbmsPacketContext: Deregister error, unknown IPC");
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_DEREGISTERNOTIFICATION_4, "CSimMbmsPacketContext: Deregister sending the request to CSimPacketContext");
			return CSimPacketContext::DeregisterNotification(aIpc);
		}
	}

TInt CSimMbmsPacketContext::NumberOfSlotsL(const TInt aIpc)
/**
* NumberOfSlotsL is called by the server when it is registering a new notification
* It enables the TSY to tell the server how many buffer slots to allocate for
* "repost immediately" notifications that may trigger before clients collect them
*
* @param aIpc the ipc number that identifies the client request
* @return err KErrNone if fine
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_NUMBEROFSLOTSL_1, "CSimMbmsPacketContext: NumberOfSlotsL called");
	TInt numberOfSlots=1;
	switch (aIpc)
		{
		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
		case EPacketContextNotifyDataTransferred:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_NUMBEROFSLOTSL_2, "CSimMbmsPacketContext: Registered with 5 slots");
			numberOfSlots=5;
			break;
		default:
			// Unknown or invalid IPC
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_NUMBEROFSLOTSL_3, "CSimMbmsPacketContext: Number of Slots error, unknown IPC");
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_NUMBEROFSLOTSL_4, "CSimMbmsPacketContext: Number of Slots: sending the request to CSimPacketContext");
			return CSimPacketContext::NumberOfSlotsL(aIpc);
		}  
	return numberOfSlots;
	}

TInt CSimMbmsPacketContext::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
/**
* ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request
* for the TSY to process.
* A request handle, request type and request data are passed to the TSY
* 
* @param aTsyReqHandle  The request handle for completing the request 
* @param aIpc Ipc representing the request
* @param aPackage any data associated with the request
* @return err KErrNone if request completes ok
*/
	{
	//< if this context has been deleted then the client can not do anything with it
	//< if the client wants to use this class, the client must open a new object by 
	//< name with the name for this context and this will reactivate the context.
	if(iDeleted && (aIpc != EPacketContextGetStatus))  
		{
		ReqCompleted(aTsyReqHandle,KErrNotReady);
		return KErrNone;
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_EXTFUNC_1, "CSimMbmsPacketContext: ExtFunc Called");
	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
		case EPacketContextGetConfig:
			return GetConfig(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextGetConfigCancel:
			return GetConfigCancel(aTsyReqHandle);
		case EPacketContextSetConfig:
			return SetConfig(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextSetConfigCancel:
			return SetConfigCancel(aTsyReqHandle);
		case EPacketContextActivate:
			{	//< Ensures that theres only one context active as only one active context is 
			//<permitted at any one time
			if(iIsActive)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_EXTFUNC_2, "Trying to activate an already activated context");
				ReqCompleted(aTsyReqHandle,KErrEtelCallAlreadyActive);
				return KErrNone;
				}
			else
				return Activate(aTsyReqHandle);
			}
		case EPacketContextActivateCancel:
			return ActivateCancel(aTsyReqHandle);
		case EPacketContextDeactivate:
			return Deactivate(aTsyReqHandle);
		case EPacketContextDeactivateCancel:
			return DeactivateCancel(aTsyReqHandle);
		case EPacketContextDelete:
			return Delete(aTsyReqHandle);
		case EPacketContextDeleteCancel:
			return DeleteCancel(aTsyReqHandle);
		case EPacketContextGetLastErrorCause:
			return GetLastErrorCause(aTsyReqHandle,
			REINTERPRET_CAST(TInt*, dataPtr));
		case EPacketContextNotifyConfigChanged:
			return NotifyConfigChanged(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextNotifyConfigChangedCancel:
			return NotifyConfigChangedCancel(aTsyReqHandle);
		case EPacketContextNotifyStatusChange:
			return NotifyStatusChange(aTsyReqHandle,
					REINTERPRET_CAST(RPacketContext::TContextStatus*, dataPtr));
		case EPacketContextNotifyStatusChangeCancel:
			return NotifyStatusChangeCancel(aTsyReqHandle);
		case EPacketContextGetStatus:
			return GetStatus(aTsyReqHandle,
					REINTERPRET_CAST(RPacketContext::TContextStatus*, dataPtr));
		case EPacketContextUpdateMbmsSessionList:
			return UpdateMbmsSessionList(aTsyReqHandle,
					REINTERPRET_CAST(TMbmsAction*, dataPtr),
					REINTERPRET_CAST(TUint*, dataPtr2));	
		case EPacketContextInitialiseContext:
			return InitialiseContext(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextInitialiseContextCancel:
			return InitialiseContextCancel(aTsyReqHandle);
		case EPacketGetMbmsSessionListPhase1:
			return GetMbmsSessionsPhase1(aTsyReqHandle, 
			REINTERPRET_CAST(TClientId*, dataPtr), 
			REINTERPRET_CAST(TInt*, dataPtr2));
		case EPacketGetMbmsSessionListPhase2:
			return GetMbmsSessionsPhase2(aTsyReqHandle, 
			REINTERPRET_CAST(TClientId*, dataPtr), aPackage.Des2n());		
		default:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_EXTFUNC_3, "CSimMbmsPacketContext: ExtFunc Unknown IPC");
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_EXTFUNC_4, "CSimMbmsPacketContext: sending the request to CSimPacketContext::ExtFunc Unknown IPC");
			return CSimPacketContext::ExtFunc(aTsyReqHandle,aIpc,aPackage);
		}
	}
		
TInt CSimMbmsPacketContext::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
* CancelService is called by the server when it is "cleaning-up" any still outstanding
* asynchronous requests before closing a client's sub-session.
* This will happen if a client closes its R-class handle without cancelling outstanding asynchronous requests.
* 
* @param aTsyReqHandle  The request handle for completing the request 
* @param aIpc Ipc representing the request
* @return err KErrNone if request completes ok
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_CANCELSERVICE_1, "CSimMbmsPacketContext: - CancelService called");

	switch (aIpc)
		{
		case EPacketContextSetConfig:
			return SetConfigCancel(aTsyReqHandle);
		case EPacketContextGetConfig:
			return GetConfigCancel(aTsyReqHandle);
		case EPacketContextActivate:
			return ActivateCancel(aTsyReqHandle);
		case EPacketContextDeactivate:
			return DeactivateCancel(aTsyReqHandle);
		case EPacketContextDeleteCancel:
			return DeleteCancel(aTsyReqHandle);
		case EPacketContextNotifyConfigChanged:
			return NotifyConfigChangedCancel(aTsyReqHandle);
		case EPacketContextNotifyStatusChange:
			return NotifyStatusChangeCancel(aTsyReqHandle);
		case EPacketContextInitialiseContext:
			return InitialiseContextCancel(aTsyReqHandle);
		case EPacketGetMbmsSessionListPhase1:
		case EPacketGetMbmsSessionListPhase2:
			return GetMbmsSessionsCancel(aTsyReqHandle);
		default:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_CANCELSERVICE_2, "CSimMbmsPacketContext: - CancelService unknown IPC called");
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_CANCELSERVICE_3, "CSimMbmsPacketContext: sending the request to CSimPacketContext::ExtFunc Unknown IPC");
			return CSimPacketContext::CancelService(aIpc,aTsyReqHandle);
		}
	}

void CSimMbmsPacketContext::TimerCallBack(TInt aId)
/**
* Timer callback function.  When the timer goes off, it will call back into this
* function for further processing.
* param - aId indicates which Timer Event has occured.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_TIMERCALLBACK_1, "CSimMbmsPacketContext: - TimerCallBack(TInt aId) called");
	switch(aId)
		{
		case ETimerIdMbmsPcktContextSetConfig:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_TIMERCALLBACK_2, "CSimMbmsPacketContext: - TimerCallBack SetConfig called");
			if(iSetConfigValues.iErrorValue == KErrNone)
				{
				iMbmsContextConfigParamsIndex = iSetConfigValues.iIndex ;
				}
			ReqCompleted(iSetConfigRequestHandle,iSetConfigValues.iErrorValue);
			break;
		case ETimerIdMbmsUpdateSessionId:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_TIMERCALLBACK_3, "CSimMbmsPacketContext: - TimerCallBack UpdateSessionID called");
			if(iUpdateSessionHandle != NULL)
				{
				ReqCompleted(iUpdateSessionHandle,KErrNone);
				}
			break;
		case ETimerIdPcktContextInitContext:
			DoInitialiseContext();
			break;
		case ETimerIdPcktContextCreateTft:
			CSimPacketContext::TimerCallBack(aId);
			break;
		default:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_TIMERCALLBACK_4, ">>CSimMbmsPacketContext::TimerCallBack Default Case for Context Events");
			switch(iCurrentEvent)
				{
				case EMbmsContextEventNone:
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_TIMERCALLBACK_5, "TimerCallBack - iCurrentEvent = [EMbmsContextEventNone]");
					break;
				case EMbmsContextEventActivate:
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_TIMERCALLBACK_6, "TimerCallBack - iCurrentEvent = [EMbmsContextEventActivate]");
					if(iState==RPacketContext::EStatusActivating)
						{
						TInt activateValue = iMbmsContextConfigParams->At(iMbmsContextConfigParamsIndex).iActivateCode;
						TInt ret;
						if(activateValue != KErrNone) 
							{ // Reject the Activation request.
							ret=ChangeState(RPacketContext::EStatusInactive);
							ReqCompleted(iActivateRequestHandle,KErrGprsActivationRejected);
							iCurrentEvent=EMbmsContextEventNone;
							}
						else
							{
							ret=ChangeState(RPacketContext::EStatusActive);
							ReqCompleted(iActivateRequestHandle,ret);
							iCurrentEvent=EMbmsContextEventNone;
							iIsActive = ETrue;
							}
						
						// Check for and complete any pending notifications
						if(iNotifyConfigMBMS.iNotifyPending)
							{
							TMbmsConfigParams thisConfig = iMbmsContextConfigParams->At(iMbmsContextConfigParamsIndex);
							iNotifyConfigMBMS.iNotifyPending=EFalse;
							RPacketMbmsContext::TContextConfigMbmsV1 *tempConfig = (RPacketMbmsContext::TContextConfigMbmsV1*)iNotifyConfigMBMS.iNotifyData;
							tempConfig->iTmgi.SetServiceId(thisConfig.iServiceId);
							tempConfig->iTmgi.SetMCC(thisConfig.iMCC);
							tempConfig->iTmgi.SetMNC(thisConfig.iMNC);
							tempConfig->iMbmsAccessBearer= thisConfig.iAccessBearer;
							tempConfig->iMbmsServicePriority = thisConfig.iServicePriority;
							tempConfig->iMbmsServiceMode = thisConfig.iServiceMode;
							ReqCompleted(iNotifyConfigMBMS.iNotifyHandle,KErrNone);
							}
						}
					else
						{
						OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_TIMERCALLBACK_7, "Unexpected iState for iCurrentEvent[EMbmsContextEventActivate]");
						}
					break;
				case EMbmsContextEventDeactivate:
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_TIMERCALLBACK_8, "TimerCallBack - iCurrentEvent = [EMbmsContextEventDeactivate]");
					if(((iState == RPacketContext::EStatusInactive) || (iState == RPacketContext::EStatusDeactivating)) && 
									(iCurrentEvent==EMbmsContextEventDeactivate))
						{
						TInt ret=ChangeState(RPacketContext::EStatusInactive);
						ReqCompleted(iDeactivateRequestHandle,ret);
						iCurrentEvent=EMbmsContextEventNone;
						iIsActive = EFalse;
						}
					else
						{
						OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_TIMERCALLBACK_9, "Unexpected iState for iCurrentEvent[EMbmsContextEventActivate]");
						}
					break;
				case EMbmsContextEventDelete:
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_TIMERCALLBACK_10, "TimerCallBack - iCurrentEvent = [EMbmsContextEventDelete]");
					if(iState == RPacketContext::EStatusInactive && !iIsActive )
						{
						TInt ret = ChangeState(RPacketContext::EStatusDeleted);
						ReqCompleted(iDeleteRequestHandle,ret);
						iCurrentEvent=EMbmsContextEventNone;
						}
					else
						{
						ReqCompleted(iDeleteRequestHandle,KErrCorrupt); // KErrCorrupt is sent to indicate that the 
						//delete request is received when the context is Not inactive.
						}
					break;
				default:
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_TIMERCALLBACK_11, "CSimMbmsPacketContext::TimerCallBack");
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_TIMERCALLBACK_12, "<<Unexpected iCurrentEvent @ TimerCallBack");
					break;
					}
				}
			}
		}
	}

TName CSimMbmsPacketContext::ContextName() const 
/**
* Returns the context name to the requested client
*
* @return TName	Name of the MBMS context
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_CONTEXTNAME_1, "CSimMbmsPacketContext::ContextName called");
	return iContextName; 
	}

const CTestConfigSection* CSimMbmsPacketContext::CfgFile()
/**
* Return a pointer to the Configuration File Section
*
* @return CTestConfigSection	pointer to the configuration file section
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_CFGFILE_1, "CSimMbmsPacketContext::CfgFile called");
	return iPacketService->CfgFile();
	}

TInt CSimMbmsPacketContext::SetConfig(const TTsyReqHandle aTsyReqHandle,const TDesC8* aConfig)
/**
* Sets the MBMS configuration parameters and notifies if
* there is a pending notification for the config change.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aConfig pointer containing the parameters to set to.
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_SETCONFIG_1, "CSimMbmsPacketContext::SetConfig called");

	TPckg<TPacketDataConfigBase>* configBase = (TPckg<TPacketDataConfigBase>*)aConfig;
	TPacketDataConfigBase& configBaseV1 = (*configBase)();
	iSetConfigRequestHandle = aTsyReqHandle;
	TInt err = KErrCorrupt;

	if(configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigMBMS)
		{
		TPckg<RPacketMbmsContext::TContextConfigMbmsV1>* configMBMSPckg = (TPckg<RPacketMbmsContext::TContextConfigMbmsV1>*)aConfig;
		RPacketMbmsContext::TContextConfigMbmsV1& configMBMS = (*configMBMSPckg)();

		TMbmsConfigParams mbmsConfigParams;

		mbmsConfigParams.iMCC = configMBMS.iTmgi.GetMCC();
		mbmsConfigParams.iMNC = configMBMS.iTmgi.GetMNC();
		mbmsConfigParams.iServiceId = configMBMS.iTmgi.GetServiceId() ;
		mbmsConfigParams.iAccessBearer = configMBMS.iMbmsAccessBearer;
		mbmsConfigParams.iServiceMode = configMBMS.iMbmsServiceMode;
		mbmsConfigParams.iServicePriority = configMBMS.iMbmsServicePriority;

		TUint index;
		for(index=0; index < iMbmsContextConfigParams->Count(); index++)
			{
			TMbmsConfigParams thisConfig = iMbmsContextConfigParams->At(index);

			if ((mbmsConfigParams.iAccessBearer != thisConfig.iAccessBearer) ||
					(mbmsConfigParams.iServiceMode != thisConfig.iServiceMode) ||
					(mbmsConfigParams.iServicePriority != thisConfig.iServicePriority) ||
					(mbmsConfigParams.iMCC != thisConfig.iMCC) ||
					(mbmsConfigParams.iMNC != thisConfig.iMNC) ||
					(mbmsConfigParams.iServiceId != thisConfig.iServiceId))
				{
				err = KErrCorrupt;
				}
			else
				{
				err = KErrNone;
				//Check for and complete any pending notifications
				if(iNotifyConfigMBMS.iNotifyPending)
					{
					iNotifyConfigMBMS.iNotifyPending=EFalse;
					RPacketMbmsContext::TContextConfigMbmsV1 *tempConfig = (RPacketMbmsContext::TContextConfigMbmsV1*)iNotifyConfigMBMS.iNotifyData;
					tempConfig->iTmgi.SetServiceId(thisConfig.iServiceId);
					tempConfig->iTmgi.SetMCC(thisConfig.iMCC);
					tempConfig->iTmgi.SetMNC(thisConfig.iMNC);
					tempConfig->iMbmsAccessBearer= thisConfig.iAccessBearer;
					tempConfig->iMbmsServicePriority = thisConfig.iServicePriority;
					tempConfig->iMbmsServiceMode = thisConfig.iServiceMode;
					ReqCompleted(iNotifyConfigMBMS.iNotifyHandle,KErrNone);
					}
				break;
				}
			}
		iSetConfigValues.iErrorValue = err;
		iSetConfigValues.iIndex = index;
		}
	else
		{ // not expected to reach here in case of MBMS context
		TUint index=0;
		iSetConfigValues.iErrorValue = err;
		iSetConfigValues.iIndex = index;
		}
	iMbmsSetConfigTimer->Start(RandTime(),this,ETimerIdMbmsPcktContextSetConfig);

	return KErrNone;
	}

TInt CSimMbmsPacketContext::SetConfigCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancels the Setconfig request.
* This method will do nothing in this version of SimTSY.
* @param aTsyReqHandle Tsy Request handle for the client request
* @return err KErrNone 
*/
	{
	iMbmsSetConfigTimer->Cancel();
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}

TInt CSimMbmsPacketContext::GetConfig(const TTsyReqHandle aTsyReqHandle,TDes8* aConfig)
/**
* Retrieves and returns the MBMS context configuration
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aConfig pointer to the configuration params to retrieve
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_GETCONFIG_1, "CSimMbmsPacketContext::GetConfig called");
	TPckg<TPacketDataConfigBase>* configBase = (TPckg<TPacketDataConfigBase>*)aConfig;
	TPacketDataConfigBase& configBaseV1 = (*configBase)();
	
	if(configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigMBMS)
		{

		TPckg<RPacketMbmsContext::TContextConfigMbmsV1>* configMBMSPckg = (TPckg<RPacketMbmsContext::TContextConfigMbmsV1>*)aConfig;
		RPacketMbmsContext::TContextConfigMbmsV1& configMBMS = (*configMBMSPckg)();
		
		configMBMS.iTmgi.SetServiceId(iMbmsContextConfigParams->At(iMbmsContextConfigParamsIndex).iServiceId);
		configMBMS.iTmgi.SetMCC(iMbmsContextConfigParams->At(iMbmsContextConfigParamsIndex).iMCC);
		configMBMS.iTmgi.SetMNC(iMbmsContextConfigParams->At(iMbmsContextConfigParamsIndex).iMNC);
		configMBMS.iMbmsAccessBearer = reinterpret_cast<TMbmsScope&>(iMbmsContextConfigParams->At(iMbmsContextConfigParamsIndex).iAccessBearer);
		configMBMS.iMbmsServicePriority = reinterpret_cast<TMbmsServicePriority&>(iMbmsContextConfigParams->At(iMbmsContextConfigParamsIndex).iServicePriority);
		configMBMS.iMbmsServiceMode = reinterpret_cast<TMbmsServiceMode&>(iMbmsContextConfigParams->At(iMbmsContextConfigParamsIndex).iServiceMode);

		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		}

	return KErrNone;
	}

TInt CSimMbmsPacketContext::GetConfigCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels the Getconfig request.  
* This method will do nothing in this version of SimTSY.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return err KErrNone 
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_GETCONFIGCANCEL_1, "CSimMbmsPacketContext::GetConfigCancel called");
	return KErrNone;
	}

TInt CSimMbmsPacketContext::GetStatus(const TTsyReqHandle aTsyReqHandle,RPacketContext::TContextStatus* aContextStatus)
/**
* Retrieves and returns the current status of the MBMS context 
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @param aContextStatus pointer to the status of the mbms packet context.
* @return KerrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_GETSTATUS_1, "CSimMbmsPacketContext::GetStatus called");
	*aContextStatus = iState;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimMbmsPacketContext::Activate(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to try to Activate a mbms context.
* In this method, we'll store the TsyReqHandle for later completion then call ActionEvent
* state machine to handle the event which will kick off a timer.
* The Actual Activation will take place in the TimerCallBack method.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return KErrNone
*/
	{
	iActivateRequestHandle = aTsyReqHandle;
	TInt ret;
	ret = ActionEvent(EMbmsContextEventActivate);
	return ret;
	}

TInt CSimMbmsPacketContext::ActivateCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to try and cancel an Activate request.
* This will only complete successfully if and only if the Activate request 
* has not completed.i.e. if we haven't already activated the context.
* Otherwise, the request will be completed with KErrNone
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_ACTIVATECANCEL_1, "CSimMbmsPacketContext::ActivateCancel called");
	iMbmsContextTimer->Cancel();
	if(((iState == RPacketContext::EStatusInactive) || (iState == RPacketContext::EStatusActivating)) && 
					(iCurrentEvent==EMbmsContextEventActivate))
		{
		iCurrentEvent= EMbmsContextEventNone;
		TInt ret = ChangeState(RPacketContext::EStatusInactive);
		if(ret!= KErrNone)
			{
			ReqCompleted(aTsyReqHandle,ret);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrCancel);
			}
		}
	else if (iState == RPacketContext::EStatusDeleted)
		{
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}

	return KErrNone;
	}

TInt CSimMbmsPacketContext::Deactivate(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to try to Deactivate an active context.
* In this method, we'll store the TsyReqHandle for later completion then call ActionEvent
* state machine to handle the event which will kick off a timer.
* The Actual Deactivation will take place in the TimerCallBack method.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_DEACTIVATE_1, "CSimMbmsPacketContext::Deactivate called");
	iDeactivateRequestHandle = aTsyReqHandle;
	TInt ret = ActionEvent(EMbmsContextEventDeactivate);
	return ret;
	}

TInt CSimMbmsPacketContext::DeactivateCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to try and cancel a Deactivate request.
* This will only complete successfully if and only if the Deactivate request has not completed.
* i.e. if we haven't already deactivated the context.
* Otherwise, the request will be completed with KErrNone.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_DEACTIVATECANCEL_1, "CSimMbmsPacketContext::DeactivateCancel called");
	iMbmsContextTimer->Cancel();
	if(((iState == RPacketContext::EStatusInactive) || (iState == RPacketContext::EStatusDeactivating)) && 
					(iCurrentEvent==EMbmsContextEventDeactivate))
		{
		iCurrentEvent= EMbmsContextEventNone;
		TInt ret = ChangeState(RPacketContext::EStatusActive);
		if(ret!= KErrNone)
			{
			ReqCompleted(aTsyReqHandle,ret);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrCancel);
			}
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	
	return KErrNone;
	}

TInt CSimMbmsPacketContext::ReActivate(CSimPacketService* aPacketService, const TName& aContextName)
/**
* This method is called by the packet Service to Re-Initialise a Deleted context.
* If the context is deleted then the client cannot retrieve any 
* infomation or use the context in any way.
* In order for the client to use the context again, the client must 
* open an object by name passing the name of this context as a pararemter.
*
* @param aPacketService pointer to the parent class CSimPacketService.
* @param aContextName Must be the same name as the name for this context.
* @return KErrNone if no problem in re-initialising this context.
*/
	{
	if(iContextName.Compare(aContextName) != KErrNone)
		{
		return KErrBadName;
		}

	iPacketService = aPacketService;

	if(iDeleted)
		{
		TInt ret = ChangeState(RPacketContext::EStatusInactive);
		if (ret != KErrNone)
			{
			return ret;
			}

		iDeleted = EFalse;
		iMbmsContextConfigParamsIndex = 0;
		iNotifyStatusChange.iNotifyPending = EFalse;
		iNotifyConfigMBMS.iNotifyPending = EFalse;
		}
	return KErrNone;
	}

TInt CSimMbmsPacketContext::Delete(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to try to Delete this context.
* In this method, we'll store the TsyReqHandle for later completion then call ActionEvent
* state machine to handle the event which will kick off a timer.
* The Actual Delete will take place in the TimerCallBack method.
*
* This request will only succeed if the context already exist (had been created) and also not
* previously deleted.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_DELETE_1, "CSimMbmsPacketContext::Delete called");
	iDeleteRequestHandle = aTsyReqHandle;
	TInt ret = ActionEvent(EMbmsContextEventDelete);
	return ret;
	}

TInt CSimMbmsPacketContext::DeleteCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Client calls this method to try and cancel a Delete context request.
* This will only complete successfully if and only if the Delete request has not completed.
* i.e. if we haven't already Deleted the context.
*
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_DELETECANCEL_1, "CSimMbmsPacketContext::DeleteCancel called");
	iMbmsContextTimer->Cancel();
	if((iState == RPacketContext::EStatusInactive) && (iCurrentEvent==EMbmsContextEventDelete))
		{
		iCurrentEvent= EMbmsContextEventNone;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimMbmsPacketContext::GetLastErrorCause(const TTsyReqHandle aTsyReqHandle,TInt* /*aError*/)
/**
* This method is supported in only for the MBMS Packet Context
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aError pointer to the last error cause
* @return KerrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_GETLASTERRORCAUSE_1, "CSimMbmsPacketContext::GetLastErrorCause called");
	
	ReqCompleted(aTsyReqHandle,iLastError);
	return KErrNone;
	}


TInt CSimMbmsPacketContext::NotifyConfigChanged(const TTsyReqHandle aTsyReqHandle, TDes8* aConfig)
/**
* Records a client interest in being notified of a change in the Context Configuration.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aConfig pointer to the context config to store the config info when the config changes.
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_NOTIFYCONFIGCHANGED_1, "CSimMbmsPacketContext::NotifyConfigChanged called");
	TPckg<TPacketDataConfigBase>* configBase = (TPckg<TPacketDataConfigBase>*)aConfig;
	TPacketDataConfigBase& configBaseV1 = (*configBase)();
	
	if(configBaseV1.ExtensionId() == TPacketDataConfigBase::KConfigMBMS)
		{
	    __ASSERT_ALWAYS(!iNotifyConfigMBMS.iNotifyPending,SimPanic(ENotificationAlreadyPending));

	    iNotifyConfigMBMS.iNotifyPending = ETrue;
	    iNotifyConfigMBMS.iNotifyHandle = aTsyReqHandle;
	    iNotifyConfigMBMS.iNotifyData = &configBaseV1;
		}
	else
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_NOTIFYCONFIGCHANGED_2, "CSimMbmsPacketContext::NotifyConfigChanged Unexpected config for MBMS context");
		}

	return KErrNone;
	}

TInt CSimMbmsPacketContext::NotifyConfigChangedCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel a client's interest in being notified when the context configuration changes
* This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_NOTIFYCONFIGCHANGEDCANCEL_1, "CSimMbmsPacketContext::NotifyConfigChangedCancel called");
	if(iNotifyConfigMBMS.iNotifyPending &&
		iNotifyConfigMBMS.iNotifyHandle == aTsyReqHandle)
		{
		iNotifyConfigMBMS.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
		
	return KErrNone;
	}

TInt CSimMbmsPacketContext::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RPacketContext::TContextStatus* aContextStatus)
/**
* Record a client's interest in being notified of the state of the packet contexts.
* First check that there isn't already a notification pending (the ETel Server should protect
* against this) and then record the information necessary to complete the request later, when
* the status does actually change.
*
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aContextStatus pointer to the packet context status
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_NOTIFYSTATUSCHANGE_1, "CSimMbmsPacketContext::NotifyStatusChange called");
	__ASSERT_ALWAYS(!iNotifyStatusChange.iNotifyPending,SimPanic(ENotificationAlreadyPending));
	iNotifyStatusChange.iNotifyPending = ETrue;
	iNotifyStatusChange.iNotifyHandle = aTsyReqHandle;
	iNotifyStatusChange.iNotifyData = aContextStatus;
	return KErrNone;
	}

TInt CSimMbmsPacketContext::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel a client's interest in being notified when the packet context status changes.
* This is acheived simply by resetting the flag that indicates a notification is pending.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_NOTIFYSTATUSCHANGECANCEL_1, "CSimMbmsPacketContext::NotifyStatusChangeCancel called");
	if(iNotifyStatusChange.iNotifyPending)
		{
		iNotifyStatusChange.iNotifyPending=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CSimMbmsPacketContext::UpdateMbmsSessionList(const TTsyReqHandle aTsyReqHandle,TMbmsAction* aAction, TUint* aSessionId)
/**
* client's interest in updating the MBMS session's list.
* This is achieved  by updating the list maintained internally by the context
* 
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aAction TMbmsAction - ADD/ REMOVE / REMOVE_ALL
* @param aSessionId sessoinID to be used for the updation.
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_UPDATEMBMSSESSIONLIST_1, "CSimMbmsPacketContext::UpdateMbmsSessionList called");

	TInt error=KErrNone;
	iUpdateSessionHandle = aTsyReqHandle;
	TKeyArrayFix key(0, ECmpTUint);
	TInt position =-1;

	switch(*aAction)
		{
		case SIMTSY_PACKET_MBMS_ADD_ENTRIES:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_UPDATEMBMSSESSIONLIST_2, "CSimMbmsPacketContext::UpdateMbmsSessionList Action: ADD ");
			
			// TRAP can contain multiple statments
			TRAP(error,iSessionIdList->AppendL(*aSessionId);
                    //iSessionIdList->At(0) = ++iNumOfSessionId;
                    iSessionIdList->Delete(0);
                    iSessionIdList->InsertL(0,++iNumOfSessionId));
			
            iMbmsUpdateSessionTimer->Start(RandTime(),this,ETimerIdMbmsUpdateSessionId);

			if(error != KErrNone)
				{
				ReqCompleted(iUpdateSessionHandle,error);
				}
			break;

		case SIMTSY_PACKET_MBMS_REM_ENTRIES:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_UPDATEMBMSSESSIONLIST_3, "CSimMbmsPacketContext::UpdateMbmsSessionList Action: REMOVE ");
			iSessionIdList->Find(*aSessionId,key,position);
			if(position != -1)
				{
				iSessionIdList->Delete(position);
				//iSessionIdList->At(0) = --iNumOfSessionId;
				TRAP(error, iSessionIdList->InsertL(0,--iNumOfSessionId));
	            if(error != KErrNone)
	                {
	                ReqCompleted(iUpdateSessionHandle,error);
	                }
				iMbmsUpdateSessionTimer->Start(RandTime(),this,ETimerIdMbmsUpdateSessionId);
				}
			else
				{
				error = KErrNotFound;
				ReqCompleted(iUpdateSessionHandle,error);
				}
			break;

		case SIMTSY_PACKET_MBMS_REM_ALL_ENTRIES:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_UPDATEMBMSSESSIONLIST_4, "CSimMbmsPacketContext::UpdateMbmsSessionList Action: REMOVE_ALL ");
			iNumOfSessionId=0;
			//iSessionIdList->At(0) = iNumOfSessionId;
			TRAP(error, iSessionIdList->InsertL(0,iNumOfSessionId));
            if(error != KErrNone)
                {
                ReqCompleted(iUpdateSessionHandle,error);
                }
			iSessionIdList->Delete(1,iSessionIdList->Count()); // removing all session ids from the list
			ReqCompleted(aTsyReqHandle, KErrNone);
			break;

		default:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_UPDATEMBMSSESSIONLIST_5, "CSimMbmsPacketContext::UpdateMbmsSessionList Action: Default ");
			ReqCompleted(aTsyReqHandle,KErrNotFound);
			break;
		}
	return KErrNone;
	}

TInt CSimMbmsPacketContext::UpdateMbmsSessionListCancel(const TTsyReqHandle aTsyReqHandle)
/**
* client's interest in cancelling the updation the MBMS session's list.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_UPDATEMBMSSESSIONLISTCANCEL_1, "CSimMbmsPacketContext::UpdateMbmsSessionListCancel called");
	iMbmsUpdateSessionTimer->Cancel();
	if( iUpdateSessionHandle == aTsyReqHandle )
		{
		ReqCompleted(iUpdateSessionHandle,KErrCancel);
		iUpdateSessionHandle=NULL;
		}
	else
		{
		ReqCompleted(iUpdateSessionHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CSimMbmsPacketContext::InitialiseContext(const TTsyReqHandle aTsyReqHandle, TDes8* aDataChannelV2Pckg)
/**
* client's interest in Initialising the context.
* 
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aDataChannelV2Pckg the information to be filled once the initialisation is complete.
* @return KErrNone
*/
	{
	TCommSetupItem item = FindCommSettings();
	iInitContextData.iTsyReqHandle = aTsyReqHandle;
	iInitContextData.iChannel = aDataChannelV2Pckg;
	iMbmsContextInitTimer->Start(RandTime(), this, ETimerIdPcktContextInitContext);

	return KErrNone;
	}
	
TInt CSimMbmsPacketContext::DoInitialiseContext()
/* Function completes the request from the client for the initialisation of the context.
 * Called when the timer event occurs after receiving the InitialiseContext request.
 * returns KErrNone.
 */
	{
	RPacketContext::TDataChannelV2Pckg* channel = (RPacketContext::TDataChannelV2Pckg*)iInitContextData.iChannel;
	RPacketContext::TDataChannelV2& channelV2 = (*channel)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(channelV2);
	if(err != KErrNone)
		{
		ReqCompleted(iInitContextData.iTsyReqHandle, err);
		return KErrNone;
		}

	TCommSetupItem item = FindCommSettings();
	channelV2.iPort = item.iPortName;
	channelV2.iCsy = item.iCsyName;
	channelV2.iChannelId = item.iPortName;
	
	ReqCompleted(iInitContextData.iTsyReqHandle, KErrNone);

	return KErrNone;
	}

TInt CSimMbmsPacketContext::InitialiseContextCancel(const TTsyReqHandle aTsyReqHandle)
/**
* client's interest in cancelling the Initialisation of the MBMS Context.
* 
* @param aTsyReqHandle Tsy Request handle for the client cancel request
* @return KErrNone
*/
	{
	iMbmsContextInitTimer->Cancel();
	ReqCompleted(aTsyReqHandle, KErrCancel);
	
	return KErrNone;
	}

TInt CSimMbmsPacketContext::GetMbmsSessionsPhase1(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TInt* aBufSize)
/**
* client's interest in retrieving the MBMS Session information.
* This is the phase1 retrieval
* 
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aClient ClientId from which the request is sent
* @param aBufSize  Size of the buffer to be allocated to retrieve the data in phase2.
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_GETMBMSSESSIONSPHASE1_1, "CSimMbmsPacketContext::GetMbmsSessionsPhase1 called");
	iGetMbmsSessionHandle = aTsyReqHandle;
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessGetMbmsSessionsPhase1L(aTsyReqHandle, aClient, aBufSize););
	if (leaveCode != KErrNone)
		{
		ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	}	

TInt CSimMbmsPacketContext::ProcessGetMbmsSessionsPhase1L(const TTsyReqHandle aTsyReqHandle, 
													 TClientId* aClient, 
													 TInt* aBufSize)
/**
* The Actual function which does the phase1 retrieval.
* 
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aClient ClientId from which the request is sent
* @param aBufSize  Size of the buffer to be allocated to retrieve the data in phase2.
* @return KErrNone
*/
	{
	// Retrieve MBMS session list,
	// Store the sessions and then return the size of the buffer to the client
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_PROCESSGETMBMSSESSIONSPHASE1L_1, "CSimMbmsPacketContext::ProcessGetMbmsSessionsPhase1L called");
	RPacketMbmsContext::CMbmsSession* mbmsSession=RPacketMbmsContext::CMbmsSession::NewL();
	CleanupStack::PushL(mbmsSession);

	for(TInt i=1;i<=iNumOfSessionId;i++)
		{
		mbmsSession->iSessionIdList.Append(iSessionIdList->At(i));
		}

	
	// Store the streamed list and the client ID
	CPcktListReadAllAttempt* read = CPcktListReadAllAttempt::NewL(*aClient,aTsyReqHandle);
	CleanupStack::PushL(read);
	
	HBufC8* iBuff=NULL;
	mbmsSession->ExternalizeL(iBuff);	
	CleanupStack::PushL(iBuff);
		
	CBufFlat* buf=CBufFlat::NewL(iBuff->Length());
	CleanupStack::PushL(buf);
	buf->InsertL(0,*iBuff);	
	
	read->iListBuf = buf;
	CleanupStack::Pop(buf);	
	CleanupStack::PopAndDestroy(iBuff);
	
	iMbmsSessionList->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	CleanupStack::PopAndDestroy(mbmsSession);
	
	// Complete first phase of list retrieval
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimMbmsPacketContext::GetMbmsSessionsPhase2(const TTsyReqHandle aTsyReqHandle,TClientId* aClient, TDes8* aBuf)
/**
* client's interest in retrieving the MBMS Session information.
* This is the phase2 retrieval
* 
* @param aTsyReqHandle Tsy Request handle for the client request
* @param aClient ClientId from which the request is sent
* @param aBuf  Buffer allocated to retrieve the data.
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_GETMBMSSESSIONSPHASE2_1, "CSimMbmsPacketContext::GetMbmsSessionsPhase2 called");
	CPcktListReadAllAttempt* read=NULL;
	// Find the get Mbms monitored services from this client
	for (TInt i=0; i<iMbmsSessionList->Count(); ++i)
		{
		read = iMbmsSessionList->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iMbmsSessionList->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	ReqCompleted(aTsyReqHandle,KErrBadHandle);
	return KErrNone;
	}
	
TInt CSimMbmsPacketContext::GetMbmsSessionsCancel(const TTsyReqHandle aTsyReqHandle)
/**
* client's interest in cancelling the MBMS Get session request.
* 
* @param aTsyReqHandle Tsy Request handle for the client request
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_GETMBMSSESSIONSCANCEL_1, "CSimMbmsPacketContext::GetMbmsSessionsCancel called");
	// Remove the MBMS sessions from iMbmsSessionList
	
	CPcktListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iMbmsSessionList->Count(); ++i)
		{
		read = iMbmsSessionList->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iMbmsSessionList->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSimMbmsPacketContext::ChangeState(RPacketContext::TContextStatus aNewState)
/**
* Attempt to change state.
* First validate that the requested state change is ok.  If it is then proceed to change
* the state and complete any pending state change notification.
*
* @param aNewState the new state to change to
* @return Error indication if change of state is successful or not
*/
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_CHANGESTATE_1, "CSimMbmsPacketContext::ChangeState [newState=%d]", aNewState);
	__ASSERT_ALWAYS(aNewState!=RPacketContext::EStatusUnknown,SimPanic(ECallStatusUnknownIllegal));

	if(iState==aNewState)
		return KErrNone;

	TInt ret=KErrGeneral;
	const struct TMbmsContextStateChangeValidity* stateChangePnt=KMbmsContextStateChangeValidity;
	while(stateChangePnt->iOldState!=KMbmsContextStateTableTerminator)
		{
		if(stateChangePnt->iOldState == RPacketContext::EStatusReceiving && stateChangePnt->iNewState == RPacketContext::EStatusActive)
			{
			iLastError = KErrMbmsNotSupported;
			}
		else if(stateChangePnt->iOldState == RPacketContext::EStatusReceiving && stateChangePnt->iNewState == RPacketContext::EStatusSuspended)
			{
			iLastError = KErrMbmsServicePreempted;
			}
		else if(stateChangePnt->iOldState == RPacketContext::EStatusSuspended && stateChangePnt->iNewState == RPacketContext::EStatusActive)
			{
			iLastError = KErrMbmsServiceUnavailable;
			}
		if((stateChangePnt->iOldState==iState) && (stateChangePnt->iNewState==aNewState))
			{
			ret=stateChangePnt->iError;
			break;
			}
		stateChangePnt++;
		}

	if(ret!=KErrNone)
		return ret;

	//Request permission from the phone to change the state of the packet connection
	ret = iPacketService->ChangeState(ConvertToPacketServiceStatus(aNewState));
	if(ret!=KErrNone)
		return ret;

	// Actually change the state.
	iState=aNewState;

	// Check for a pending line state notification.
	if(iNotifyStatusChange.iNotifyPending)
		{
		iNotifyStatusChange.iNotifyPending = EFalse;
		*(RPacketContext::TContextStatus*)iNotifyStatusChange.iNotifyData = iState;
		ReqCompleted(iNotifyStatusChange.iNotifyHandle,KErrNone);
		}

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_CHANGESTATE_2, "<<CSimMbmsPacketContext::ChangeState Completed");
	return KErrNone;
	}

TInt CSimMbmsPacketContext::ActionEvent(TMbmsContextEvent aEvent)
/**
* Entry point when an event has occured that may advance the state machine.
* The aEvent parameter describes the event.
*
* This function contains the main state machine for the packet Context.  
*
* @param aEvent The Packet Context event to handle
* @return value represents the error state caused by the attempted state machine jump.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_ACTIONEVENT_1, "<<CSimMbmsPacketContext::ActionEvent Completed");
	TInt ret=KErrNone;
	__ASSERT_ALWAYS(iState!=RPacketContext::EStatusUnknown,SimPanic(ECallStatusUnknownIllegal));

	switch(aEvent)
		{
		case EMbmsContextEventActivate:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_ACTIONEVENT_2, "CSimMbmsPacketContext::ActionEvent = [EMbmsContextEventActivate]");
			if(iState==RPacketContext::EStatusInactive)
				{
				iCurrentEvent=EMbmsContextEventActivate;
				ret = ChangeState(RPacketContext::EStatusActivating);
				iMbmsContextTimer->Start(RandTime(),this);  
				}
			else
				{
				ReqCompleted(iActivateRequestHandle, KErrGprsActivationRejected);
				}
			break;

		case EMbmsContextEventDeactivate:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_ACTIONEVENT_3, "CSimMbmsPacketContext::ActionEvent = [EMbmsContextEventDeactivate]");
			if(iState==RPacketContext::EStatusActive || iState==RPacketContext::EStatusActivating)
				{
				iCurrentEvent=EMbmsContextEventDeactivate;
				ret = ChangeState(RPacketContext::EStatusDeactivating);
				iMbmsContextTimer->Start(RandTime(),this);
				}
			else
				{
				ReqCompleted(iDeactivateRequestHandle, KErrNone);
				}
			break;

		case EMbmsContextEventDelete:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_ACTIONEVENT_4, "CSimMbmsPacketContext::ActionEvent = [EMbmsContextEventDelete]");
			iDeleted = ETrue;
			if(iState==RPacketContext::EStatusInactive)
				{
				iCurrentEvent=EMbmsContextEventDelete;
				iMbmsContextTimer->Start(RandTime(),this);
				}
			else
				ReqCompleted(iDeleteRequestHandle, KErrInUse);
			break;
		default:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_ACTIONEVENT_5, "CSimMbmsPacketContext::ActionEvent = [Default Case]");
			break;
		}
	return ret;
	}

RPacketService::TStatus CSimMbmsPacketContext::ConvertToPacketServiceStatus(RPacketContext::TContextStatus aNewState)
/**
* Converts Packet Context Status (RPacketContext::TContextStatus) to 
*          Packet Service Status (RPacketService::TStatus)
*
* @param aNewState the Context status to convert to packet service status
* @return RPacketService::TStatus The packet service status conversion
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_CONVERTTOPACKETSERVICESTATUS_1, "CSimMbmsPacketContext::ConvertToPacketServiceStatus called");
	RPacketService::TStatus serviceStatus;
	switch (aNewState)
		{
		case RPacketContext::EStatusInactive:
		case RPacketContext::EStatusActivating:
		case RPacketContext::EStatusDeleted:
			serviceStatus = RPacketService::EStatusAttached;
			break;
		case RPacketContext::EStatusActive:
		case RPacketContext::EStatusDeactivating:
			serviceStatus = RPacketService::EStatusActive;
			break;
		case RPacketContext::EStatusSuspended:
			serviceStatus = RPacketService::EStatusSuspended;
			break;
		default:
			serviceStatus = RPacketService::EStatusUnattached;
			break;
		}
	return serviceStatus;
	}

TInt CSimMbmsPacketContext::RandTime()
/* 
 * Function that generates time(in seconds) at random for the SimTSY 
 * Maximum of four seconds; zero seconds also included just to simulate the 
 * synchronous call scenario 
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMMBMSPACKETCONTEXT_RANDTIME_1, "CSimMbmsPacketContext::RandTime called");
	TInt ranT= Math::Random()%4; 
	return(ranT);
	}
