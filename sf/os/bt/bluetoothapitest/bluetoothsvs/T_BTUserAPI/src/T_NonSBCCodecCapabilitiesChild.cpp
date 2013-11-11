/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "bluetoothav.h"

//	User Includes
#include "T_AvdtpServiceCapabilityData.h"
#include "T_AvdtpMediaCodecCapabilitiesData.h"
#include "T_NonSBCCodecCapabilitiesChild.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

T_NonSBCCodecCapabilitiesChild::T_NonSBCCodecCapabilitiesChild(SymbianBluetoothAV::TBluetoothMediaType aMediaType,
		TBluetoothMediaCodecType aCodecType)
:TNonSBCCodecCapabilities(aMediaType, aCodecType)
	{
	
	}

void T_NonSBCCodecCapabilitiesChild::SetMediaCodecType(TBluetoothMediaCodecType /*aMediaCodecType*/)
	{
	//TAvdtpMediaCodecCapabilities::SetMediaCodecType(aMediaCodecType);
	}

void T_NonSBCCodecCapabilitiesChild::SetMediaType(SymbianBluetoothAV::TBluetoothMediaType /*aMediaType*/)
	{
	//TAvdtpMediaCodecCapabilities::SetMediaType(aMediaType);
	}
