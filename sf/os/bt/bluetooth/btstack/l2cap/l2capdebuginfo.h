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

/**
 @file
 @internalComponent
*/

#ifndef L2CAPDEBUGINFO_H
#define L2CAPDEBUGINFO_H

#include <e32base.h>

// L2CAP DEBUG P&S 
const TUint KUidBluetoothDebugPubSubKeyBase = 0x10272368; //	Range of 550 values registered

// L2CAP debug P&S keys.
const TUint KPropertyKeyBluetoothL2CapDebugBase = (KUidBluetoothDebugPubSubKeyBase + 0);

const TUint KPropertyKeyBluetoothBasePDUAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x00);
const TUint KPropertyKeyBluetoothBasePDUDel		= (KUidBluetoothDebugPubSubKeyBase + 0x01);
const TUint KPropertyKeyBluetoothBasePDUPeak	= (KUidBluetoothDebugPubSubKeyBase + 0x02);

const TUint KPropertyKeyBluetoothBFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x10);
const TUint KPropertyKeyBluetoothBFrameDel		= (KUidBluetoothDebugPubSubKeyBase + 0x11);
const TUint KPropertyKeyBluetoothBFramePeak		= (KUidBluetoothDebugPubSubKeyBase + 0x12);
const TUint KPropertyKeyBluetoothIFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x20);
const TUint KPropertyKeyBluetoothIFrameDel		= (KUidBluetoothDebugPubSubKeyBase + 0x21);
const TUint KPropertyKeyBluetoothIFramePeak		= (KUidBluetoothDebugPubSubKeyBase + 0x22);
const TUint KPropertyKeyBluetoothSFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x30);
const TUint KPropertyKeyBluetoothSFrameDel		= (KUidBluetoothDebugPubSubKeyBase + 0x31);
const TUint KPropertyKeyBluetoothSFramePeak		= (KUidBluetoothDebugPubSubKeyBase + 0x32);
const TUint KPropertyKeyBluetoothCFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x40);
const TUint KPropertyKeyBluetoothCFrameDel		= (KUidBluetoothDebugPubSubKeyBase + 0x41);
const TUint KPropertyKeyBluetoothCFramePeak		= (KUidBluetoothDebugPubSubKeyBase + 0x42);
const TUint KPropertyKeyBluetoothGFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x50);
const TUint KPropertyKeyBluetoothGFrameDel		= (KUidBluetoothDebugPubSubKeyBase + 0x51);
const TUint KPropertyKeyBluetoothGFramePeak		= (KUidBluetoothDebugPubSubKeyBase + 0x52);
const TUint KPropertyKeyBluetoothDataFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x60);
const TUint KPropertyKeyBluetoothDataFrameDel	= (KUidBluetoothDebugPubSubKeyBase + 0x61);
const TUint KPropertyKeyBluetoothDataFramePeak	= (KUidBluetoothDebugPubSubKeyBase + 0x62);
const TUint KPropertyKeyBluetoothFragFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x70);
const TUint KPropertyKeyBluetoothFragFrameDel	= (KUidBluetoothDebugPubSubKeyBase + 0x71);
const TUint KPropertyKeyBluetoothFragFramePeak	= (KUidBluetoothDebugPubSubKeyBase + 0x72);

const TUint KPropertyKeyBluetoothSDUAlloc		= (KUidBluetoothDebugPubSubKeyBase + 0x80);
const TUint KPropertyKeyBluetoothSDUDel			= (KUidBluetoothDebugPubSubKeyBase + 0x81);
const TUint KPropertyKeyBluetoothSDUPeak		= (KUidBluetoothDebugPubSubKeyBase + 0x82);

const TUint KPropertyKeyBluetoothMuxerAlloc		= (KUidBluetoothDebugPubSubKeyBase + 0x90);
const TUint KPropertyKeyBluetoothMuxerDel		= (KUidBluetoothDebugPubSubKeyBase + 0x91);
const TUint KPropertyKeyBluetoothMuxerPeak		= (KUidBluetoothDebugPubSubKeyBase + 0x92);

const TUint KPropertyKeyBluetoothLSAPAlloc		= (KUidBluetoothDebugPubSubKeyBase + 0xa0);
const TUint KPropertyKeyBluetoothLSAPDel		= (KUidBluetoothDebugPubSubKeyBase + 0xa1);
const TUint KPropertyKeyBluetoothLSAPPeak		= (KUidBluetoothDebugPubSubKeyBase + 0xa2);

const TUint KPropertyKeyBluetoothASAPAlloc		= (KUidBluetoothDebugPubSubKeyBase + 0xb0);
const TUint KPropertyKeyBluetoothASAPDel		= (KUidBluetoothDebugPubSubKeyBase + 0xb1);
const TUint KPropertyKeyBluetoothASAPPeak		= (KUidBluetoothDebugPubSubKeyBase + 0xb2);

const TUint KPropertyKeyBluetoothPSAPAlloc		= (KUidBluetoothDebugPubSubKeyBase + 0xc0);
const TUint KPropertyKeyBluetoothPSAPDel		= (KUidBluetoothDebugPubSubKeyBase + 0xc1);
const TUint KPropertyKeyBluetoothPSAPPeak		= (KUidBluetoothDebugPubSubKeyBase + 0xc2);

const TUint KPropertyKeyBluetoothCommandAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0xd0);
const TUint KPropertyKeyBluetoothCommandDel		= (KUidBluetoothDebugPubSubKeyBase + 0xd1);
const TUint KPropertyKeyBluetoothCommandPeak	= (KUidBluetoothDebugPubSubKeyBase + 0xd2);

const TUint KPropertyKeyBluetoothL2DBGGetTimerGrpCID	= (KUidBluetoothDebugPubSubKeyBase + 0x100);
const TUint KPropertyKeyBluetoothL2DBGSetTimerGrpCID	= (KUidBluetoothDebugPubSubKeyBase + 0x101);

const TUint KPropertyKeyBluetoothL2DBGMinPDUGetTime		= (KUidBluetoothDebugPubSubKeyBase + 0x110);
const TUint KPropertyKeyBluetoothL2DBGMaxPDUGetTime		= (KUidBluetoothDebugPubSubKeyBase + 0x111);

const TUint KPropertyKeyBluetoothL2DBGMinPDUSentTime	= (KUidBluetoothDebugPubSubKeyBase + 0x112);
const TUint KPropertyKeyBluetoothL2DBGMaxPDUSentTime	= (KUidBluetoothDebugPubSubKeyBase + 0x113);

const TUint KPropertyKeyBluetoothL2DBGAvePDUGetTime		= (KUidBluetoothDebugPubSubKeyBase + 0x114);
const TUint KPropertyKeyBluetoothL2DBGAvePDUSentTime	= (KUidBluetoothDebugPubSubKeyBase + 0x115);


const TUint KPropertyKeyBluetoothQueuedSDUFlushedCounter		= (KUidBluetoothDebugPubSubKeyBase + 0x200);
const TUint KPropertyKeyBluetoothPartialSentSDUFlushedCounter	= (KUidBluetoothDebugPubSubKeyBase + 0x201);
const TUint KPropertyKeyBluetoothSentSDUFlushedCounter			= (KUidBluetoothDebugPubSubKeyBase + 0x202);
const TUint KPropertyKeyBluetoothFlushedPDUMuxerResend			= (KUidBluetoothDebugPubSubKeyBase + 0x203);
const TUint KPropertyKeyBluetoothFlushedPDUDataFlowResend		= (KUidBluetoothDebugPubSubKeyBase + 0x204);
const TUint KPropertyKeyBluetoothFlushedPDUIFrame				= (KUidBluetoothDebugPubSubKeyBase + 0x205);
const TUint KPropertyKeyBluetoothIncompleteErroredSDUReceived	= (KUidBluetoothDebugPubSubKeyBase +  0x206);

namespace L2capDebugInfo
	{
	enum TAllocationType
		{
		EAllocated = 0,
		EDeleted,
		EPeak,
		EMaxAllocationType,
		};
		
	enum TObjectType
		{
		EBasePDU = 0,
		EBFrame,
		EIFrame,
		ESFrame,
		ECFrame,
		EGFrame,
		EDataFrame,
		EFragFrame,
		ESDU,
		EMuxes,
		ESAP,
		ESigHandlers,
		EDataPlanes,
		EL2CapCommand,
		EMaxType,
		};
	
	enum TPDUTimerEvent
		{
		EBFrameCreated,
		EUnsegmentedFrameCreated,
		EIFrameCreated,
		EGetPDUCalled,
		EPDUSent,
		};

	enum TFlushCountersEvent
		{
		ESDUFlushQueuedSDUFlushed,
		ESDUFlushPartialSentSDUFlushed,
		ESDUFlushSentSDUFlushed,
		EFlushedPDUMuxerResend,
		EFlushedPDUDataFlowResend,
		EFlushedPDUIFrame,
		EIncompleteErroredSDUReceived,
		};
	}

#endif
