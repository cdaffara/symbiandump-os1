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
// getting and setting audio settings
//

#if (!defined __TESTSTEPS_GETSET_H__)
#define __TESTSTEPS_GETSET_H__

#include <e32std.h>
#include <e32base.h>

#include "TS_MMF_ACTRL_TestStep.h"

/**
 *
 * CTestStepGetSetVolumeInPrimed
 * This class provides a test step to set/get the volume
 * in the primed state
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepGetSetVolumeInPrimed :public CTestStepActrl
{
public:               
	CTestStepGetSetVolumeInPrimed();
	~CTestStepGetSetVolumeInPrimed();
	virtual TVerdict  DoTestStepL( void );
};


/**
 *
 * CTestStepGetSetVolumeInPlay
 * This class provides a test step to set/get the volume
 * in the primed state
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepGetSetVolumeInPlay :public CTestStepActrl
{
public:               
	CTestStepGetSetVolumeInPlay();
	~CTestStepGetSetVolumeInPlay();
	virtual TVerdict  DoTestStepL( void );
};


/**
 *
 * CTestStepGetSetVolumeInPlay
 * This class provides a test step to set/get the volume
 * in the primed state
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepGetSetVolumeInStopped :public CTestStepActrl
{
public:               
	CTestStepGetSetVolumeInStopped();
	~CTestStepGetSetVolumeInStopped();
	virtual TVerdict  DoTestStepL( void );
};


/**
 *
 * CTestStepGetSetBalanceInPrimed
 * This class provides a test step to set/get the balance
 * in the primed state
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepGetSetBalanceInPrimed :public CTestStepActrl
{
public:               
	CTestStepGetSetBalanceInPrimed();
	~CTestStepGetSetBalanceInPrimed();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetBalanceInPlay
 * This class provides a test step to set/get the balance
 * in the playing state
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepGetSetBalanceInPlay :public CTestStepActrl
{
public:
	CTestStepGetSetBalanceInPlay();
	~CTestStepGetSetBalanceInPlay();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetBalanceInStopped
 * This class provides a test step to set/get the balance
 * in the primed state
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepGetSetBalanceInStopped :public CTestStepActrl
{
public:               
	CTestStepGetSetBalanceInStopped();
	~CTestStepGetSetBalanceInStopped();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetPositionInStopped
 * This class provides a test step to set/get the position
 * in the stopped state.
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepGetSetPositionInStopped :public CTestStepActrl
{
public:               
	CTestStepGetSetPositionInStopped();
	~CTestStepGetSetPositionInStopped();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetPositionInPrimed
 * This class provides a test step to set/get the position
 * in the primed state.
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepGetSetPositionInPrimed :public CTestStepActrl
{
public:               
	CTestStepGetSetPositionInPrimed();
	~CTestStepGetSetPositionInPrimed();
	virtual TVerdict DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetPositionInPlay
 * This class provides a test step to set/get the position
 * in the play state.
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepGetSetPositionInPlay :public CTestStepActrl
{
public:
	CTestStepGetSetPositionInPlay();
	~CTestStepGetSetPositionInPlay();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetPositionInPaused
 * This class provides a test step to set/get the position
 * in the paused state.
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepGetSetPositionInPaused :public CTestStepActrl
{
public:               
	CTestStepGetSetPositionInPaused();
	~CTestStepGetSetPositionInPaused();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetNumChannelsOnSource
 * This class provides a test step to set/get the number of channels
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepGetSetNumChannelsOnSource :public CTestStepActrl
{
public:
	CTestStepGetSetNumChannelsOnSource();
	~CTestStepGetSetNumChannelsOnSource();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetNumChannelsOnSink
 * This class provides a test step to set/get the number of channels
 * on a Sink
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepGetSetNumChannelsOnSink :public CTestStepActrl
{
public:               
	CTestStepGetSetNumChannelsOnSink();
	~CTestStepGetSetNumChannelsOnSink();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetSinkFormat
 * This class gets/sets the format of the sink
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepGetSetSinkFormat :public CTestStepActrl
{
public:               
	CTestStepGetSetSinkFormat();
	~CTestStepGetSetSinkFormat();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetSourceFormat
 * This class gets/sets the format of the Source
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepGetSetSourceFormat :public CTestStepActrl
{
public:
	CTestStepGetSetSourceFormat();
	~CTestStepGetSetSourceFormat();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetSourceSampleRate
 * This class provides a test step to set/get the sample rate
 * of the source
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepGetSetSourceSampleRate :public CTestStepActrl
{
public:
	CTestStepGetSetSourceSampleRate();
	~CTestStepGetSetSourceSampleRate();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetSinkSampleRate
 * This class provides a test step to set/get the sample rate
 * of the sink
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepGetSetSinkSampleRate :public CTestStepActrl
{
public:
	CTestStepGetSetSinkSampleRate();
	~CTestStepGetSetSinkSampleRate();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetSourceBitRate
 * This class provides a test step get/set the source bit rate
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepGetSetSourceBitRate :public CTestStepActrl
{
public:
	CTestStepGetSetSourceBitRate();
	~CTestStepGetSetSourceBitRate();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetSourceDataType
 * This class provides a test step to test the source data type
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepGetSetSourceDataType :public CTestStepActrl
{
public:
	CTestStepGetSetSourceDataType();
	~CTestStepGetSetSourceDataType();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetSinkBitRate
 * This class provides a test step to set/get the sample rate
 * of the sink
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepGetSetSinkBitRate :public CTestStepActrl
{
public:
	CTestStepGetSetSinkBitRate();
	~CTestStepGetSetSinkBitRate();
	virtual TVerdict DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetSinkDataType
 * This class provides a test step to set/get sink data type
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */

class CTestStepGetSetSinkDataType :public CTestStepActrl
{
public:
	CTestStepGetSetSinkDataType();
	~CTestStepGetSetSinkDataType();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSetMetaData
 * This class provides a test step to set/get metadata
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepGetSetMetaData :public CTestStepActrl
{
public:
	CTestStepGetSetMetaData();
	~CTestStepGetSetMetaData();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSupportedSinkDataTypes
 * This class provides a test for the supported sink data types
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepGetSupportedSinkDataTypes :public CTestStepActrl
{
public:
	CTestStepGetSupportedSinkDataTypes();
	~CTestStepGetSupportedSinkDataTypes();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSupportedSourceDataTypes
 * This class provides a test for the supported sink data types
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepGetSupportedSourceDataTypes :public CTestStepActrl
{
public:
	CTestStepGetSupportedSourceDataTypes();
	~CTestStepGetSupportedSourceDataTypes();
	virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepGetSupportedSourceNumChannels
 * This class provides a test for the supported number of channels
 * on a source.
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepGetSupportedSourceNumChannels :public CTestStepActrl
{
public:
	CTestStepGetSupportedSourceNumChannels();
	~CTestStepGetSupportedSourceNumChannels();
	virtual TVerdict  DoTestStepL( void );
};

#endif //(__TESTSTEPS_GETSET_H__)
