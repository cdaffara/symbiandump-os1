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
// LoopbackConfig.cpp
// This file handles retrieving settings from the config file for the loopback test driver
// 
//

/**
 @file
*/

#ifndef __LOOPBACK_CONFIG_H__
#define __LOOPBACK_CONFIG_H__

#include <e32def.h>

#include <testconfigfileparser.h>

#include "FlowControlChange.h"

const TUint KDefaultDelay = 100;	// in milliseconds
const TUint KDefaultPacketLength = 1024;
const TUint KDefaultQueueLength = 30;
const TUint KDefaultBufferSize = 10000;

enum TPortType
	{
	EPacketLoopbackPortType,
	ESerialLoopbackPortType
	};

class TLoopbackConfigItem
/**
 Each of the elements in this file match to a port entry in the loopback.ini file
 */
 	{
public:
	/** The port connected to iPortB via loopback connection */
	TUint iPortA;
	/** The port connected to iPortA via loopback connection */
	TUint iPortB;
	/** The delay, in milliseconds for each write for these 2 ports */
	TUint iDelay;
	/** packet length gives the size of the buffer to be allocated for each read and write */
 	TUint iPacketLength;
 	/** The number of entries in the Read and Write queues */
	TUint iQueueLength;
	/** The total buffer size (in bytes) for serial mode */
	TUint iBufferSize;
	/** Default values for a loopback config item */
	TLoopbackConfigItem()
		{
		iPortA = 0;
		iPortB = 1;
		iDelay = KDefaultDelay;
		iPacketLength = KDefaultPacketLength;
		iQueueLength = KDefaultQueueLength;
		iBufferSize = KDefaultBufferSize;
		}
	};
	
NONSHARABLE_CLASS(CLoopbackConfig) : CBase
/**
 * This class is used to get settings from the loopback.ini file
 */
	{
public:
	~CLoopbackConfig();
	static CLoopbackConfig* NewL();
	TUint Count() const;
	TInt GetPortSettings(TUint aUnit, TLoopbackConfigItem& aPortSettings) const;
	static TInt GetTestNumber(TInt& aTestNumber);
	static TInt GetFlowControl(TBool& aFlowControlOn);
	TPortType PortType() const;
private:
	CLoopbackConfig();
	void ConstructL();
	
	/** Array of all of the items in the loopback config file */
	CArrayFixFlat<TLoopbackConfigItem> *iConfigItems;
	/** The port type (either packet or serial) */
	TPortType iPortType;
	};

_LIT(KLoopbackConfigFilename,"c:\\loopback.ini");
_LIT(KConfigFileDir,"loopback");
_LIT8(KSectionNameFormat,"test%d");
_LIT8(KPort,"Port");
_LIT8(KSerialLoopbackPortType, "SerialLoopbackPort");
_LIT8(KPacketLoopbackPortType, "PacketLoopbackPort");
const TUint KMaxPortTypeNameLength = 64;


// Loopback CSY UIDs

static const TUid KUidLoopbackCSYMin = {0x10206864};
static const TUid KUidLoopbackCSYMax = {0x1020686F};

static const TUid KUidLoopbackCsyDll = {0x10206864}; // KUidLoopbackCSYMin
static const TUid KUidTestLoopbackCsyDll = {0x10206865}; // KUidLoopbackCSYMin + 1
static const TUid KUidPSPacketLoopbackCsyCategory = {0x10206866}; // KUidLoopbackCSYMin + 2
static const TUid KUidPSCsyFlowControlCategory = {0x10206867}; // KUidLoopbackCSYMin + 3
static const TUid KUidPSCsyReadResultCategory = {0x10206868}; // KUidLoopbackCSYMin + 4
static const TUid KUidPSCsyWriteResultCategory = {0x10206869}; //KUidLoopbackCSYMin + 5

// KUidPSPacketLoopbackCsyCategory constants
const TInt KPSLoopbackCsyTestNumber = 0;
const RProperty::TType KPSLoopbackCsyTestNumberKeyType = RProperty::EInt;

// KUidPSCsyFlowControlCategory constants
const RProperty::TType KPSLoopbackCsyFlowControlOnKeyType = RProperty::EInt;

// KUidPSCsyReadResultCategory constants
const RProperty::TType KUidPSCsyReadResultCategoryKeyType = RProperty::EInt;

// KUidPSCsyWriteResultCategory constants
const RProperty::TType KUidPSCsyWriteResultCategoryKeyType = RProperty::EInt;

#endif

