// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SUBTITLEGRAPHICTESTSTEP_H
#define SUBTITLEGRAPHICTESTSTEP_H

#include <testframework.h>
#include <w32std.h>

#include "mmfsubtitlegraphic.h"

/* 
Basic test for subtitle graphic client API
*/
class RSubtitleGraphicTestStep : public RTestStep
	{
protected:   
	RSubtitleGraphicTestStep(const TDesC& aStepName);
	
	void InitWservL();
	void DeInitWserv();
	
	// from RTestStep
    TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    TVerdict StartCrpDrawL();
    
	void PrepGc();
	void RetireGc();
	void InitCrpL();
	void DestroyCrp();
	TBool CreateBitmap1L();
	TBool CreateBitmap2L();
	TBool CreateBitmap3L();
	void DestroyBitmap1();
	void DestroyBitmap2();
	void DestroyBitmap3();
	
protected:
	CActiveScheduler* iActiveScheduler;
	TBool iActiveSchedulerStarted;
	RFs iFs;

    // Window server related objects
    RWsSession iWs;
    CWsScreenDevice* iScreen;
    RWindow* iWindow;
    RWindowGroup iRootWindow;
    CWindowGc *iGc;
    CMMFSubtitleGraphic* iCRPGraphic;
    CFbsBitmap* iBitmap1;
	CFbsBitmap* iBitmap2;
	CFbsBitmap* iBitmap3;
    
private:
    TWsGraphicId iCRPId;
	};

#endif // SUBTITLEGRAPHICTESTSTEP_H
