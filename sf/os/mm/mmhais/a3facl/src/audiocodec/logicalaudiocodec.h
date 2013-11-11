/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef LOGICALAUDIOCODEC_H
#define LOGICALAUDIOCODEC_H

#include <e32base.h>
#include <a3f/a3f_trace_utils.h>
#include <a3f/a3fbase.h>
#include <a3f/maudiocodec.h>

#include "componentparameters.h"
#include "audioprocessingunit.h"
#include "mconfigurationhelper.h"

// Remove when the MMRC server is ready
#include "maudiocodecadaptationobserver.h"

class MLogicalSettingObserver;
class MAudioProcessingUnitObserver;
class MConfigurationHelper;

/**
	This is the reference implementation of MAudioCodec API.
	@lib logicalaudiocodec.lib
 */

NONSHARABLE_CLASS(CLogicalAudioCodec) : public CAudioProcessingUnit,
										public MAudioCodec,
										public MAudioCodecAdaptationObserver
	{
friend class CAudioContext;
friend class CLogicalAudioStream;
public:
	/**
	Create a new instance.
	*/
	static CLogicalAudioCodec* NewL(TAny* aParam);

	/**
	Destructor.
	Deletes all objects and releases all resource owned by this instance.
	*/
	virtual ~CLogicalAudioCodec();

	// from base class MAudioCodec
	TInt SetFormat(TUid aFormat);
	TInt SetSampleRate(TInt aSampleRate);
	TInt SetMode(TUid aMode);
	TInt GetSupportedSamplesRates(RArray<TInt>& aSupportedRates);
	TInt GetSupportedModes(RArray<TUid>& aSupportedModes);
	TInt RegisterAudioCodecObserver(MAudioCodecObserver& aObserver);
	void UnregisterAudioCodecObserver(MAudioCodecObserver& aObserver);

	// Override just this
	TBool IsTypeOf(TUid aTypeId) const;
	TAny* Interface(TUid aType);

	// from MAudioCodecAdaptationObserver
	void AllBuffersProcessed();
	void ProcessingUnitError(TInt aError);
	void GetSupportedAModesComplete(TInt aError);
	void GetSupportedARatesComplete(TInt aError);

private:

	/**
	Second phase constructor
	*/
	void ConstructL();

	/**
	Constructor
	*/
	CLogicalAudioCodec(TAny* aParam);

protected: // data

	// Format uid

	TUid	iFormat;

	TInt				iSampleRateConfig;
	TUid				iModeConfig;
	RPointerArray<MAudioCodecObserver> iAudioCodecObserver;

	MConfigurationHelper*		iAdaptationStream;

	};

#endif // LOGICALAUDIOCODEC_H
