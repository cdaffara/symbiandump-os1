// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Test flags. Lightweight functions to provide basic coverage test
//
//
//  This is a 3-plane comms layer implementation example, which has been customised to be a test layer which gobbles and releases ESOCK MBUFs.
//  The MBuf gobbling functionality can be disabled by undefining the macro SYMBIAN_COMMSFW_MBUF_GOBBLER which is specified in mbufgobblerproviders.mmp.
//  When SYMBIAN_COMMSFW_MBUF_GOBBLER is undefined, the source code specified by mbufgobblerproviders.mmp becomes a pass through layer i.e. it passes the data
//  through to the layer above or below without altering it. This makes it useful as a starting point for implementing your own layers / providers;
//  useful documentation on how to customise your own passthrough layer can be found in ..\docs\MbufGobblerLayer.doc
//

/**
 @file
 @internalComponent
*/

#ifndef MBUFGOBBLERTESTFLAGS_H_
#define MBUFGOBBLERTESTFLAGS_H_


#include <e32property.h>

#ifdef _DEBUG
#define MBUFGOBBLER_TEST_POINT(flag,result) if(iTestFlags) {MbufGobblerTestFlags::SetResult(MbufGobblerTestFlags::flag,result);}
#define MBUFGOBBLER_TEST_DATA TBool iTestFlags;
#define MBUFGOBBLER_TEST_DATA_INIT iTestFlags = MbufGobblerTestFlags::TestFlagsActive();


namespace MbufGobblerTestFlags
{
const TUid KPubSubUid = {0x00285DAC};

enum
	{
	// set by test library, read by binder
	KFlagsActive = 100,
	
	// set by binder, read by test library
	KTierUp = 101,
	
	KMCPrUp = 111,
	
	KCPrUp = 121,
	
	KSCPrUp = 131,
	
	KBind = 141,
	KBinderSend = 142,
	KBinderReceive = 143,
	};


// used by client test library
static TInt ActivateTestFlags()
	{
	return RProperty::Define(KPubSubUid, KFlagsActive, RProperty::EInt);
	}

// used by client test library
static TInt GetResult(TInt aFlag)
	{
	TInt value=0;
	TInt retcode=RProperty::Get(KPubSubUid, aFlag, value);
	return (retcode==KErrNone) ? value : retcode;
	}
	
// used by binder
static TBool TestFlagsActive()
	{
	return GetResult(KFlagsActive) == KErrNone;
	}

// used by binder
static void SetResult(TInt aFlag, TInt aValue)
	{
	// Yes it is very inefficient making these calls
	//  on e.g. each packet send but we only do this when
	//  the test is running i.e. if it was explicitly
	//  switched on, so it won't affect "normal" operation.
	RProperty::Define(KPubSubUid, aFlag, RProperty::EInt);
	RProperty::Set(KPubSubUid, aFlag, aValue);
	}
} // namespace MbufGobblerTestFlags

#else // _DEBUG
#define MBUFGOBBLER_TEST_POINT(flag,result)
// It's better to make the urel and udeb objects the same size,
//  at the expense of 4 bytes:
#define MBUFGOBBLER_TEST_DATA TBool iTestFlags_unused;
#define MBUFGOBBLER_TEST_DATA_INIT
#endif // _DEBUG

#endif /* MBUFGOBBLERTESTFLAGS_H_ */
