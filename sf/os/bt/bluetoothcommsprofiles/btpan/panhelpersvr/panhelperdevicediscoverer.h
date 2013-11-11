// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PANHELPERDEVICEDISCOVERER_H
#define PANHELPERDEVICEDISCOVERER_H

#include <e32base.h>
#include <e32std.h>
#include <btsdp.h>
#include <es_sock.h>
#include <bt_sock.h>

namespace PanHelper
	{
	
const TUint KCharComma = ',';

NONSHARABLE_CLASS(CPanHelperDeviceDiscoverer) : public CActive
/**
Helper class to perform the device discovery
@internalTechnology
*/
	{
public:
	static CPanHelperDeviceDiscoverer* NewL();
	~CPanHelperDeviceDiscoverer();
	
	// CActive methods
	virtual void RunL();
	virtual void DoCancel();
	
	void FindDevice(TDesC& aDeviceAddrList, TDes8& aDeviceAddr, TRequestStatus& aStatus);
	void CancelFindDevice();
	

private:
	CPanHelperDeviceDiscoverer();
	void ConstructL();
	
	void ConvertListToArray(TDesC& aDeviceAddrList);
public:

private:
	RSocketServ iSockServ;
	RHostResolver iHostResolver;
	TInquirySockAddr iInquiryAddr;
	TNameEntry iNameEntry;
	RArray<TBTDevAddr> iDeviceAddresses;
	TPckgBuf<TBTDevAddr>* iRemDevAddrBuf;
	TRequestStatus* iClientStatus;
	};
	}
#endif //PANHELPERDEVICEDISCOVERER_H
