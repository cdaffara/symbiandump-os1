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

inline const TBTDevAddr& CPhysicalLink::BDAddr() const
	{
	return iDevice.Address();
	}

inline const TBTBasebandLinkState& CPhysicalLink::LinkState() const
	{
	return iLinkState;
	}

inline TBool CPhysicalLink::IsConnected() const
	{
	return (iLinkState.LinkState() == TBTBasebandLinkState::ELinkUp);
	}

inline THCIConnHandle CPhysicalLink::Handle() const
	{
	return iHandle;
	}

inline TBTBasebandRole CPhysicalLink::Role() const
	{
	return iLinkState.LinkRole();
	}

inline TBool CPhysicalLink::IsParked() const
	{
	return (iLinkState.LinkMode() == EParkMode);
	}

inline TBool CPhysicalLink::Authenticated() const
	{
	return iLinkState.Authenticated();
	}

inline TBool CPhysicalLink::Encrypted() const
	{
	return iLinkState.Encrypted();
	}

inline const TBTNamelessDevice& CPhysicalLink::RemoteDevice() const
	{
	return iDevice;
	}

inline TBTLinkMode CPhysicalLink::LinkMode() const
	{
	return (iLinkState.LinkMode());
	}

inline const TLinkPolicy& CPhysicalLink::LinkPolicy() const
	{
	return iLinkPolicy;
	}

inline const TBTDeviceClass& CPhysicalLink::DeviceClass() const
	{
	return iDevice.DeviceClass();
	}

inline const TBTFeatures& CPhysicalLink::RemoteFeatures() const
	{
	return iRemoteFeatures;
	}

inline TBool CPhysicalLink::IsAuthenticationPending() const
	{
	return (iAuthStateMask != 0);
	}
