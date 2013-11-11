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
// This test case is added to improve code coverage of MDF
// 
//

#ifndef TSU_MDF_VIDEOCODECS_PLAY_COV_H
#define TSU_MDF_VIDEOCODECS_PLAY_COV_H

#include "tsu_mdf_videocodecs_step.h"
#include <mmf/devvideo/devvideoplay.h>



class RTestStepVideoCodecs;
class CPlayVideoFileCov;


class RTestStepVideoDecoderPlayCov : public RTestStepVideoCodecs
	{
public:
	RTestStepVideoDecoderPlayCov();
	virtual TVerdict DoTestStepL();
private:
	CPlayVideoFileCov* iPlayVideoFile;
	};

class RTestStepVideoDecoderPlayDSACov : public RTestStepVideoCodecs
	{
public:
	RTestStepVideoDecoderPlayDSACov();
	virtual TVerdict DoTestStepL();
private:
	CPlayVideoFileCov* iPlayVideoFile;
	};
	
class RTestStepVideoDecoderPlayNegTestsCov : public RTestStepVideoCodecs
	{
public:
	RTestStepVideoDecoderPlayNegTestsCov(TInt32 aNumber, TBuf<KMaxLenTestStepName> aTestStepName);
	virtual TVerdict DoTestStepL();
private:
	CPlayVideoFileCov* iPlayVideoFile;
	TInt32 iSubTestNumber;
	};	
	
class RTestStepVideoDecoderPanicCov : public RTestStepVideoCodecs
	{
public:
	RTestStepVideoDecoderPanicCov(TInt32 aNumber, TBuf<KMaxLenTestStepName> aTestStepName);
	virtual TVerdict DoTestStepL();
private:
	CPlayVideoFileCov* iPlayVideoFile;
	TInt32 iSubTestNumber;
	};	
	


#endif // TSU_MDF_VIDEOCODECS_PLAY_COV_H


