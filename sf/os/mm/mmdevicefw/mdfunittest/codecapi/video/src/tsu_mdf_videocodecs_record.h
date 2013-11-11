// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSU_MDF_VIDEOCODECS_RECORD_H
#define TSU_MDF_VIDEOCODECS_RECORD_H

#include "tsu_mdf_videocodecs_step.h"
#include <mmf/devvideo/devvideorecord.h>

class RTestStepVideoCodecs;	
class CRecordVideoFile;

class RTestStepVideoEncoderLoad : public RTestStepVideoCodecs
	{
public:
	RTestStepVideoEncoderLoad();
	virtual TVerdict DoTestStepL();
private:
	CRecordVideoFile* iRecordVideoFile;
	};


class RTestStepVideoEncoderRecord : public RTestStepVideoCodecs
	{
public:
	RTestStepVideoEncoderRecord();
	virtual TVerdict DoTestStepL();
private:
	CRecordVideoFile* iRecordVideoFile;
	};

#endif // TSU_MDF_VIDEOCODECS_RECORD_H


