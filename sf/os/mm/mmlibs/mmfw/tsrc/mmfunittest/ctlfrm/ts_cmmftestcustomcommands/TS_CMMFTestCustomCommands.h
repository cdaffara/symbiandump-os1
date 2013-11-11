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
// CMMFTestCustomCommands.h
// 
//

#ifndef __CMMFTESTCUSTOMCOMMANDS_H__
#define __CMMFTESTCUSTOMCOMMANDS_H__

// Standard EPOC32 includes required by this header file
#include <e32base.h>
#include <e32std.h>

#include <e32hal.h>
#include <e32svr.h>


// Public Media Server includes

//#include <mmfdataSink.h>
#include <mmf/server/mmfdatapath.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/common/mmfcontroller.h>
#include "TS_Codes.h"
#include "CmmfTestControllerUIDs.hrh"

class MMMFTestCustomCommandImplementor;


class CMMFTestCustomCommandParser : public CMMFCustomCommandParserBase
{
public:
	IMPORT_C static CMMFTestCustomCommandParser* NewL(MMMFTestCustomCommandImplementor& aImplementor);
	IMPORT_C static CMMFTestCustomCommandParser* NewLC(MMMFTestCustomCommandImplementor& aImplementor);

	IMPORT_C void HandleRequest(TMMFMessage& aMessage);
	void DoHandleRequestL(TMMFMessage& aMessage);


private:
	CMMFTestCustomCommandParser(MMMFTestCustomCommandImplementor& aImplementor);
	~CMMFTestCustomCommandParser(void);
	void ConstructL (void);

	void DoSetAudioQuality(TMMFMessage& aMessage);
	void DoGetAudioQuality(TMMFMessage& aMessage);

	MMMFTestCustomCommandImplementor& iImplementor;
};


class MMMFTestCustomCommandImplementor
	{
	public:
		virtual TInt MtccSetAudioQuality(TInt aQuality) = 0;
		virtual TInt MtccGetAudioQuality(TInt& aQuality) = 0;
		virtual void MtccCustomCommandSyncL(TMMFMessage& aMessage) = 0;
		virtual void MtccCustomCommandSyncWithReturnL(TMMFMessage& aMessage) = 0;
		virtual void MtccCustomCommandAsyncL(TMMFMessage& aMessage) = 0;
		virtual void MtccCustomCommandAsyncWithReturnL(TMMFMessage& aMessage) = 0;
		virtual void MtccCustomCommandRequestGeneralEventL(TMMFMessage& aMessage) = 0;
		virtual void MtccCustomCommandSimulateReloadingL(TMMFMessage& aMessage) = 0;
		virtual void MtccCustomCommandCheckUrlL(TMMFMessage& aMessage) = 0;
		virtual void MtccCustomCommandCheckLongUrlL(TMMFMessage& aMessage) = 0;
	};

class RMMFTestCustomCommands : public RMMFCustomCommandsBase
	{
public:
	IMPORT_C RMMFTestCustomCommands(RMMFController& aController, TUid aInterfaceId);
	// These are Dummy commands to prove the framework is working
	IMPORT_C TInt SetAudioQuality(TInt aAudioQualityLevel);
	IMPORT_C TInt GetAudioQuality(TInt& aAudioQualityLevel);
private:
	TBuf<64> iMemFunctionText;
	};

#endif
