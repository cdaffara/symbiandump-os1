// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CATGetModemStatus
// Description:
// CGetPhoneID

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include "getphoneid.h"
#include "mslogger.h"
#include "atgetimei.h"
#include "atgetmanufacturer.h"
#include "atgetmodel.h"
#include "atgetrevision.h"


// Class CGetPhoneID
// ---------------------------------------------------------------------------
// CGetPhoneID::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CGetPhoneID* CGetPhoneID::NewL(CGlobalPhonemanager& aGloblePhone, 
	                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CGetPhoneID* self = new(ELeave) CGetPhoneID(aGloblePhone,
			                                    aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop();
	return self ;
	}
// ---------------------------------------------------------------------------
// CGetPhoneID::CGetPhoneID
// other items were commented in a header
// ---------------------------------------------------------------------------
CGetPhoneID::CGetPhoneID(CGlobalPhonemanager& aGloblePhone, 
	                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CATCommandControlBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CGetPhoneID::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetPhoneID::ConstructL()
	{
	iATGetImei = CATGetImei::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	iATGetManufacturer = CATGetManufacturer::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	iATGetModel = CATGetModel::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	iATGetRevision = CATGetRevision::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	}
// ---------------------------------------------------------------------------
// CGetPhoneID::~CGetPhoneID
// other items were commented in a header
// ---------------------------------------------------------------------------
CGetPhoneID::~CGetPhoneID()
	{
	delete iATGetImei;
	iATGetImei = NULL;
	delete iATGetManufacturer;
	iATGetManufacturer = NULL;
	delete iATGetModel;
	iATGetModel = NULL;
	delete iATGetRevision;
	iATGetRevision = NULL;
	}
// ---------------------------------------------------------------------------
// CGetPhoneID::AtCommandExecuteComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetPhoneID::AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus)
	{
	if((aEventSource == EWriteCompletion)&&(KErrNone == aStatus))
		{
		return;
		}
	switch(iATType)
		{
		case ELtsyAT_Phone_GetImei:
			if((KErrNone == aStatus)&&(aEventSource == EReadCompletion))
				{
				aStatus = iATGetImei->GetErrorValue();
				}
			GetImeiComplete(aStatus);
			break;
		case ELtsyAT_Phone_GetManufacturer: 
			if((KErrNone == aStatus)&&(aEventSource == EReadCompletion))
				{
				aStatus = iATGetManufacturer->GetErrorValue();
				}
			GetManufacturerComplete(aStatus);
			break;
		case ELtsyAT_Phone_GetModel:
			if((KErrNone == aStatus)&&(aEventSource == EReadCompletion))
				{
				aStatus = iATGetModel->GetErrorValue();
				}
			GetModelComplete(aStatus);
			break;
		case ELtsyAT_Phone_GetRevision:
			if((KErrNone == aStatus)&&(aEventSource == EReadCompletion))
				{
				aStatus = iATGetRevision->GetErrorValue();
				if(KErrNone == aStatus)
					{
					RMobilePhone::TMobilePhoneIdentityV1 PhoneId;
					PhoneId = iATGetRevision->GetPhoneRevision();
					iPhoneId.iRevision = PhoneId.iRevision;
					}
				}
			iCtsyDispatcherCallback.CallbackPhoneGetPhoneIdComp(aStatus,iPhoneId);
			EndATCommand(iATGetRevision);
			iPhoneGlobals.iEventSignalActive = EFalse;
			break;
		default:
			break;
		}
	}
// ---------------------------------------------------------------------------
// CGetPhoneID::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetPhoneID::StartRequest()
	{	
	StartATCommand(iATGetImei);
	}
// ---------------------------------------------------------------------------
// CGetPhoneID::GetImeiComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetPhoneID::GetImeiComplete(const TInt aError)
	{
	if(KErrNone == aError)
		{
		RMobilePhone::TMobilePhoneIdentityV1 PhoneId;
		PhoneId = iATGetImei->GetPhoneImei();
		iPhoneId.iSerialNumber = PhoneId.iSerialNumber;
		EndATCommand(iATGetImei);
		iPhoneGlobals.iEventSignalActive = EFalse;
		StartATCommand(iATGetManufacturer);
		}
	else
		{
		EndATCommand(iATGetImei);
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhoneGetPhoneIdComp(aError,iPhoneId);
		}
	}
// ---------------------------------------------------------------------------
// CGetPhoneID::GetManufacturerComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetPhoneID::GetManufacturerComplete(const TInt aError)
	{
	if(KErrNone == aError)
		{
		RMobilePhone::TMobilePhoneIdentityV1 PhoneId;
		PhoneId = iATGetManufacturer->GetPhoneManufacturer();
		iPhoneId.iManufacturer = PhoneId.iManufacturer;
		EndATCommand(iATGetManufacturer);
		StartATCommand(iATGetModel);
		}
	else
		{
		EndATCommand(iATGetManufacturer);
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhoneGetPhoneIdComp(aError,iPhoneId);
		}
	}
// ---------------------------------------------------------------------------
// CGetPhoneID::GetModelComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetPhoneID::GetModelComplete(const TInt aError)
	{
	if(KErrNone == aError)
		{
		RMobilePhone::TMobilePhoneIdentityV1 PhoneId;
		PhoneId = iATGetModel->GetPhoneModel();
		iPhoneId.iModel = PhoneId.iModel;
		EndATCommand(iATGetModel);
		StartATCommand(iATGetRevision);
		}
	else
		{
		EndATCommand(iATGetModel);
		iCtsyDispatcherCallback.CallbackPhoneGetPhoneIdComp(aError,iPhoneId);
		}
	}
// ---------------------------------------------------------------------------
// CGetPhoneID::StartATCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetPhoneID::StartATCommand(CAtCommandBase* aAtCommand)
	{
	aAtCommand->SetAtCommandObserver(this);
	iATType = aAtCommand->AtType();
	aAtCommand->StartRequest();
	}
// ---------------------------------------------------------------------------
// CGetPhoneID::EndATCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetPhoneID::EndATCommand(CAtCommandBase* aAtCommand)
	{
	aAtCommand->RemoveAtCommandObserver();
	aAtCommand->Complete();
	}
//
// End of file
