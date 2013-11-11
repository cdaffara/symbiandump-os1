
// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestPlayerUtils.h
// Header file: Basic  tests.
// 
//

/**
 @file TestPlayerUtils.h
*/

#ifndef TESTPLAYERCAPS0010_H__
#define TESTPLAYERCAPS0010_H__

#include "TSI_MMF_SDEVSOUND_MPBASE.h"
#include <mmf/server/mmfbuffer.h>

/**
 * Load and initialise an audio file.
 *
 * @class CTestStepSDevSoundRecordData
 *
 */
class CTestStepSDevSoundRecordDataCap : public CSDSMPTSBase
	{
public:
	static CTestStepSDevSoundRecordDataCap* NewL(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);
	~CTestStepSDevSoundRecordDataCap();
	virtual TVerdict DoTestStepL();

	void DoProcess(TInt aError);

private:
	CTestStepSDevSoundRecordDataCap(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);

	TVerdict DoRecordData();
	TVerdict RecordDataFile(TDesC& aFilename);
	TVerdict TestRecordInit();
	TVerdict TestRecordData();

	TVerdict TestInitialize(TMMFState aMode);

	};
#endif
