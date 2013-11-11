// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PANPROG_H
#define PANPROG_H

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <panctrl.h>
#endif

#include <nifvar.h>
#include <es_sock.h>

enum TPANAgentProgress
/**
PAN agent progress values
@publishedAll
@released
*/
	{
	EPanAgtInitialising		= KMinAgtProgress,
	EPanAgtConnected        = KConnectionOpen,		//< Agent is up and running
	EPanAgtIdle,									//< Agent is idle
	EPanAgtListening,								//< Listening for incoming connections
	EPanAgtURole,									//< In U role
	EPanAgtGnRole,									//< In GN role
	EPanAgtNapRole,									//< In NAP role
	EPanAgtConnectedNewDevice,						//< A device has connected
	EPanAgtDisconnectedDevice,						//< A device has disconnected
	EPanAgtReconfiguringPiconet,					//< Role change is in progress
	EPanAgtUplinkRequired,							//< A connection authorised to use the uplink exists
	EPanAgtUplinkNotRequired,						//< A connection authorised to use the uplink does not exists
	EPanAgtNoResourceForPanConnection,              //< Lack of resource to setup a BT PAN connection
	EPanAgtDisconnecting 	= KConnectionStartingClose,
	EPanAgtDisconnected     = KConnectionClosed
	};
	
enum TPanNapNetworkType
/**
Type of network provided by the NAP gateway
@publishedAll
@released
*/
	{
	EPstn = 0x0000,
	EIsdn = 0x0001,
	EDsl = 0x0002,
	ECableModem = 0x0003,
	E10MbEthernet = 0x0004,
	E100MbEthernet = 0x0005,
	E4MbTokenRing = 0x0006,
	E16MbTokenRing = 0x0007,
	E100MbTokenRing = 0x0008,
	EFddi = 0x0009,
	EGsm = 0x000A,
	ECdma = 0x000B,
	EGprs = 0x000C,
	E3GCellular = 0x000D,
	EOther = 0xFFFE
	};

#endif // PANPROG_H
