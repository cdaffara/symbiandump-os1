/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Internal interface and structures for sending vendor specific frames
* and receiving associated events
* 
*
*/



/**
 @publishedPartner
 @released
*/

#ifndef __VENDORSPECIFIC_H__
#define __VENDORSPECIFIC_H__

#include <e32std.h>
#include <bluetooth/hci/hcitypes.h>

#include <bluetooth/hci/hciconsts.h>
#include <bluetooth/hci/hciframe.h>

//
// Vendor specific ioctl values
//
static const TUint KHCIWriteVendorSpecificFrameIoctl				= 28;
	//writes a command to hci and completes on command complete event
static const TUint KHCIWriteVendorSpecificFrameNoEventExpectedIoctl	= 29;
	//writes a command to hci and completes immediately
static const TUint KHCIWaitForVendorSpecificDebugEventIoctl			= 32;
	//waits for a vendor debug event before completion

//
// Baseband SAP type for a vendor specific SAP
//
static const TUint KSockBluetoothTypeVendorSpecific = KMaxTUint - 3;

struct THCIWriteVendorSpecificFrameIoctl
/**
 * Structure for writing a vendor-specific frame to
 * the host controller.  Specify the OCF required
 * along with an OGF of KVendorDebugOGF in iOpcode
 * and the correctly formatted parameters in iParams
 *
 * Use with KHCIWriteVendorSpecificFrameIoctl
 *
 * @publishedPartner
 * @released
 */
	{
	THCIOpcode iOpcode;
	TBuf8<CHctlCommandFrame::KHCIMaxCommandLength> iParams;
	};

//
// Package for sending a THCIWriteVendorSpecificFrameIoctl via IPC
//
typedef TPckgBuf<THCIWriteVendorSpecificFrameIoctl> THCIWriteVendorSpecificFrameBuf;

#endif //__VENDORSPECIFIC_H__
