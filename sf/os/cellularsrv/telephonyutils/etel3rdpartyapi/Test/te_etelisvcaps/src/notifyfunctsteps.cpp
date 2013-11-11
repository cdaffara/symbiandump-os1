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
// ETel3rdParty notifiers. 
// 
//

/**
 @file 
 @internalTechnology
*/

#include "notifyfunctsteps.h"
#include "te_etelisvcapsdefs.h"

//
CNotifyFlightModeTestStep::CNotifyFlightModeTestStep()
	:iFlightModePckg(iFlightMode)
	{
	}
TInt CNotifyFlightModeTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->NotifyChange(aStatus, CTelephony::EFlightModeChange, iFlightModePckg);
	return KErrNone;
	}
TInt CNotifyFlightModeTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EFlightModeChangeCancel);
	return KErrNone;
	}
	
//	
CNotifyIndicatorTestStep::CNotifyIndicatorTestStep()
	:iIndicatorPckg(iIndicator)
	{
	}
TInt CNotifyIndicatorTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->NotifyChange(aStatus, CTelephony::EIndicatorChange, iIndicatorPckg);
	return KErrNone;
	}
TInt CNotifyIndicatorTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EIndicatorChangeCancel);
	return KErrNone;	
	}
	
// 

CNotifyBatteryInfoTestStep::CNotifyBatteryInfoTestStep()
	:iBatteryInfoPckg(iBatteryInfo)
	{
	}
TInt CNotifyBatteryInfoTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->NotifyChange(aStatus, CTelephony::EBatteryInfoChange, iBatteryInfoPckg);
	return KErrNone;
	}
TInt CNotifyBatteryInfoTestStep::CancelRequestL()
	{
	 iTelephony->CancelAsync(CTelephony::EBatteryInfoChangeCancel);
	return KErrNone;   
	}
	
// 
CNotifySignalStrengthTestStep::CNotifySignalStrengthTestStep()
	:iSignalStrengthPckg(iSignalStrength)
	{
	}
TInt CNotifySignalStrengthTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->NotifyChange(aStatus, CTelephony::ESignalStrengthChange, iSignalStrengthPckg);
	return KErrNone;	
	}
TInt CNotifySignalStrengthTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::ESignalStrengthChangeCancel);
	return KErrNone;   
	}
	
// 
CNotifyPin1LockInfoTestStep::CNotifyPin1LockInfoTestStep()
	:iLockInfoPckg(iLockInfo)
	{
	}
TInt CNotifyPin1LockInfoTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->NotifyChange(aStatus, CTelephony::EPin1LockInfoChange, iLockInfoPckg);
	return KErrNone;   
	}
TInt CNotifyPin1LockInfoTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EPin1LockInfoChangeCancel);
	return KErrNone;   
	}
	
// 
CNotifyPin2LockInfoTestStep::CNotifyPin2LockInfoTestStep()
	:iLockInfoPckg(iLockInfo)
	{
	}
TInt CNotifyPin2LockInfoTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->NotifyChange(aStatus, CTelephony::EPin2LockInfoChange, iLockInfoPckg);
	return KErrNone;	
	}
TInt CNotifyPin2LockInfoTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EPin2LockInfoChangeCancel);
	return KErrNone;  
	}
	
// 
CNotifyVoiceLineStatusTestStep::CNotifyVoiceLineStatusTestStep()
	:iCallStatusPckg(iCallStatus)
	{
	}
TInt CNotifyVoiceLineStatusTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->NotifyChange(aStatus, CTelephony::EVoiceLineStatusChange, iCallStatusPckg);
	return KErrNone;	
	}
TInt CNotifyVoiceLineStatusTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);
	return KErrNone;   
	}
	
// 
CNotifyOwnedCall1StatusTestStep::CNotifyOwnedCall1StatusTestStep()
	:iCallStatusPckg(iCallStatusPckg)
	{
	}
TInt CNotifyOwnedCall1StatusTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->NotifyChange(aStatus, CTelephony::EOwnedCall1StatusChange, iCallStatusPckg);
	return KErrNone;   
	}
TInt CNotifyOwnedCall1StatusTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EOwnedCall1StatusChangeCancel);
	return KErrNone;	
	}
	
// 
CNotifyOwnedCall2StatusTestStep::CNotifyOwnedCall2StatusTestStep()
	:iCallStatusPckg(iCallStatus)
	{
	}
TInt CNotifyOwnedCall2StatusTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->NotifyChange(aStatus, CTelephony::EOwnedCall2StatusChange, iCallStatusPckg);
	return KErrNone;   
	}
TInt CNotifyOwnedCall2StatusTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EOwnedCall2StatusChangeCancel);
	return KErrNone;	
	}
	
// 
CNotifyOwnedCall1RemotePartyInfoTestStep::CNotifyOwnedCall1RemotePartyInfoTestStep()
	:iRemoteInfoPckg(iRemoteInfo)
	{
	}
TInt CNotifyOwnedCall1RemotePartyInfoTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->NotifyChange(aStatus,CTelephony::EOwnedCall1RemotePartyInfoChange , iRemoteInfoPckg);
	return KErrNone;  
	}
TInt CNotifyOwnedCall1RemotePartyInfoTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EOwnedCall1RemotePartyInfoChangeCancel);
	return KErrNone;	
	}
	
// 
CNotifyOwnedCall2RemotePartyInfoTestStep::CNotifyOwnedCall2RemotePartyInfoTestStep()
	:iRemoteInfoPckg(iRemoteInfo)
	{
	}
TInt CNotifyOwnedCall2RemotePartyInfoTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->NotifyChange(aStatus, CTelephony::EOwnedCall2RemotePartyInfoChange, iRemoteInfoPckg);
	return KErrNone;   
	}
TInt CNotifyOwnedCall2RemotePartyInfoTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EOwnedCall2RemotePartyInfoChangeCancel);
	return KErrNone;   
	}

// 

CNotifyNetworkRegistrationStatusTestStep::CNotifyNetworkRegistrationStatusTestStep()
	:iRegistrationInfoPckg(iRegistrationInfo)
	{
	}
TInt CNotifyNetworkRegistrationStatusTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->NotifyChange(aStatus, CTelephony::ENetworkRegistrationStatusChange, iRegistrationInfoPckg);
	return KErrNone;	
	}
TInt CNotifyNetworkRegistrationStatusTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::ENetworkRegistrationStatusChangeCancel);
	return KErrNone;	
	}

// 
CNotifyCurrentNetworkInfoTestStep::CNotifyCurrentNetworkInfoTestStep()
	:iNetworkInfoPckg(iNetworkInfo)
	{
	}
TInt CNotifyCurrentNetworkInfoTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->NotifyChange(aStatus, CTelephony::ECurrentNetworkInfoChange, iNetworkInfoPckg);
	return KErrNone;	
	}
TInt CNotifyCurrentNetworkInfoTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::ECurrentNetworkInfoChangeCancel);
	return KErrNone;	
	}
	
// 



