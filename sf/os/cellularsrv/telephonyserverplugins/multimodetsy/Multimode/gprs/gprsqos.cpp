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


#include "mSLOGGER.H"
#include <pcktcs.h>
#include "gprsqos.h"
#include "ATIO.H"
#include "NOTIFY.H"
#include "ATSTD.H"
#include "atgprsqosprofile.h"

/**
 * @file
 * This file implements the CGprsQoS class. Used to implement the RPacketQoS API.
 */


CGprsQoS* CGprsQoS::NewL(CGprsContext* aParent, TInt aCid, CATIO* aIo, CATInit*	aInit, CPhoneGlobals* aPhoneGlobals, TDes& aName)
/**
 * Standard 2 phase constructor.
 *
 * @param aTelObject pointer to parent.
 * @param aIo pointer to communication object.
 * @param aInit pointer to AT phone init object.
 * @param aPhoneGlobals pointer to phone global wide states.
 */ 
	{
	CGprsQoS* gprs=new(ELeave) CGprsQoS(aParent, aCid, aIo, aInit, aPhoneGlobals);
	CleanupStack::PushL(gprs);
	gprs->ConstructL(aName);
	CleanupStack::Pop();
	return gprs;
	}


CGprsQoS::CGprsQoS(CGprsContext* aParent, TInt aCid, CATIO* aIo, CATInit*	aInit, CPhoneGlobals* aPhoneGlobals) 
	: CSubSessionExtBase(), 
	iCid(aCid), 
	iParent(aParent), 
	iIo(aIo), 
	iInit(aInit), 
	iPhoneGlobals(aPhoneGlobals)
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

 
void CGprsQoS::ConstructL(TDes& aName)
/**
 * This function creates all objects on the heap that can leave.
 * @param aName Name of QoS profile
 */
	{
	LOGTEXT(_L8("CGprsQoS: Entered ConstructL"));
	iQoSName.Copy(aName);
	iATGprsSetProfile =				CATGprsSetProfile::NewL(iCid, iIo, this, iInit, iPhoneGlobals); 
	iATGprsGetProfile =				CATGprsGetProfile::NewL(iCid, iIo, this, iInit, iPhoneGlobals);
//	iATGprsGetCapsProfile=			CATGprsGetCapsProfile::NewL(iCid, iIo, this, iInit, iPhoneGlobals); 
	}
//TODO fix above!

CGprsQoS::~CGprsQoS()
/**
 * Destructor
 */
	{
	LOGTEXT(_L8("CGprsQoS: Entered destructor"));
	iParent->RemoveQoSProfileIfFound(this);
	iPhoneGlobals->iNotificationStore->RemoveClientFromLastEvents(this);
	delete iATGprsSetProfile;
	delete iATGprsGetProfile;
	}


void CGprsQoS::Init()
/**
 * This is not used. Init is done from the CPhone class.
 */
	{
	LOGTEXT(_L8("CGprsQoS: init()"));
	}


CTelObject* CGprsQoS::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * This function is not used.
 */
	{
	LOGTEXT(_L8("CGprsQoS::OpenNewObjectByNameL called"));
	User::Leave(KErrNotSupported);
	return NULL;
	}


CTelObject* CGprsQoS::OpenNewObjectL(TDes& /*aNewName*/)
/**
 * This function is not used.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	} 


CTelObject::TReqMode CGprsQoS::ReqModeL(const TInt aIpc)
/**
 * This function is called from the framework and checks the flow control for a IPC call.
 * @param aIpc IPC call from client.
 */
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
		case EPacketQoSGetProfileParams:
		case EPacketQoSSetProfileParams:
			ret = KReqModeFlowControlObeyed;
		break;

		case EPacketQoSGetProfileCaps:
			ret = KReqModeMultipleCompletionEnabled;
		break;

		case EPacketQoSNotifyProfileChanged:
			ret = KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
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
	

TInt CGprsQoS::RegisterNotification(const TInt aIpc)
/**
 * This function is called from the framework.
 * @param  aIpc IPC call from client.
 */
	{
	switch (aIpc)
		{
		case EPacketQoSNotifyProfileChanged:
			LOGTEXT(_L8("CGprsQoS: RegisterNotification - Config Changed"));
			return KErrNone;

		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CGprsQoS: Register error, unknown IPC"));
			return KErrNotSupported;
		}
	}


TInt CGprsQoS::DeregisterNotification(const TInt aIpc)
/**
 * This function is called when the there is no client listening to a notification anymore.
 *
 * @param aIpc IPC call from client.
 */
	{
	switch (aIpc)
		{
		case EPacketQoSNotifyProfileChanged:
			LOGTEXT(_L8("CGprsQoS: DeregisterNotification - Config Changed"));
			return KErrNone;

		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CGprsQoS: Deregister error, unknown IPC"));
			return KErrNotSupported;
		}
	}


TInt CGprsQoS::NumberOfSlotsL(const TInt aIpc)
	{
	TInt numberOfSlots=0;
	switch (aIpc)
		{
		case EPacketQoSNotifyProfileChanged:
			LOGTEXT(_L8("CGprsQoS: Registered with 5 slots"));
			numberOfSlots=5;
			break;

		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CGprsQoS: Number of Slots error, unknown IPC"));
			User::Leave(KErrNotSupported);
			break;
		}  
	return numberOfSlots;

	}


TInt CGprsQoS::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage)
/**
 * This function is called from the framework.
 * @param aTsyReqHandle handle to client.
 * @param aIpc IPC call from client.
 * @param aPackage agruments with the IPC call.
 */
	{
	
	if(ShouldNotProcessIpcCall(aIpc))
		{// If the context is in state deleted these calls should not be processed.

		ReqCompleted(aTsyReqHandle, KErrGeneral);
		return KErrNone;
		}
	if(ShouldNotProcessIpcCallWhenComportIsLoaned(aIpc))
		{// If the comport is loaned out ipcs that generate an AT-command shall be returned with KErrAccesssDenied
		ReqCompleted(aTsyReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	switch (aIpc)
		{
		case EPacketQoSSetProfileParams:
			return SetProfile(aTsyReqHandle, aPackage.Des1n());

		case EPacketQoSGetProfileParams:
			return GetProfile(aTsyReqHandle, aPackage.Des1n());

		case EPacketQoSGetProfileCaps:
			return GetProfileCaps(aTsyReqHandle, aPackage.Des1n());

		case EPacketQoSNotifyProfileChanged:
			return NotifyProfileChanged(aTsyReqHandle, aPackage.Des1n());

		default:
			return KErrNotSupported;
		}
	}


TDes* CGprsQoS::GetProfileName()
/**
 * This function returns the context name.
 *
 */
	{
	return &iQoSName;
	}


RPacketQoS::TQoSGPRSRequested* CGprsQoS::GetReqQoSProfile()
/**
 * This function returns the requested QoS profile
 *
 */
	{
	return &iQoSRequested;
	}


void CGprsQoS::SetReqQoSProfile(const RPacketQoS::TQoSGPRSRequested* aQoSRequested)
/**
 * This function sets the context configuration.
 * @param iContextConfigV10 Configuration to set.
 */

	{
	iQoSRequested = *aQoSRequested;
	}	

void CGprsQoS::SetNegQoSProfile(const RPacketQoS::TQoSGPRSNegotiated* aQoSNegotiated)
/**
 * This function sets the context configuration.
 * @param iContextConfigV10 Configuration to set.
 */

	{
	iQoSNegotiated = *aQoSNegotiated;
	}	

TInt CGprsQoS::CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle)
/**
 * This function is called from the framework and cancels a outstanding request.
 * @param aIpc The IPC to cancel.
 * @param aTsyReqHandle the client handle to cancel.
 */
	{
	LOGTEXT(_L8("CGprsATContextTsy: - CancelService called"));
	switch (aIpc)
		{
		case EPacketQoSSetProfileParams:
			return SetProfileCancel(aTsyReqHandle);

		case EPacketQoSGetProfileParams:
			return GetProfileCancel(aTsyReqHandle);

		case EPacketQoSGetProfileCaps:
			return GetProfileCapsCancel(aTsyReqHandle);

		case EPacketQoSNotifyProfileChanged:
			return NotifyProfileChangedCancel(aTsyReqHandle);

		default:
			return KErrNotSupported; 
		}	 
	}


 
TInt CGprsQoS::SetProfile(const TTsyReqHandle aTsyReqHandle,const TDesC8* aReqProfile)
/**
 * Called from a client via ExtFunc.
 * This function will start a chain of AT commands to the phone(AT+CGQMIN and AT+CGQREQ).
 * 
 * @param aTsyReqHandle request handle that is completed when done with request.
 * @param  aConfig Pointer to a RPacketContext::TContextConfigPckg.
 */
	{
	LOGTEXT(_L8("CGprsQoS::SetProfile called"));

	// check the client's passed me a GPRS pckg & not a CDMA package
	TPacketDataConfigBase& aCheck = (*(TPckg<TPacketDataConfigBase>*)aReqProfile)();
	if (aCheck.ExtensionId() == TPacketDataConfigBase::KConfigGPRS)
		iATGprsSetProfile->ExecuteCommand(aTsyReqHandle, (TAny*)aReqProfile);
	else
		ReqCompleted(aTsyReqHandle, KErrNotSupported); // I don't do CDMA
	return KErrNone;
	}


TInt CGprsQoS::SetProfileCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * 
 * @param aTsyReqHandle  The handle to cancel
 */
	{
	LOGTEXT(_L8("CGprsQoS::SetProfileCancel called"));
	iATGprsSetProfile->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}


TInt CGprsQoS::GetProfile(const TTsyReqHandle aTsyReqHandle, TDes8* aNegProfile)
/**
 * Called from a client via ExtFunc.
 * This function will start a chain of AT commands to the phone(AT+CGQREQ?, AT+CGQMIN?)
 * and query the settings in the phone.
 * 
 * @param aTsyReqHandle request handle that is completed when done with request.
 * @param  aConfig Pointer to a RPacketContext::TContextConfigPckg.
 */
	{
	LOGTEXT(_L8("CGprsQoS::GetProfile called"));

	// check the client's passed me a GPRS pckg & not a CDMA package
	TPacketDataConfigBase& aCheck = (*(TPckg<TPacketDataConfigBase>*)aNegProfile)();
	if (aCheck.ExtensionId() == TPacketDataConfigBase::KConfigGPRS)
		iATGprsGetProfile->ExecuteCommand(aTsyReqHandle, (TAny*)aNegProfile);
	else
		ReqCompleted(aTsyReqHandle, KErrNotSupported); // I don't do CDMA
	return KErrNone;
	}


TInt CGprsQoS::GetProfileCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * 
 * @param aTsyReqHandle  The handle to cancel
 */
	{
	LOGTEXT(_L8("CGprsQoS::GetProfileCancel called"));
	iATGprsGetProfile->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}


TInt CGprsQoS::NotifyProfileChanged(const TTsyReqHandle aTsyReqHandle, TDes8* aNegConfig)
/**
 * Called from a client via ExtFunc.
 * This function will complete when EPacketQoSProfileChange event is triggered.
 *
 * @param aTsyReqHandle  request handle that is completed.
 * @param  aConfig  Pointer to a RPacketContext::TContextConfigPckg      
 */
	{
	LOGTEXT(_L8("CGprsQoS::NotifyProfileChanged called"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(EPacketQoSProfileChange, aTsyReqHandle, this, aNegConfig);
	return KErrNone;
	}


TInt CGprsQoS::NotifyProfileChangedCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancels a outstanding request. Called via CancelService.
 * 
 * @param aTsyReqHandle  The handle to cancel
 */
	{
	LOGTEXT(_L8("CGprsQoS::NotifyProfileChangedCancel called"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}

TInt CGprsQoS::GetProfileCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCapsProfile)
/**
 * @param aTsyReqHandle   request handle that is completed.
 */
	{
	LOGTEXT(_L8("CGprsQoS::GetProfileCaps called"));

// check that the client has passed us a GPRS package & not a CDMA package
	TPacketDataConfigBase& aCheck = (*(TPckg<TPacketDataConfigBase>*)aCapsProfile)();
	if (aCheck.ExtensionId() == TPacketDataConfigBase::KConfigGPRS)
		{
		TPckg<RPacketQoS::TQoSCapsGPRS>* qosCapsGPRSPckg = (TPckg<RPacketQoS::TQoSCapsGPRS>*)aCapsProfile;
		RPacketQoS::TQoSCapsGPRS& qosCaps = (*qosCapsGPRSPckg)();

		qosCaps.iDelay = iPhoneGlobals->iStaticQoSCaps.iDelay;
		qosCaps.iMean = iPhoneGlobals->iStaticQoSCaps.iMean;
		qosCaps.iPeak = iPhoneGlobals->iStaticQoSCaps.iPeak;
		qosCaps.iPrecedence = iPhoneGlobals->iStaticQoSCaps.iPrecedence;
		qosCaps.iReliability = iPhoneGlobals->iStaticQoSCaps.iReliability;
		
		ReqCompleted(aTsyReqHandle, KErrNone);
		}
	else
		ReqCompleted(aTsyReqHandle,KErrNotSupported); // CDMA not supported here
	return KErrNone;
	}

TInt CGprsQoS::GetProfileCapsCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * @param aTsyReqHandle   request handle that is completed.
 */
	{
	LOGTEXT(_L8("CGprsQoS::GetProfileCapsCancel called"));
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TBool CGprsQoS::ShouldNotProcessIpcCall(TInt aIpc)
/**
 * This function indicates if a IPC should not be handled.
 * @param aIPC A IPC call number.
 */
	{
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
			LOGTEXT(_L8("CGprsQoS::ShouldNotProcessIpcCall, context deleted. This ipc is not legal"));
			return ETrue;
		default:
			return EFalse;
		}
	}


TBool CGprsQoS::ShouldNotProcessIpcCallWhenComportIsLoaned(TInt aIpc)
/**
 * This function indicates if a IPC should not be handled when comport is loaned.
 * @param aIPC A IPC call number.
 */
	{
	if(iPhoneGlobals->iPhoneStatus.iDataPortLoaned == EFalse)
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
			LOGTEXT(_L8("CGprsQoS::ShouldNotProcessIpcCallWhenComportIsLoaned, comport loaned out. This ipc is not legal"));
			return ETrue;
		default:
			return EFalse;

		}
	}
