// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ATGprsContextActivate.H"
#include "mSLOGGER.H"
#include <pcktcs.h>
#include "ATGprsConfig.h"
#include "ATIO.H"
#include "TSYCONFG.H"
#include "NOTIFY.H"
#include "ATSTD.H"
#include "ATGprsContextDeactivate.H"
#include "atgprscontextdelete.h"		// for CATGprsContextDelete class
#include "gprsqos.h"

#include <commsdattypesv1_1.h>

/**
 * @file
 * This file implements the CGprsContext class. Used to implement the RPacketContext API.
 */

CGprs* CGprsContext::Parent()
	{return iParent;};

CGprsContext* CGprsContext::NewL(CGprs* aParent,CATIO* aIo, CATInit* aInit, CPhoneGlobals* aPhoneGlobals, const TName& aName, TInt aCid, RPacketContext::TContextConfigGPRS* aContextConfigGPRS)
/**
 * Standard 2 phase constructor.
 *
 * @param aTelObject pointer to parent.
 * @param aIo pointer to communication object.
 * @param aInit pointer to AT phone init object.
 * @param aPhoneGlobals pointer to phone global wide states.
 * @param aName new name for the context which this object will represent
 * @param aCid context ID of the context which this object will represent
 * @param aContextConfigGPRS configuration of the context which this object will represent
 */ 
	{
	CGprsContext* gprs=new(ELeave) CGprsContext(aParent, aIo, aInit, aPhoneGlobals, aCid);
	CleanupStack::PushL(gprs);
	gprs->ConstructL(aName, aContextConfigGPRS);
	CleanupStack::Pop();
	return gprs;
	}


CGprsContext::CGprsContext(CGprs* aParent, CATIO* aIo, CATInit*	aInit, CPhoneGlobals* aPhoneGlobals, TInt aCid) 
	: CSubSessionExtBase(), iCid(aCid), iParent(aParent), iIo(aIo), iInit(aInit), iPhoneGlobals(aPhoneGlobals)
/**
 * Constructor
 *
 * @param aParent pointer to parent.
 * @param aIo pointer to communication object.
 * @param aInit pointer to AT phone init object.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	}

 
void CGprsContext::ConstructL(const TName& aName, RPacketContext::TContextConfigGPRS* aContextConfigGPRS)
/**
 * This function creates all objects on the heap that can leave.
 * @param aName Name of context.
 * @param aContextConfigV1 Default contest config.
 */
	{
	LOGTEXT(_L8("CGprsContext: Entered constructor"));
	iConfigGPRS = *aContextConfigGPRS;
	iInfo.iName.Copy(aName);
	iATGprsContextActivate			= CATGprsContextActivate::NewL(iCid, iIo, this, iInit, iPhoneGlobals); 
	iATGPRSSetConfig				= CATGPRSSetConfig::NewL(iCid, iIo, this, iInit, iPhoneGlobals);
	iATGPRSGetConfig				= CATGPRSGetConfig::NewL(iCid, iIo, this, iInit, iPhoneGlobals);
	iATGprsContextDeactivate		= CATGprsContextDeactivate::NewL(iIo, this, iInit, iPhoneGlobals); 
	iATGprsContextDelete		    = CATGprsContextDelete::NewL(iIo, this, iInit, iPhoneGlobals); 
	}


CGprsContext::~CGprsContext()
/**
 * Destructor
 */
	{
	LOGTEXT(_L8("CGprsContext: Entered destructor"));
	iParent->RemoveContext(this);
	iPhoneGlobals->iNotificationStore->RemoveClientFromLastEvents(this);
	delete iATGprsContextDelete;
	delete iATGprsContextActivate;
	delete iATGPRSSetConfig;
	delete iATGPRSGetConfig;
	delete iATGprsContextDeactivate;
	}


void CGprsContext::Init()
/**
 * This is not used. Init is done from the CPhone class.
 */
	{
	LOGTEXT(_L8("CGprsContext: init()"));
	}


CTelObject* CGprsContext::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * This function is not used.
 */
	{
	LOGTEXT(_L8("CGprsContext::OpenNewObjectByNameL called"));
	User::Leave(KErrNotSupported);
	return NULL;
	}


CTelObject* CGprsContext::OpenNewObjectL(TDes& aNewName)
/**
 * This function returns a pointer to a new QoS profile objecft (CGprsQoS)
 */
	{
	GenerateNewProfileName(aNewName); // create a unique name for this qos profile
#if defined __LOGDEB__ 
    TBuf8<40> tmpLog;
    tmpLog.Copy(aNewName);
    LOGTEXT2(_L8("CGprs::OpenNewObjectL(), %S"), &tmpLog);
#endif
	CGprsQoS* qos = CGprsQoS::NewL(this, iCid, iIo, iInit, iPhoneGlobals, aNewName);
	return qos;
	} 


CTelObject::TReqMode CGprsContext::ReqModeL(const TInt aIpc)
/**
 * This function is called from the framework and checks the flow control for a IPC call.
 * @param aIpc IPC call from client.
 */
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
		case EPacketContextActivate:
		case EPacketContextDeactivate:
		case EPacketContextSetConfig:
		case EPacketContextLoanCommPort:
			ret = KReqModeFlowControlObeyed;
			break;

		case EPacketContextDelete:
		case EPacketContextGetStatus:
		case EPacketContextGetLastErrorCause:
		case EPacketContextGetConfig:
		case EPacketContextGetDataVolumeTransferred:
		case EPacketContextRecoverCommPort:
			break;

		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
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
	

TInt CGprsContext::RegisterNotification(const TInt aIpc)
/**
 * This function is called from the framework.
 *
 * @param  aIpc IPC call from client.
 */
	{
	LOGTEXT2(_L8("CGprsContext::RegisterNotification called with aIpc=%d"),aIpc);
	switch (aIpc)
		{
		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
		case EPacketContextNotifyDataTransferred:
			return KErrNone;
		default:
			// Unknown or invalid IPC
            LOGTEXT2(_L8("CGprsContext::RegisterNotification Error Unknown IPC, aIpc=%d"),aIpc);
			return KErrNotSupported;
		}
	}


TInt CGprsContext::DeregisterNotification(const TInt aIpc)
/**
 * This function is called when the there is no client listening to a notification anymore.
 *
 * @param aIpc IPC call from client.
 */
	{
	LOGTEXT2(_L8("CGprs::DeregisterNotification called with aIpc=%d"),aIpc);
	switch (aIpc)
		{
		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
		case EPacketContextNotifyDataTransferred:
			return KErrNone;
		default:
			// Unknown or invalid IPC
            LOGTEXT2(_L8("CGprsContext::DeregisterNotification Error Unknown IPC, aIpc=%d"),aIpc);
			return KErrNotSupported;
		}
	}


TInt CGprsContext::NumberOfSlotsL(const TInt aIpc)
	{
	TInt numberOfSlots=0;
	switch (aIpc)
		{
		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
		case EPacketContextNotifyDataTransferred:
			LOGTEXT(_L8("CGprsContext: Registered with 5 slots"));
			numberOfSlots=5;
			break;
		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CGprsContext: Number of Slots error, unknown IPC"));
			User::Leave(KErrNotSupported);
			break;
		}  
	return numberOfSlots;

	}


TInt CGprsContext::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage)
/**
 * This function is called from the framework.
 *
 * @param aTsyReqHandle handle to client.
 * @param aIpc IPC call from client.
 * @param aPackage agruments with the IPC call.
 */
	{
	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();
	
	if(ShouldNotProcessIpcCall(aIpc))
		{// If the context is in state deleted these calls should not be processed.
		ReqCompleted(aTsyReqHandle, KErrAccessDenied);
		return KErrNone;
		}
	if(ShouldNotProcessIpcCallWhenComportIsLoaned(aIpc))
		{// If the comport is loaned out ipcs that generate a at command shall be returned with KErrAccesssDenied
		ReqCompleted(aTsyReqHandle, KErrAccessDenied);
		return KErrNone;
		}
	switch (aIpc)
		{
		case EPacketContextSetConfig:
			return SetConfig(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextGetConfig:
			return GetConfig(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextNotifyConfigChanged:
			return NotifyConfigChanged(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextActivate:
			return Activate(aTsyReqHandle);
		case EPacketContextDeactivate:
			return Deactivate(aTsyReqHandle);
		case EPacketContextDelete:
			return Delete(aTsyReqHandle);
		case EPacketContextLoanCommPort:
			return LoanCommPort(aTsyReqHandle,
			REINTERPRET_CAST(RCall::TCommPort*, dataPtr));
		case EPacketContextRecoverCommPort:
			return RecoverCommPort(aTsyReqHandle);
		case EPacketContextGetStatus:
			return GetStatus(aTsyReqHandle,
			REINTERPRET_CAST(RPacketContext::TContextStatus*, dataPtr));
		case EPacketContextNotifyStatusChange:
			return NotifyStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RPacketContext::TContextStatus*, dataPtr));
		case EPacketContextGetDataVolumeTransferred:
			return GetDataVolumeTransferred(aTsyReqHandle,
			REINTERPRET_CAST(RPacketContext::TDataVolume*, dataPtr));
		case EPacketContextNotifyDataTransferred:
			return NotifyDataTransferred(aTsyReqHandle,
			REINTERPRET_CAST(RPacketContext::TDataVolume*, dataPtr),
			REINTERPRET_CAST(RPacketContext::TNotifyDataTransferredRequest*, dataPtr2));
		case EPacketContextGetLastErrorCause:
			return GetLastErrorCause(aTsyReqHandle,
			REINTERPRET_CAST(TInt*, dataPtr));
		default:
			return KErrNotSupported;
		}
	}


RPacketService::TContextInfo* CGprsContext::ContextInfo()
/**
 * This function returns the current context info.
 */
	{
	return &iInfo;
	}


TInt CGprsContext::Cid()
/**
 * This function returns the context id.
 */
	{
	return iCid;
	}


void CGprsContext::ContextInfo(RPacketService::TContextInfo* aInfo)
/**
 * This function returns the current context info.
 * @param aInfo context info that is returned.
 */
	{
	aInfo->iName.Copy(iInfo.iName);
	aInfo->iStatus = iInfo.iStatus;
	}


void CGprsContext::SetContextInfo(const RPacketService::TContextInfo* aContextInfo)
/**
 * This function sets the current context info.
 * @param aContextInfo context into to set.
 */
	{
	iInfo.iName.Copy(aContextInfo->iName);
	iInfo.iStatus = aContextInfo->iStatus;
	}


TDes* CGprsContext::ContextName()
/**
 * This function returns the context name.
 */

	{
	return &iInfo.iName;
	}


RPacketContext::TContextConfigGPRS* CGprsContext::Config()
/**
 * This function returns the context configuration.
 *
 */

	{
	return &iConfigGPRS;
	}


void CGprsContext::SetConfig(const RPacketContext::TContextConfigGPRS* aContextConfigGPRS)
/**
 * This function sets the context configuration.
 * @param iContextConfigV10 Configuration to set.
 */

	{
	iConfigGPRS = *aContextConfigGPRS;
	}	


void CGprsContext::GenerateNewProfileName(TDes& aName)
/**
 * This function generates a unique qos profile name and a free qosId number.
 *
 * @param aName New name of the profile that is generated.
 */
    {
    _LIT(KGprsName, "GPRS_QOS");
    aName.Append(KGprsName);
    aName.AppendNum(++iGprsQoSProfileCount);
    }

TInt CGprsContext::CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle)
/**
 * This function is called from the framework and cancels a outstanding request.
 * @param aIpc The IPC to cancel.
 * @param aTsyReqHandle the client handle to cancel.
 */
	{
	LOGTEXT(_L8("CGprsATContextTsy: - CancelService called"));
	switch (aIpc)
		{
		case EPacketContextSetConfig:
			return SetConfigCancel(aTsyReqHandle);
		case EPacketContextGetConfig:
			return GetConfigCancel(aTsyReqHandle);
		case EPacketContextNotifyConfigChanged:
			return NotifyConfigChangedCancel(aTsyReqHandle);
		case EPacketContextActivate:
			return ActivateCancel(aTsyReqHandle);
		case EPacketContextDeactivate:
			return DeactivateCancel(aTsyReqHandle);
		case EPacketContextDelete:
			return DeleteCancel(aTsyReqHandle);
		case EPacketContextLoanCommPort:
			return LoanCommPortCancel(aTsyReqHandle);
		case EPacketContextRecoverCommPort:
			return RecoverCommPortCancel(aTsyReqHandle);
		case EPacketContextNotifyStatusChange:
			return NotifyStatusChangeCancel(aTsyReqHandle);
		case EPacketContextNotifyDataTransferred:
			return NotifyDataTransferredCancel(aTsyReqHandle);
		default:
			return KErrNotSupported; 
		}	 
	}


 
TInt CGprsContext::SetConfig(const TTsyReqHandle aTsyReqHandle,const TDesC8* aConfig)
/**
 * Called from a client via ExtFunc.
 * This function will start a chain of AT commands to the phone(AT+CGQMIN and AT+CGQREQ).
 * 
 * @param aTsyReqHandle request handle that is completed when done with request.
 * @param  aConfig Pointer to a RPacketContext::TContextConfigPckg.
 */
	{
	LOGTEXT(_L8("CGprsContext::SetConfig called"));
	iATGPRSSetConfig->ExecuteCommand(aTsyReqHandle, (TAny*)aConfig);
	return KErrNone;
	}


TInt CGprsContext::SetConfigCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * 
 * @param aTsyReqHandle  The handle to cancel
 */
	{
	LOGTEXT(_L8("CGprsContext::SetConfigCancel called"));
	iATGPRSSetConfig->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}


TInt CGprsContext::GetConfig(const TTsyReqHandle aTsyReqHandle,TDes8* aConfig)
/**
 * Called from a client via ExtFunc.
 * This function will start a chain of AT commands to the phone(AT+CGQREQ?, AT+CGDCONT? and AT+CGQMIN?)
 * and query the settings in the phone.
 * 
 * @param aTsyReqHandle request handle that is completed when done with request.
 * @param  aConfig Pointer to a RPacketContext::TContextConfigPckg.
 */
	{
	LOGTEXT(_L8("CGprsContext::GetConfig called"));
	iATGPRSGetConfig->ExecuteCommand(aTsyReqHandle, aConfig);
	return KErrNone;
	}


TInt CGprsContext::GetConfigCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * 
 * @param aTsyReqHandle  The handle to cancel
 */
	{
	LOGTEXT(_L8("CGprsContext::GetConfigCancel called"));
	iATGPRSGetConfig->CancelCommand(aTsyReqHandle);		
	return KErrNone;
	}



TInt CGprsContext::NotifyConfigChanged(const TTsyReqHandle aTsyReqHandle, TDes8* aConfig)
/**
 * Called from a client via ExtFunc.
 * This function will complete when EPacketContextConfigChange event is triggered.
 *
 * @param aTsyReqHandle  request handle that is completed.
 * @param  aConfig  Pointer to a RPacketContext::TContextConfigPckg      
 */
	{
	LOGTEXT(_L8("CGprsContext::NotifyConfigChanged called"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(EPacketContextConfigChange, aTsyReqHandle, this, aConfig);
	return KErrNone;
	}


TInt CGprsContext::NotifyConfigChangedCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
	{
	LOGTEXT(_L8("CGprsContext::NotifyConfigChangedCancel called"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}


TInt CGprsContext::Deactivate(const TTsyReqHandle aTsyReqHandle)
/**
 * Called from a client via ExtFunc.
 * This function will start the sending of a AT command(ATD*99# or ATD*99***1#).
 * @param aTsyReqHandle  request handle that is completed when done with request.
 */
	{
	LOGTEXT(_L8("CGprsContext::Deactivate called"));
	iIo->DropDtr();		// Drops DTR	
	RPacketService::TContextInfo iContextInfo;
	ContextInfo(&iContextInfo);
	iContextInfo.iStatus = RPacketContext::EStatusDeactivating;
	SetContextInfo(&iContextInfo);
	iPhoneGlobals->iNotificationStore->CheckNotification(this, EPacketContextStatusChanged);
	
	// Can not call ExecuteCommand as we do not want the 
	// iATSetToOnlineCommandMode->StartEscapeSequence() to be called.
	iATGprsContextDeactivate->Start(aTsyReqHandle, NULL);
	return KErrNone;
	}


TInt CGprsContext::DeactivateCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * 
 * @param aTsyReqHandle  The handle to cancel
 */
	{
	LOGTEXT(_L8("CGprsContext::DeactivateCancel called"));
	iATGprsContextDeactivate->CancelCommand( aTsyReqHandle);
	return KErrNone;
	}


TInt CGprsContext::Activate(const TTsyReqHandle aTsyReqHandle)
/**
 * Called from a client via ExtFunc.
 * This function will start the sending of a AT command(AT+CGATT=1).
 *
 * @param aTsyReqHandle  request handle that is completed when done with request.
 */

	{
	LOGTEXT(_L8("CGprsContext::Activate called"));
	RPacketService::TContextInfo iContextInfo;
	iContextInfo = *ContextInfo();
	iContextInfo.iStatus = RPacketContext::EStatusActivating;
	SetContextInfo(&iContextInfo);
	iPhoneGlobals->iNotificationStore->CheckNotification(this, EPacketContextStatusChanged);
	iATGprsContextActivate->ExecuteCommand(aTsyReqHandle,NULL);
	return KErrNone;
	}


TInt CGprsContext::ActivateCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * 
 * @param aTsyReqHandle  The handle to cancel
 */
	{
	LOGTEXT(_L8("CGprsContext::ActivateCancel called"));
	iATGprsContextActivate->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}


TInt CGprsContext::Delete(const TTsyReqHandle aTsyReqHandle)
/**
 * Called from a client via ExtFunc.
 * This function will set a context as deleted, thus rendering it useless.
 * @param aTsyReqHandle  request handle that is completed when done with request.
 */
	{
	LOGTEXT(_L8("CGprsContext::Delete called"));
	if(iInfo.iStatus!=RPacketContext::EStatusUnknown && iInfo.iStatus!=RPacketContext::EStatusInactive)
		{
		//
		// Complete with KErrGeneral as this context is not in a state
		// which will allow a deletion
		ReqCompleted(aTsyReqHandle, KErrGeneral);
		}
	else
		{
		//
		// Start an AT command state machine which 
		// will delete the context from the phone
		iATGprsContextDelete->ExecuteCommand(aTsyReqHandle,&iCid);
		}
	return KErrNone;
	}


TInt CGprsContext::DeleteCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * @param aTsyReqHandle  The handle to cancel
 */
	{
	LOGTEXT(_L8("CGprsContext::DeleteCancel called"));
	iATGprsContextDelete->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}


TInt CGprsContext::LoanCommPort(const TTsyReqHandle aTsyReqHandle,RCall::TCommPort* aDataPort)
/**
 * Called from a client via ExtFunc.
 * This function will stop the TSY from using the comport and loan it to the client.
 *
 * @param aTsyReqHandle  request handle that is completed when done with request.
 * @param  aDataPort      Pointer to a RCall::TCommPort
 */
	{
	LOGTEXT(_L8("CGprsContext::LoanCommPort called"));

	// Check that this context is active
	if(ContextInfo()->iStatus != RPacketContext::EStatusActive)
		{
		ReqCompleted(aTsyReqHandle,KErrNotReady);
		return KErrNone;
		}

	// Check we have not already loaned the port
	if(iPhoneGlobals->iPhoneStatus.iDataPortLoaned)
		{
		ReqCompleted(aTsyReqHandle,KErrEtelPortAlreadyLoaned);
		return KErrNone;
		}

	TFileName csy;
	TName port;
	TInt ret=iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameCsyName),csy);
	if (ret!=KErrNone)
		{
		ReqCompleted(aTsyReqHandle, ret);
		return KErrNone;
		}
	ret=iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNamePortName),port);
	if (ret!=KErrNone)
		{
		ReqCompleted(aTsyReqHandle, ret);
		return KErrNone;
		}
	aDataPort->iCsy.Copy(csy);
	aDataPort->iPort.Copy(port);

	iPhoneGlobals->iPhoneStatus.iDataPortLoaned = ETrue;
	iPhoneGlobals->iNotificationStore->CheckNotification(this,EDataPortLoaned);

	// Cancel all reads and writes on the serial port
	iIo->Cancel();
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}


TInt CGprsContext::LoanCommPortCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * 
 * @param aTsyReqHandle  The handle to cancel
 */
	{
	LOGTEXT(_L8("CGprsContext::LoanCommPortCancel called"));
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


TInt CGprsContext::RecoverCommPort(const TTsyReqHandle aTsyReqHandle)
/**
 * Called from a client via ExtFunc.
 * This function will enable the TSY to use the comport again.
 *
 * @param aTsyReqHandle  request handle that is completed when done with request.
 * @param  aDataPort      Pointer to a RCall::TCommPort
 */
	{
	LOGTEXT(_L8("CGprsContext::RecoverCommPort called"));
	if(iPhoneGlobals->iPhoneStatus.iDataPortLoaned)
		{
		iIo->Read();				// Queue a read again on the comm port
		iPhoneGlobals->iPhoneStatus.iDataPortLoaned = EFalse;
		LOGTEXT2(_L8("Comm signals : %x (hex)"),iIo->Signals());
		LOGTEXT2(_L8("Comm signals after ResetBuffers : %x (hex)"),iIo->Signals());
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrEtelPortNotLoanedToClient);
		return KErrNone;
		}
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}


TInt CGprsContext::RecoverCommPortCancel(const TTsyReqHandle aTsyReqHandle)	
/**
 * Cancels a outstanding request. Called via CancelService.
 * 
 * @param aTsyReqHandle  The handle to cancel
 */
	{
	LOGTEXT(_L8("CGprsContext::RecoverCommPortCancel called"));
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


TInt CGprsContext::GetStatus(const TTsyReqHandle aTsyReqHandle,RPacketContext::TContextStatus* aContextStatus)
/**
 * Called from a client via ExtFunc.
 * This function will return the current state of the CGprsContext object.
 *
 * @param aTsyReqHandle    request handle that is completed when done with request.
 * @param  aContextStatus  Pointer to a RPacketContext::TContextStatus.
 */
	{
	LOGTEXT(_L8("CGprsContext::GetStatus called"));
	*aContextStatus = ContextInfo()->iStatus;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}


TInt CGprsContext::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RPacketContext::TContextStatus* aContextStatus)
/**
 * Called from a client via ExtFunc.
 * This function will complete when EPacketContextStatusChanged event is triggered.
 *
 * @param aTsyReqHandle  request handle that is completed.
 * @param  aConfig  Pointer to a RPacketContext::TContextStatus    
 */
	{
	LOGTEXT(_L8("CGprsContext::NotifyStatusChange called"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(EPacketContextStatusChange, aTsyReqHandle, this, aContextStatus);
	return KErrNone;
	}


TInt CGprsContext::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * 
 * @param aTsyReqHandle  The handle to cancel
 */
	{
	LOGTEXT(_L8("CGprsContext::NotifyStatusChangeCancel called"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}


TInt CGprsContext::GetDataVolumeTransferred(const TTsyReqHandle aTsyReqHandle,RPacketContext::TDataVolume* /*aVolume*/)
/**
 * This function is not supported.
 * 
 * @param aTsyReqHandle   request handle that is completed.
 */
	{
	LOGTEXT(_L8("CGprsContext::GetDataVolumeTransferred called"));
	ReqCompleted(aTsyReqHandle, KErrNotSupported); // Req67
	return KErrNone;
	}


TInt CGprsContext::NotifyDataTransferred(const TTsyReqHandle aTsyReqHandle, RPacketContext::TDataVolume* /*aVolume*/, RPacketContext::TNotifyDataTransferredRequest* /*aData*/)
/**
 * This function is not supported.
 * 
 * @param aTsyReqHandle   request handle that is completed.
 */
	{
	LOGTEXT(_L8("CGprsContext::NotifyStatusChange called"));
	ReqCompleted(aTsyReqHandle, KErrNotSupported); // Req68
	return KErrNone;
	}


TInt CGprsContext::NotifyDataTransferredCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding notification. Called via CancelService.
 * 
 * @param aTsyReqHandle  The handle to cancel
 */
	{
	LOGTEXT(_L8("CGprsContext::NotifyDataTransferredCancel called"));
	ReqCompleted(aTsyReqHandle, KErrCancel); // Req69
	return KErrNone;
	}


TInt CGprsContext::GetLastErrorCause(const TTsyReqHandle aTsyReqHandle,TInt* /*aError*/)
/**
 * This function is not supported.
 * 
 * @param aTsyReqHandle   request handle that is completed.
 */
	{
	LOGTEXT(_L8("CGprsContext::GetLastErrorCause called"));
	ReqCompleted(aTsyReqHandle,KErrNotSupported); // Req70
	return KErrNone;
	}


TBool CGprsContext::ShouldNotProcessIpcCall(TInt aIpc)
/**
 * This function indicates if a IPC should not be handled.
 * @param aIPC A IPC call number.
 */
	{
	if(iInfo.iStatus != RPacketContext::EStatusDeleted)
		return EFalse;
	
	switch(aIpc)
		{
		case EPacketContextSetConfig:
		case EPacketContextGetConfig:
		case EPacketContextNotifyConfigChanged:
		case EPacketContextActivate:
		case EPacketContextDeactivate:
		case EPacketContextDelete:
		case EPacketContextLoanCommPort:
		case EPacketContextRecoverCommPort:
			LOGTEXT(_L8("CGprsContext::ShouldNotProcessIpcCall, context deleted. This ipc is not legal"));
			return ETrue;
		default:
			return EFalse;
		}
	}


TBool CGprsContext::ShouldNotProcessIpcCallWhenComportIsLoaned(TInt aIpc)
/**
 * This function indicates if a IPC should not be handled when comport is loaned.
 * @param aIPC A IPC call number.
 */
	{
	if(!iPhoneGlobals->iPhoneStatus.iDataPortLoaned)
		return EFalse;
	switch(aIpc)
		{
		case EPacketContextSetConfig:
		case EPacketContextGetConfig:
		case EPacketContextNotifyConfigChanged:
		case EPacketContextActivate:
		case EPacketContextDeactivate:
		case EPacketContextDelete:
		case EPacketContextLoanCommPort:
			LOGTEXT(_L8("CGprsContext::ShouldNotProcessIpcCallWhenComportIsLoaned, comport loaned out. This ipc is not legal"));
			return ETrue;
		default:
			return EFalse;
		}
	}

void CGprsContext::RemoveQoSProfileIfFound(CGprsQoS* aProfile)
/**
 * This function removes a QoS profile from the list.
 * @param aProfile The QoS profile to remove from linked list.
 */
    {
#ifdef _DEBUG
    TBuf8<KMaxName> tmpLog;
    tmpLog.Copy(*(aProfile->GetProfileName()));	// Convert 16bit string to 8bit so we can log it
    LOGTEXT2(_L8("CGprsContext::RemoveQoSProfileIfFound called, %S"), &tmpLog);
#endif
    CGprsQoSEntry* profileEntry;
    TDblQueIter<CGprsQoSEntry> iter(iProfiles);
    while (profileEntry=iter++, profileEntry)
        {
        if (profileEntry->iProfile == aProfile)
            {
            profileEntry->Deque();
            delete profileEntry;    // just deletes list entry, not the profile itself
            break;
            }
        }
    }

//
// CGprsQoSEntry class
//

CGprsQoSEntry::CGprsQoSEntry(CGprsQoS* aProfile)
: iProfile(aProfile)
/**
 * Constructor
 * @param aProfile QoS Profile to add to entry
 */
    {}


CGprsQoSEntry::~CGprsQoSEntry()
/**
 * Destructor
 */
    {}


void CGprsQoSEntry::Deque()
/**
 * Remove entry from TDblQue link
 */
    {
    iLink.Deque();
    iLink.iPrev=iLink.iNext=NULL;
    }
