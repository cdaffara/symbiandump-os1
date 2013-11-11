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
//

#include <e32base.h>
#include <e32def.h>
#include <bttypes.h>


#include <bluetooth/pan/BnepPacketNotifier.h>

const TInt KMaxBytesToSniff = 1000;
const TInt KTicksToSniff = 4;
const TInt KMinBytesToUnSniff = 100;

class TByteCount
	{
public:
	TInt iByteCount;
	TInt iTickCount;
	TBTDevAddr iAddr;
	};

NONSHARABLE_CLASS(CRefBnepPacketNotifier): public CBase, public MBnepPacketNotifier
	{
public:
	~CRefBnepPacketNotifier();
	static MBnepPacketNotifierBase* NewL();

	
	void MbpnBnepDataTransferred(const TArray<TBnepBytesTransferred> & );
	void MbpnSetLinkControl(MPanLinkControlBase& aPanLinkControl);
	void MbpnDeviceAdded(const TBTDevAddr& aAddr);
	void MbpnDeviceRemoved(const TBTDevAddr& aAddr);
	
	MBnepPacketNotifier* Interface();
	TAny* MbpnbGetInterface(const TUid & aInterface);
	void MbpnbRelease();

private:
	CRefBnepPacketNotifier();
	
private:
	MPanLinkControl* iPanLinkControl;
	RArray<TByteCount> iCounter;
	};
