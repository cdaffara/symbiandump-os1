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
// Example implementation of CDvbhReceiverInfo
// 
//

/**
 @file
 @internalComponent
 @prototype
*/

#include "dvbhreceiverinfo.h"
#include "dvbhreceiverinfoobserver.h"
#include "dvbhtypes.h"
#include "dvbhstubcommon.h"
#include <in_sock.h>
			
/**
* CDvbhReceiverInfo::CPrivateData contains the (implementation specific) private data of CDvbhReceiverInfo.
*/
NONSHARABLE_CLASS(CDvbhReceiverInfo::CPrivateData) : public CBase
    {
public:
	~CPrivateData();
public:
    // Internal observer classes to monitor for RProperty changes.
    CDvbhPropertyObserver*  iStatePropertyObserver;
    CDvbhPropertyObserver*	iSignalQualityPropertyObserver;
    CDvbhPropertyObserver*	iPlatformPropertyObserver;
    CDvbhPropertyObserver*	iNetworkTimePropertyObserver;
    CDvbhPropertyObserver*	iFrequencyPropertyObserver;
    CDvbhPropertyObserver*	iCellIdPropertyObserver;
    CDvbhPropertyObserver*	iNetworkIdPropertyObserver;
    CDvbhPropertyObserver*	iExtBatteryPropertyObserver;
    CDvbhPropertyObserver*	iExtConnectionPropertyObserver;
    CDvbhPropertyObserver*	iMtvAccConnectionPropertyObserver;
    CDvbhPropertyObserver*	iMtvAccAntennaConnectionPropertyObserver;

    // Wrappers around the external observer classes. To be called upon
    // when a property change is detected.
    TDvbhStateNotifier iStateNotifier;
	TDvbhSignalQualityNotifier iSignalQualityNotifier;
	TDvbhPlatformNotifier iPlatformNotifier;	
	TDvbhNetworkTimeNotifier iNetworkTimeNotifier;
	TDvbhFrequencyNotifier iFrequencyNotifier;
	TDvbhCellIdNotifier iCellIdNotifier;
	TDvbhNetworkIdNotifier iNetworkIdNotifier;	
    TDvbhExtBatteryStateNotifier iExtBatteryNotifier;
    TDvbhExtConnectionStateNotifier iExtConnectionNotifier;
    TMtvAccConnectionStateNotifier iMtvAccConnectionNotifier;
    TMtvAccAntennaConnectionStateNotifier iMtvAccAntennaConnectionNotifier;
    };

CDvbhReceiverInfo::CPrivateData::~CPrivateData()
	{
    delete iStatePropertyObserver;
    delete iSignalQualityPropertyObserver;
    delete iPlatformPropertyObserver;
    delete iNetworkTimePropertyObserver;
    delete iFrequencyPropertyObserver;
    delete iCellIdPropertyObserver;
    delete iNetworkIdPropertyObserver;
    delete iExtBatteryPropertyObserver;
    delete iExtConnectionPropertyObserver;
    delete iMtvAccConnectionPropertyObserver;
    delete iMtvAccAntennaConnectionPropertyObserver;
	}
	
/**
* Helper function to check if the current receiver state is EDvbhStateReceiving or EDvbhStateReady.
* If aCheckReceivingOnly is EFalse, then it returns KErrNone if the current state is either EDvbhStateReceiving or EDvbhStateReady.
* If aCheckReceivingOnly is ETrue, then will only return KErrNone if the current state is EDvbhStateReceiving.
*
* @param aCheckReceivingOnly ETrue if you only want to check for EDvbhStateReceiving, otherwise will check for EDvbhStateReceiving or EDvbhStateReady.
* @return KErrNone if the current state equals EDvbhStateReceiving or EDvbhStateReady, and aCheckReceivingOnly equals EFalse.  If aCheckReceivingOnly equals ETrue, the KErrNone only if current state is EDvbhStateReceiving.  Otherwise a system-wide error code.
*/
static TInt VerifyReadyOrReceiving(TBool aCheckReceivingOnly = EFalse)
	{
	TDvbhState state = EDvbhStateInactive;
	TInt result = CDvbhReceiverInfo::GetState(state);
	if (result != KErrNone)
		{
		return result;
		}		
		
	if (state != EDvbhStateReceiving && (state != EDvbhStateReady || aCheckReceivingOnly) )
		{
		return KErrNotReady;
		}	
	return KErrNone;
	}
	
/*
* Example implementation of CDvbhReceiverInfo
*/

EXPORT_C CDvbhReceiverInfo* CDvbhReceiverInfo::NewL()
	{
	CDvbhReceiverInfo* self = CDvbhReceiverInfo::NewLC();
	CleanupStack::Pop(self);
	return self;
	}


EXPORT_C CDvbhReceiverInfo* CDvbhReceiverInfo::NewLC()
	{
	CDvbhReceiverInfo* self = new (ELeave) CDvbhReceiverInfo;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

EXPORT_C CDvbhReceiverInfo::CDvbhReceiverInfo()
	{
	}
	
void CDvbhReceiverInfo::ConstructL()
	{
	iData = new (ELeave) CPrivateData;
	}
	
EXPORT_C CDvbhReceiverInfo::~CDvbhReceiverInfo()
	{
	delete iData;
	}

EXPORT_C TInt CDvbhReceiverInfo::GetState( TDvbhState& aState )
	{
	TInt state = 0;
	TInt result = RProperty::Get(KDvbhPropertyCategory, KDvbhPropertyKeyState, state);
	if (result == KErrNone)
		{
		aState = static_cast<TDvbhState>(state);
		}
	return result;	
	}

EXPORT_C TInt CDvbhReceiverInfo::SetStateObserver( MDvbhStateObserver& aObserver )
	{
	delete iData->iStatePropertyObserver;
	iData->iStatePropertyObserver = NULL;
	if ((iData->iStatePropertyObserver = new CDvbhPropertyObserver) == NULL)
		{
		return KErrNoMemory;
		}

	iData->iStateNotifier.SetExternalObserver(&aObserver);
	return iData->iStatePropertyObserver->SetObserver(&iData->iStateNotifier);
	}

EXPORT_C TInt CDvbhReceiverInfo::GetSignalQuality( TDvbhSignalQuality& aSignalQuality )
	{
	// API documentation states that this method may only be called if the receiver 
	// is in the Receiving state.
	TInt result = VerifyReadyOrReceiving(ETrue); //ETrue parameter means check for Receiving state only
	if (result != KErrNone)
		{
		return result;
		}
	
	// The receiver is in the correct state so we may proceed.
	TInt quality = 0;
	result = RProperty::Get(KDvbhPropertyCategory, KDvbhPropertyKeySignalQuality, quality);
	if (result == KErrNone)
		{
		aSignalQuality = static_cast<TDvbhSignalQuality>(quality);
		}
	return result;
	}

EXPORT_C TInt CDvbhReceiverInfo::SetSignalQualityObserver( MDvbhSignalQualityObserver& aObserver )
	{
	delete iData->iSignalQualityPropertyObserver;
	iData->iSignalQualityPropertyObserver = NULL;
	if ((iData->iSignalQualityPropertyObserver = new CDvbhPropertyObserver) == NULL)
		{
		return KErrNoMemory;
		}
	iData->iSignalQualityNotifier.SetExternalObserver(&aObserver);
	return iData->iSignalQualityPropertyObserver->SetObserver(&iData->iSignalQualityNotifier);
	}

EXPORT_C TInt CDvbhReceiverInfo::GetPlatform( TDvbhPlatform& aPlatform, TIp6Addr& aESGRoot )
	{
	// API documentation states that this method may only be called if the receiver 
	// is in the Ready or Receiving state.
	TInt result = VerifyReadyOrReceiving();
	if (result != KErrNone)
		{
		return result;
		}
	
	// The receiver is in the correct state so we may proceed.
	TDvbhPlatformProperty platform;		
	TPckg<TDvbhPlatformProperty> platformPckg(platform);
	result = RProperty::Get(KDvbhPropertyCategory, KDvbhPropertyKeyPlatform, platformPckg);
	if (result == KErrNone)
		{
		aPlatform.iId = platform.iPlatform.iId;
		aPlatform.iName = platform.iPlatform.iName;
		aESGRoot = platform.iEsgRoot;
		}
	return result;
	}

EXPORT_C TInt CDvbhReceiverInfo::SetPlatformObserver( MDvbhPlatformObserver& aObserver )
	{
	delete iData->iPlatformPropertyObserver;
	iData->iPlatformPropertyObserver = NULL;
	if ((iData->iPlatformPropertyObserver = new CDvbhPropertyObserver) == NULL)
		{
		return KErrNoMemory;
		}
	iData->iPlatformNotifier.SetExternalObserver(&aObserver);
	return iData->iPlatformPropertyObserver->SetObserver(&iData->iPlatformNotifier);
	}

EXPORT_C TInt CDvbhReceiverInfo::GetNetworkTime( TTime& aNetworkTime, TBool& aValid )
	{		
	TDvbhNetworkTime networkTime;
	TPckg<TDvbhNetworkTime> timePckg(networkTime);
	TInt result = RProperty::Get(KDvbhPropertyCategory, KDvbhPropertyKeyNetworkTime, timePckg);
	if (result == KErrNone)
		{
		//API documentation states that KErrNotReady should be returned if a platform has
		//never been set on the receiver.
		if (networkTime.iPlatformId == KDvbhInvalidPlatform)
			{
			result = KErrNotReady;
			}
		else
			{
			//The platform is valid, but it might be old.  It will be old if there is no
			//current platform set, i.e. if the state is not Ready or Receiving.  Set the aValid
			//bool based on whether or not it the platform is old.
			if (VerifyReadyOrReceiving() == KErrNone)
				{
				aValid = ETrue;
				}
			else
				{
				aValid = EFalse;
				}
				
			//Finally, update the time
			aNetworkTime = networkTime.iNetworkTime;	
			}
		}
	return result;
	}

EXPORT_C TInt CDvbhReceiverInfo::SetNetworkTimeObserver( MDvbhNetworkTimeObserver& aObserver )
	{
	delete iData->iNetworkTimePropertyObserver;
	iData->iNetworkTimePropertyObserver = NULL;
	if ((iData->iNetworkTimePropertyObserver = new CDvbhPropertyObserver) == NULL)
		{
		return KErrNoMemory;
		}
	iData->iNetworkTimeNotifier.SetExternalObserver(&aObserver);
	return iData->iNetworkTimePropertyObserver->SetObserver(&iData->iNetworkTimeNotifier);
	}

EXPORT_C TInt CDvbhReceiverInfo::GetPerformanceData( TDvbhPerformanceData& aPerformanceData )
	{
	TPckg<TDvbhPerformanceData> performancePckg(aPerformanceData);
	return RProperty::Get(KDvbhPropertyCategory, KDvbhPropertyKeyPerformanceData, performancePckg);
	}

EXPORT_C TInt CDvbhReceiverInfo::GetFrequency( TDvbhFrequency& aFrequency )
	{
	// API documentation states that this method may only be called if the receiver 
	// is in the Ready or Receiving state.
	TInt result = VerifyReadyOrReceiving();
	if (result != KErrNone)
		{
		return result;
		}
	
	// The receiver is in the correct state so we may proceed.	
	TInt frequency = 0;
	result = RProperty::Get(KDvbhPropertyCategory, KDvbhPropertyKeyFrequency, frequency);
	if (result == KErrNone)
		{
		aFrequency = static_cast<TDvbhFrequency>(frequency);
		}
	return result;
	}

EXPORT_C TInt CDvbhReceiverInfo::SetFrequencyObserver( MDvbhFrequencyObserver& aObserver )
	{
	delete iData->iFrequencyPropertyObserver;
	iData->iFrequencyPropertyObserver = NULL;
	if ((iData->iFrequencyPropertyObserver = new CDvbhPropertyObserver) == NULL)
		{
		return KErrNoMemory;
		}
	iData->iFrequencyNotifier.SetExternalObserver(&aObserver);
	return iData->iFrequencyPropertyObserver->SetObserver(&iData->iFrequencyNotifier);
	}

EXPORT_C TInt CDvbhReceiverInfo::GetCellId( TDvbhCellId& aCellId )
	{
	// API documentation states that this method may only be called if the receiver 
	// is in the Ready or Receiving state.
	TInt result = VerifyReadyOrReceiving();
	if (result != KErrNone)
		{
		return result;
		}
	
	// The receiver is in the correct state so we may proceed.
	TInt cellId = 0;
	result = RProperty::Get(KDvbhPropertyCategory, KDvbhPropertyKeyCellId, cellId);
	if (result == KErrNone)
		{
		aCellId = static_cast<TDvbhCellId>(cellId);
		}
	return result;
	}

EXPORT_C TInt CDvbhReceiverInfo::SetCellIdObserver( MDvbhCellIdObserver& aObserver )
	{
	delete iData->iCellIdPropertyObserver;
	iData->iCellIdPropertyObserver = NULL;
	if ((iData->iCellIdPropertyObserver = new CDvbhPropertyObserver) == NULL)
		{
		return KErrNoMemory;
		}
	iData->iCellIdNotifier.SetExternalObserver(&aObserver);
	return iData->iCellIdPropertyObserver->SetObserver(&iData->iCellIdNotifier);
	}

EXPORT_C TInt CDvbhReceiverInfo::GetNetworkId( TDvbhNetworkId& aNetworkId )
	{
	// API documentation states that this method may only be called if the receiver 
	// is in the Ready or Receiving state.
	TInt result = VerifyReadyOrReceiving();
	if (result != KErrNone)
		{
		return result;
		}
	
	// The receiver is in the correct state so we may proceed.
	TInt networkId = 0;
	result = RProperty::Get(KDvbhPropertyCategory, KDvbhPropertyKeyNetworkId, networkId);
	if (result == KErrNone)
		{
		aNetworkId = static_cast<TDvbhNetworkId>(networkId);
		}
	return result;
	}

EXPORT_C TInt CDvbhReceiverInfo::SetNetworkIdObserver( MDvbhNetworkIdObserver& aObserver )
	{
	delete iData->iNetworkIdPropertyObserver;
	iData->iNetworkIdPropertyObserver = NULL;
	if ((iData->iNetworkIdPropertyObserver = new CDvbhPropertyObserver) == NULL)
		{
		return KErrNoMemory;
		}
	iData->iNetworkIdNotifier.SetExternalObserver(&aObserver);
	return iData->iNetworkIdPropertyObserver->SetObserver(&iData->iNetworkIdNotifier);
	}

EXPORT_C TInt CDvbhReceiverInfo::GetBatteryState( TDvbhExtBatteryState& aState )
	{
	// The receiver is in the correct state so we may proceed.
	TInt state = 0;
	TInt result = RProperty::Get(KDvbhPropertyCategory, KDvbhPropertyKeyExtBatteryState, state);
	if (result == KErrNone)
		{
		aState = static_cast<TDvbhExtBatteryState>(state);
		}
	return result;
	}

EXPORT_C TInt CDvbhReceiverInfo::SetBatteryStateObserver( MDvbhExtBatteryStateObserver& aObserver )
	{
	delete iData->iExtBatteryPropertyObserver;
	iData->iExtBatteryPropertyObserver = NULL;
	if ((iData->iExtBatteryPropertyObserver = new CDvbhPropertyObserver) == NULL)
		{
		return KErrNoMemory;
		}
	iData->iExtBatteryNotifier.SetExternalObserver(&aObserver);
	return iData->iExtBatteryPropertyObserver->SetObserver(&iData->iExtBatteryNotifier);
	}
    

EXPORT_C TInt CDvbhReceiverInfo::GetExtConnectionState( TDvbhExtConnectionState& aConnectionState, const TDvbhReceiverType& aReceiver )
	{
	// The receiver is in the correct state so we may proceed.
	TInt state = 0;
    TInt result = KErrNone;
    switch (aReceiver)
        {
        case EDvbhReceiverIntegrated:
            result = KErrArgument;
            break;
	    case EDvbhReceiverBluetooth:
            result = RProperty::Get(KDvbhPropertyCategory, KDvbhPropertyKeyExtConnectionState, state);
            break;
	    case EDvbhReceiverUsbAccessory:
            result = RProperty::Get(KDvbhPropertyCategory, KMtvAccAttachedKey, state);
            break;
        default:
            result = KErrArgument;
            break;
        }
    aConnectionState = EDvbhExtNotConnected;
    if (result == KErrNone)
		{
		aConnectionState = static_cast<TDvbhExtConnectionState>(state);
		}
	result = KErrNone;
    return result;
	}

EXPORT_C TInt CDvbhReceiverInfo::SetExtConnectionStateObserver( MDvbhExtConnectionObserver& aObserver )
	{
	delete iData->iExtConnectionPropertyObserver;
	delete iData->iMtvAccConnectionPropertyObserver;
	iData->iExtConnectionPropertyObserver = NULL;
	iData->iMtvAccConnectionPropertyObserver = NULL;
	if ((iData->iExtConnectionPropertyObserver = new CDvbhPropertyObserver) == NULL)
		{
		return KErrNoMemory;
		}
	if ((iData->iMtvAccConnectionPropertyObserver = new CDvbhPropertyObserver) == NULL)
		{
		return KErrNoMemory;
		}
	iData->iExtConnectionNotifier.SetExternalObserver(&aObserver);
	TInt result = iData->iExtConnectionPropertyObserver->SetObserver(&iData->iExtConnectionNotifier);
	if (result == KErrNone)
	    {
        iData->iMtvAccConnectionNotifier.SetExternalObserver(&aObserver);
        result = iData->iMtvAccConnectionPropertyObserver->SetObserver(&iData->iMtvAccConnectionNotifier);
	    }
    return result;
	}


EXPORT_C TInt CDvbhReceiverInfo::GetExtAntennaConnectionState( TDvbhExtAntennaConnectionState& aConnectionState, const TDvbhReceiverType& aReceiver )
	{
	// The receiver is in the correct state so we may proceed.
	TInt state = 0;
    TInt result = KErrNone;
    switch (aReceiver)
        {
        case EDvbhReceiverIntegrated:
            result = KErrArgument;
            break;
	    case EDvbhReceiverBluetooth:
            result = KErrArgument;
            break;
	    case EDvbhReceiverUsbAccessory:
            result = RProperty::Get(KDvbhPropertyCategory, KMtvAccAntennaKey, state);
            break;
        default:
            result = KErrArgument;
            break;
        }
    aConnectionState = EDvbhExtAntennaNotConnected;
    if (result == KErrNone)
		{
		aConnectionState = static_cast<TDvbhExtAntennaConnectionState>(state);
		}
	result = KErrNone;
    return result;
	}

EXPORT_C TInt CDvbhReceiverInfo::SetExtAntennaConnectionStateObserver( MDvbhExtAntennaConnectionObserver& aObserver )
	{
	delete iData->iMtvAccAntennaConnectionPropertyObserver;
	iData->iMtvAccAntennaConnectionPropertyObserver = NULL;
	if ((iData->iMtvAccAntennaConnectionPropertyObserver = new CDvbhPropertyObserver) == NULL)
		{
		return KErrNoMemory;
		}
	iData->iMtvAccAntennaConnectionNotifier.SetExternalObserver(&aObserver);
    return iData->iMtvAccAntennaConnectionPropertyObserver->SetObserver(&iData->iMtvAccAntennaConnectionNotifier);
	}

EXPORT_C TInt CDvbhReceiverInfo::GetExtDeviceInfo( TDvbhAccessoryInfo& aDeviceInfo, const TDvbhReceiverType& /*aReceiver*/ )
    {
	TPckg<TDvbhAccessoryInfo> devinfoPckg(aDeviceInfo);
	return RProperty::Get(KDvbhPropertyCategory, KMtvAccInfoKey, devinfoPckg);
    }
