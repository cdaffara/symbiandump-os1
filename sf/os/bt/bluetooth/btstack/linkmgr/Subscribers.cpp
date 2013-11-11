// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements stack-side objects that subscribe to properties
// 
//

#include <bt_subscribe.h>
#include <bt_sock.h>
#include <utf.h>
#include <bluetooth/hci/hciconsts.h>
#include "Subscribers.h"
#include "linkmgr.h"
#include "physicallinksmanager.h"

CSubscriber::CSubscriber(CLinkMgrProtocol& aLinkMgrProtocol)
: CActive(CActive::EPriorityStandard), iParent(aLinkMgrProtocol)
	{
	CActiveScheduler::Add(this);
	}

void CSubscriber::DoCancel()
	{
	iProperty.Cancel();
	}

CSubscriber::~CSubscriber()
	{
	Cancel();
	iProperty.Close();
	}

TInt CSubscriber::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

void CSubscriber::ConstructL(const TUint aKey)
	{
	User::LeaveIfError(iProperty.Attach(KPropertyUidBluetoothCategory,
										aKey));
	Subscribe();
	}

void CSubscriber::Subscribe()
   	{
   	iProperty.Subscribe(iStatus);
   	SetActive();
   	}

CDiscoverabilitySubscriber* CDiscoverabilitySubscriber::NewL(CLinkMgrProtocol& aLinkMgrProtocol)
	{
	CDiscoverabilitySubscriber* s = new(ELeave) CDiscoverabilitySubscriber(aLinkMgrProtocol);
	CleanupStack::PushL(s);
	s->ConstructL(KPropertyKeyBluetoothSetScanningStatus);
	CleanupStack::Pop(s);
	return s;
	}

CDiscoverabilitySubscriber::CDiscoverabilitySubscriber(CLinkMgrProtocol& aLinkMgrProtocol)
: CSubscriber(aLinkMgrProtocol)
	{
	}

void CDiscoverabilitySubscriber::RunL()
	{
	Subscribe();
	iParent.SetInquiryAndPageScanningL();
	}


CLimitedSubscriber* CLimitedSubscriber::NewL(CLinkMgrProtocol& aLinkMgrProtocol)
	{
	CLimitedSubscriber* s = new(ELeave) CLimitedSubscriber(aLinkMgrProtocol);
	CleanupStack::PushL(s);
	s->ConstructL(KPropertyKeyBluetoothSetLimitedDiscoverableStatus);
	CleanupStack::Pop(s);
	return s;
	}

CLimitedSubscriber::CLimitedSubscriber(CLinkMgrProtocol& aLinkMgrProtocol)
: CSubscriber(aLinkMgrProtocol)
	{
	}

void CLimitedSubscriber::RunL()
	{
	Subscribe();
	TBool limited;
	iProperty.Get(limited);
	iParent.SetLimitedDiscoverableIfChangedL(limited);
	}

//-----------------

CDeviceClassSubscriber* CDeviceClassSubscriber::NewL(CLinkMgrProtocol& aLinkMgrProtocol)
	{
	CDeviceClassSubscriber* s = new(ELeave) CDeviceClassSubscriber(aLinkMgrProtocol);
	CleanupStack::PushL(s);
	s->ConstructL(KPropertyKeyBluetoothSetDeviceClass);
	CleanupStack::Pop(s);
	return s;
	}

CDeviceClassSubscriber::CDeviceClassSubscriber(CLinkMgrProtocol& aLinkMgrProtocol)
: CSubscriber(aLinkMgrProtocol)
	{
	}

void CDeviceClassSubscriber::RunL()
	{
	Subscribe();
	TInt cod;
	iProperty.Get(cod);
	iParent.SetDeviceClassL(static_cast<TUint32>(cod));
	}

//-----------------

CDeviceNameSubscriber* CDeviceNameSubscriber::NewL(CLinkMgrProtocol& aLinkMgrProtocol)
	{
	CDeviceNameSubscriber* s = new(ELeave) CDeviceNameSubscriber(aLinkMgrProtocol);
	CleanupStack::PushL(s);
	s->ConstructL(KPropertyKeyBluetoothSetDeviceName);
	CleanupStack::Pop(s);
	return s;
	}

CDeviceNameSubscriber::CDeviceNameSubscriber(CLinkMgrProtocol& aLinkMgrProtocol)
: CSubscriber(aLinkMgrProtocol)
	{
	}

void CDeviceNameSubscriber::RunL()
	{
	Subscribe();
	
	// The P&S value is in Unicode for the convenience of clients ...
	TBuf16<KHCILocalDeviceNameMaxLength> unicodeName;
	iProperty.Get(unicodeName);
	
	// ... but the BT spec requires the device name in UTF-8 so do the conversion here:
	TBuf8<KHCILocalDeviceNameMaxLength> utf8Name;
	TInt ret = CnvUtfConverter::ConvertFromUnicodeToUtf8(utf8Name, unicodeName);

	// ret should never be > 0 since unicodeName is the same size as utf8Name hence we 
	// shouldn't have any unconverted chars left over.
	if (ret == KErrNone)
		{
		iParent.SetLocalDeviceName(utf8Name);
		}
	else
		{
		// Set the P&S key with the old value so that clients can figure out their set failed.
		TBuf16<KHCILocalDeviceNameMaxLength> oldUnicodeName;
		RProperty::Get(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothGetDeviceName, oldUnicodeName);
		RProperty::Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothGetDeviceName, oldUnicodeName);
		}
	}

//-----------------

CAFHChannelAssessmentModeSubscriber* CAFHChannelAssessmentModeSubscriber::NewL(CLinkMgrProtocol& aLinkMgrProtocol)
	{
	CAFHChannelAssessmentModeSubscriber* s = new(ELeave) CAFHChannelAssessmentModeSubscriber(aLinkMgrProtocol);
	CleanupStack::PushL(s);
	s->ConstructL(KPropertyKeyBluetoothSetAFHChannelAssessmentMode);
	CleanupStack::Pop(s);
	return s;
	}

CAFHChannelAssessmentModeSubscriber::CAFHChannelAssessmentModeSubscriber(CLinkMgrProtocol& aLinkMgrProtocol)
: CSubscriber(aLinkMgrProtocol)
	{
	}

void CAFHChannelAssessmentModeSubscriber::RunL()
	{
	//Get this value first before we subscribe again
	TInt ret = iStatus.Int();
	Subscribe();
	if(ret==KErrNone)
		{
		TInt mode;
		ret = iProperty.Get(mode);
		if(ret==KErrNone)
			{
			iParent.SetAFHChannelAssessmentModeL(static_cast<TBool>(mode));
			}
		}
	}


//-----------------

CAFHHostChannelClassificationSubscriber* CAFHHostChannelClassificationSubscriber::NewL(CLinkMgrProtocol& aLinkMgrProtocol)
	{
	CAFHHostChannelClassificationSubscriber* s = new(ELeave) CAFHHostChannelClassificationSubscriber(aLinkMgrProtocol);
	CleanupStack::PushL(s);
	s->ConstructL(KPropertyKeyBluetoothSetAFHHostChannelClassification);
	CleanupStack::Pop(s);
	return s;
	}

CAFHHostChannelClassificationSubscriber::CAFHHostChannelClassificationSubscriber(CLinkMgrProtocol& aLinkMgrProtocol)
: CSubscriber(aLinkMgrProtocol)
	{
	}

void CAFHHostChannelClassificationSubscriber::RunL()
	{
	//Get this value first before we subscribe again
	TInt ret = iStatus.Int();
	Subscribe();
	if(ret==KErrNone)
		{
		TBTAFHHostChannelClassification classification;
		ret = iProperty.Get(classification);
		if(ret==KErrNone)
			{
			iParent.SetAFHHostChannelClassificationL(classification);
			}
		}
	}

//-----------------

CRegistryRemoteTableSubscriber* CRegistryRemoteTableSubscriber::NewL(CLinkMgrProtocol& aLinkMgrProtocol)
	{
	CRegistryRemoteTableSubscriber* s = new(ELeave) CRegistryRemoteTableSubscriber(aLinkMgrProtocol);
	CleanupStack::PushL(s);
	s->ConstructL(KPropertyKeyBluetoothGetRegistryTableChange);
	CleanupStack::Pop(s);
	return s;
	}

CRegistryRemoteTableSubscriber::CRegistryRemoteTableSubscriber(CLinkMgrProtocol& aLinkMgrProtocol)
: CSubscriber(aLinkMgrProtocol)
	{
	}

void CRegistryRemoteTableSubscriber::RunL()
	{
	//Get this value first before we subscribe again
	TInt ret = iStatus.Int();
	Subscribe();
	
	if(ret==KErrNone)
		{
		// To accommodate race conditions, do this whichever table has been updated.
		iParent.PhysicalLinksMgr().UpdateRemoteDevicesDetails();
		}
	}

//-----------------

CAcceptPairedOnlySubscriber* CAcceptPairedOnlySubscriber::NewL(CLinkMgrProtocol& aLinkMgrProtocol)
	{
	CAcceptPairedOnlySubscriber* s = new(ELeave) CAcceptPairedOnlySubscriber(aLinkMgrProtocol);
	CleanupStack::PushL(s);
	s->ConstructL(KPropertyKeyBluetoothSetAcceptPairedOnlyMode);
	CleanupStack::Pop(s);
	return s;
	}

CAcceptPairedOnlySubscriber::CAcceptPairedOnlySubscriber(CLinkMgrProtocol& aLinkMgrProtocol)
: CSubscriber(aLinkMgrProtocol)
	{
	}

void CAcceptPairedOnlySubscriber::RunL()
	{
	//Get this value first before we subscribe again
	TInt ret = iStatus.Int();
	Subscribe();
	
	if(ret==KErrNone)
		{
		TInt acceptPairedOnlyMode;
		ret = iProperty.Get(acceptPairedOnlyMode);
		if(ret==KErrNone)
			{
			iParent.PhysicalLinksMgr().SetAcceptPairedOnlyMode(static_cast<TBool>(acceptPairedOnlyMode));
			}
		}
	}


//-----------------

CDebugModeSubscriber* CDebugModeSubscriber::NewL(CLinkMgrProtocol& aLinkMgrProtocol)
	{
	CDebugModeSubscriber* s = new(ELeave) CDebugModeSubscriber(aLinkMgrProtocol);
	CleanupStack::PushL(s);
	s->ConstructL(KPropertyKeyBluetoothSetSimplePairingDebugMode);
	CleanupStack::Pop(s);
	return s;
	}

CDebugModeSubscriber::CDebugModeSubscriber(CLinkMgrProtocol& aLinkMgrProtocol)
	: CSubscriber(aLinkMgrProtocol)
	{
	}

void CDebugModeSubscriber::RunL()
	{
	//Get this value first before we subscribe again
	TInt ret = iStatus.Int();
	Subscribe();
	
	if(ret==KErrNone)
		{
		TInt debugMode;
		ret = iProperty.Get(debugMode);
		if(ret==KErrNone)
			{
			iParent.PhysicalLinksMgr().SetSimplePairingDebugMode(static_cast<TBool>(debugMode));
			}
		}
	}

//-----------------

CPageScanParametersSubscriber* CPageScanParametersSubscriber::NewL(CLinkMgrProtocol& aLinkMgrProtocol)
	{
	CPageScanParametersSubscriber* s = new(ELeave) CPageScanParametersSubscriber(aLinkMgrProtocol);
	CleanupStack::PushL(s);
	s->ConstructL(KPropertyKeyBluetoothSetPageScanParameters);
	CleanupStack::Pop(s);
	return s;
	}

CPageScanParametersSubscriber::CPageScanParametersSubscriber(CLinkMgrProtocol& aLinkMgrProtocol)
: CSubscriber(aLinkMgrProtocol)
	{
	}

void CPageScanParametersSubscriber::RunL()
	{
	//Get this value first before we subscribe again
	TInt ret = iStatus.Int();
	Subscribe();

	if(ret==KErrNone)
		{
		TInt pageScanParameters;
		ret = iProperty.Get(pageScanParameters);
		if(ret == KErrNone)
			{
			iParent.SetPageScanParameters(static_cast<TPageScanParameterSettings>(pageScanParameters));
			}
		}
	}
