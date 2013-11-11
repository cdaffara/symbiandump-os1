// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements the CGprs class. Used to implement the RPacketService API.
// This file also implements the CGprsContextEntry class used to store a pointer to a CGprsContext object.
// 
//

/**
 @file
*/

#include "Gprscontext.h"
#include "mSLOGGER.H"
#include "NOTIFY.H"
#include "ATGprsAttach.H"
#include "ATGprsDetach.H"
#include "AtGprsClass.h"
#include "atgprsntwkregstatus.h"					// for CATGprsNtwkRegStatus
#include "atgprsntwkregstatuschange.h"		// for CATGprsNtwkRegStatusChange
#include "ATIO.H"
#include "ATHANGUP.H"


CGprs* CGprs::NewL(CATIO* aIo, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
/**
 * Standard 2 phase constructor.
 *
 * @param aIo pointer to communication object.
 * @param aInit pointer to AT phone init object.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
    {
    CGprs* gprs=new(ELeave) CGprs(aIo, aInit, aPhoneGlobals);
    CleanupStack::PushL(gprs);
    gprs->ConstructL();
    CleanupStack::Pop();
    return gprs;
    }


CGprs::CGprs(CATIO* aIo, CATInit*   aInit, CPhoneGlobals* aPhoneGlobals) 
        : CSubSessionExtBase(), iIo(aIo), iInit(aInit), iPhoneGlobals(aPhoneGlobals)
/**
 * Constructor
 *
 * @param aIo pointer to communication object.
 * @param aInit pointer to AT phone init object.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
    {
    LOGTEXT(_L8("CGprs: Entered constructor"));
    }    


void CGprs::ConstructL()
/**
 * Construct all objects that can leave.
 */
    {
    LOGTEXT(_L8("CGprs: ConstructL"));
    iContexts.SetOffset(_FOFF(CGprsContextEntry,iLink));
    iATGPRSAttach       = CATGprsAttach::NewL(iIo, this, iInit, iPhoneGlobals); 
    iATGprsDetach       = CATGprsDetach::NewL(iIo, this, iInit, iPhoneGlobals); 
    iATGprsClass        = CATGprsClass::NewL(iIo, this, iInit, iPhoneGlobals);
    iATGprsNtwkRegStatusChange= CATGprsNtwkRegStatusChange::NewL(iIo,this,iPhoneGlobals);
    iATGprsNtwkRegStatus= CATGprsNtwkRegStatus::NewL(iIo, this, iInit, iPhoneGlobals,iATGprsNtwkRegStatusChange);
   	User::LeaveIfError(iMutex.CreateGlobal(KNullDesC, EOwnerProcess));
    }
    
RHandleBase* CGprs::GlobalKernelObjectHandle()
/**
* Implements CSubSessionExtBase virtual method, which provides
* handle of the mutex object to etel server that passes it up to clients.
* @return pointer to synchronisation mutex object
*/
	{
		return &iMutex;
	}


CGprs::~CGprs()
/**
 * Destructor
 */
    {
    LOGTEXT(_L8("CGprs: Entered destructor"));
	iMutex.Close();
    iPhoneGlobals->iNotificationStore->RemoveClientFromLastEvents(this);
    CGprsContextEntry* contextEntry;
    TDblQueIter<CGprsContextEntry> iter(iContexts);
    while (contextEntry=iter++, contextEntry)
        {
        contextEntry->Deque();
        contextEntry->iContext->Close();
        delete contextEntry;
        __ASSERT_ALWAYS(iContexts.IsEmpty(),Panic(EPacketContextsRemaining));
        }

		delete iATGprsNtwkRegStatusChange;
		delete iATGprsNtwkRegStatus;
    delete iATGprsClass;
    delete iATGprsDetach; 
		delete iATGPRSAttach;
    }


void CGprs::Init()
/**
 * This function does nothing.
 */
    {
    LOGTEXT(_L8("CGprs: init()"));
    }


CTelObject* CGprs::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * This functionality does not have to be supported as it is
 * not requitred by the Etel Packet API (etelpckt).
 */
    {  
    LOGTEXT(_L8("CGprs::OpenNewObjectByNameL Leaving with KErrNotFound as this is not supported"));

    User::Leave(KErrNotFound);		// Have to leave otherwise etel will think we have returned a
		                              // valid CTelObject and will cause an access violation.

		return NULL;
    }


CTelObject* CGprs::OpenNewObjectL(TDes& aNewName)
/**
 * This function returns a pointer to a new context object.
 *
 * @param  aNewName name of the new context.
 */
    {
    TInt cid = 0;
    GenerateNewContextNameAndCid(aNewName, cid);
    if(cid > MaxNumberOfContexts())
        User::Leave(KErrOverflow);
#if defined __LOGDEB__ 
    TBuf8<40> tmpLog;
    tmpLog.Copy(aNewName);
    LOGTEXT2(_L8("CGprs::OpenNewObjectL(), %S"), &tmpLog);
#endif
    RPacketContext::TContextConfigGPRS contextConfigV01;
    ContextConfigGPRS(&contextConfigV01);
    CGprsContext* context = CGprsContext::NewL(this, iIo, iInit, iPhoneGlobals, aNewName,  cid, &contextConfigV01);
	CleanupStack::PushL(context);
    AppendNewContextL(context);
	CleanupStack::Pop(context);
    iPhoneGlobals->iNotificationStore->CheckNotification(this, EPacketContextAddedChanged);
    return context;
    } 


CTelObject::TReqMode CGprs::ReqModeL(const TInt aIpc)
/**
 * This function is called from the framework and checks the flow control for a IPC call.
 * @param aIpc IPC call from client.
 */
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
		case EPacketAttach:
		case EPacketDetach:
		case EPacketSetMSClass:
			ret = KReqModeFlowControlObeyed;
			break;
		case EPacketGetNtwkRegStatus:
			if (!iPhoneGlobals->iPhoneStatus.iDataPortLoaned)
				{
				ret = KReqModeFlowControlObeyed;
				}
			break;
		case EPacketGetMSClass:
		case EPacketGetDynamicCaps:
		case EPacketGetDefaultContextParams:
		case EPacketGetStatus:
		case EPacketEnumerateContexts:
		case EPacketGetContextInfo:
		case EPacketGetAttachMode:
		case EPacketGetStaticCaps: 
		case EPacketSetDefaultContextParams:
		case EPacketRejectActivationRequest:
		case EPacketSetAttachMode:
			break;
		case EPacketNotifyContextAdded:
		case EPacketNotifyStatusChange:
		case EPacketNotifyChangeOfNtwkRegStatus:	
			ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
			break; 
		default:
			User::Leave(KErrNotSupported);
			break;
		}

	// Check if the data port is currently loaned. If it is and the requested IPC
	// is flow controlled then block Etel calling the IPC by leaving with KErrInUse
	if((ret&KReqModeFlowControlObeyed) && iPhoneGlobals->iPhoneStatus.iDataPortLoaned)
		{
		LOGTEXT2(_L8("ReqModeL Leaving with KErrInUse as data port is loaned (aIpc=%d)"),aIpc);
		User::Leave(KErrInUse);
		}

    return ret;
    }

TInt CGprs::RegisterNotification(const TInt aIpc)
/**
 * This function is called from the framework.
 * @param  aIpc IPC call from client.
 */
    {
	LOGTEXT2(_L8("CGprs::RegisterNotification called with aIpc=%d"),aIpc);
    switch (aIpc)
        {
        case EPacketNotifyContextAdded:
        case EPacketNotifyStatusChange:
        case EPacketNotifyContextActivationRequested:
        case EPacketNotifyChangeOfNtwkRegStatus:
        case EPacketNotifyMSClassChange:
        case EPacketNotifyDynamicCapsChange:
            return KErrNone;
        default:
            // Unknown or invalid IPC
            LOGTEXT2(_L8("CGprs::RegisterNotification Error Unknown IPC, aIpc=%d"),aIpc);
            return KErrNotSupported;
        }
    }


TInt CGprs::DeregisterNotification(const TInt aIpc)
/**
 * This function is called when there is no client listening to a notification anymore.
 * @param aIpc IPC call from client.
 */
    {
	LOGTEXT2(_L8("CGprs::DeregisterNotification called with aIpc=%d"),aIpc);
    switch (aIpc)
        {
        case EPacketNotifyContextAdded:
        case EPacketNotifyStatusChange:
        case EPacketNotifyContextActivationRequested:
        case EPacketNotifyChangeOfNtwkRegStatus:
        case EPacketNotifyMSClassChange:
        case EPacketNotifyDynamicCapsChange:
            return KErrNone;
        default:
            // Unknown or invalid IPC
            LOGTEXT2(_L8("CGprs::DeregisterNotification Error Unknown IPC, aIpc=%d"),aIpc);
            return KErrNotSupported;
        }
    }


TInt CGprs::NumberOfSlotsL(const TInt aIpc)
/**
 * This function is called from the framework and checks the number of slots for a notification.
 * @param aIpc IPC call from client.
 */
    {
    TInt numberOfSlots=0;
    switch (aIpc)
        {
        case EPacketNotifyStatusChange:
        case EPacketNotifyDynamicCapsChange:
            LOGTEXT(_L8("CGprs: Registered with 10 slot"));
            numberOfSlots=10;
            break;
        case EPacketNotifyContextAdded:
        case EPacketNotifyContextActivationRequested:
        case EPacketNotifyChangeOfNtwkRegStatus:
        case EPacketNotifyMSClassChange:
            LOGTEXT(_L8("CGprs: Registered with 2 slot"));
            numberOfSlots=2;
            break;
        default:
            // Unknown or invalid IPC
            LOGTEXT2(_L8("CGprs: Number of Slots error, unknown IPC:%d"),aIpc);
            User::Leave(KErrNotSupported);
            break;
        }  
    return numberOfSlots;
    }


TInt CGprs::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage)
/**
 * This function is called from the framework.
 * @param aTsyReqHandle handle to client.
 * @param aIpc IPC call from client.
 * @param aPackage agruments with the IPC call.
 */
    {
    TAny* dataPtr=aPackage.Ptr1();
    TAny* dataPtr2=aPackage.Ptr2();
    switch (aIpc)
        {
        case EPacketNotifyContextAdded:
            return NotifyContextAdded(aTsyReqHandle, aPackage.Des1u());
        case EPacketGetStatus:
            return GetStatus(aTsyReqHandle,
            REINTERPRET_CAST(RPacketService::TStatus*, dataPtr));
        case EPacketNotifyStatusChange:
            return NotifyStatusChange(aTsyReqHandle,
            REINTERPRET_CAST(RPacketService::TStatus*, dataPtr));
        case EPacketNotifyContextActivationRequested:
            return NotifyContextActivationRequested(aTsyReqHandle,
			REINTERPRET_CAST(RPacketContext::TProtocolType*, dataPtr));
        case EPacketEnumerateContexts:
            return EnumerateContexts(aTsyReqHandle,
            REINTERPRET_CAST(TInt*, dataPtr),
            REINTERPRET_CAST(TInt*, dataPtr2));
        case EPacketGetContextInfo:
            return GetContextInfo(aTsyReqHandle,
            *REINTERPRET_CAST(TInt*, dataPtr),
            REINTERPRET_CAST(RPacketService::TContextInfo*, dataPtr2));
        case EPacketGetNtwkRegStatus:
            return GetNtwkRegStatus(aTsyReqHandle,
            REINTERPRET_CAST(RPacketService::TRegistrationStatus*, dataPtr));
        case EPacketNotifyChangeOfNtwkRegStatus:
            return NotifyChangeOfNtwkRegStatus(aTsyReqHandle,
            REINTERPRET_CAST(RPacketService::TRegistrationStatus*, dataPtr));
        case EPacketGetMSClass:
            return GetMSClass(aTsyReqHandle,
            REINTERPRET_CAST(RPacketService::TMSClass*, dataPtr),
            REINTERPRET_CAST(RPacketService::TMSClass*, dataPtr2));
        case EPacketSetMSClass:
            return SetMSClass(aTsyReqHandle,
            REINTERPRET_CAST(RPacketService::TMSClass*, dataPtr));
        case EPacketNotifyMSClassChange:
            return NotifyMSClassChange(aTsyReqHandle,
            REINTERPRET_CAST(RPacketService::TMSClass*, dataPtr));
        case EPacketGetStaticCaps:
            return GetStaticCaps(aTsyReqHandle, 
				REINTERPRET_CAST(TUint*, dataPtr),
				REINTERPRET_CAST(RPacketContext::TProtocolType*, dataPtr2));
        case EPacketGetDynamicCaps:
            return GetDynamicCaps(aTsyReqHandle,
            REINTERPRET_CAST(RPacketService::TDynamicCapsFlags*, dataPtr));
        case EPacketNotifyDynamicCapsChange:
            return NotifyDynamicCapsChange(aTsyReqHandle,
            REINTERPRET_CAST(RPacketService::TDynamicCapsFlags*, dataPtr));
        case EPacketSetAttachMode:
            return SetAttachMode(aTsyReqHandle,
            REINTERPRET_CAST(RPacketService::TAttachMode*, dataPtr));
        case EPacketGetAttachMode:
            return GetAttachMode(aTsyReqHandle,
            REINTERPRET_CAST(RPacketService::TAttachMode*, dataPtr));
        case EPacketAttach:
            return Attach(aTsyReqHandle);
        case EPacketDetach:
            return Detach(aTsyReqHandle);
        case EPacketRejectActivationRequest:
            return RejectActivationRequest(aTsyReqHandle);
        case EPacketSetDefaultContextParams:
            return SetDefaultContextParams(aTsyReqHandle, aPackage.Des1n());
        case EPacketGetDefaultContextParams:
            return GetDefaultContextParams(aTsyReqHandle, aPackage.Des1n());
        default:
            return KErrNotSupported;
        }
    }


TInt CGprs::CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle)
/**
 * This function is called from the framework and cancels a outstanding request.
 * @param aIpc The IPC to cancel.
 * @param aTsyReqHandle the client handle to cancel.
 */
    {
    LOGTEXT(_L8("CGprs: - CancelService called"));
    switch (aIpc)
        {
        case EPacketNotifyContextAdded:
            return NotifyContextAddedCancel(aTsyReqHandle);
        case EPacketAttach:
            return AttachCancel(aTsyReqHandle);
        case EPacketDetach:
            return DetachCancel(aTsyReqHandle);
        case EPacketNotifyStatusChange:
            return NotifyStatusChangeCancel(aTsyReqHandle);
        case EPacketNotifyContextActivationRequested:
            return NotifyContextActivationRequestedCancel(aTsyReqHandle);
        case EPacketRejectActivationRequest:
            return RejectActivationRequestCancel(aTsyReqHandle);
        case EPacketGetContextInfo:
            return GetContextInfoCancel(aTsyReqHandle);
        case EPacketNotifyChangeOfNtwkRegStatus:
            return NotifyChangeOfNtwkRegStatusCancel(aTsyReqHandle);
        case EPacketGetMSClass:
            return GetMSClassCancel(aTsyReqHandle);
        case EPacketSetMSClass:
            return SetMSClassCancel(aTsyReqHandle);
        case EPacketNotifyMSClassChange:
            return NotifyMSClassChangeCancel(aTsyReqHandle);
        case EPacketNotifyDynamicCapsChange:
            return NotifyDynamicCapsChangeCancel(aTsyReqHandle);
				case EPacketGetNtwkRegStatus:
            return GetNtwkRegStatusCancel(aTsyReqHandle);
        default:
            return KErrGeneral; // Should never get to this point, as Etel should try to cancel IPC we do not support
        } 
    }


void CGprs::AppendNewContextL(CGprsContext* aNewContext)
/**
 * This function adds a context to the list.
 *
 * @param aNewContext Adds a context to the linked list of contexts.
 */
    {
    LOGTEXT(_L8("CGprs::AppendNewContextL called"));
    CGprsContextEntry* entry = new (ELeave) CGprsContextEntry(aNewContext);
    iContexts.AddLast(*entry);
    }


void CGprs::RemoveContext(CGprsContext* aContext)
/**
 * This function removes a context from the list.
 *
 * @param aContext Context to remove from linked list.
 */
    {
#ifdef _DEBUG
    TBuf8<KMaxName> tmpLog;
    tmpLog.Copy(*(aContext->ContextName()));		// Convert 16bit string to 8bit so we can log it
    LOGTEXT2(_L8("CGprs::RemoveContext called, %S"), &tmpLog);
#endif
    CGprsContextEntry* contextEntry;
    TDblQueIter<CGprsContextEntry> iter(iContexts);
    while (contextEntry=iter++, contextEntry)
        {
        if (contextEntry->iContext == aContext)
            {
            contextEntry->Deque();
            delete contextEntry;    // just deletes list entry, not the context itself
            break;
            }
        }
    }


TDes* CGprs::NameOfLastAddedContext()
/**
 * This function gets the name of the last added context.
 */
    {
    if (iContexts.IsEmpty())
        return NULL;
    CGprsContextEntry* contextEntry = iContexts.Last();
    __ASSERT_ALWAYS(contextEntry,Panic(EPacketContextDoesNotExist));
    return (contextEntry->iContext->ContextName());
    }


RPacketService::TStatus CGprs::Status()
/**
 * This function returns the GPRS status.
 */
    {
    return iPhoneGlobals->iGprsStatus;
    }


void CGprs::SetStatus(RPacketService::TStatus aStatus)
/**
 * This function sets the GPRS status.
 * @param aStatus Status to set.
 */

    {
    iPhoneGlobals->iGprsStatus = aStatus;
    }

RPacketService::TRegistrationStatus CGprs::RegistrationStatus()
/**
 * This function returns the GPRS network registration status.
 */
    {
    return iPhoneGlobals->iGprsRegistrationStatus;
    }


void CGprs::SetRegistrationStatus(RPacketService::TRegistrationStatus aStatus)
/**
 * This function sets the GPRS registration status.
 * @param aStatus Status to set.
 */
    {
    iPhoneGlobals->iGprsRegistrationStatus = aStatus;
    }


TInt CGprs::NumberOfContexts()
/**
 * This function returns the number of contexts.
 */
    {
    TInt count=0;
    TDblQueIter<CGprsContextEntry> iter(iContexts);
    while (iter++)
        {
        count++;
        }
    return count;
    }


void CGprs::GenerateNewContextNameAndCid(TDes& aName, TInt& aCid)
/**
 * This function generates a unique context name and the first not taken cid number.
 * There is a 1 to 1 mapping between this Cid and the one in the phone. 
 * In order to do this we need to check all existing Cids, since they may not
 * be in conical order and there can be holes in the list(3,1,4,6,5) we need 
 * to loop around and find the lowest that we can use to create a name and a new
 * Cid.
 * @param aName New name that is generated.
 * @param aCid New context id.
 */
    {
    TInt lowestAvailableCid = 1;
    TInt count = NumberOfContexts();
    if(count != 0 )
        {
        // There can be gaps between the Cids that the tsy has, so get the
        // Cid number between them, if any. This nested for loop will find 
        // the lowest available cid there is.
        for(TInt k = 0; k < count;k++)
            {
            for(TInt i = 0; i < count;i++)
                {
                CGprsContext* context = Context(i);
                if(context->Cid() == lowestAvailableCid)
                    ++lowestAvailableCid;
                }
            }
        }
    aCid = lowestAvailableCid;
    _LIT(KGprsName, "GPRS_CONTEXT");
	aName.Zero();
    aName.Append(KGprsName);
    aName.AppendNum(lowestAvailableCid);
    }


RPacketService::TMSClass CGprs::CurrentMSClass()
/**
 * This function returns the current MS class.
 */
    {
    return iPhoneGlobals->iMSClass;
    }


void CGprs::SetCurrentMSClass(RPacketService::TMSClass aMSClass)
/**
 * This function sets the current MS class.
 *
 * @param aMSClass Current MSClass to set.
 */
    {
    iPhoneGlobals->iMSClass = aMSClass;
    }


RPacketService::TMSClass CGprs::MaxMSClass()
/**
 * This function returns the maximum MS class.
 */
    {
    return iPhoneGlobals->iMaxMSClass;
    }



void CGprs::SetContextConfigGPRS(const RPacketContext::TContextConfigGPRS* aContextConfigV01)
/**
 * This function sets the default context configuration.
 *
 * @param aContextConfigV01 Set default context configuration.
 */
    {
    iDefContextConfigGPRS.iPdpType = aContextConfigV01->iPdpType;
    iDefContextConfigGPRS.iAccessPointName.Copy(aContextConfigV01->iAccessPointName);
    iDefContextConfigGPRS.iPdpAddress.Copy(aContextConfigV01->iPdpAddress);
    iDefContextConfigGPRS.iPdpCompression = aContextConfigV01->iPdpCompression;
    iDefContextConfigGPRS.iAnonymousAccessReqd = aContextConfigV01->iAnonymousAccessReqd;
    }


void CGprs::ContextConfigGPRS(RPacketContext::TContextConfigGPRS* aContextConfigV01)
/**
 * This function gets the default context configuration.
 *
 * @param aContextConfigV01 Default context configuration to get.
 */
    {
    aContextConfigV01->iPdpType = iDefContextConfigGPRS.iPdpType;
    aContextConfigV01->iAccessPointName.Copy(iDefContextConfigGPRS.iAccessPointName);
    aContextConfigV01->iPdpAddress.Copy(iDefContextConfigGPRS.iPdpAddress);
    aContextConfigV01->iPdpCompression = iDefContextConfigGPRS.iPdpCompression;
    aContextConfigV01->iAnonymousAccessReqd = iDefContextConfigGPRS.iAnonymousAccessReqd;
    }


TInt CGprs::MaxNumberOfContexts()
/**
 * This function returns the maximum number of contexts that the phone supports.
 */

    {
    return iPhoneGlobals->iGprsMaxNumContexts;
    }


CGprsContext* CGprs::Context(TInt aCount)
/**
 * This function returns a specific context
 * @param aCount context to Retrieve in linked list.
 */
    {
    if(aCount >= NumberOfContexts())
        return NULL;
    CGprsContextEntry* contextEntry;
    TDblQueIter<CGprsContextEntry> iter(iContexts);
    iter.SetToFirst();
    for (TInt i=0;i<(aCount);i++)
        {
        iter++;
        }
    contextEntry = iter;    
    return contextEntry->iContext;
    }


TInt CGprs::NotifyContextAdded(const TTsyReqHandle aTsyReqHandle, TDes* aContextId)
    {
    LOGTEXT(_L8("CGprs::NotifyContextAdded called"));
    iPhoneGlobals->iNotificationStore->RegisterNotification(EPacketContextAdded, aTsyReqHandle, this, aContextId);
    return KErrNone;
    }


TInt CGprs::NotifyContextAddedCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
    {
    LOGTEXT(_L8("CGprs::NotifyContextAddedCancel called"));
    iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
    return KErrNone;
    }


TInt CGprs::Attach(const TTsyReqHandle aTsyReqHandle)
/**
 * Called from a client via ExtFunc.
 * This function will start the sending of a AT command(AT+CGATT=1). 
 * @param aTsyReqHandle  request handle that is completed when done with request.
 * @return 
 */
    {
    LOGTEXT(_L8("CGprs::Attach called"));
    iATGPRSAttach->ExecuteCommand(aTsyReqHandle, NULL);		
    return KErrNone;
    }


TInt CGprs::AttachCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
    {
    LOGTEXT(_L8("CGprs::AttachCancel called"));
    iATGPRSAttach->CancelCommand(aTsyReqHandle);	
    return KErrNone;
    }


TInt CGprs::Detach(const TTsyReqHandle aTsyReqHandle)
/**
 * Called from a client via ExtFunc.
 * This function will start the sending of a AT command(AT+CGATT=0). 
 * @param aTsyReqHandle  request handle that is completed when done with request.
 * @return 
 */
    {
    LOGTEXT(_L8("CGprs::Detach called"));
    iATGprsDetach->ExecuteCommand(aTsyReqHandle, NULL);	
    return KErrNone;
    }


TInt CGprs::DetachCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
    {
    LOGTEXT(_L8("CGprs::DetachCancel called"));
    iATGprsDetach->CancelCommand(aTsyReqHandle);	
    return KErrNone;
    }


TInt CGprs::GetStatus(const TTsyReqHandle aTsyReqHandle, RPacketService::TStatus* aGprsStatus)
/**
 * Called from a client via ExtFunc.
 * This function will return the state of the CGprs object
 * @param aTsyReqHandle  request handle that is completed when done with request.
 * @param  aGprsStatus   Pointer to a RPacketService::TStatus.
 */
    {
    LOGTEXT(_L8("CGprs::GetStatus called"));
    *aGprsStatus = Status();
    ReqCompleted(aTsyReqHandle,KErrNone);
    return KErrNone;
    }

 
TInt CGprs::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TStatus* aGprsStatus)
/**
 * Called from a client via ExtFunc.
 * This function will complete when EPacketStatusChanged event is triggered.
 * @param aTsyReqHandle  request handle that is completed.
 * @param  aConfig  Pointer to a RPacketService::TStatus
 */
    {
    LOGTEXT(_L8("CGprs::NotifyStatusChange called"));
    iPhoneGlobals->iNotificationStore->RegisterNotification(EPacketStatusChange, aTsyReqHandle, this, aGprsStatus);
    return KErrNone;
    }


TInt CGprs::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding notification request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
    {
    LOGTEXT(_L8("CGprs::NotifyStatusChangeCancel called"));
    iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
    return KErrNone;
    }


TInt CGprs::NotifyContextActivationRequested(const TTsyReqHandle aTsyReqHandle, RPacketContext::TProtocolType* /*aPdpType*/)
/**
 * This function is not supported.
 * @param aTsyReqHandle   request handle that is completed.
 */
    {
    LOGTEXT(_L8("CGprs::NotifyContextActivationRequested called"));
    ReqCompleted(aTsyReqHandle, KErrNotSupported); // Req22
    return KErrNone;
    }


TInt CGprs::NotifyContextActivationRequestedCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding notification request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
    {
    LOGTEXT(_L8("CGprs::NotifyContextActivationRequestedCancel called"));
    ReqCompleted(aTsyReqHandle, KErrNone); // Req23
    return KErrNone;
    }


TInt CGprs::RejectActivationRequest(const TTsyReqHandle aTsyReqHandle)
/**
 * This function is not supported.
 * @param aTsyReqHandle   request handle that is completed.
 */
    {
    LOGTEXT(_L8("CGprs::RejectActivationRequest called"));
    ReqCompleted(aTsyReqHandle, KErrNotSupported); // Req24
    return KErrNone;
    }


TInt CGprs::RejectActivationRequestCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
    {
    LOGTEXT(_L8("CGprs::RejectActivationRequestCancel called"));
    ReqCompleted(aTsyReqHandle, KErrCancel); // Req25
    return KErrNone;
    }


TInt CGprs::EnumerateContexts(const TTsyReqHandle aTsyReqHandle, TInt* aCount, TInt* aMaxAllowed)
/**
 * Called from a client via ExtFunc.
 * This function will return the current amount of contexts that the TSY has loaded and the
 * maximum amount of contexts that the phone support.
 * @param aTsyReqHandle  request handle that is completed.
 * @param  aCount Number of CGprsContext that the CGprs object has in its list.
 * @param  aMaxAllowed The largest context number, this is read from the phone.
 */
    {
    LOGTEXT(_L8("CGprs::EnumerateContexts called"));
    *aCount = NumberOfContexts();
    *aMaxAllowed = MaxNumberOfContexts();
    ReqCompleted(aTsyReqHandle,KErrNone);
    return KErrNone;
    }


TInt CGprs::GetContextInfo(const TTsyReqHandle aTsyReqHandle, TInt aIndex, RPacketService::TContextInfo* aInfo)
/**
 * Called from a client via ExtFunc.
 * Returns the context info of aIndex in context list.
 * @param aTsyReqHandle  request handle that is completed.
 * @param  aIndex Context number
 * @param  aInfo  Pointer to a RPacketService::TContextInfo
 */
    {
    LOGTEXT2(_L8("CGprs::GetContextInfo called with aIndex=%d"),aIndex);

	TInt ret(KErrNone);
	if(aIndex>=0 &&	aIndex<=MaxNumberOfContexts())
		{
		TDblQueIter<CGprsContextEntry> iter(iContexts);
		iter.SetToFirst();
		for (TInt i=0;i!=aIndex;++i)
			iter++;
	    
	    CGprsContextEntry* contextEntry=iter;
		if (contextEntry)
			*aInfo = *contextEntry->iContext->ContextInfo();
		else
			ret=KErrArgument;
		}
	else
		ret=KErrArgument;

	ReqCompleted(aTsyReqHandle,ret);
    return KErrNone;
    }


TInt CGprs::GetContextInfoCancel(const TTsyReqHandle aTsyReqHandle) 
/**
 * Cancels a outstanding request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
    {
    LOGTEXT(_L8("CGprs::GetContextInfoCancel called"));
    ReqCompleted(aTsyReqHandle, KErrCancel);
    return KErrNone;
    }


TInt CGprs::GetNtwkRegStatus(const TTsyReqHandle aTsyReqHandle, RPacketService::TRegistrationStatus* aRegistrationStatus)
/**
 * @param aTsyReqHandle request handle that is completed.
 */
	{
	LOGTEXT(_L8("CGprs::GetNtwkRegStatus called"));
	if (iPhoneGlobals->iPhoneStatus.iDataPortLoaned)
		{
		ReqCompleted(aTsyReqHandle, KErrNone);
		}
	else
		{
		iATGprsNtwkRegStatus->ExecuteCommand(aTsyReqHandle,aRegistrationStatus);
		}
	return KErrNone;
	}

TInt CGprs::GetNtwkRegStatusCancel(const TTsyReqHandle aTsyReqHandle) 
/**
 * Cancels a outstanding request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
  {
  LOGTEXT(_L8("CGprs::GetNtwkRegStatusCancel called"));
  iATGprsNtwkRegStatus->CancelCommand(aTsyReqHandle);			
  return KErrNone;
  }


TInt CGprs::NotifyChangeOfNtwkRegStatus(const TTsyReqHandle aTsyReqHandle, RPacketService::TRegistrationStatus* aRegistrationStatus)
/**
 * This function is not supported.
 * @param aTsyReqHandle   request handle that is completed.
 */
    {
    LOGTEXT(_L8("CGprs::NotifyChangeOfNtwkRegStatus called"));		
	if(iPhoneGlobals->iModemSupportsCGREGNotification)
		{
		// Make sure AT class watching for unsolicited registration result code is enabled
		iATGprsNtwkRegStatusChange->Enable();
		iPhoneGlobals->iNotificationStore->RegisterNotification(EPacketNtwkRegStatusChange,aTsyReqHandle,this,aRegistrationStatus);
		return KErrNone;
		}

    LOGTEXT(_L8("CGprs::NotifyChangeOfNtwkRegStatus returning KErrNotSupported"));		
	return KErrNotSupported;		// Etel will complete the client request for us
	}


TInt CGprs::NotifyChangeOfNtwkRegStatusCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
    {
    LOGTEXT(_L8("CGprs::NotifyChangeOfNtwkRegStatusCancel called"));
    iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
    return KErrNone;
    }

TInt CGprs::GetMSClass(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* aCurrentClass, RPacketService::TMSClass* aMaxClass)
/**
 * Called from a client via ExtFunc.
 * This function returns the current RPacketService::TMSClass. It also returns the maximum class that the 
 * phone supports.
 * @param aTsyReqHandle  request handle that is completed.
 * @param  aCurrentClass  pointer to a  RPacketService::TMSClass
 * @param  aMaxClass      pointer to a  RPacketService::TMSClass
 */
    {
    LOGTEXT(_L8("CGprs::GetMSClass called"));
    *aMaxClass = MaxMSClass();
    *aCurrentClass = CurrentMSClass();
    ReqCompleted(aTsyReqHandle, KErrNone);
    return KErrNone;
    }


TInt CGprs::GetMSClassCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
    {
    LOGTEXT(_L8("CGprs::GetMSClassCancel called"));
    ReqCompleted(aTsyReqHandle, KErrCancel);
    return KErrNone;
    }


TInt CGprs::SetMSClass(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* aClass)
/**
 * Called from a client via ExtFunc.
 * This function will start the sending of a AT command to the phone(AT+CGCLASS).
 * @param aTsyReqHandle  request handle that is completed.
 * @param  aClass  pointer to a  RPacketService::TMSClass
 */
    {
    LOGTEXT(_L8("CGprs::SetMSClass called"));
    iATGprsClass->ExecuteCommand(aTsyReqHandle, aClass);		
    return KErrNone;
    }


TInt CGprs::SetMSClassCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
    {
    LOGTEXT(_L8("CGprs::SetMSClassCancel called"));
    iATGprsClass->CancelCommand(aTsyReqHandle);			
    return KErrNone;
    }   


TInt CGprs::NotifyMSClassChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* /*aNewClass*/)
/**
 * This function is not supported.
 * @param aTsyReqHandle   request handle that is completed.
 */
    {
    LOGTEXT(_L8("CGprs::NotifyMSClassChange called"));
    ReqCompleted(aTsyReqHandle, KErrNotSupported); // Req26
    return KErrNone;
    }


TInt CGprs::NotifyMSClassChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
    {
    LOGTEXT(_L8("CGprs::NotifyMSClassChangeCancel called"));
    ReqCompleted(aTsyReqHandle, KErrCancel); // Req27
    return KErrNone;
    }
   

TInt CGprs::GetStaticCaps(const TTsyReqHandle aTsyReqHandle, TUint* aCaps, RPacketContext::TProtocolType* aPdpType)
/**
 * Called from a client via ExtFunc.
 * This function returns the static caps that from the phone.
 * Note that in this function we validate the value of aPdpType.
 * @param aTsyReqHandle request handle that is completed. 
 * @param  aCaps     Pointer to a RPacketService::TStaticCaps  
 * @param  aPdpType  Pointer to a TPdpType
 */
    {
    LOGTEXT(_L8("CGprs::GetStaticCaps called"));
    if (*aPdpType!=RPacketContext::EPdpTypeIPv4) 
        {
        LOGTEXT(_L8("CGprs::GetStaticCaps aPdpType is not EPdpTypeIPv4, completeing with KErrNotSupported"));
				ReqCompleted(aTsyReqHandle,KErrNotSupported);
        return KErrNone;
        }
		//
		// Static caps are hardcoded according to the MMTSY design document.
		*aCaps=RPacketService::KCapsSetDefaultContextSupported;
		*aCaps|=RPacketService::KCapsMSClassSupported;

    ReqCompleted(aTsyReqHandle,KErrNone);
    return KErrNone;
    }


TInt CGprs::GetDynamicCaps(const TTsyReqHandle aTsyReqHandle, RPacketService::TDynamicCapsFlags* /*aCaps*/)
/**
 * This function is not supported.
 * @param aTsyReqHandle   request handle that is completed.
 */
    {
    LOGTEXT(_L8("CGprs::GetDynamicCaps called"));
    ReqCompleted(aTsyReqHandle, KErrNotSupported); // Req28
    return KErrNone;
    }


TInt CGprs::NotifyDynamicCapsChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TDynamicCapsFlags* /*aCaps*/)
/**
 * This function is not supported.
 * @param aTsyReqHandle   request handle that is completed.
 */
    {
    LOGTEXT(_L8("CGprs::NotifyDynamicCapsChange called"));
    ReqCompleted(aTsyReqHandle, KErrNotSupported); // Req29
    return KErrNone;
    }


TInt CGprs::NotifyDynamicCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
    {
    LOGTEXT(_L8("CGprs::NotifyDynamicCapsChangeCancel called"));
    ReqCompleted(aTsyReqHandle, KErrCancel); // Req30
    return KErrNone;
    }


TInt CGprs::SetAttachMode(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* /*aMode*/)
/**
 * This function is not supported.
 * @param aTsyReqHandle   request handle that is completed.
 */
    {
    LOGTEXT(_L8("CGprs::SetAttachMode called"));
    ReqCompleted(aTsyReqHandle, KErrNotSupported); // Req40
    return KErrNone;
    }


TInt CGprs::GetAttachMode(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* /*aMode*/)
/**
 * This function is not supported.
 * @param aTsyReqHandle   request handle that is completed.
 */
    {
    LOGTEXT(_L8("CGprs::GetAttachMode called"));
    ReqCompleted(aTsyReqHandle, KErrNotSupported); // Req41
    return KErrNone;
    }


TInt CGprs::SetDefaultContextParams(const TTsyReqHandle aTsyReqHandle, const TDesC8* aPckg)
/**
 * Called from a client via ExtFunc.
 * This function will set the default parameters for a context.
 * These parameters are used when a new context is created.
 * @param aTsyReqHandle  request handle that is completed.
 * @param  aPckg Pointer to a RPacketContext::TContextConfigPckg
 */

    {
    LOGTEXT(_L8("CGprs::SetDefaultContextParams called"));
	TPckg<RPacketContext::TContextConfigGPRS>* defaultContextV1Pckg = (TPckg<RPacketContext::TContextConfigGPRS>*) aPckg;
	RPacketContext::TContextConfigGPRS& defaultContextV1 = (*defaultContextV1Pckg)();
	SetContextConfigGPRS(&defaultContextV1);
    ReqCompleted(aTsyReqHandle,KErrNone);
    return KErrNone;
    }


TInt CGprs::GetDefaultContextParams(const TTsyReqHandle aTsyReqHandle, TDes8* aPckg)
/**
 * Called from a client via ExtFunc.
 * This function will get the default parameters for a context.
 * @param aTsyReqHandle  request handle that is completed.
 * @param  aPckg Pointer to a RPacketContext::TContextConfigPckg
 */

    {
    LOGTEXT(_L8("CGprs::GetDefaultContextParams called"));
	TPckg<RPacketContext::TContextConfigGPRS>* defaultContextV1Pckg = (TPckg<RPacketContext::TContextConfigGPRS>*) aPckg;
	RPacketContext::TContextConfigGPRS& defaultContextV1 = (*defaultContextV1Pckg)();
    ContextConfigGPRS(&defaultContextV1);
    ReqCompleted(aTsyReqHandle,KErrNone);
    return KErrNone;
    }


//
// CGprsContextEntry class
//
CGprsContextEntry::CGprsContextEntry(CGprsContext* aContext)
: iContext(aContext)
/**
 * Constructor
 * @param aContext Context to add to entry
 */
    {}


CGprsContextEntry::~CGprsContextEntry()
/**
 * Destructor
 */
    {}


void CGprsContextEntry::Deque()
/**
 * Remove entry from TDblQue link
 */
    {
    iLink.Deque();
    iLink.iPrev=iLink.iNext=NULL;
    }
