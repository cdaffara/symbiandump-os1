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
// CPNSChecker implementation
//



/**
 @file pnschecker.cpp
 @internalTechnology
*/

#include "pnschecker.h"

CPNSChecker::CPNSChecker()
	{
	// No implementation required
	}

CPNSChecker::~CPNSChecker ()
	{
	iEvents.Close();
	}

CPNSChecker* CPNSChecker::NewLC ()
	{
	CPNSChecker* self = new (ELeave)CPNSChecker();
	CleanupStack::PushL (self);
	self->ConstructL ();
	return self;
	}

CPNSChecker* CPNSChecker::NewL ()
	{
	CPNSChecker* self=CPNSChecker::NewLC ();
	CleanupStack::Pop (self);
	return self;
	}

void CPNSChecker::ConstructL ()
	{
	}

void CPNSChecker::CheckPNSKeys(TBool &areAvailableKeys, TBool &isBTPSYLoaded, TBool &areValidKeys)
	{
	areAvailableKeys = ETrue;
	isBTPSYLoaded = ETrue;
	areValidKeys = ETrue;
	TInt err = KErrNone;
	TInt intValue = KErrGeneral;
	TBuf8<512> descValue; // 512 == RProperty::KMaxPropertySize
	
	err = iEvents.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyState,           intValue);
	CheckResults(areAvailableKeys, isBTPSYLoaded, areValidKeys, err, intValue, KBluetoothGpsPsyState);
	err = iEvents.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryLevel,    intValue);
	CheckResults(areAvailableKeys, isBTPSYLoaded, areValidKeys, err, intValue, KBluetoothGpsPsyBatteryLevel);
	err = iEvents.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryState,    intValue); 
	CheckResults(areAvailableKeys, isBTPSYLoaded, areValidKeys, err, intValue, KBluetoothGpsPsyBatteryState);
	err = iEvents.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtAntennaState, intValue);
	CheckResults(areAvailableKeys, isBTPSYLoaded, areValidKeys, err, intValue, KBluetoothGpsPsyExtAntennaState);
	err = iEvents.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtPowerState,   intValue);
	CheckResults(areAvailableKeys, isBTPSYLoaded, areValidKeys, err, intValue, KBluetoothGpsPsyExtPowerState);
	err = iEvents.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyVersion,         descValue); 
	CheckResults(areAvailableKeys, areValidKeys, err, KBluetoothGpsPsyVersion);
	err = iEvents.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtHwVersion,     descValue); 
	CheckResults(areAvailableKeys, areValidKeys, err, KBluetoothGpsPsyBtHwVersion);
	err = iEvents.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtSwVersion,     descValue); 
	CheckResults(areAvailableKeys, areValidKeys, err, KBluetoothGpsPsyBtSwVersion);
	err = iEvents.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsHwVersion,    descValue); 
	CheckResults(areAvailableKeys, areValidKeys, err, KBluetoothGpsPsyGpsHwVersion);
	err = iEvents.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsSwVersion,    descValue);
	CheckResults(areAvailableKeys, areValidKeys, err, KBluetoothGpsPsyGpsSwVersion);
	}

void CPNSChecker::CheckResults(TBool &areAvailableKeys, TBool &isBTPSYLoaded, TBool &areValidKeys, TInt aErr, TInt aValue, TUint aKey)
	{
	if(KErrNotFound == aErr)
		{
		areAvailableKeys = EFalse;
		return;
		}
	if(KErrNone != aErr)
		{
		areValidKeys = EFalse;
		return;
		}
	switch (aKey)
		{
		case KBluetoothGpsPsyState:
			if(aValue == EPSYNotLoaded)
				{
				isBTPSYLoaded = EFalse;
				}
			else
				{
				if((aValue < EPSYNotLoaded) && (aValue > EPSYInStandBy))
					{
					areValidKeys = EFalse;
					}
				}
			break;
		case KBluetoothGpsPsyBatteryLevel:
			if((aValue < 0) && (aValue > 100))
				{
				areValidKeys = EFalse;
				}
			break;
		case KBluetoothGpsPsyBatteryState:
			if((aValue < EBatteryStateNA) && (aValue > EBatteryStateFull))
				{
				areValidKeys = EFalse;
				}
			break;
		case KBluetoothGpsPsyExtAntennaState:
			if((aValue < EExtAntennaNA) && (aValue > EExtAntennaOn))
				{
				areValidKeys = EFalse;
				}
			break;
		case KBluetoothGpsPsyExtPowerState:
			if((aValue < EExtPowerNA) && (aValue > EExtPowerOn))
				{
				areValidKeys = EFalse;
				}
			break;
		default :
			areValidKeys = EFalse;
			break;
		}
	}

void CPNSChecker::CheckResults(TBool &areAvailableKeys, TBool &areValidKeys, TInt aErr, TUint /*aKey*/)
	{
	if(KErrNotFound == aErr)
		{
		areAvailableKeys = EFalse;
		return;
		}
	if(KErrNone != aErr)
		{
		areValidKeys = EFalse;
		return;
		}
	}

TInt CPNSChecker::CheckKey(TUint aKey, TInt aValue)
	{
	TInt err = KErrNone;
	TInt value = 0;
	err = iEvents.Get(KPsUidBluetoothGpsPsy, aKey, value);
	if(KErrNone == err)
		{
		if(value != aValue)
			{
			err = KErrCorrupt;
			}
		}
	return err;
	}

TInt CPNSChecker::CheckKey(TUint aKey, const TDesC8& aValue)
	{
	TInt err = KErrNone;
	TBuf8<512> descValue; // 512 == RProperty::KMaxPropertySize
	err = iEvents.Get(KPsUidBluetoothGpsPsy, aKey, descValue);
	if(KErrNone == err)
		{
		if(aValue.Compare(descValue) != 0)
			{
			err = KErrCorrupt;
			}
		}
	return err;
	}

void CPNSChecker::DeleteKey(TUint aKey)
	{
	iEvents.Delete(KPsUidBluetoothGpsPsy, aKey);
	}

void CPNSChecker::DeleteAllKeys()
	{
	iEvents.Delete(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyState);
	iEvents.Delete(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryLevel);
	iEvents.Delete(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryState);
	iEvents.Delete(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtAntennaState);
	iEvents.Delete(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtPowerState);
	iEvents.Delete(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyVersion);
	iEvents.Delete(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtHwVersion);
	iEvents.Delete(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtSwVersion);
	iEvents.Delete(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsHwVersion);
	iEvents.Delete(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsSwVersion);
	}
