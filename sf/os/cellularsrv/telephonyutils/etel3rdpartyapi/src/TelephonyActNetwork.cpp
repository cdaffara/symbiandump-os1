/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* Code for TelephonyActNetwork class, used by CTelephonyFunctions class.
*
*/


/**
 @file
*/

#include "TelephonyFunctions.h" 
#include "TelephonyActNetwork.h"


CGetNetworkRegistrationStatusAct* CGetNetworkRegistrationStatusAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CGetNetworkRegistrationStatusAct* self = new(ELeave) CGetNetworkRegistrationStatusAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CGetNetworkRegistrationStatusAct::~CGetNetworkRegistrationStatusAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CGetNetworkRegistrationStatusAct::GetNetworkRegistrationStatus(TDes8& aId)
/**
Issue Request
*/
	{
	iISVNetworkRegistration = reinterpret_cast<CTelephony::TNetworkRegistrationV1*> ( const_cast<TUint8*> (aId.Ptr()) );
	iTelephonyFunctions->Phone()->GetNetworkRegistrationStatus(iStatus, iMMNetworkRegistration);
	SetActive();
	}

void CGetNetworkRegistrationStatusAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EGetNetworkRegistrationStatus))
		{
		if(iStatus==KErrNone)
			{
			GetRegistrationStatus(iMMNetworkRegistration, iISVNetworkRegistration->iRegStatus);
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetNetworkRegistrationStatus, iStatus.Int());	
		}
	}
	
void CGetNetworkRegistrationStatusAct::GetRegistrationStatus(const RMobilePhone::TMobilePhoneRegistrationStatus aMMRegStatus, CTelephony::TRegistrationStatus& aTelRegStatus)
	{
	switch(aMMRegStatus)
		{
		case RMobilePhone::ERegistrationUnknown:  
			aTelRegStatus=CTelephony::ERegistrationUnknown;
			break;
		case RMobilePhone::ENotRegisteredNoService:
			aTelRegStatus=CTelephony::ENotRegisteredNoService;
			break;
		case RMobilePhone::ENotRegisteredEmergencyOnly:
			aTelRegStatus=CTelephony::ENotRegisteredEmergencyOnly;
			break;
		case RMobilePhone::ENotRegisteredSearching:
			aTelRegStatus=CTelephony::ENotRegisteredSearching;
			break;			
		case RMobilePhone::ERegisteredBusy:
			aTelRegStatus=CTelephony::ERegisteredBusy;
			break;
		case RMobilePhone::ERegisteredOnHomeNetwork:
			aTelRegStatus=CTelephony::ERegisteredOnHomeNetwork;
			break;
		case RMobilePhone::ERegistrationDenied:
			aTelRegStatus=CTelephony::ERegistrationDenied;
			break;
		case RMobilePhone::ERegisteredRoaming:
			aTelRegStatus=CTelephony::ERegisteredRoaming;
			break;
		default:
			aTelRegStatus=CTelephony::ERegistrationUnknown;
		}
	}

TInt CGetNetworkRegistrationStatusAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetNetworkRegistrationStatus, aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CGetNetworkRegistrationStatusAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneGetNetworkRegistrationStatus);
	}


CGetNetworkRegistrationStatusAct::CGetNetworkRegistrationStatusAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}
	

CGetCurrentNetworkInfoAct* CGetCurrentNetworkInfoAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CGetCurrentNetworkInfoAct* self = new(ELeave) CGetCurrentNetworkInfoAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CGetCurrentNetworkInfoAct::~CGetCurrentNetworkInfoAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CGetCurrentNetworkInfoAct::GetCurrentNetworkInfo(TDes8& aPckg)
/**
Issue Request
*/
	{
	iISVNetworkInfo = reinterpret_cast<CTelephony::TEtelISVType*> ( const_cast<TUint8*> (aPckg.Ptr()) );

	iPostedIPC = EMobilePhoneGetCurrentNetwork;
	iTelephonyFunctions->Phone()->GetCurrentNetwork(iStatus, iMMNetworkInfoPckg, iMMArea);

	SetActive();
	}


void CGetCurrentNetworkInfoAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EGetCurrentNetworkInfo))
		{
		if(iStatus==KErrNone)
			{
			MapNetworkInfo(iISVNetworkInfo, &iMMNetworkInfo, &iMMArea);
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetCurrentNetworkInfo, iStatus.Int());		
		}
	}


TInt CGetCurrentNetworkInfoAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetCurrentNetworkInfo, aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CGetCurrentNetworkInfoAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(iPostedIPC);
	}

	
/**
Map RMobilePhone::TMobilePhoneNetworkInfoV2 to CTelephony::TEtelISVType
*/
void CGetCurrentNetworkInfoAct::MapNetworkInfo(CTelephony::TEtelISVType* aISVNetworkInfo, RMobilePhone::TMobilePhoneNetworkInfoV2* aMMNetworkInfo, RMobilePhone::TMobilePhoneLocationAreaV1* aMMArea)
	{
	switch (aISVNetworkInfo->VersionId())
		{
	case CTelephony::KEtelISVV1:
		{
		CTelephony::TNetworkInfoV1* tempInfo = reinterpret_cast<CTelephony::TNetworkInfoV1*>(aISVNetworkInfo);
		switch(aMMNetworkInfo->iMode)
			{
			case RMobilePhone::ENetworkModeUnknown: 
				tempInfo->iMode=CTelephony::ENetworkModeUnknown;
				break;
			case RMobilePhone::ENetworkModeUnregistered:
				tempInfo->iMode=CTelephony::ENetworkModeUnregistered;
				break;
			case RMobilePhone::ENetworkModeGsm:
				tempInfo->iMode=CTelephony::ENetworkModeGsm;
				break;
			case RMobilePhone::ENetworkModeAmps:
				tempInfo->iMode=CTelephony::ENetworkModeAmps;
				break;			
			case RMobilePhone::ENetworkModeCdma95:
				tempInfo->iMode=CTelephony::ENetworkModeCdma95;
				break;
			case RMobilePhone::ENetworkModeCdma2000:
				tempInfo->iMode=CTelephony::ENetworkModeCdma2000;
				break;
			case RMobilePhone::ENetworkModeWcdma:
				tempInfo->iMode=CTelephony::ENetworkModeWcdma;
				break;
			case RMobilePhone::ENetworkModeTdcdma:
				tempInfo->iMode=CTelephony::ENetworkModeTdcdma;
				break;
			default:
				tempInfo->iMode=CTelephony::ENetworkModeUnknown;
			}	

		switch(aMMNetworkInfo->iStatus)
			{
			case RMobilePhone::ENetworkStatusUnknown: 
				tempInfo->iStatus=CTelephony::ENetworkStatusUnknown;
				break;
			case RMobilePhone::ENetworkStatusAvailable:
				tempInfo->iStatus=CTelephony::ENetworkStatusAvailable;
				break;
			case RMobilePhone::ENetworkStatusCurrent:
				tempInfo->iStatus=CTelephony::ENetworkStatusCurrent;
				break;
			case RMobilePhone::ENetworkStatusForbidden:
				tempInfo->iStatus=CTelephony::ENetworkStatusForbidden;
				break;			
			default:
				tempInfo->iStatus=CTelephony::ENetworkStatusUnknown;
			}	
				
		switch(aMMNetworkInfo->iBandInfo)
			{
			case RMobilePhone::EBandUnknown: 
				tempInfo->iBandInfo=CTelephony::EBandUnknown;
				break;
			case RMobilePhone::E800BandA:
				tempInfo->iBandInfo=CTelephony::E800BandA;
				break;
			case RMobilePhone::E800BandB:
				tempInfo->iBandInfo=CTelephony::E800BandB;
				break;
			case RMobilePhone::E800BandC:
				tempInfo->iBandInfo=CTelephony::E800BandC;
				break;			
			case RMobilePhone::E1900BandA:
				tempInfo->iBandInfo=CTelephony::E1900BandA;
				break;
			case RMobilePhone::E1900BandB:
				tempInfo->iBandInfo=CTelephony::E1900BandB;
				break;
			case RMobilePhone::E1900BandC:
				tempInfo->iBandInfo=CTelephony::E1900BandC;
				break;
			case RMobilePhone::E1900BandD:
				tempInfo->iBandInfo=CTelephony::E1900BandD;
				break;
			case RMobilePhone::E1900BandE:
				tempInfo->iBandInfo=CTelephony::E1900BandE;
				break;
			case RMobilePhone::E1900BandF:
				tempInfo->iBandInfo=CTelephony::E1900BandF;
				break;
			default:
				tempInfo->iBandInfo=CTelephony::EBandUnknown;
			}			
		
		tempInfo->iCountryCode.Copy(aMMNetworkInfo->iCountryCode);
		tempInfo->iNetworkId.Copy(aMMNetworkInfo->iNetworkId);
		tempInfo->iDisplayTag.Copy(aMMNetworkInfo->iDisplayTag);
		
		TInt maxLength = tempInfo->iShortName.MaxLength();
		if (aMMNetworkInfo->iShortName.Length() > maxLength)
			{
			tempInfo->iShortName.Copy(aMMNetworkInfo->iShortName.Ptr(), maxLength);
			}
		else
			{
			tempInfo->iShortName.Copy(aMMNetworkInfo->iShortName);		
			}
		
		maxLength = tempInfo->iLongName.MaxLength();

		if (aMMNetworkInfo->iLongName.Length() > maxLength)
			{
			tempInfo->iLongName.Copy(aMMNetworkInfo->iLongName.Ptr(), maxLength);
			}
		else
			{
			tempInfo->iLongName.Copy(aMMNetworkInfo->iLongName);	
			}
		
		tempInfo->iCdmaSID.Copy(aMMNetworkInfo->iCdmaSID);
		
		if(aMMArea)
			{
			tempInfo->iAreaKnown=aMMArea->iAreaKnown;
			tempInfo->iLocationAreaCode=aMMArea->iLocationAreaCode;
			tempInfo->iCellId=aMMArea->iCellId;	
			}
		else
			{
			tempInfo->iAreaKnown=EFalse;
			tempInfo->iLocationAreaCode=0;
			tempInfo->iCellId=0;	
			}
		
		//check which paramater class version this is.		
		switch(aMMNetworkInfo->iAccess)
			{
			case RMobilePhone::ENetworkAccessUnknown: 
				tempInfo->iAccess=CTelephony::ENetworkAccessUnknown;
				break;
			case RMobilePhone::ENetworkAccessGsm:
				tempInfo->iAccess=CTelephony::ENetworkAccessGsm;
				break;
			case RMobilePhone::ENetworkAccessGsmCompact:
				tempInfo->iAccess=CTelephony::ENetworkAccessGsmCompact;
				break;
			case RMobilePhone::ENetworkAccessUtran:
				tempInfo->iAccess=CTelephony::ENetworkAccessUtran;
				break;			
			default:
				tempInfo->iAccess=CTelephony::ENetworkAccessUnknown;
			}						
			
		}
		break;
	case CTelephony::KEtelISVV2:
		{
		CTelephony::TNetworkInfoV2* tempInfo = reinterpret_cast<CTelephony::TNetworkInfoV2*>(aISVNetworkInfo);
		switch(aMMNetworkInfo->iMode)
			{
			case RMobilePhone::ENetworkModeUnknown: 
				tempInfo->iMode=CTelephony::ENetworkModeUnknown;
				break;
			case RMobilePhone::ENetworkModeUnregistered:
				tempInfo->iMode=CTelephony::ENetworkModeUnregistered;
				break;
			case RMobilePhone::ENetworkModeGsm:
				tempInfo->iMode=CTelephony::ENetworkModeGsm;
				break;
			case RMobilePhone::ENetworkModeAmps:
				tempInfo->iMode=CTelephony::ENetworkModeAmps;
				break;			
			case RMobilePhone::ENetworkModeCdma95:
				tempInfo->iMode=CTelephony::ENetworkModeCdma95;
				break;
			case RMobilePhone::ENetworkModeCdma2000:
				tempInfo->iMode=CTelephony::ENetworkModeCdma2000;
				break;
			case RMobilePhone::ENetworkModeWcdma:
				tempInfo->iMode=CTelephony::ENetworkModeWcdma;
				break;
			case RMobilePhone::ENetworkModeTdcdma:
				tempInfo->iMode=CTelephony::ENetworkModeTdcdma;
				break;
			default:
				tempInfo->iMode=CTelephony::ENetworkModeUnknown;
			}	

		switch(aMMNetworkInfo->iStatus)
			{
			case RMobilePhone::ENetworkStatusUnknown: 
				tempInfo->iStatus=CTelephony::ENetworkStatusUnknown;
				break;
			case RMobilePhone::ENetworkStatusAvailable:
				tempInfo->iStatus=CTelephony::ENetworkStatusAvailable;
				break;
			case RMobilePhone::ENetworkStatusCurrent:
				tempInfo->iStatus=CTelephony::ENetworkStatusCurrent;
				break;
			case RMobilePhone::ENetworkStatusForbidden:
				tempInfo->iStatus=CTelephony::ENetworkStatusForbidden;
				break;			
			default:
				tempInfo->iStatus=CTelephony::ENetworkStatusUnknown;
			}	
				
		switch(aMMNetworkInfo->iBandInfo)
			{
			case RMobilePhone::EBandUnknown: 
				tempInfo->iBandInfo=CTelephony::EBandUnknown;
				break;
			case RMobilePhone::E800BandA:
				tempInfo->iBandInfo=CTelephony::E800BandA;
				break;
			case RMobilePhone::E800BandB:
				tempInfo->iBandInfo=CTelephony::E800BandB;
				break;
			case RMobilePhone::E800BandC:
				tempInfo->iBandInfo=CTelephony::E800BandC;
				break;			
			case RMobilePhone::E1900BandA:
				tempInfo->iBandInfo=CTelephony::E1900BandA;
				break;
			case RMobilePhone::E1900BandB:
				tempInfo->iBandInfo=CTelephony::E1900BandB;
				break;
			case RMobilePhone::E1900BandC:
				tempInfo->iBandInfo=CTelephony::E1900BandC;
				break;
			case RMobilePhone::E1900BandD:
				tempInfo->iBandInfo=CTelephony::E1900BandD;
				break;
			case RMobilePhone::E1900BandE:
				tempInfo->iBandInfo=CTelephony::E1900BandE;
				break;
			case RMobilePhone::E1900BandF:
				tempInfo->iBandInfo=CTelephony::E1900BandF;
				break;
			default:
				tempInfo->iBandInfo=CTelephony::EBandUnknown;
			}			
		
		tempInfo->iCountryCode.Copy(aMMNetworkInfo->iCountryCode);
		tempInfo->iNetworkId.Copy(aMMNetworkInfo->iNetworkId);
		tempInfo->iDisplayTag.Copy(aMMNetworkInfo->iDisplayTag);
		
		TInt maxLength = tempInfo->iShortName.MaxLength();
		if (aMMNetworkInfo->iShortName.Length() > maxLength)
			{
			tempInfo->iShortName.Copy(aMMNetworkInfo->iShortName.Ptr(), maxLength);
			}
		else
			{
			tempInfo->iShortName.Copy(aMMNetworkInfo->iShortName);		
			}
		
		maxLength = tempInfo->iLongName.MaxLength();
		if (aMMNetworkInfo->iLongName.Length() > maxLength)
			{
			tempInfo->iLongName.Copy(aMMNetworkInfo->iLongName.Ptr(), maxLength);
			}
		else
			{
			tempInfo->iLongName.Copy(aMMNetworkInfo->iLongName);	
			}
		
		tempInfo->iCdmaSID.Copy(aMMNetworkInfo->iCdmaSID);
		
		if(aMMArea)
			{
			tempInfo->iAreaKnown=aMMArea->iAreaKnown;
			tempInfo->iLocationAreaCode=aMMArea->iLocationAreaCode;
			tempInfo->iCellId=aMMArea->iCellId;	
			}
		else
			{
			tempInfo->iAreaKnown=EFalse;
			tempInfo->iLocationAreaCode=0;
			tempInfo->iCellId=0;	
			}
		
		//check which paramater class version this is.		
		switch(aMMNetworkInfo->iAccess)
			{
			case RMobilePhone::ENetworkAccessUnknown: 
				tempInfo->iAccess=CTelephony::ENetworkAccessUnknown;
				break;
			case RMobilePhone::ENetworkAccessGsm:
				tempInfo->iAccess=CTelephony::ENetworkAccessGsm;
				break;
			case RMobilePhone::ENetworkAccessGsmCompact:
				tempInfo->iAccess=CTelephony::ENetworkAccessGsmCompact;
				break;
			case RMobilePhone::ENetworkAccessUtran:
				tempInfo->iAccess=CTelephony::ENetworkAccessUtran;
				break;			
			default:
				tempInfo->iAccess=CTelephony::ENetworkAccessUnknown;
			}		
		}
		break;
		}
	}

CGetCurrentNetworkInfoAct::CGetCurrentNetworkInfoAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions),
  iMMNetworkInfoPckg(iMMNetworkInfo)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}


CGetCurrentNetworkNameAct* CGetCurrentNetworkNameAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CGetCurrentNetworkNameAct* self = new(ELeave) CGetCurrentNetworkNameAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}
	
CGetCurrentNetworkNameAct::~CGetCurrentNetworkNameAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CGetCurrentNetworkNameAct::GetCurrentNetworkName(TDes8& aPckg)
/**
Issue Request
*/
	{
	iISVCurrentNetworkName = reinterpret_cast<CTelephony::TEtelISVType*> ( const_cast<TUint8*> (aPckg.Ptr()) );

	iTelephonyFunctions->Phone()->GetCurrentNetworkName(iStatus, iMMNetworkNameV3Pckg, iLocInfoPckg);
	SetActive();
	}

void CGetCurrentNetworkNameAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EGetCurrentNetworkName))
		{
		if(iStatus==KErrNone)
			{
			switch(iISVCurrentNetworkName->VersionId())
				{
			case CTelephony::KEtelISVV1:
				{
				CTelephony::TNetworkNameV1* tempNetName = reinterpret_cast<CTelephony::TNetworkNameV1*>(iISVCurrentNetworkName);
				TInt maxLength = tempNetName->iNetworkName.MaxLength();
				if (iMMNetworkNameV3.iLongName.Length() > maxLength)
					{
					tempNetName->iNetworkName.Copy(iMMNetworkNameV3.iLongName.Ptr(), maxLength);
					}
				else
					{
					tempNetName->iNetworkName.Copy(iMMNetworkNameV3.iLongName);		
					}
				}
				break;
			case CTelephony::KEtelISVV2:
				{
				CTelephony::TNetworkNameV2* tempNetName = reinterpret_cast<CTelephony::TNetworkNameV2*>(iISVCurrentNetworkName);
				TInt maxLength = tempNetName->iNetworkName.MaxLength();
				if (iMMNetworkNameV3.iLongName.Length() > maxLength)
					{
					tempNetName->iNetworkName.Copy(iMMNetworkNameV3.iLongName.Ptr(), maxLength);
					}
				else
					{
					tempNetName->iNetworkName.Copy(iMMNetworkNameV3.iLongName);		
					}	
				}
				break;
				}

			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetCurrentNetworkName, iStatus.Int());
		}
	}


TInt CGetCurrentNetworkNameAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetCurrentNetworkName, aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CGetCurrentNetworkNameAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneGetNetworkName);
	}


CGetCurrentNetworkNameAct::CGetCurrentNetworkNameAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions),
  iMMNetworkNameV3Pckg(iMMNetworkNameV3),
  iLocInfoPckg(iLocInfo)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}


CGetOperatorNameAct* CGetOperatorNameAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CGetOperatorNameAct* self = new(ELeave) CGetOperatorNameAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}
	
CGetOperatorNameAct::~CGetOperatorNameAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CGetOperatorNameAct::GetOperatorName(TDes8& aId)
/**
Issue Request
*/
	{
	iISVOperatorName = reinterpret_cast<CTelephony::TOperatorNameV1*> ( const_cast<TUint8*> (aId.Ptr()) );
	iTelephonyFunctions->Phone()->GetServiceProviderName(iStatus, iMMServiceProviderNamePckg);
	SetActive();
	}

void CGetOperatorNameAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EGetOperatorName))
		{
		if(iStatus==KErrNone)
			{
			TInt maxLength = iISVOperatorName->iOperatorName.MaxLength();
			if (iMMServiceProviderName.iSPName.Length() > maxLength)
				{
				iISVOperatorName->iOperatorName.Copy(iMMServiceProviderName.iSPName.Ptr(), maxLength);
				}
			else
				{
				iISVOperatorName->iOperatorName.Copy(iMMServiceProviderName.iSPName);
				}
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetOperatorName, iStatus.Int());
		}
	}


TInt CGetOperatorNameAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetOperatorName, aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CGetOperatorNameAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneGetServiceProviderName);
	}


CGetOperatorNameAct::CGetOperatorNameAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions),
  iMMServiceProviderNamePckg(iMMServiceProviderName)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}



CNotifyNetworkRegistrationStatusAct* CNotifyNetworkRegistrationStatusAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CNotifyNetworkRegistrationStatusAct* self = new(ELeave) CNotifyNetworkRegistrationStatusAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CNotifyNetworkRegistrationStatusAct::~CNotifyNetworkRegistrationStatusAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CNotifyNetworkRegistrationStatusAct::NotifyNetworkRegistrationStatus(TDes8& aId)
/**
Issue Request
*/
	{
	iISVNetworkRegistration = reinterpret_cast<CTelephony::TNetworkRegistrationV1*> ( const_cast<TUint8*> (aId.Ptr()) );
	iTelephonyFunctions->Phone()->NotifyNetworkRegistrationStatusChange(iStatus, iMMNetworkRegistration);
	SetActive();
	}

void CNotifyNetworkRegistrationStatusAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyNetworkRegistrationStatus))
		{
		if(iStatus==KErrNone)
			{
			CGetNetworkRegistrationStatusAct::GetRegistrationStatus(iMMNetworkRegistration, iISVNetworkRegistration->iRegStatus);
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyNetworkRegistrationStatus, iStatus.Int());
		}
	}

TInt CNotifyNetworkRegistrationStatusAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyNetworkRegistrationStatus, aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CNotifyNetworkRegistrationStatusAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneNotifyNetworkRegistrationStatusChange);
	}


CNotifyNetworkRegistrationStatusAct::CNotifyNetworkRegistrationStatusAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}
	

CNotifyCurrentNetworkInfoAct* CNotifyCurrentNetworkInfoAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CNotifyCurrentNetworkInfoAct* self = new(ELeave) CNotifyCurrentNetworkInfoAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CNotifyCurrentNetworkInfoAct::~CNotifyCurrentNetworkInfoAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CNotifyCurrentNetworkInfoAct::NotifyCurrentNetworkInfo(TDes8& aPckg)
/**
Issue Request
*/
	{
	iISVNetworkInfo = reinterpret_cast<CTelephony::TEtelISVType*> ( const_cast<TUint8*> (aPckg.Ptr()) );

	iPostedIPC = EMobilePhoneNotifyCurrentNetworkChange;
	iTelephonyFunctions->Phone()->NotifyCurrentNetworkChange(iStatus,iMMNetworkInfoPckg, iMMArea);

	SetActive();
	}

/**
Service Completed request.
*/
void CNotifyCurrentNetworkInfoAct::Complete()
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyCurrentNetworkInfo))
		{
		if(iStatus==KErrNone)
			{
			CGetCurrentNetworkInfoAct::MapNetworkInfo(iISVNetworkInfo, &iMMNetworkInfo, &iMMArea);
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyCurrentNetworkInfo, iStatus.Int());
		}
	}


TInt CNotifyCurrentNetworkInfoAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyCurrentNetworkInfo, aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CNotifyCurrentNetworkInfoAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(iPostedIPC);
	}


CNotifyCurrentNetworkInfoAct::CNotifyCurrentNetworkInfoAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions),
  iMMNetworkInfoPckg(iMMNetworkInfo)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}
	

