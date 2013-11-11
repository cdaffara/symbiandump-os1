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

#ifndef __TSI_MMFCUSTOMCOMMANDS_H__
#define __TSI_MMFCUSTOMCOMMANDS_H__

// Standard EPOC32 includes required by this header file
#include <e32base.h>
#include <e32std.h>

#include <e32hal.h>
#include <e32svr.h>

// Public Media Server includes
#include <mmf/server/mmfdatapath.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/common/mmfcontroller.h>
#include "TSI_MmfCodes.h"

// enums for custom command function numbers
enum TDummyFNum
	{
	EDummyFNumSetAudioQuality = 1,
	EDummyFNumGetAudioQuality = 2
	};

class CTSIMmfCustomCommandParser : public CMMFCustomCommandParserBase
	{
public:
	IMPORT_C static CTSIMmfCustomCommandParser* NewL(TUid aInterfaceId);
	IMPORT_C static CTSIMmfCustomCommandParser* NewLC(TUid aInterfaceId);
	IMPORT_C void HandleRequest(TMMFMessage& aMessage);

private:
	CTSIMmfCustomCommandParser(TUid aInterfaceId);
	~CTSIMmfCustomCommandParser(void);
	void ConstructL (void);

	TBool DoSetAudioQuality(TMMFMessage& aMessage);
	TInt DoGetAudioQuality(TMMFMessage& aMessage);
	};


class CTSIMmfCustomCommandImplementor
	{
public:
	virtual TInt SetAudioQuality(TInt aQuality) = 0;
	virtual TInt GetAudioQuality(TInt& aQuality) = 0;
	};

class RTSIMmfCustomCommands : public RMMFCustomCommandsBase
	{
public:
	IMPORT_C static RTSIMmfCustomCommands* NewL(RMMFController& aController, TUid aInterfaceId);
	IMPORT_C static RTSIMmfCustomCommands* NewLC(RMMFController& aController, TUid aInterfaceId);
	void ConstructL(void);

	// These are Dummy commands to prove the framework is working
	IMPORT_C TBool SetAudioQuality(const TDesC8& aAudioQualityLevel);
	IMPORT_C TBool GetAudioQuality(TDes8& aAudioQualityLevel);

private:
	RTSIMmfCustomCommands(RMMFController& aController, TUid aInterfaceId);
	~RTSIMmfCustomCommands();
	};

#endif
