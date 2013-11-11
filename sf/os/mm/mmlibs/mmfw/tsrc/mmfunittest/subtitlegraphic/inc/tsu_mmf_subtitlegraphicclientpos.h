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

#ifndef TSU_MMF_SUBTITLEGRAPHICCLIENTPOS_H
#define TSU_MMF_SUBTITLEGRAPHICCLIENTPOS_H

// INCLUDES
#include "subtitlegraphicteststep.h"


// Positive testing

/* 
MM-MMF-SUBTITLE-CRP-0001
Test sending a 'drawframe' message to the CRP
*/
class RSubtitleGraphicTestStepDrawFrame : public RSubtitleGraphicTestStep
	{
public:
	static RSubtitleGraphicTestStepDrawFrame* NewL(const TDesC& aStepName);
	
	// From RTestStep
	TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    TVerdict DoTestStepL();
    
private:
	RSubtitleGraphicTestStepDrawFrame(const TDesC& aStepName);
	};
	

/* 
MM-MMF-SUBTITLE-CRP-0002
Test sending a 'swapframe' message to the CRP
*/
class RSubtitleGraphicTestStepSwapFrame : public RSubtitleGraphicTestStep
	{
public:
	static RSubtitleGraphicTestStepSwapFrame* NewL(const TDesC& aStepName);
	
	// From RTestStep
	TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    TVerdict DoTestStepL();
    
private:
	RSubtitleGraphicTestStepSwapFrame(const TDesC& aStepName);
	};


/* 
MM-MMF-SUBTITLE-CRP-0003
Test requests the current CRP UID 
*/
class RSubtitleGraphicTestStepGetUid : public RSubtitleGraphicTestStep
	{
public:
	static RSubtitleGraphicTestStepGetUid* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
	RSubtitleGraphicTestStepGetUid(const TDesC& aStepName);
	};

/* 
MM-MMF-SUBTITLE-CRP-0004
Test sending a 'clear' message to the CRP
*/
class RSubtitleGraphicTestStepClear : public RSubtitleGraphicTestStep
	{
public:
	static RSubtitleGraphicTestStepClear* NewL(const TDesC& aStepName);
	
	// From RTestStep
	TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    TVerdict DoTestStepL();
    
private:
	RSubtitleGraphicTestStepClear(const TDesC& aStepName);
	};

#endif // TSU_MMF_SUBTITLEGRAPHICCLIENTPOS_H
