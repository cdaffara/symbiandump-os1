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
* Code for TelephonyActPhone class, used by CTelephonyFunctions class.
*
*/


/**
 @file
*/

#include "TelephonyFunctions.h" 
#include "TelephonyActPhone.h"


CGetPhoneIdAct* CGetPhoneIdAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CGetPhoneIdAct* self = new(ELeave) CGetPhoneIdAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CGetPhoneIdAct::~CGetPhoneIdAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CGetPhoneIdAct::GetPhoneId(TDes8& aId)
/**
Issue Request
*/
	{
	iISVphoneId = reinterpret_cast<CTelephony::TPhoneIdV1*> ( const_cast<TUint8*> (aId.Ptr()));
	iTelephonyFunctions->Phone()->GetPhoneId(iStatus, iMMphoneId);
	SetActive();
	}


void CGetPhoneIdAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EGetPhoneId))
		{
		if(iStatus==KErrNone)
			{
			iISVphoneId->iManufacturer.Copy(iMMphoneId.iManufacturer);
			iISVphoneId->iModel.Copy(iMMphoneId.iModel);
			iISVphoneId->iSerialNumber.Copy(iMMphoneId.iSerialNumber);		
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetPhoneId, iStatus.Int());
		}
	}


TInt CGetPhoneIdAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetPhoneId, aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CGetPhoneIdAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneGetPhoneId);
	}


CGetPhoneIdAct::CGetPhoneIdAct(CTelephonyFunctions* aTelephonyFunctions)
: 	CAsyncRequestBaseAct(),
	iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}


CGetSubscriberIdAct* CGetSubscriberIdAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CGetSubscriberIdAct* self = new(ELeave) CGetSubscriberIdAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CGetSubscriberIdAct::~CGetSubscriberIdAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CGetSubscriberIdAct::GetSubscriberId(TDes8& aId)
/**
Issue Request
*/
	{
	iISVSubscriberIdV1 = reinterpret_cast<CTelephony::TSubscriberIdV1*> ( const_cast<TUint8*> (aId.Ptr()));
	iTelephonyFunctions->Phone()->GetSubscriberId(iStatus, iMMSubscriberId);
	SetActive();
	}


void CGetSubscriberIdAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EGetSubscriberId))
		{
		if(iStatus==KErrNone)
			{
			iISVSubscriberIdV1->iSubscriberId.Copy(iMMSubscriberId);		
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetSubscriberId, iStatus.Int());
		}
	}


TInt CGetSubscriberIdAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetSubscriberId, aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CGetSubscriberIdAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneGetSubscriberId);
	}


CGetSubscriberIdAct::CGetSubscriberIdAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}


CGetIndicatorAct* CGetIndicatorAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CGetIndicatorAct* self = new(ELeave) CGetIndicatorAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CGetIndicatorAct::~CGetIndicatorAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CGetIndicatorAct::GetIndicator(TDes8& aId)
/**
Issue Request
*/
	{
	TUint32 actionCaps; //not interested!
	iISVIndicator = reinterpret_cast<CTelephony::TIndicatorV1*> ( const_cast<TUint8*> (aId.Ptr()) );
	iTelephonyFunctions->Phone()->GetIndicatorCaps(actionCaps, iIndicatorCaps);
	iTelephonyFunctions->Phone()->GetIndicator(iStatus, iIndicator);
	SetActive();
	}


void CGetIndicatorAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EGetIndicator))
		{
		if(iStatus==KErrNone)
			{
			iISVIndicator->iIndicator=iIndicator;
			iISVIndicator->iCapabilities=iIndicatorCaps;		
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetIndicator, iStatus.Int());
		}
	}


TInt CGetIndicatorAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetIndicator, aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CGetIndicatorAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneGetIndicator);
	}


CGetIndicatorAct::CGetIndicatorAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}

CGetBatteryInfoAct* CGetBatteryInfoAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CGetBatteryInfoAct* self = new(ELeave) CGetBatteryInfoAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CGetBatteryInfoAct::~CGetBatteryInfoAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CGetBatteryInfoAct::GetBatteryInfo(TDes8& aId)
/**
Issue Request
*/
	{
	iISVBatteryInfo = reinterpret_cast<CTelephony::TBatteryInfoV1*> ( const_cast<TUint8*> (aId.Ptr()) );
	iTelephonyFunctions->Phone()->GetBatteryInfo(iStatus, iMMBatteryInfo);
	SetActive();
	}


void CGetBatteryInfoAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EGetBatteryInfo))
		{
		if(iStatus==KErrNone)
			{
			switch(iMMBatteryInfo.iStatus)
				{
				case RMobilePhone::EPoweredByBattery:
					iISVBatteryInfo->iStatus=CTelephony::EPoweredByBattery;
					break;
				case RMobilePhone::EBatteryConnectedButExternallyPowered:
					iISVBatteryInfo->iStatus=CTelephony::EBatteryConnectedButExternallyPowered;
					break;
				case RMobilePhone::ENoBatteryConnected:
					iISVBatteryInfo->iStatus=CTelephony::ENoBatteryConnected;
					break;
				case RMobilePhone::EPowerFault:
					iISVBatteryInfo->iStatus=CTelephony::EPowerFault;
					break;
				default:
						iISVBatteryInfo->iStatus=CTelephony::EPowerStatusUnknown;
				}		
			iISVBatteryInfo->iChargeLevel=iMMBatteryInfo.iChargeLevel;		
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetBatteryInfo, iStatus.Int());
		}
	}


TInt CGetBatteryInfoAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetBatteryInfo, aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CGetBatteryInfoAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneGetBatteryInfo);
	}


CGetBatteryInfoAct::CGetBatteryInfoAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}


CGetSignalStrengthAct* CGetSignalStrengthAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CGetSignalStrengthAct* self = new(ELeave) CGetSignalStrengthAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CGetSignalStrengthAct::~CGetSignalStrengthAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CGetSignalStrengthAct::GetSignalStrength(TDes8& aId)
/**
Issue Request
*/
	{
	iISVSignalStrength = reinterpret_cast<CTelephony::TSignalStrengthV1*> (const_cast<TUint8*> (aId.Ptr()));
	iTelephonyFunctions->Phone()->GetSignalStrength(iStatus, iSignalStrength, iBar);
	SetActive();
	}


void CGetSignalStrengthAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EGetSignalStrength))
		{
		if(iStatus==KErrNone)
			{
			iISVSignalStrength->iSignalStrength=iSignalStrength;
			iISVSignalStrength->iBar=iBar;		
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetSignalStrength, iStatus.Int());
		}
	}


TInt CGetSignalStrengthAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetSignalStrength, aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CGetSignalStrengthAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneGetSignalStrength);
	}


CGetSignalStrengthAct::CGetSignalStrengthAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}


CGetIccLockInfoAct* CGetIccLockInfoAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CGetIccLockInfoAct* self = new(ELeave) CGetIccLockInfoAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CGetIccLockInfoAct::~CGetIccLockInfoAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CGetIccLockInfoAct::GetIccLockInfo(CTelephony::TIccLock aLock, TDes8& aId)
/**
Issue Request
*/
	{
	iISVLockInfo = reinterpret_cast<CTelephony::TIccLockInfoV1*> ( const_cast<TUint8*> (aId.Ptr()) );
	
	RMobilePhone::TMobilePhoneLock thisLock;
	
	switch(aLock)
		{
	case CTelephony::ELockPin1:
		thisLock=RMobilePhone::ELockICC;
		break;
	case CTelephony::ELockPin2:
		thisLock=RMobilePhone::ELockPin2;
		break;
	default:
		thisLock=RMobilePhone::ELockICC;
		break;
		}
	
	iTelephonyFunctions->Phone()->GetLockInfo(iStatus, thisLock, iMMLockInfo1Pckg);
	SetActive();
	}


void CGetIccLockInfoAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EGetIccLockInfo))
		{
		if(iStatus==KErrNone)
			{
				switch(iMMLockInfo.iStatus)
					{
				case RMobilePhone::EStatusLocked:
					iISVLockInfo->iStatus=CTelephony::EStatusLocked;
					break;
				case RMobilePhone::EStatusUnlocked:
					iISVLockInfo->iStatus=CTelephony::EStatusUnlocked;
					break;
				case RMobilePhone::EStatusBlocked:
					iISVLockInfo->iStatus=CTelephony::EStatusBlocked;
					break;
				default:
					iISVLockInfo->iStatus=CTelephony::EStatusLockUnknown;
					break;
					}
				
				switch(iMMLockInfo.iSetting)
					{
				case RMobilePhone::ELockSetEnabled:
					iISVLockInfo->iSetting=CTelephony::ELockSetEnabled;
					break;
				case RMobilePhone::ELockSetDisabled:
					iISVLockInfo->iSetting=CTelephony::ELockSetDisabled;
					break;
				default:
					iISVLockInfo->iSetting=CTelephony::ELockSetUnknown;
					}			
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetIccLockInfo,  iStatus.Int());
		}
	}


TInt CGetIccLockInfoAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetIccLockInfo,  aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CGetIccLockInfoAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneGetLockInfo);
	}


CGetIccLockInfoAct::CGetIccLockInfoAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions),
  iMMLockInfo1Pckg(iMMLockInfo)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}


CSendDTMFTonesAct* CSendDTMFTonesAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CSendDTMFTonesAct* self = new(ELeave) CSendDTMFTonesAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CSendDTMFTonesAct::~CSendDTMFTonesAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CSendDTMFTonesAct::SendDTMFTones(const TDesC& aTones)
/**
Issue Request
*/
	{
	iTelephonyFunctions->Phone()->SendDTMFTones(iStatus, aTones);
	SetActive();
	}


void CSendDTMFTonesAct::Complete()
/**
Service Completed request.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ESendDTMFTones,  iStatus.Int());
	}


TInt CSendDTMFTonesAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ESendDTMFTones,  aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CSendDTMFTonesAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneSendDTMFTones);
	}


CSendDTMFTonesAct::CSendDTMFTonesAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}

CFlightModeChangeAct* CFlightModeChangeAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CFlightModeChangeAct* self = new(ELeave) CFlightModeChangeAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CFlightModeChangeAct::~CFlightModeChangeAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CFlightModeChangeAct::FlightModeChangeL(TDes8& aDes)
/**
Issue Request
*/
	{	
	iFlightMode = reinterpret_cast<CTelephony::TFlightModeV1*> ( const_cast<TUint8*> (aDes.Ptr()) );
	
	iTelephonyFunctions->PhonePowerProperty()->Subscribe(iStatus);

	SetActive();	
	}


void CFlightModeChangeAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EFlightModeChange))
		{
		if(iStatus==KErrNone)
			{
			TInt newPhonePowerState;
			if (iTelephonyFunctions->PhonePowerProperty()->Get(newPhonePowerState) != KErrNone)
			  newPhonePowerState = ESAPhoneOff;
			
			if(newPhonePowerState==ESAPhoneOn) 
				{
				iFlightMode->iFlightModeStatus=CTelephony::EFlightModeOff;
				}
			else 
				{
				iFlightMode->iFlightModeStatus=CTelephony::EFlightModeOn;
				}			
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EFlightModeChange,  iStatus.Int());
		}
	}


TInt CFlightModeChangeAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EFlightModeChange,  aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CFlightModeChangeAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->PhonePowerProperty()->Cancel();
	}


CFlightModeChangeAct::CFlightModeChangeAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}
	
CNotifyIndicatorAct* CNotifyIndicatorAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CNotifyIndicatorAct* self = new(ELeave) CNotifyIndicatorAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CNotifyIndicatorAct::~CNotifyIndicatorAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CNotifyIndicatorAct::NotifyIndicator(TDes8& aId)
/**
Issue Request
*/
	{
	TUint32 actionCaps; //not interested!
	iISVIndicator = reinterpret_cast<CTelephony::TIndicatorV1*> ( const_cast<TUint8*> (aId.Ptr()) );
	iTelephonyFunctions->Phone()->GetIndicatorCaps(actionCaps, iIndicatorCaps);
	iTelephonyFunctions->Phone()->NotifyIndicatorChange(iStatus, iIndicator);
	SetActive();
	}


void CNotifyIndicatorAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyIndicator))
		{
		if(iStatus==KErrNone)
			{
			iISVIndicator->iIndicator=iIndicator;
			iISVIndicator->iCapabilities=iIndicatorCaps;		
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyIndicator,  iStatus.Int());
		}
	}


TInt CNotifyIndicatorAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyIndicator,  aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CNotifyIndicatorAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneNotifyIndicatorChange);
	}


CNotifyIndicatorAct::CNotifyIndicatorAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}

CNotifyBatteryInfoAct* CNotifyBatteryInfoAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CNotifyBatteryInfoAct* self = new(ELeave) CNotifyBatteryInfoAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CNotifyBatteryInfoAct::~CNotifyBatteryInfoAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CNotifyBatteryInfoAct::NotifyBatteryInfo(TDes8& aId)
/**
Issue Request
*/
	{
	iISVBatteryInfo = reinterpret_cast<CTelephony::TBatteryInfoV1*> ( const_cast<TUint8*> (aId.Ptr()) );
	iTelephonyFunctions->Phone()->NotifyBatteryInfoChange(iStatus, iMMBatteryInfo);
	SetActive();
	}


void CNotifyBatteryInfoAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyBatteryInfo))
		{
		if(iStatus==KErrNone)
			{
			switch(iMMBatteryInfo.iStatus)
				{
				case RMobilePhone::EPoweredByBattery:
					iISVBatteryInfo->iStatus=CTelephony::EPoweredByBattery;
					break;
				case RMobilePhone::EBatteryConnectedButExternallyPowered:
					iISVBatteryInfo->iStatus=CTelephony::EBatteryConnectedButExternallyPowered;
					break;
				case RMobilePhone::ENoBatteryConnected:
					iISVBatteryInfo->iStatus=CTelephony::ENoBatteryConnected;
					break;
				case RMobilePhone::EPowerFault:
					iISVBatteryInfo->iStatus=CTelephony::EPowerFault;
					break;
				default:
						iISVBatteryInfo->iStatus=CTelephony::EPowerStatusUnknown;
				}		
			iISVBatteryInfo->iChargeLevel=iMMBatteryInfo.iChargeLevel;		
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyBatteryInfo,  iStatus.Int());
		}
	}


TInt CNotifyBatteryInfoAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyBatteryInfo,  aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CNotifyBatteryInfoAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneNotifyBatteryInfoChange);
	}


CNotifyBatteryInfoAct::CNotifyBatteryInfoAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}

CNotifySignalStrengthAct* CNotifySignalStrengthAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CNotifySignalStrengthAct* self = new(ELeave) CNotifySignalStrengthAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CNotifySignalStrengthAct::~CNotifySignalStrengthAct()
//
//Destructor
//
	{
	Cancel();
	Complete();
	}

void CNotifySignalStrengthAct::NotifySignalStrength(TDes8& aId)
//
//Issue Request
//
	{
	iISVSignalStrength = reinterpret_cast<CTelephony::TSignalStrengthV1*> ( const_cast<TUint8*> (aId.Ptr()) );
	iTelephonyFunctions->Phone()->NotifySignalStrengthChange(iStatus, iSignalStrength, iBar);
	SetActive();
	}


void CNotifySignalStrengthAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifySignalStrength))
		{
		if(iStatus==KErrNone)
			{
			iISVSignalStrength->iSignalStrength=iSignalStrength;
			iISVSignalStrength->iBar=iBar;		
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifySignalStrength,  iStatus.Int());
		}
	}


TInt CNotifySignalStrengthAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifySignalStrength,  aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CNotifySignalStrengthAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneNotifySignalStrengthChange);
	}


CNotifySignalStrengthAct::CNotifySignalStrengthAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}


CNotifyIccLockInfoAct* CNotifyIccLockInfoAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CNotifyIccLockInfoAct* self = new(ELeave) CNotifyIccLockInfoAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CNotifyIccLockInfoAct::~CNotifyIccLockInfoAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CNotifyIccLockInfoAct::NotifyIccLockInfo(CTelephony::TIccLock aLock, TDes8& aId)
/**
Issue Request
*/
	{
	if (aLock == CTelephony::ELockPin1)
		{
		iISVPin1LockInfo = reinterpret_cast<CTelephony::TIccLockInfoV1*> ( const_cast<TUint8*> (aId.Ptr()) );
		}
	else if (aLock == CTelephony::ELockPin2)
		{
		iISVPin2LockInfo = reinterpret_cast<CTelephony::TIccLockInfoV1*> ( const_cast<TUint8*> (aId.Ptr()) );
		}

	if (!((iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyPin1LockInfo)) && (iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyPin2LockInfo))))
		{
		iTelephonyFunctions->Phone()->NotifyLockInfoChange(iStatus, iMMLock, iMMLockInfoV1Pckg);
		SetActive();
		}
	}


void CNotifyIccLockInfoAct::Complete()
/**
Service Completed request.
*/
	{
	if(iStatus==KErrNone)
		{
		if ((iMMLock == RMobilePhone::ELockICC) && (iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyPin1LockInfo)))
			{
			switch(iMMLockInfo.iStatus)
				{
			case RMobilePhone::EStatusLocked:
				iISVPin1LockInfo->iStatus=CTelephony::EStatusLocked;
				break;
			case RMobilePhone::EStatusUnlocked:
				iISVPin1LockInfo->iStatus=CTelephony::EStatusUnlocked;
				break;
			case RMobilePhone::EStatusBlocked:
				iISVPin1LockInfo->iStatus=CTelephony::EStatusBlocked;
				break;
			default:
				iISVPin1LockInfo->iStatus=CTelephony::EStatusLockUnknown;
				break;
				}
			
			switch(iMMLockInfo.iSetting)
				{
			case RMobilePhone::ELockSetEnabled:
				iISVPin1LockInfo->iSetting=CTelephony::ELockSetEnabled;
				break;
			case RMobilePhone::ELockSetDisabled:
				iISVPin1LockInfo->iSetting=CTelephony::ELockSetDisabled;
				break;
			default:
				iISVPin1LockInfo->iSetting=CTelephony::ELockSetUnknown;
				}
			iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyPin1LockInfo,  KErrNone);
			}
		else if ((iMMLock == RMobilePhone::ELockPin2) && (iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyPin2LockInfo)))
			{
			switch(iMMLockInfo.iStatus)
				{
			case RMobilePhone::EStatusLocked:
				iISVPin2LockInfo->iStatus=CTelephony::EStatusLocked;
				break;
			case RMobilePhone::EStatusUnlocked:
				iISVPin2LockInfo->iStatus=CTelephony::EStatusUnlocked;
				break;
			case RMobilePhone::EStatusBlocked:
				iISVPin2LockInfo->iStatus=CTelephony::EStatusBlocked;
				break;
			default:
				iISVPin2LockInfo->iStatus=CTelephony::EStatusLockUnknown;
				break;
				}
			
			switch(iMMLockInfo.iSetting)
				{
			case RMobilePhone::ELockSetEnabled:
				iISVPin2LockInfo->iSetting=CTelephony::ELockSetEnabled;
				break;
			case RMobilePhone::ELockSetDisabled:
				iISVPin2LockInfo->iSetting=CTelephony::ELockSetDisabled;
				break;
			default:
				iISVPin2LockInfo->iSetting=CTelephony::ELockSetUnknown;
				}
			iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyPin2LockInfo,  KErrNone);
			}
		}
	else
		{
		if (iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyPin1LockInfo))
			{
			iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyPin1LockInfo,  iStatus.Int());
			}
		
		if (iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyPin2LockInfo))
			{
			iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyPin2LockInfo,  iStatus.Int());
			}
		}
	if ((iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyPin1LockInfo)) || (iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyPin2LockInfo)))
		{
		iTelephonyFunctions->Phone()->NotifyLockInfoChange(iStatus, iMMLock, iMMLockInfoV1Pckg);
		SetActive();
		}
	}


TInt CNotifyIccLockInfoAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	if (iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyPin1LockInfo))
		{
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyPin1LockInfo,  aLeaveCode);
		}
	
	if (iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::ENotifyPin2LockInfo))
		{
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ENotifyPin2LockInfo,  aLeaveCode);
		}
	return KErrNone;	// to ActiveScheduler.
	}

void CNotifyIccLockInfoAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneNotifyLockInfoChange);
	}


CNotifyIccLockInfoAct::CNotifyIccLockInfoAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions),
  iMMLockInfoV1Pckg(iMMLockInfo)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}

