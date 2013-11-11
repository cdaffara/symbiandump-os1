// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// 
//

#ifndef _TE_WAPTHDRBASE_H_
#define _TE_WAPTHDRBASE_H_

#include <e32def.h>
#include <e32std.h>
#include <f32file.h>
#include "smsstacklog.h"
#include "wapthdr.h"

#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

_LIT(KConfigFileDir,"sms");
_LIT(KConfigFilename,"TE_waphdr.txt");
// config file section name
_LIT8(KEncodingWapHeader,"EncodingWapHeader");
// Item names
_LIT8(KResult,                      "Result");
_LIT8(KPort,                        "Port");
_LIT8(KData,                        "Data");
_LIT8(KOther,                       "Other");
_LIT8(KRefNum,                      "RefNum");
// config file section name
_LIT8(KParsingWapHeader,          "ParsingWapHeader");
// item names
_LIT8(KWapHeader,     "WapHeader");


class CWapThdrTestStep : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

	// definition of helper functions
	void _Print(TPtrC8 aStr);
	void _PrintSegments(CArrayPtrFlat<HBufC8>& SegmentArray);
	void _PrintHeaderL(const TWapTextMessage& aHdr);

protected:
	RFs iFs;
	};


#endif
