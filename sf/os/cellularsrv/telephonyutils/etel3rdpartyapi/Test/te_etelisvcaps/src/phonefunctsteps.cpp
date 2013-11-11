// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implmentation of all test objects required to verify capability check on 
// ETel3rdParty phone functionality part.
// 
//

/**
 @file 
 @internalTechnology
*/

#include "phonefunctsteps.h"

_LIT(KDtmfString,"45p6789A");

//
CGetPhoneIdTestStep::CGetPhoneIdTestStep()
	:iPhoneIdPckg(iPhoneId)
   {
	_LIT16(manufacturerid, "PSION");
	_LIT16(modelid, "666");
	_LIT16(serialid, "1234567890");
	iPhoneId.iManufacturer=manufacturerid;
	iPhoneId.iModel=modelid;
	iPhoneId.iSerialNumber=serialid;
   }

TInt CGetPhoneIdTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetPhoneId(aStatus,iPhoneIdPckg);
	return KErrNone;
	}
	
TInt CGetPhoneIdTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetPhoneIdCancel);
	return KErrNone;
	}
	
//

CGetSubscriberIdTestStep::CGetSubscriberIdTestStep()
	:iSubscriberIdPckg(iSubscriberId)
	{
	}
   
TInt CGetSubscriberIdTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	//Set expected IMSI value
	_LIT16(SubscriberId, "56789012345678");
	iSubscriberId.iSubscriberId=SubscriberId;
	
	iTelephony->GetSubscriberId(aStatus, iSubscriberIdPckg);	
	return KErrNone;
	}

TInt CGetSubscriberIdTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetSubscriberIdCancel);
	return KErrNone;
	}

//

CGetFlightModeTestStep::CGetFlightModeTestStep()
	:iFlightIdPckg(iFlightId)
	{
	iFlightId.iFlightModeStatus = CTelephony::EFlightModeOn;
	}
   
TInt CGetFlightModeTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetFlightMode(aStatus, iFlightIdPckg);	
	return KErrNone;
	}

TInt CGetFlightModeTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetFlightModeCancel);
	return KErrNone;
	}

//

CGetIndicatorTestStep::CGetIndicatorTestStep()
	:iIndicatorPckg(iIndicator)
	{
	}
   
TInt CGetIndicatorTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetIndicator(aStatus, iIndicatorPckg);	
	return KErrNone;
	}

TInt CGetIndicatorTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetIndicatorCancel);
	return KErrNone;
	}

//

CGetBatteryInfoTestStep::CGetBatteryInfoTestStep()
	:iBatteryInfoPckg(iBatteryInfo)
	{
	}
   
TInt CGetBatteryInfoTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetBatteryInfo(aStatus, iBatteryInfoPckg);	
	return KErrNone;
	}

TInt CGetBatteryInfoTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetBatteryInfoCancel);
	return KErrNone;
	}

//

CGetSignalStrengthTestStep::CGetSignalStrengthTestStep()
	:iSignalStrengthPckg(iSignalStrength)
	{
	}
   
TInt CGetSignalStrengthTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetSignalStrength(aStatus, iSignalStrengthPckg);	
	return KErrNone;
	}

TInt CGetSignalStrengthTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetSignalStrengthCancel);
	return KErrNone;
	}

//

CGetLockInfoTestStep::CGetLockInfoTestStep()
	:iLockInfoPckg(iLockInfo)
	{
	iLockInfo.iSetting = CTelephony::ELockSetDisabled;
	iLockInfo.iStatus = CTelephony::EStatusUnlocked;
	iLock = CTelephony::ELockPin1;
	}
   
TInt CGetLockInfoTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetLockInfo(aStatus, iLock,iLockInfoPckg);	
	return KErrNone;
	}

TInt CGetLockInfoTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetLockInfoCancel);
	return KErrNone;
	}

//
CSendDTMFTonesTestStep::CSendDTMFTonesTestStep()
	{
	}

TInt CSendDTMFTonesTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->SendDTMFTones(aStatus, KDtmfString);	
	return KErrNone;
	}

TInt CSendDTMFTonesTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::ESendDTMFTonesCancel);
	return KErrNone;
	}

//
