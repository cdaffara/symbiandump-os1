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

#ifndef TSU_MMF_SUBTITLEGRAPHICCLIENTNEG_H
#define TSU_MMF_SUBTITLEGRAPHICCLIENTNEG_H

// INCLUDES
#include "subtitlegraphicteststep.h"

// Negative testing

/* 
MM-MMF-SUBTITLE-CRP-0101 Test sending a 'drawframe' message to the CRP using 
an invalid bitmap handle
*/
class RSubtitleGraphicTestNegDrawFrame : public RSubtitleGraphicTestStep
	{
public:
	static RSubtitleGraphicTestNegDrawFrame* NewL(const TDesC& aStepName);
	~RSubtitleGraphicTestNegDrawFrame();
	
	// From RTestStep
	TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    TVerdict DoTestStepL();
    
private:
	void ConstructL();
	RSubtitleGraphicTestNegDrawFrame(const TDesC& aStepName);
	};
	
/* 
MM-MMF-SUBTITLE-CRP-0102 Test sending a 'drawframe' message to the CRP 
using an invalid dirty region
*/
class RSubtitleGraphicTestNegDrawFrame2 : public RSubtitleGraphicTestStep
	{
public:
	static RSubtitleGraphicTestNegDrawFrame2* NewL(const TDesC& aStepName);
	~RSubtitleGraphicTestNegDrawFrame2();
	
	// From RTestStep
	TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    TVerdict DoTestStepL();
    
private:
	void ConstructL();
	RSubtitleGraphicTestNegDrawFrame2(const TDesC& aStepName);
	};
	

/* 
MM-MMF-SUBTITLE-CRP-0103 Test sending a 'Initialize' message to the CRP
using bad bitmap handles
*/
class RSubtitleGraphicTestNegInitialize : public RSubtitleGraphicTestStep
	{
public:
	static RSubtitleGraphicTestNegInitialize* NewL(const TDesC& aStepName);
	~RSubtitleGraphicTestNegInitialize();
	
	// From RTestStep
	TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    TVerdict DoTestStepL();
    
private:
	void ConstructL();
	RSubtitleGraphicTestNegInitialize(const TDesC& aStepName);
	};


/* 
MM-MMF-SUBTITLE-CRP-0104 Test sending a 'swapframe' message to the CRP
using an invalid dirty region
*/
class RSubtitleGraphicTestNegSwapFrame1 : public RSubtitleGraphicTestStep
	{
public:
	static RSubtitleGraphicTestNegSwapFrame1* NewL(const TDesC& aStepName);
	~RSubtitleGraphicTestNegSwapFrame1();
	
	// From RTestStep
	TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    TVerdict DoTestStepL();
    
private:
	void ConstructL();
	RSubtitleGraphicTestNegSwapFrame1(const TDesC& aStepName);
	};

/* 
MM-MMF-SUBTITLE-CRP-0105
Test sending a 'swapframe' message to the CRP using an invalid expected buffer
*/
class RSubtitleGraphicTestNegSwapFrame2 : public RSubtitleGraphicTestStep
	{
public:
	static RSubtitleGraphicTestNegSwapFrame2* NewL(const TDesC& aStepName);
	~RSubtitleGraphicTestNegSwapFrame2();
	
	// From RTestStep
	TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    TVerdict DoTestStepL();
    
private:
	void ConstructL();
	RSubtitleGraphicTestNegSwapFrame2(const TDesC& aStepName);
	};
	
/* 
MM-MMF-SUBTITLE-CRP-0106
Test sending a 'swapframe' message to the CRP using an invalid (negative) display duration
*/
class RSubtitleGraphicTestNegDuration : public RSubtitleGraphicTestStep
	{
public:
	static RSubtitleGraphicTestNegDuration* NewL(const TDesC& aStepName);
	~RSubtitleGraphicTestNegDuration();
	
	// From RTestStep
	TVerdict DoTestStepPreambleL();	
    TVerdict DoTestStepPostambleL();
    TVerdict DoTestStepL();
    
private:
	void ConstructL();
	RSubtitleGraphicTestNegDuration(const TDesC& aStepName);
	};

#endif // TSU_MMF_SUBTITLEGRAPHICCLIENTNEG_H
