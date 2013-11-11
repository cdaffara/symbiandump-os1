// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSU_MDF_VIDEOCODECS_RECORD_COV_H
#define TSU_MDF_VIDEOCODECS_RECORD_COV_H

#include "tsu_mdf_videocodecs_step.h"
#include <mmf/devvideo/devvideorecord.h>

class RTestStepVideoCodecs;	
class CRecordVideoFileCov;




class RTestStepVideoEncoderCov : public RTestStepVideoCodecs
	{
public:
	RTestStepVideoEncoderCov();
	virtual TVerdict DoTestStepL();
private:
	CRecordVideoFileCov* iRecordVideoFile;
	};
	
class RTestStepVideoEncoderNegativeCov : public RTestStepVideoCodecs
	{
public:
	RTestStepVideoEncoderNegativeCov();
	virtual TVerdict DoTestStepL();
private:
	CRecordVideoFileCov* iRecordVideoFile;
	};	

#endif // TSU_MDF_VIDEOCODECS_RECORD_COV_H


