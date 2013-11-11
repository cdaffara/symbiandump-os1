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

#ifndef TSU_MDF_VIDEOCODECS_PLAY_H
#define TSU_MDF_VIDEOCODECS_PLAY_H

#include "tsu_mdf_videocodecs_step.h"
#include <mmf/devvideo/devvideoplay.h>


class RTestStepVideoCodecs;
class CPlayVideoFile;

class RTestStepVideoDecoderLoad : public RTestStepVideoCodecs
	{
public:
	RTestStepVideoDecoderLoad();
	virtual TVerdict DoTestStepL();
private:
	CPlayVideoFile* iPlayVideoFile;
	};


class RTestStepVideoDecoderPlay : public RTestStepVideoCodecs
	{
public:
	RTestStepVideoDecoderPlay();
	virtual TVerdict DoTestStepL();
private:
	CPlayVideoFile* iPlayVideoFile;
	};


class RTestStepVideoDecoderPlayDSA : public RTestStepVideoCodecs
	{
public:
	RTestStepVideoDecoderPlayDSA();
	virtual TVerdict DoTestStepL();
private:
	CPlayVideoFile* iPlayVideoFile;
	};

#endif // TSU_MDF_VIDEOCODECS_PLAY_H


