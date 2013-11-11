// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef TSU_MDF_OMXVORBISCODECS_STEP_H
#define TSU_MDF_OMXVORBISCODECS_STEP_H

#include <testframework.h>
#include <mmf/server/mmfhwdevice.h>
#include "tsu_mdf_omxvorbiscodecs_constants.h"

/**
 */
class RTestStepOmxVorbisCodecs : public RTestStep
	{
public:
	RTestStepOmxVorbisCodecs();
	~RTestStepOmxVorbisCodecs();
	
	// will be used by the audio play/record test cases
	virtual void SetVerdict(TPtrC16 aText, TVerdict aVerdict = EPass){	
		INFO_PRINTF3(_L("Text: %s, Verdict:%d"), aText.Ptr(), aVerdict);
	}
protected:
	virtual TVerdict OpenL();
	virtual void CleanupAfterOpenFail();
	virtual void Close();
	virtual TVerdict DoTestStepL() = 0;
private:
	CActiveScheduler* iActiveScheduler;
	};

#endif// TSU_MDF_OMXVORBISCODECS_STEP_H
