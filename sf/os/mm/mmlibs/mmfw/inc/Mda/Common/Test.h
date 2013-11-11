// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mda\Common\Test.h
// 
//

#ifndef __MDA_COMMON_TEST_H__
#define __MDA_COMMON_TEST_H__

// Standard EPOC32 includes
#include <e32base.h>

// Public Media Server includes
#include <mda/common/base.h>
#include <mda/common/controller.h>
#include <mda/common/port.h>
#include <mda/common/resource.h>

#include "Mda/Common/Test.hrh"

// Uids

/**
 *@publishedAll 
 *@deprecated 
 */
const TUint KUidMdaHttpResLocValue = KUidMdaHttpResLocDefine;

/**
 *@publishedAll 
 *@deprecated 
 */
const TUid KUidMdaHttpResLoc = {KUidMdaHttpResLocValue};

/**
 *@publishedAll 
 *@deprecated 
 */
const TUint KUidMdaEventTesterValue = KUidMdaEventTesterDefine;

/**
 *@publishedAll 
 *@deprecated 
 */
const TUid KUidMdaEventTester = {KUidMdaEventTesterValue};

/**
 *@publishedAll 
 *@deprecated 
 */
const TUint KUidMdaTestSrcPortValue = KUidMdaTestSrcPortDefine;

/**
 *@publishedAll 
 *@deprecated 
 */
const TUid KUidMdaTestSrcPort = {KUidMdaTestSrcPortValue};

/**
 *@publishedAll 
 *@deprecated 
 */
const TUint KUidMdaTestDstPortValue = KUidMdaTestDstPortDefine;

/**
 *@publishedAll 
 *@deprecated 
 */
const TUid KUidMdaTestDstPort = {KUidMdaTestDstPortValue};

/**
 *@publishedAll 
 *@deprecated 
 */
const TUint KUidMdaDatatypeValue = KUidMdaDatatypeDefine;

/**
 *@publishedAll 
 *@deprecated 
 */
const TUid KUidMdaDatatype = {KUidMdaDatatypeValue};

/**
 *@publishedAll 
 *@deprecated 
 */
const TUint KUidMdaAudioTestSrcPortValue = KUidMdaAudioTestSrcPortDefine;

/**
 *@publishedAll 
 *@deprecated 
 */
const TUid KUidMdaAudioTestSrcPort = {KUidMdaAudioTestSrcPortValue};

/**
 *@publishedAll 
 *@deprecated 
 */
const TUint KUidMdaAudioTestDstPortValue = KUidMdaAudioTestDstPortDefine;

/**
 *@publishedAll 
 *@deprecated 
 */
const TUid KUidMdaAudioTestDstPort = {KUidMdaAudioTestDstPortValue};

/**
 *@publishedAll 
 *@deprecated 
 */
const TUint KUidMdaTestThreadValue = KUidMdaTestThreadDefine;

/**
 *@publishedAll 
 *@deprecated 
 */
const TUid KUidMdaTestThread = {KUidMdaTestThreadValue};

/**
 *@publishedAll 
 *@deprecated 
 */
const TUint KUidMdaTestServerHeapValue = KUidMdaTestServerHeapDefine;

/**
 *@publishedAll 
 *@deprecated 
 */
const TUid KUidMdaTestServerHeap = {KUidMdaTestServerHeapValue};

// Functions 

/**
 *@publishedAll 
 *@deprecated 
 */
enum TMdaEventTesterFunctions
	{
	EMdaEventTesterGenerateEvents=0
	};

/**
 *@publishedAll 
 *@deprecated 
 */
enum TMdaTestPortFunctions
	{
	EMdaTestPortBaseFunction = 3,       // was EMLbPortNegotiate
	EMdaTestPortSetOpenParams,
	EMdaTestPortCompleteDelayedOpen
	};

/**
 *@publishedAll 
 *@deprecated 
 */
enum TMdaTestAudioPortFunctions
	{
	EMdaTestAudioPortBaseFunction = 3,  // was EMLbPortNegotiate
	EMdaTestAudioPortSetAudio,
	EMdaTestAudioPortGetAudio,
	EMdaTestAudioPortSetBuffers,
	EMdaTestAudioPortGetBuffers
	};

/**
 *@publishedAll 
 *@deprecated 
 */
enum TMdaTestThreadFunctions
	{
	EMdaTestThreadStartFunction,
	EMdaTestThreadStopFunction
	};

/**
 *@publishedAll 
 *@deprecated 
 */
enum TMdaTestServerHeapFunctions
	{
	EMdaTestServerHeapMarkStartFunction,
	EMdaTestServerHeapMarkCheckFunction,
	EMdaTestServerHeapMarkEndFunction,
	EMdaTestServerHeapSetAllocFailFunction
	};

// Packages



class TMdaHttpClipLocation : public TMdaClipLocation
/**
 * @internalTechnology 
 *
 * Clip Locations
 */
	{
public:
	inline TMdaHttpClipLocation();
	inline TMdaHttpClipLocation(const TDesC& aName);
public:
	TBuf<256> iName;
	TTimeIntervalMicroSeconds32 iDelay;
	};

// Inline functions

inline TMdaHttpClipLocation::TMdaHttpClipLocation() : 
TMdaClipLocation(KUidMdaHttpResLoc,sizeof(TMdaHttpClipLocation)) {}

inline TMdaHttpClipLocation::TMdaHttpClipLocation(const TDesC& aName) :
TMdaClipLocation(KUidMdaHttpResLoc,sizeof(TMdaHttpClipLocation)), 
iName(aName) {}

#endif

