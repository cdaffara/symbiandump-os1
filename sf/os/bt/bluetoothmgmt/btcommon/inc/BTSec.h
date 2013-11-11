// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Bluetooth security component
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef BTSEC_H
#define BTSEC_H

#include <e32std.h>

const TUint32 KBTManServerSID = 0x100069cc;
const TUint32 KSdpServerSID = 0x10009220;

_LIT_SECURITY_POLICY_C1(KLOCAL_SERVICES,ECapabilityLocalServices);
_LIT_SECURITY_POLICY_S1(KBTMAN_SID_PROT_SERV,KBTManServerSID,ECapabilityProtServ);
_LIT_SECURITY_POLICY_C2(KLOCAL_SERVICES_AND_NETWORK_CONTROL,ECapabilityLocalServices,ECapabilityNetworkControl);
_LIT_SECURITY_POLICY_C1(KNETWORK_CONTROL, ECapabilityNetworkControl);
_LIT_SECURITY_POLICY_C2(KLOCAL_SERVICES_AND_READ_DEVICE_DATA,ECapabilityLocalServices,ECapabilityReadDeviceData);
_LIT_SECURITY_POLICY_C2(KLOCAL_SERVICES_AND_WRITE_DEVICE_DATA,ECapabilityLocalServices,ECapabilityWriteDeviceData);
_LIT_SECURITY_POLICY_S1(KSDP_SID_PROT_SERV, KSdpServerSID,ECapabilityProtServ);

_LIT_SECURITY_POLICY_C1(KCOMMDD,ECapabilityCommDD);

_LIT(KSECURITY_PANIC,"BT Security Checker invalid");

enum TSecurityPanics
	{
	EBTPanicNullSecurityChecker	
	};


#endif //BTSEC_H
