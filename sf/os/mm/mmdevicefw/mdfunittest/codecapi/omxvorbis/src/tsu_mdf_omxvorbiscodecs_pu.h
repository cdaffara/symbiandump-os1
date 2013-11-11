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
// Header file: PU related tests for PREQ1024 OpenMax Vorbis PU.
// 
//

/**
 @file tsu_mdf_omxvorbiscodecs_pu.h
*/

#ifndef TSU_MDF_OMXVORBISCODECS_PU_H
#define TSU_MDF_OMXVORBISCODECS_PU_H

#include "tsu_mdf_omxvorbiscodecs_step.h"
#include <mmf/server/mmfhwdevice.h>
#include <oggvorbiscodec/oggutil.h>

class RTestStepOmxVorbisCodecs;

/**
 * 
 * Test Case : MM-MDF-OMXVORBISCODECS-U-0010-HP
 * General testing relating to PU layer
 *
 * @class RTestStepOmxVorbisCodecPu 
 *
 */
class RTestStepOmxVorbisCodecPu : public RTestStepOmxVorbisCodecs
	{
public:
	RTestStepOmxVorbisCodecPu();
	virtual TVerdict DoTestStepL();
private:
	TVerdict iVerdict;
	};

#endif // TSU_MDF_OMXVORBISCODECS_PU_H


