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

/**
 @file
 @note PAN helper server client API
 @internalTechnology
*/

#ifndef PANHELPER_H
#define PANHELPER_H

#include <e32std.h>
#include <e32base.h>
#include <es_sock.h>
#include <bt_sock.h>
#include "pancommon.h"
#include "panprog.h"


namespace PanHelper
{

_LIT(KPanHelperName, "PANHelper");

enum TPanHelperPanic
/**
Panic helper server panic codes
@internalComponent
*/
	{
	EInvalidIapId,
	ENullSdpAttrValue,								//< The SDP query returned a NULL SDP attribute value record
	ESecondQueryWhileFirstStillActive, //< A second SDP query was initiated while the first was still active
	};

void PanHelperPanic(TPanHelperPanic aPanicCode);

/** 
Flags for support of various roles
@see TPanDeviceRolesList
@internalComponent
*/
//@{
const TUint KPanDeviceRolesListNone	  = 0x00;
const TUint KPanDeviceRolesListPanU   = 0x01;
const TUint KPanDeviceRolesListPanGn  = 0x02;
const TUint KPanDeviceRolesListPanNap = 0x04;
const TUint KPanDeviceRolesListAll	  = KPanDeviceRolesListPanU | KPanDeviceRolesListPanGn | KPanDeviceRolesListPanNap;
//@}

NONSHARABLE_CLASS(TPanDeviceRolesList)
/**
PAN roles array, to provide an easy way of passing around which roles a remote device supports
@internalTechnology
@released
*/
	{	
public:

	IMPORT_C TPanDeviceRolesList();
	
	IMPORT_C TBool SupportsU() const;
	IMPORT_C TBool SupportsGn() const;
	IMPORT_C TBool SupportsNap() const;
	
	IMPORT_C void SetSupportsU();
	IMPORT_C void SetSupportsU(TBool aSupportsU);
	IMPORT_C void SetSupportsGn();
	IMPORT_C void SetSupportsGn(TBool aSupportsGn);
	IMPORT_C void SetSupportsNap();
	IMPORT_C void SetSupportsNap(TBool aSupportsNap);

	IMPORT_C TBool IsSupported(TBluetoothPanRole aPanRole) const;

private:
	TInt iSupportedRoles;
	};
	
NONSHARABLE_CLASS(TPanDeviceWorthTryingRolesList)
/**
PAN roles array, to provide an easy way of passing around which roles are worth trying with a remote device
@internalTechnology
@released
*/
	{	
public:

	IMPORT_C TPanDeviceWorthTryingRolesList();

	IMPORT_C TBool IsWorthTryingU() const;
	IMPORT_C TBool IsWorthTryingGn() const;
	IMPORT_C TBool IsWorthTryingNap() const;
	
	IMPORT_C void SetWorthTrying(TBluetoothPanRole aPanRole, TBool aWorthTrying);

	IMPORT_C void SetWorthTryingU();
	IMPORT_C void SetWorthTryingU(TBool aWorthTryingU);
	IMPORT_C void SetWorthTryingGn();
	IMPORT_C void SetWorthTryingGn(TBool aWorthTryingGn);
	IMPORT_C void SetWorthTryingNap();
	IMPORT_C void SetWorthTryingNap(TBool aWorthTryingNap);

	IMPORT_C TBool IsWorthTryingARole(TBluetoothPanRole aPanRole) const;
	IMPORT_C void ResetWorthTryingRoles();

private:
	TInt iWorthTryingRoles;
	};
	
	
class CPanHelperRemoteSdpQuerier;
NONSHARABLE_CLASS(RPanRemoteSdpQuerier) 
/**
Client side of PAN SDP query engine, for looking up SDP records on remote devices
@internalTechnology
@released
*/
	{
public:
	IMPORT_C TInt Open();
	IMPORT_C void Close();
		
	IMPORT_C TInt Query(const TBTDevAddr& aAddr, TDes8& aSupportedRemoteRoles, TRequestStatus& aStatus);
	IMPORT_C void CancelQuery();

private:
	CPanHelperRemoteSdpQuerier* iSdpQuerier;
	};
	
class CPanHelperDeviceDiscoverer;
NONSHARABLE_CLASS(RPanRemoteDeviceDiscoverer) 
/**
Client side of remote device discovery engine, for discovering which devices are around us and
selecting one from the supplied list
@internalTechnology
@released
*/
	{
public:
	IMPORT_C TInt Open();
	IMPORT_C void Close();
		
	IMPORT_C void FindDevice(TDesC& aDeviceAddrList, TDes8& aDeviceAddr, TRequestStatus& aStatus);
	IMPORT_C void CancelFindDevice();

private:
	CPanHelperDeviceDiscoverer* iDeviceDiscoverer;
	};
	
} // PanHelper

#endif // PANHELPER_H
