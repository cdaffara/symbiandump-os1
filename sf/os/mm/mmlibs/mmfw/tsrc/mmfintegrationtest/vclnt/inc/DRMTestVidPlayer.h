// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// REQ1952
// 
//

#ifndef __DRMTESTVIDPLAYER_H__
#define __DRMTESTVIDPLAYER_H__

#include <fbs.h>
#include "TSI_MMFVCLNT.h"
#include "TestPlayer.h"

/**
 *
 * DRM Get Frame from unprotected file 
 *
 * @class CTestStepVidPlayerDRMGetFrame
 *
 * @xxxx
 *
 */

class CTestStepVidPlayerDRMGetFrame : public CTestVclntVideoFile
	{
public:
	static CTestStepVidPlayerDRMGetFrame* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
private:
	CTestStepVidPlayerDRMGetFrame(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	};

/**
 *
 * DRM Get Frame from protected file 
 *
 * @class CTestStepVidPlayerDRMGetFrame
 *
 * @xxxx
 *
 */
class CTestStepVidPlayerDRMGetFrameProtected : public CTestVclntVideoFile
	{
public:
	static CTestStepVidPlayerDRMGetFrameProtected* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
private:
	CTestStepVidPlayerDRMGetFrameProtected(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	};

/**
 *
 * Attempt ot open a DRM protected file and play with no rights set
 *
 * @class CTestStepVidPlayerDRMPlayNoRights
 *
 * @xxxx
 *
 */
class CTestStepVidPlayerDRMPlayNoRights :public CTestVclntVideoFile
	{
public:
	static CTestStepVidPlayerDRMPlayNoRights* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestStepVidPlayerDRMPlayNoRights(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);

	};

/**
 *
 * Open a DRM protected video file and play with DRM agent providing 
 * rights and MIME typ set
 *
 * @class CTestStepVidPlayerDRMPlayRights
 *
 * @xxxx
 *
 */
class CTestStepVidPlayerDRMPlayRights :public CTestVclntVideoFile
	{
public:
	static CTestStepVidPlayerDRMPlayRights* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestStepVidPlayerDRMPlayRights(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);

	};

TAny	GetDriveName(TDes& aFileName);

#endif //__DRMTESTVIDPLAYER_H__
