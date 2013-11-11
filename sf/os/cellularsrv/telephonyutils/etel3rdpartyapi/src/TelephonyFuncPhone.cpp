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
* Code for CTelephonyFunctions class, used by CTelephony class.
*
*/


/**
 @file
*/

#include <e32def.h>
#include "TelephonyFunctions.h"
#include "TelephonyActPhone.h"

CTelephony::TEtelISVType::TEtelISVType()
	{
	iVersionId=KEtelISVV1;
	}

TInt CTelephonyFunctions::GetPhoneIdL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Initiate async request to retrieve the phone identifier.
*/
	{
	if(IsRequestPending(EGetPhoneId)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetPhoneId, ETrue);
		}
			
	iReqStatusTable[EGetPhoneId] = &aRequestStatus;
	
	if(!iGetPhoneId)
		{
		iGetPhoneId = CGetPhoneIdAct::NewL(this);		
		}
	
	iGetPhoneId->GetPhoneId(aDes);	
	 
	return KErrNone;
	}	
	
TInt CTelephonyFunctions::GetSubscriberIdL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Initiate async request to retrieve the subscriber Id.
*/
	{
	if(IsRequestPending(EGetSubscriberId)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetSubscriberId, ETrue);
		}
			
	iReqStatusTable[EGetSubscriberId] = &aRequestStatus;
	
	if(!iGetSubscriberId)
		{
		iGetSubscriberId = CGetSubscriberIdAct::NewL(this);	
		}
	
	iGetSubscriberId->GetSubscriberId(aDes);	
	 
	return KErrNone;
	}	

TInt CTelephonyFunctions::GetIndicatorL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Initiate async request to retrieve the phones indicators.
*/
	{
	if(IsRequestPending(EGetIndicator)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetIndicator, ETrue);
		}
			
	iReqStatusTable[EGetIndicator] = &aRequestStatus;
	
	if(!iGetIndicator)
		{
		iGetIndicator = CGetIndicatorAct::NewL(this);
		}
		
	iGetIndicator->GetIndicator(aDes);	
	 
	return KErrNone;
	}	
	
TInt CTelephonyFunctions::GetBatteryInfoL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Initiate async request to retrieve the battery information.
*/
	{
	if(IsRequestPending(EGetBatteryInfo)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetBatteryInfo, ETrue);
		}
			
	iReqStatusTable[EGetBatteryInfo] = &aRequestStatus;
	
	if(!iGetBatteryInfo)
		{
		iGetBatteryInfo = CGetBatteryInfoAct::NewL(this);	
		}
	
	iGetBatteryInfo->GetBatteryInfo(aDes);	
	 
	return KErrNone;
	}	

TInt CTelephonyFunctions::GetSignalStrengthL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Initiate async request to retrieve the signal strength.
*/
	{
	if(IsRequestPending(EGetSignalStrength)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetSignalStrength, ETrue);
		}
			
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
		
	iReqStatusTable[EGetSignalStrength] = &aRequestStatus;
	
	if(!iGetSignalStrength)	
		{	
		iGetSignalStrength = CGetSignalStrengthAct::NewL(this);	
		}
	
	iGetSignalStrength->GetSignalStrength(aDes);	
	 
	return KErrNone;
	}	

TInt CTelephonyFunctions::GetLockInfoL(TRequestStatus& aRequestStatus, const CTelephony::TIccLock& aLock, TDes8& aDes)
/**
Initiate async request to retrieve the ICC lock information.
*/
	{
	if(IsRequestPending(EGetIccLockInfo)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetIccLockInfo, ETrue);
		}
		
	iReqStatusTable[EGetIccLockInfo] = &aRequestStatus;
	
	if(!iGetIccLockInfo)	
		{
		iGetIccLockInfo = CGetIccLockInfoAct::NewL(this);	
		}
	
	iGetIccLockInfo->GetIccLockInfo(aLock, aDes);	
	 
	return KErrNone;
	}	

TInt CTelephonyFunctions::SendDTMFTonesL(TRequestStatus& aRequestStatus,  const TDesC& aTones)
/**
Initiate async request to transmit DTMF tones.
*/
	{
	if(IsRequestPending(ESendDTMFTones)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::ESendDTMFTones, ETrue);
		}
			
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
		
	iReqStatusTable[ESendDTMFTones] = &aRequestStatus;
	
	if(!iSendDTMFTones)
		{
		iSendDTMFTones = CSendDTMFTonesAct::NewL(this);		
		}
	
	iSendDTMFTones->SendDTMFTones(aTones);
	
	return KErrNone;
	}	


	
TInt CTelephonyFunctions::GetFlightMode(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Initiate async request to retrieve the current flight mode status.
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	if(IsRequestPending(EGetFlightMode)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{	
		SetRequestPending(CTelephonyFunctions::EGetFlightMode, ETrue);
		}
			
	iReqStatusTable[EGetFlightMode] = &aRequestStatus;
	
	CTelephony::TFlightModeV1& flightModeV1 = reinterpret_cast<CTelephony::TFlightModeV1&>
													( const_cast<TUint8&> ( *aDes.Ptr() ) );

	TInt newPhonePowerState;
	if (iPhonePowerProperty.Get(newPhonePowerState) != KErrNone)
		User::RequestComplete(reqStatusPtr, KErrAccessDenied);
	
	if(newPhonePowerState==ESAPhoneOn) 
		{
		flightModeV1.iFlightModeStatus=CTelephony::EFlightModeOff;
		User::RequestComplete(reqStatusPtr, KErrNone);
		}	 
	else
		{
		flightModeV1.iFlightModeStatus=CTelephony::EFlightModeOn;
		User::RequestComplete(reqStatusPtr, KErrNone);
		}
		
	SetRequestPending(CTelephonyFunctions::EGetFlightMode, EFalse);		
	return KErrNone;
	}	
