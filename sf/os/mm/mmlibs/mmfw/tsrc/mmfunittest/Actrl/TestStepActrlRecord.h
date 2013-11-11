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
// This file contains Test step implementations for
// recording audio via the audio controller.
//

#if (!defined __TESTSTEP_RECORD_H__)
#define __TESTSTEP_RECORD_H__

#include <e32std.h>
#include <e32base.h>

#include "TS_MMF_ACTRL_TestStep.h"

/**
 *
 * CTestStepActrlRecord
 * This class provides a test step to Record an audio clip
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepActrlRecord : public CTestStepActrl
	{
public:
	                  CTestStepActrlRecord();
                 	 ~CTestStepActrlRecord();
	virtual TVerdict  DoTestStepL( void );

	};

/**
 *
 * CTestStepSetGetRecordGain
 * This class tests setting and getting the record gain
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepSetGetRecordGain : public CTestStepActrl
	{
public:
	CTestStepSetGetRecordGain();
	~CTestStepSetGetRecordGain();
	virtual TVerdict  DoTestStepL( void );
	};

/**
 *
 * CTestStepSetGetRecordBalance
 * This class tests setting and getting the record balance
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepSetGetRecordBalance : public CTestStepActrl
	{
public:
	CTestStepSetGetRecordBalance();
	~CTestStepSetGetRecordBalance();
	virtual TVerdict  DoTestStepL( void );
	};

/**
 *
 * CTestStepRecordNewFileMem
 * This class tests recording to file from a memory descriptor
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepRecordMemToFile : public CTestStepActrl
	{
public:
	CTestStepRecordMemToFile();
	~CTestStepRecordMemToFile();
	virtual TVerdict  DoTestStepL( void );
	};

/**
 *
 * CTestStepRecordMemToMem
 * This class tests recording to a mem descriptor from a mem descriptor
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepRecordMemToMem : public CTestStepActrl
	{
public:
	CTestStepRecordMemToMem();
	~CTestStepRecordMemToMem();
	virtual TVerdict  DoTestStepL( void );
	};

/**
 *
 * CTestStepMarcSetMaxFileSize
 * This class tests setting the maxfile size while recording
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepMarcSetMaxFileSize : public CTestStepActrl
	{
public:
	CTestStepMarcSetMaxFileSize();
	~CTestStepMarcSetMaxFileSize();
	virtual TVerdict  DoTestStepL( void );
	};

/**
 *
 * CTestStepSetGetRecordTimeAvailable
 * This class tests the record time available api
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepSetGetRecordTimeAvailable : public CTestStepActrl
	{
public:
	CTestStepSetGetRecordTimeAvailable();
	~CTestStepSetGetRecordTimeAvailable();
	virtual TVerdict  DoTestStepL( void );
	};


// forward reference
class TRecParameters;
/**
 *
 * CTestStepRecordFormat
 * This class provides a test step to test recording
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepRecordFormat : public CTestStepActrl
	 {
public:
	CTestStepRecordFormat( TUint aTestIndex );
	virtual ~CTestStepRecordFormat();
	virtual enum TVerdict DoTestStepL();
	TVerdict DoConvertStepL( void );
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
private:
	const TRecParameters *iTestParameters;
	};


/**
 *
 * CTestStepSetGetRecordBalanceNoSink
 * INC090160: Incorrect precondition check in audio controller
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepSetGetRecordBalanceNoSink : public CTestStepActrl
	{
public:
	CTestStepSetGetRecordBalanceNoSink();
	~CTestStepSetGetRecordBalanceNoSink();
	virtual TVerdict  DoTestStepL( void );
	};

#endif //(__TESTSTEP_RECORD_H__)
