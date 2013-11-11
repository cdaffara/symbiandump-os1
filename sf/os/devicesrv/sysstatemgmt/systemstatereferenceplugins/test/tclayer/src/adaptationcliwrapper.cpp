// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains wrapper helper functions for test code to control the test wrappers 
// Adaptation client wrapper
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <ssm/ssmadaptationcli.h>
#include "adaptationcliwrapper.h"

TInt RSsmAdaptationBase::Connect()
	{
	return AdaptationCliWrapper::GetRSsmAdaptationBaseNextConnectReturn();
	}

void RSsmEmergencyCallRfAdaptation::ActivateRfForEmergencyCall(TRequestStatus& aStatus)
	{
	TRequestStatus* statusPtr = &aStatus;
	User::RequestComplete(statusPtr, AdaptationCliWrapper::GetRSsmEmergencyCallReturnValue());
	}

void RSsmEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall(TRequestStatus& aStatus)
	{
	TRequestStatus* statusPtr = &aStatus;
	User::RequestComplete(statusPtr, AdaptationCliWrapper::GetRSsmEmergencyCallReturnValue());
	}

void RSsmRtcAdaptation::ValidateRtc(TDes8& aValiditiyPckg, TRequestStatus& aStatus)
	{
	TBool ret = AdaptationCliWrapper::GetRSsmRtcAdaptationNextValidity();
	// Package up the return value and copy it into the descriptor
	TPckg<TBool> retPckg(ret);
	aValiditiyPckg.Copy(retPckg);
	// Complete the status
	TRequestStatus* statusPtr = &aStatus;
	User::RequestComplete(statusPtr, AdaptationCliWrapper::GetRSsmRtcAdaptationNextValidityReturn());
	}

void RSsmSimAdaptation::GetSimOwned(TDes8& aOwnedPckg, TRequestStatus& aStatus)
	{
	TRequestStatus* statusPtr = &aStatus;
	User::RequestComplete(statusPtr, AdaptationCliWrapper::GetSimOwnedReturnValue());
	if (AdaptationCliWrapper::GetSimOwnedReturnValue() == KErrNone)
		{
		TPckgBuf<TBool> boolPckg;
		boolPckg() = AdaptationCliWrapper::GetSimOwned();
		aOwnedPckg.Copy(boolPckg);
		}
	}

void RSsmSimAdaptation::GetCancel()
	{
	// Do nothing
	}

void RSsmSimAdaptation::NotifySimEvent(TDes8& aTypePckg, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	AdaptationCliWrapper::SetSimEventPckg(&aTypePckg);
	AdaptationCliWrapper::SetSimNotifyStatus(&aStatus);
	}

void RSsmSimAdaptation::NotifyCancel()
	{
	TRequestStatus* simNotify = AdaptationCliWrapper::GetSimNotifyStatus();
	if(simNotify != NULL)
		{
		User::RequestComplete(simNotify, KErrCancel);
		AdaptationCliWrapper::SetSimNotifyStatus(NULL);
		AdaptationCliWrapper::SetSimEventPckg(NULL);
		}
	}

/**
 * Sets the value that should be returned by RSsmAdaptationBase::Connect()
 * 
 * @test
 * @internalComponent
 */
void AdaptationCliWrapper::SetRSsmAdaptationBaseNextConnectReturn(TInt aRet)
	{
	iSsmAdaptationBaseNextConnect = aRet;
	}

/**
 * Gets the value that should be returned by RSsmAdaptationBase::Connect()
 * 
 * @test
 * @internalComponent
 */
TInt AdaptationCliWrapper::GetRSsmAdaptationBaseNextConnectReturn()
	{
	return iSsmAdaptationBaseNextConnect;
	}

/**
 * Sets the validity that should be returned by RSsmRtcAdaptation::ValidateRtc()
 * 
 * @test
 * @internalComponent
 */
void AdaptationCliWrapper::SetRSsmRtcAdaptationNextValidity(TBool aValidity)
	{
	iSsmRtcAdaptationNextValidity = aValidity;
	}

/**
 * Gets the validity that should be returned by RSsmRtcAdaptation::ValidateRtc()
 * 
 * @test
 * @internalComponent
 */

TBool AdaptationCliWrapper::GetRSsmRtcAdaptationNextValidity()
	{
	return iSsmRtcAdaptationNextValidity;
	}

/**
 * Sets the completion status that should be returned by RSsmRtcAdaptation::ValidateRtc()
 * 
 * @test
 * @internalComponent
 */
void AdaptationCliWrapper::SetRSsmRtcAdaptationNextValidityReturn(TInt aRet)
	{
	iSsmRtcAdaptationNextValidityReturn = aRet;
	}

/**
 * Gets the completion status that should be returned by RSsmRtcAdaptation::ValidateRtc()
 * 
 * @test
 * @internalComponent
 */
TInt AdaptationCliWrapper::GetRSsmRtcAdaptationNextValidityReturn()
	{
	return iSsmRtcAdaptationNextValidityReturn;
	}

/**
 * Sets the completion status that should be returned by RSsmEmergencyCallRfAdaption::ActivateRfForEmergencyCall()
 * and RSsmEmergencyCallRfAdaption::DeactivateRfForEmergencyCall()
 * 
 * @test
 * @internalComponent
 */
void AdaptationCliWrapper::SetRSsmEmergencyCallReturnValue(TInt aRet)
	{
	iSsmEmergencyCallReturnValue = aRet;
	}

/**
 * Gets the completion status that should be returned by RSsmEmergencyCallRfAdaption::ActivateRfForEmergencyCall()
 * and RSsmEmergencyCallRfAdaption::DeactivateRfForEmergencyCall()
 * 
 * @test
 * @internalComponent
 */
TInt AdaptationCliWrapper::GetRSsmEmergencyCallReturnValue()
	{
	return iSsmEmergencyCallReturnValue;
	}

/**
 * Sets the completion status for calls to RSsmSimAdaptation::GetSimOwned() 
 * 
 * @test
 * @internalComponent
 */
void AdaptationCliWrapper::SetSimOwnedReturnValue(TInt aRet)
	{
	iSimOwnedReturn = aRet;
	}

/**
 * Returns the completion status for calls to RSsmSimAdaptation::GetSimOwned() 
 * 
 * @test
 * @internalComponent
 */
TInt AdaptationCliWrapper::GetSimOwnedReturnValue()
	{
	return iSimOwnedReturn;
	}

/**
 * Sets the owned status for calls to RSsmSimAdaptation::GetSimOwned() 
 * 
 * @test
 * @internalComponent
 */
void AdaptationCliWrapper::SetSimOwned(TBool aOwned)
	{
	iSimOwned = aOwned;
	}

/**
 * Returns the owned status for calls to RSsmSimAdaptation::GetSimOwned() 
 * 
 * @test
 * @internalComponent
 */
TBool AdaptationCliWrapper::GetSimOwned()
	{
	return iSimOwned;
	}
	
/**
 * Sets the completion status for calls to RSsmSimAdaptation::GetSimChanged() 
 * 
 * @test
 * @internalComponent
 */
void AdaptationCliWrapper::SetSimChangedReturnValue(TInt aRet)
	{
	iSimChangedReturn = aRet;
	}

/**
 * Returns the completion status for calls to RSsmSimAdaptation::GetSimChanged() 
 * 
 * @test
 * @internalComponent
 */
TInt AdaptationCliWrapper::GetSimChangedReturnValue()
	{
	return iSimChangedReturn;
	}

/**
 * Sets the changed status for calls to RSsmSimAdaptation::GetSimChanged() 
 * 
 * @test
 * @internalComponent
 */
void AdaptationCliWrapper::SetSimChanged(TBool aChanged)
	{
	iSimChanged = aChanged;
	}

/**
 * Sets the pointer to the status for calls to RSsmSimAdaptation::NotifySimEvent() 
 * 
 * @test
 * @internalComponent
 */
void AdaptationCliWrapper::SetSimNotifyStatus(TRequestStatus* aStatus)
	{
	iSimNotifyStatus = aStatus;
	}

/**
 * Returns the pointer to the status for calls to RSsmSimAdaptation::NotifySimEvent() 
 * 
 * @test
 * @internalComponent
 */
TRequestStatus* AdaptationCliWrapper::GetSimNotifyStatus()
	{
	return iSimNotifyStatus;
	}
	
/**
 * Sets the pointer to the pckg for calls to RSsmSimAdaptation::NotifySimEvent() 
 * 
 * @test
 * @internalComponent
 */
void AdaptationCliWrapper::SetSimEventPckg(TDes8* aPckg)
	{
	iSimEventPckg = aPckg;
	}

/**
 * Returns the pointer to the pckg for calls to RSsmSimAdaptation::NotifySimEvent() 
 * 
 * @test
 * @internalComponent
 */
TDes8* AdaptationCliWrapper::GetSimEventPckg()
	{
	return iSimEventPckg;
	}
	
/**
 * Holds the value to be returned by calls to RSsmAdaptionBase::Connect()
 * 
 * @test
 * @internalComponent
 */
TInt AdaptationCliWrapper::iSimChangedReturn = KErrNone;

/**
 * Holds the value to be returned by calls to RSsmAdaptionBase::Connect()
 * 
 * @test
 * @internalComponent
 */
TInt AdaptationCliWrapper::iSimOwnedReturn = KErrNone;

/**
 * Holds a pointer to the TRequestStatus give to RSsmSimAdaption::NotifySimEvent()
 * 
 * @test
 * @internalComponent
 */
TRequestStatus* AdaptationCliWrapper::iSimNotifyStatus = NULL;

/**
 * Holds the value to be returned by calls to RSsmSimAdaption::GetSimChanged()
 * 
 * @test
 * @internalComponent
 */
TBool AdaptationCliWrapper::iSimChanged = EFalse;

/**
 * Holds the value to be returned by calls to RSsmSimAdaption::GetSimOwned()
 * 
 * @test
 * @internalComponent
 */
TBool AdaptationCliWrapper::iSimOwned = EFalse;

/**
 * Holds a pointer to the package given to RSsmSimAdaption::NotifySimEvent()
 * 
 * @test
 * @internalComponent
 */
TDes8* AdaptationCliWrapper::iSimEventPckg = NULL;

/**
 * Holds the value to be returned by calls to RSsmAdaptionBase::Connect()
 * 
 * @test
 * @internalComponent
 */
TInt AdaptationCliWrapper::iSsmAdaptationBaseNextConnect = KErrNone;


/**
 * Holds the validity value to be returned by calls to RSsmRtcAdaption::ValidateRtc()
 * 
 * @test
 * @internalComponent
 */
TBool AdaptationCliWrapper::iSsmRtcAdaptationNextValidity = EFalse;

/**
 * Holds the value for completion of aStatus by calls to RSsmRtcAdaption::ValidateRtc()
 * 
 * @test
 * @internalComponent
 */
TInt AdaptationCliWrapper::iSsmRtcAdaptationNextValidityReturn = KErrNone;

/**
 * Holds the value for completion of aStatus by calls to RSsmEmergencyCallRfAdaption::ActivateRfForEmergencyCall()
 * and RSsmEmergencyCallRfAdaption::DeactivateRfForEmergencyCall()
 * 
 * @test
 * @internalComponent
 */
TInt AdaptationCliWrapper::iSsmEmergencyCallReturnValue = KErrNone;
