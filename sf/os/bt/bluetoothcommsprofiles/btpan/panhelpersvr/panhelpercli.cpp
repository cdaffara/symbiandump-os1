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

#include <c32root.h>
#include "panhelper.h"
#include "panhelperdevicediscoverer.h"
#include "panhelperremotesdp.h"

using namespace PanHelper;

/**
@file
@internalTechnology
*/


//
// PAN device roles list
//

EXPORT_C TPanDeviceRolesList::TPanDeviceRolesList()
/**
Set the initially supported set of roles to none
*/
	{
	iSupportedRoles = KPanDeviceRolesListNone;
	}

EXPORT_C TBool TPanDeviceRolesList::SupportsU() const
/**
Does the device support the PAN-U role?
*/
	{
	return((iSupportedRoles & KPanDeviceRolesListPanU) ? ETrue : EFalse);
	}

EXPORT_C TBool TPanDeviceRolesList::SupportsGn() const
/**
Does the device support the PAN-GN role?
*/
	{
	return((iSupportedRoles & KPanDeviceRolesListPanGn) ? ETrue : EFalse);
	}

EXPORT_C TBool TPanDeviceRolesList::SupportsNap() const
/**
Does the device support the PAN-NAP role?
*/
	{
	return((iSupportedRoles & KPanDeviceRolesListPanNap) ? ETrue : EFalse);
	}
	
EXPORT_C void TPanDeviceRolesList::SetSupportsU()
/**
Indicate that the device supports PAN-U
*/
	{
	iSupportedRoles = iSupportedRoles | KPanDeviceRolesListPanU;
	}

EXPORT_C void TPanDeviceRolesList::SetSupportsU(TBool aSupportsU)
/**
Set whether the device supports PAN-U or not
@param aSupportsU Indicate whether the device support PAN-U role or not
*/
	{
	if(aSupportsU)
		{
		SetSupportsU();
		}
	else
		{
		iSupportedRoles = iSupportedRoles & ~KPanDeviceRolesListPanU;	
		}
	}
	
EXPORT_C void TPanDeviceRolesList::SetSupportsGn()
/**
Indicate that the remote device supports PAN-GN
*/
	{
	iSupportedRoles = iSupportedRoles | KPanDeviceRolesListPanGn;
	}

EXPORT_C void TPanDeviceRolesList::SetSupportsGn(TBool aSupportsGn)
/**
Indicate that the remote device supports PAN-GN
@param aSupportsGn Indicate whether the remote device support PAN-GN role or not
*/
	{
	if(aSupportsGn)
		{
		SetSupportsGn();
		}
	else
		{
		iSupportedRoles = iSupportedRoles & ~KPanDeviceRolesListPanGn;	
		}
	}

EXPORT_C void TPanDeviceRolesList::SetSupportsNap()
/**
Indicate that the remote device supports PAN-NAP
*/
	{
	iSupportedRoles = iSupportedRoles | KPanDeviceRolesListPanNap;
	}

EXPORT_C void TPanDeviceRolesList::SetSupportsNap(TBool aSupportsNap)
/**
Indicate that the remote device supports PAN-NAP
@param aSupportsNap Indicate whether the remote device support PAN-NAP role or not
*/
	{
	if(aSupportsNap)
		{
		SetSupportsNap();
		}
	else
		{
		iSupportedRoles = iSupportedRoles & ~KPanDeviceRolesListPanNap;	
		}
	}
	
EXPORT_C TBool TPanDeviceRolesList::IsSupported(TBluetoothPanRole aPanRole) const
/**
Checks whether the supplied role is supported
@param aPanRole The role to check
@return ETrue if the role is supported, else EFalse
*/
	{
	switch(aPanRole)
		{
		case EPanRoleU:
			return(SupportsU());
		case EPanRoleGn:
			return(SupportsGn());
		case EPanRoleNap:
			return(SupportsNap());
		default:
			return EFalse;
		}
	}


EXPORT_C TPanDeviceWorthTryingRolesList::TPanDeviceWorthTryingRolesList()
/**
Set the initial set of worth trying roles to all
*/
	{
	iWorthTryingRoles = KPanDeviceRolesListAll;
	}


EXPORT_C TBool TPanDeviceWorthTryingRolesList::IsWorthTryingU() const
/**
Is the PAN-U role worth trying?
*/
	{
	return((TBool)(iWorthTryingRoles & KPanDeviceRolesListPanU));
	}

EXPORT_C TBool TPanDeviceWorthTryingRolesList::IsWorthTryingGn() const
/**
Is the PAN-Gn role worth trying?
*/
	{
	return((TBool)(iWorthTryingRoles & KPanDeviceRolesListPanGn));
	}

EXPORT_C TBool TPanDeviceWorthTryingRolesList::IsWorthTryingNap() const
/**
Is the PAN-Nap role worth trying?
*/
	{
	return((TBool)(iWorthTryingRoles & KPanDeviceRolesListPanNap));
	}
	
EXPORT_C void TPanDeviceWorthTryingRolesList::SetWorthTrying(TBluetoothPanRole aPanRole, TBool aWorthTrying)
	{
	switch(aPanRole)
		{ 
		case EPanRoleU:
			SetWorthTryingU(aWorthTrying);
			break;
		case EPanRoleGn:
			SetWorthTryingGn(aWorthTrying);
			break;
		case EPanRoleNap:
			SetWorthTryingNap(aWorthTrying);
			break;
		}
	}

EXPORT_C void TPanDeviceWorthTryingRolesList::SetWorthTryingU()
/**
Set the PAN-U role to worth trying
*/
	{
	iWorthTryingRoles = iWorthTryingRoles | KPanDeviceRolesListPanU;
	}

EXPORT_C void TPanDeviceWorthTryingRolesList::SetWorthTryingU(TBool aWorthTryingU)
/**
Set whether we the PAN-U role is worth trying
@param aWorthTryingU Indicate whether the PAN-U role is worth trying with the remote device
*/
	{
	if(aWorthTryingU)
		{
		SetWorthTryingU();
		}
	else
		{
		iWorthTryingRoles = iWorthTryingRoles & ~KPanDeviceRolesListPanU;	
		}
	}
	
EXPORT_C void TPanDeviceWorthTryingRolesList::SetWorthTryingGn()
/**
Set the PAN-Gn role to worth trying
*/
	{
	iWorthTryingRoles = iWorthTryingRoles | KPanDeviceRolesListPanGn;
	}

EXPORT_C void TPanDeviceWorthTryingRolesList::SetWorthTryingGn(TBool aWorthTryingGn)
/**
Set whether the PAN-Gn role is worth trying
@param aWorthTryingGn Indicate whether or not the PAN-GN role is worth trying with the remote device
*/
	{
	if(aWorthTryingGn)
		{
		SetWorthTryingGn();
		}
	else
		{
		iWorthTryingRoles = iWorthTryingRoles & ~KPanDeviceRolesListPanGn;	
		}
	}

EXPORT_C void TPanDeviceWorthTryingRolesList::SetWorthTryingNap()
/**
Set the PAN-Nap role to worth trying
*/
	{
	iWorthTryingRoles = iWorthTryingRoles | KPanDeviceRolesListPanNap;
	}

EXPORT_C void TPanDeviceWorthTryingRolesList::SetWorthTryingNap(TBool aWorthTryingNap)
/**
Set whether the PAN-Nap role is worth trying
@param aWorthTryingNap Indicate whether or not the PAN-NAP role is worth trying with the remote device
*/
	{
	if(aWorthTryingNap)
		{
		SetWorthTryingNap();
		}
	else
		{
		iWorthTryingRoles = iWorthTryingRoles & ~KPanDeviceRolesListPanNap;	
		}
	}
	
EXPORT_C TBool TPanDeviceWorthTryingRolesList::IsWorthTryingARole(TBluetoothPanRole aPanRole) const
/**
Checks whether the supplied role is worth trying
@param aPanRole The role to check
@return ETrue if the role is worth trying, else ETrue (safer return for the use of this)
*/
	{
	TBool isWorthTryingRole = ETrue;

	switch(aPanRole)
		{
		case EPanRoleU:
			isWorthTryingRole = IsWorthTryingU();
			break;
		case EPanRoleGn:
			isWorthTryingRole = IsWorthTryingGn();
			break;
		case EPanRoleNap:
			isWorthTryingRole = IsWorthTryingNap();
			break;
		}
	return(isWorthTryingRole);
	}


EXPORT_C void TPanDeviceWorthTryingRolesList::ResetWorthTryingRoles()
/**
Clear all the worth trying flag so all roles are worth trying now
*/
	{
	iWorthTryingRoles = KPanDeviceRolesListAll;
	}
	
//
// Remote SDP querier
//

EXPORT_C TInt RPanRemoteSdpQuerier::Open()
/**
Open a handle to the PAN remote SDP query service
*/
	{
	TRAPD(err, iSdpQuerier = CPanHelperRemoteSdpQuerier::NewL());
	return err;
	}
	
EXPORT_C void RPanRemoteSdpQuerier::Close()
/**

*/
	{
	if(iSdpQuerier)
		{
		delete iSdpQuerier;
		iSdpQuerier = NULL;
		}
	}
	
EXPORT_C TInt RPanRemoteSdpQuerier::Query(const TBTDevAddr& aAddr, TDes8& aSupportedRemoteRoles, TRequestStatus& aStatus)
/**
Query the given remote device to find out what roles it supports
@param aAddr A descriptor-packaged Bluetooth device address of the device to query
@param aSupportedRemoteRoles A descriptor-packaged remote role class.  On return, contains the roles that the remote device is advertising in SDP
@param aStatus The TRequestStatus to be signalled on completion
*/
	{
	aStatus = KRequestPending;
	TRAPD(err, iSdpQuerier->QueryL(aAddr, aSupportedRemoteRoles, aStatus));
	return err;
	}
	
EXPORT_C void RPanRemoteSdpQuerier::CancelQuery()
/**
Cancel the SDP query of the remote device
*/
	{
	iSdpQuerier->CancelQuery();
	}
	
//
// Remote device discovery
//

EXPORT_C TInt RPanRemoteDeviceDiscoverer::Open()
/**

*/
	{
	TRAPD(rerr, iDeviceDiscoverer = CPanHelperDeviceDiscoverer::NewL());
	return rerr;
	}
	
EXPORT_C void RPanRemoteDeviceDiscoverer::Close()
/**

*/
	{
	if(iDeviceDiscoverer)
		{
		delete iDeviceDiscoverer;
		iDeviceDiscoverer = NULL;
		}
	}
		
EXPORT_C void RPanRemoteDeviceDiscoverer::FindDevice(TDesC& aDeviceAddrList, TDes8& aDeviceAddr, TRequestStatus& aStatus)
/**
Find a device from the supplied list of comma-seperated readable BT device addresses
@param aDeviceAddrList A comma-separated list of BT dev addrs in readable format
@param aDeviceAddr On return, contains a device address from the list that was found; if no devices
are found then the method completes with an error code
*/
	{
	aStatus = KRequestPending;
	iDeviceDiscoverer->FindDevice(aDeviceAddrList, aDeviceAddr, aStatus);
	}
	
EXPORT_C void RPanRemoteDeviceDiscoverer::CancelFindDevice()
/**

*/
	{
	iDeviceDiscoverer->CancelFindDevice();
	}
