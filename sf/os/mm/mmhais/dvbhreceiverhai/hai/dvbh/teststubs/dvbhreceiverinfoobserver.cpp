// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file 
 @internalComponent
 @prototype
*/

#include "dvbhreceiverinfoobserver.h"
#include "dvbhreceiverinfo.h"
#include <in_sock.h>
	
//
// Implementation of CDvbhPropertyObserver.
//
	
CDvbhPropertyObserver::CDvbhPropertyObserver()
: CActive(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}
	

CDvbhPropertyObserver::~CDvbhPropertyObserver()
	{
	Cancel();
	iProperty.Close();
	}


TInt CDvbhPropertyObserver::SetObserver(MPropertyNotifier* aPropertyNotifier)
	{
	ASSERT(iPropertyNotifier == NULL);
	ASSERT(aPropertyNotifier != NULL);
	
	iPropertyNotifier = aPropertyNotifier;
	
	TInt result = iProperty.Attach(KDvbhPropertyCategory, iPropertyNotifier->GetPropertyKey());
	if (result != KErrNone)
		{
		return result;
		}

	iProperty.Subscribe(iStatus);
	SetActive();
	return KErrNone;
	}
	
/**
* Simply re-subscribes to the RProperty and calls the corresponding
* external observer via NotifyPropertyChanged()
*/
void CDvbhPropertyObserver::RunL()
	{
	iProperty.Subscribe(iStatus);
	SetActive();
	iPropertyNotifier->NotifyPropertyChanged(iProperty);
	}
	
void CDvbhPropertyObserver::DoCancel()
	{
	iProperty.Cancel();
	}
	



// 
// TDvbhNotifierBase
//

TDvbhNotifierBase::TDvbhNotifierBase()
:iExternalObserver(NULL)
	{
	}
	

void TDvbhNotifierBase::SetExternalObserver(TAny* aObserver)
	{
	ASSERT(aObserver != NULL);
	iExternalObserver = aObserver; 
	}




//
// TDvbhStateNotifier
//
	
TInt TDvbhStateNotifier::GetPropertyKey() const
	{
	return KDvbhPropertyKeyState;
	}

/**
* Calls the external observer associated with the state property
* to notify it of a state change. 
*/
void TDvbhStateNotifier::NotifyPropertyChanged(RProperty& aProperty)
	{
	TInt value = 0;
	TInt err = aProperty.Get(value);
	if (err == KErrNone)
		{
		MDvbhStateObserver* stateObserver = static_cast<MDvbhStateObserver*>(iExternalObserver);
		stateObserver->DvbhStateChange(static_cast<TDvbhState>(value));
		}	
	}



//
// TDvbhSignalQualityNotifier
//

/**
* Calls the external observer associated with the SignalQuality property
* to notify it of a SignalQuality change. 
*/
void TDvbhSignalQualityNotifier::NotifyPropertyChanged(RProperty& aProperty)
	{
	TInt value = 0;
	TInt err = aProperty.Get(value);
	if (err == KErrNone)
		{
		MDvbhSignalQualityObserver* observer = static_cast<MDvbhSignalQualityObserver*>(iExternalObserver);
		observer->DvbhSignalQualityChange(static_cast<TDvbhSignalQuality>(value));
		}	
	}

TInt TDvbhSignalQualityNotifier::GetPropertyKey() const
	{
	return KDvbhPropertyKeySignalQuality;
	}




//
// TDvbhPlatformNotifier
//

/**
* Calls the external observer associated with the Platform property
* to notify it of a Platform change. 
*/
void TDvbhPlatformNotifier::NotifyPropertyChanged(RProperty& aProperty)
	{
	TDvbhPlatformProperty platform;
	TPckg<TDvbhPlatformProperty> platformPckg(platform);
	TInt err = aProperty.Get(platformPckg);
	if (err == KErrNone)
		{
		MDvbhPlatformObserver* observer = static_cast<MDvbhPlatformObserver*>(iExternalObserver);
		observer->DvbhPlatformChange(platform.iPlatform, platform.iEsgRoot);
		}	
	}
	
TInt TDvbhPlatformNotifier::GetPropertyKey() const
	{
	return KDvbhPropertyKeyPlatform;
	}
	


//
// TDvbhNetworkTimeNotifier
//

/**
* Calls the external observer associated with the NetworkTime property
* to notify it of a NetworkTime change. 
*/
void TDvbhNetworkTimeNotifier::NotifyPropertyChanged(RProperty& /*aProperty*/)
	{
	MDvbhNetworkTimeObserver* observer = static_cast<MDvbhNetworkTimeObserver*>(iExternalObserver);
	observer->DvbhNetworkTimeUpdate();
	}

TInt TDvbhNetworkTimeNotifier::GetPropertyKey() const
	{
	return KDvbhPropertyKeyNetworkTime;
	}



	
//
// TDvbhFrequencyNotifier
//

/**
* Calls the external observer associated with the Frequency property
* to notify it of a Frequency change. 
*/
void TDvbhFrequencyNotifier::NotifyPropertyChanged(RProperty& aProperty)
	{
	TInt value = 0;
	TInt err = aProperty.Get(value);
	if (err == KErrNone)
		{
		MDvbhFrequencyObserver* observer = static_cast<MDvbhFrequencyObserver*>(iExternalObserver);
		observer->DvbhFrequencyChange(static_cast<TDvbhFrequency>(value));
		}	
	}
		
TInt TDvbhFrequencyNotifier::GetPropertyKey() const
	{
	return KDvbhPropertyKeyFrequency;
	}




//
// TDvbhCellIdNotifier
//

/**
* Calls the external observer associated with the CellId property
* to notify it of a CellId change. 
*/
void TDvbhCellIdNotifier::NotifyPropertyChanged(RProperty& aProperty)
	{
	TInt value = 0;
	TInt err = aProperty.Get(value);
	if (err == KErrNone)
		{
		MDvbhCellIdObserver* observer = static_cast<MDvbhCellIdObserver*>(iExternalObserver);
		observer->DvbhCellIdChange(static_cast<TDvbhCellId>(value));
		}	
	}	

TInt TDvbhCellIdNotifier::GetPropertyKey() const
	{
	return KDvbhPropertyKeyCellId;
	}



		
//
// TDvbhNetworkIdNotifier
//

/**
* Calls the external observer associated with the NetworkId property
* to notify it of a NetworkId change. 
*/
void TDvbhNetworkIdNotifier::NotifyPropertyChanged(RProperty& aProperty)
	{
	TInt value = 0;
	TInt err = aProperty.Get(value);
	if (err == KErrNone)
		{
		MDvbhNetworkIdObserver* observer = static_cast<MDvbhNetworkIdObserver*>(iExternalObserver);
		observer->DvbhNetworkIdChange(static_cast<TDvbhNetworkId>(value));
		}	
	}
	
TInt TDvbhNetworkIdNotifier::GetPropertyKey() const
	{
	return KDvbhPropertyKeyNetworkId;
	}


//
// TDvbhExtBatteryStateNotifier
//

/**
* Calls the external observer associated with the NetworkId property
* to notify it of a NetworkId change. 
*/
void TDvbhExtBatteryStateNotifier::NotifyPropertyChanged(RProperty& aProperty)
	{
	TInt value = 0;
	TInt err = aProperty.Get(value);
	if (err == KErrNone)
		{
		MDvbhExtBatteryStateObserver* observer = static_cast<MDvbhExtBatteryStateObserver*>(iExternalObserver);
		observer->DvbhExtBatteryStateChange(static_cast<TDvbhExtBatteryState>(value));
		}	
	}
	
TInt TDvbhExtBatteryStateNotifier::GetPropertyKey() const
	{
	return KDvbhPropertyKeyExtBatteryState;
	}
			
//
// TDvbhExtConnectionStateNotifier
//

/**
* Calls the external observer associated with the NetworkId property
* to notify it of a NetworkId change. 
*/
void TDvbhExtConnectionStateNotifier::NotifyPropertyChanged(RProperty& aProperty)
	{
	TInt value = 0;
	TInt err = aProperty.Get(value);
	if (err == KErrNone)
		{
		MDvbhExtConnectionObserver* observer = static_cast<MDvbhExtConnectionObserver*>(iExternalObserver);
		observer->DvbhExtConnectionStateChange(static_cast<TDvbhExtConnectionState>(value), EDvbhReceiverBluetooth);
		}	
	}
	
TInt TDvbhExtConnectionStateNotifier::GetPropertyKey() const
	{
	return KDvbhPropertyKeyExtConnectionState;
	}


//
// TMtvAccConnectionStateNotifier
//

/**
* Calls the external observer associated with the NetworkId property
* to notify it of a NetworkId change. 
*/
void TMtvAccConnectionStateNotifier::NotifyPropertyChanged(RProperty& aProperty)
	{
	TInt value = 0;
	TInt err = aProperty.Get(value);
	if (err == KErrNone)
		{
		MDvbhExtConnectionObserver* observer = static_cast<MDvbhExtConnectionObserver*>(iExternalObserver);
		observer->DvbhExtConnectionStateChange(static_cast<TDvbhExtConnectionState>(value), EDvbhReceiverUsbAccessory);
		}	
	}
	
TInt TMtvAccConnectionStateNotifier::GetPropertyKey() const
	{
	return KMtvAccAttachedKey;
	}

//
// TMtvAccAntennaConnectionStateNotifier
//

/**
* Calls the external observer associated with the NetworkId property
* to notify it of a NetworkId change. 
*/
void TMtvAccAntennaConnectionStateNotifier::NotifyPropertyChanged(RProperty& aProperty)
	{
	TInt value = 0;
	TInt err = aProperty.Get(value);
	if (err == KErrNone)
		{
		MDvbhExtAntennaConnectionObserver* observer = static_cast<MDvbhExtAntennaConnectionObserver*>(iExternalObserver);
		observer->DvbhExtAntennaConnectionStateChange(static_cast<TDvbhExtAntennaConnectionState>(value), EDvbhReceiverUsbAccessory);
		}	
	}
	
TInt TMtvAccAntennaConnectionStateNotifier::GetPropertyKey() const
	{
	return KMtvAccAntennaKey;
	}
