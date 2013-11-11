// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CMMFVIDEOTESTCUSTOMCOMMANDS_H__
#define __CMMFVIDEOTESTCUSTOMCOMMANDS_H__

// Standard EPOC32 includes required by this header file
#include <e32base.h>
#include <e32std.h>

#include <e32hal.h>
#include <e32svr.h>

// Public Media Server includes

//#include <mmf/server/mmfdataSink.h>
#include <mmf/server/mmfdatapath.h>
#include <mmf/common/mmffourcc.h>
//#include <mmf/plugin/mmfAudioIOInterfaceUIDs.hrh>

//#include <mmf/server/mmfcodec.h>
//#include <mmf/server/mmfdatabuffer.h>
//#include <mmf/common/mmfpaniccodes.h>

#include <mmf/common/mmfcontroller.h>
#include "TS_Codes.h"

//const TUint KAudioOutputDefaultFrameSize = 0x1000;

//void Panic(TInt aPanicCode);


class MMMFTestCustomCommandImplementor
	{
	public:
		virtual void MvtccCustomCommandSyncL(TMMFMessage& aMessage) = 0;
		virtual void MvtccCustomCommandSyncWithReturnL(TMMFMessage& aMessage) = 0;
		virtual void MvtccCustomCommandAsyncL(TMMFMessage& aMessage) = 0;
		virtual void MvtccCustomCommandAsyncWithReturnL(TMMFMessage& aMessage) = 0;
		virtual void MvtccCustomCommandRequestGeneralEventL(TMMFMessage& aMessage) = 0;
		virtual void MvtccCustomCommandSimulateReloadingL(TMMFMessage& aMessage) = 0;
		virtual void MvtccCustomCommandCheckUrlL(TMMFMessage& aMessage) = 0;
#ifdef SYMBIAN_BUILD_GCE
		virtual void MvtccCustomCommandSimulateSurfaceCreatedL(TMMFMessage& aMessage) = 0;
		virtual void MvtccCustomCommandSimulateParaChangedL(TMMFMessage& aMessage) = 0;
		virtual void MvtccCustomCommandSimulateRemoveSurfaceL(TMMFMessage& aMessage) = 0;

#endif
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
		virtual void MvtccCustomCommandSetSubtitleDisplayCheckL(TMMFMessage& aMessage) = 0;
		virtual void MvtccCustomCommandGetSubtitleCallCount(TMMFMessage& aMessage) = 0;
		virtual void MvtccCustomCommandSimulateCrpReadyEvent(TMMFMessage& aMessage) = 0;
		virtual void MvtccCustomCommandSetLanguageNotAvail(TMMFMessage& aMessage) = 0;
#endif
	};

class CMMFTestCustomCommandParser : public CMMFCustomCommandParserBase
{
public:
	IMPORT_C static CMMFTestCustomCommandParser* NewL(TUid aInterfaceId, MMMFTestCustomCommandImplementor& aImplementor);
	IMPORT_C static CMMFTestCustomCommandParser* NewLC(TUid aInterfaceId, MMMFTestCustomCommandImplementor& aImplementor);

	IMPORT_C void HandleRequest(TMMFMessage& aMessage);

private:
	CMMFTestCustomCommandParser(TUid aInterfaceId, MMMFTestCustomCommandImplementor& aImplementor);
	~CMMFTestCustomCommandParser(void);

	void DoHandleRequestL(TMMFMessage& aMessage);
	void ConstructL (void);

	void DoSetFrameRate(TMMFMessage& aMessage);
	void DoGetFrameRate(TMMFMessage& aMessage);

	MMMFTestCustomCommandImplementor& iImplementor;
};



class RMMFTestCustomCommands : public RMMFCustomCommandsBase
	{
public:
	IMPORT_C RMMFTestCustomCommands(RMMFController& aController, TUid aInterfaceId);
	// These are Dummy commands to prove the framework is working
	IMPORT_C TInt MvcSetFrameRateL(TReal32 aFramesPerSecond);
	IMPORT_C TInt MvcGetFrameRateL(TReal32& aFramesPerSecond);
	//IMPORT_C TInt SetAudioQuality(TInt aAudioQualityLevel);
	//IMPORT_C TInt GetAudioQuality(TInt& aAudioQualityLevel);
	};

#endif
