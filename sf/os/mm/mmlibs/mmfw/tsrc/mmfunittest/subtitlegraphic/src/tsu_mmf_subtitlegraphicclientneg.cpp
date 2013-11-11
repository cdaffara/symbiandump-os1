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

#include "tsu_mmf_subtitlegraphicclientneg.h"
#include "subtitlecommonutils.h"

// constants
// common
const TInt  KDisplayDuration(1000000);

// MM-MMF-SUBTITLE-CRP-0101
const TInt KBadBitmapHandle(0xFF00FF00);

// MM-MMF-SUBTITLE-CRP-0102
const TInt KExpectedFrame1(1);

// MM-MMF-SUBTITLE-CRP-0104
const TInt KExpectedFrame(1);

// MM-MMF-SUBTITLE-CRP-0105
const TInt KBadExpectedFrame(3);

// MM-MMF-SUBTITLE-CRP-0106
const TInt KInvalidDisplayDuration(-200);

/* 
MM-MMF-SUBTITLE-CRP-U-0101-HP
Test sending a 'drawframe' message to the CRP using an invalid bitmap handle
Test sending a 'drawframe' message to the CRP using an invalid initialization method
*/
RSubtitleGraphicTestNegDrawFrame* RSubtitleGraphicTestNegDrawFrame::NewL(const TDesC& aStepName)
	{
	RSubtitleGraphicTestNegDrawFrame* self = new (ELeave) RSubtitleGraphicTestNegDrawFrame(aStepName);
	return self;
	}

RSubtitleGraphicTestNegDrawFrame::RSubtitleGraphicTestNegDrawFrame(const TDesC& aStepName)
	:RSubtitleGraphicTestStep(aStepName)
	{
	
	}

TVerdict RSubtitleGraphicTestNegDrawFrame::DoTestStepPreambleL()
	{
	RSubtitleGraphicTestStep::DoTestStepPreambleL();
	
	if ( !CreateBitmap1L() )
		{
		return EFail;
		}
		
	if ( !CreateBitmap2L() )
		{
		return EFail;
		}
		
	return EPass;
	}

TVerdict RSubtitleGraphicTestNegDrawFrame::DoTestStepPostambleL()
	{
	DestroyBitmap1();
	DestroyBitmap2();
	return RSubtitleGraphicTestStep::DoTestStepPostambleL();
	}

TVerdict RSubtitleGraphicTestNegDrawFrame::DoTestStepL()
	{
	__MM_HEAP_MARK;
	
	RSubtitleGraphicTestStep::StartCrpDrawL();
	
	TTimeIntervalMicroSeconds displayDuration(KDisplayDuration);
	TSize testBitmapSize(iBitmap1->SizeInPixels());
	
	// Send message to CRP
	iCRPGraphic->Initialize();
	
	if ( iCRPGraphic->DrawFrame(KBadBitmapHandle, testBitmapSize, displayDuration ) != KErrArgument )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
	
	// Reinitialize for swapframe calls
	TEST(iCRPGraphic->Initialize(iBitmap1->Handle(), iBitmap2->Handle()) == KErrNone );	
	
	if ( iCRPGraphic->DrawFrame(iBitmap1->Handle(), testBitmapSize, displayDuration ) != KErrNotReady )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
		
	// Try a swapframe to ensure the CRP can still render correctly.
	TRect dirtyRegion(iBitmap1->SizeInPixels());
	if ( iCRPGraphic->SwapFrame(KExpectedFrame1, dirtyRegion, displayDuration ) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}

	__MM_HEAP_MARKEND;
	return EPass;
	}
    
/* 
MM-MMF-SUBTITLE-CRP-0102
Test sending a 'drawframe' message to the CRP using an invalid dirty region
*/
RSubtitleGraphicTestNegDrawFrame2* RSubtitleGraphicTestNegDrawFrame2::NewL(const TDesC& aStepName)
	{
	RSubtitleGraphicTestNegDrawFrame2* self = new (ELeave) RSubtitleGraphicTestNegDrawFrame2(aStepName);
	return self;
	}

RSubtitleGraphicTestNegDrawFrame2::RSubtitleGraphicTestNegDrawFrame2(const TDesC& aStepName)
	:RSubtitleGraphicTestStep(aStepName)
	{
	
	}

TVerdict RSubtitleGraphicTestNegDrawFrame2::DoTestStepPreambleL()
	{
	RSubtitleGraphicTestStep::DoTestStepPreambleL();
	
	// Send message to CRP
	iCRPGraphic->Initialize();
	
	if ( !CreateBitmap1L() )
		{
		return EFail;
		}
		
	return EPass;
	}

TVerdict RSubtitleGraphicTestNegDrawFrame2::DoTestStepPostambleL()
	{
	DestroyBitmap1();
	return RSubtitleGraphicTestStep::DoTestStepPostambleL();
	}

TVerdict RSubtitleGraphicTestNegDrawFrame2::DoTestStepL()
	{
	__MM_HEAP_MARK;
	
	RSubtitleGraphicTestStep::StartCrpDrawL();
	
	TRect invalidDirtyRegion(0,0,0,0);
	if ( iCRPGraphic->DrawFrame(iBitmap1->Handle(), invalidDirtyRegion, TTimeIntervalMicroSeconds(0)) != KErrArgument )
		{
		__MM_HEAP_MARKEND;
		ERR_PRINTF1(_L("Failed on iCRPGraphic->DrawFrame(iBitmap1->Handle(), KInvalidDirtyRegion, TTimeIntervalMicroSeconds(0)"));
		return EFail;
		}
	
	__MM_HEAP_MARKEND;
	return EPass;
	}
	
/* 
MM-MMF-SUBTITLE-CRP-0103
Test sending a 'Initialize' message to the CRP using bad bitmap handles
*/
RSubtitleGraphicTestNegInitialize* RSubtitleGraphicTestNegInitialize::NewL(const TDesC& aStepName)
	{
	RSubtitleGraphicTestNegInitialize* self = new (ELeave) RSubtitleGraphicTestNegInitialize(aStepName);
	return self;
	}

RSubtitleGraphicTestNegInitialize::RSubtitleGraphicTestNegInitialize(const TDesC& aStepName)
	:RSubtitleGraphicTestStep(aStepName)
	{
	
	}
	
TVerdict RSubtitleGraphicTestNegInitialize::DoTestStepPreambleL()
	{
	RSubtitleGraphicTestStep::DoTestStepPreambleL();
	
	if ( !CreateBitmap1L() )
		{
		return EFail;
		}
		
	return EPass;
	}

TVerdict RSubtitleGraphicTestNegInitialize::DoTestStepPostambleL()
	{
	DestroyBitmap1();
	return RSubtitleGraphicTestStep::DoTestStepPostambleL();
	}

TVerdict RSubtitleGraphicTestNegInitialize::DoTestStepL()
	{
	__MM_HEAP_MARK;
	
	RSubtitleGraphicTestStep::StartCrpDrawL();
	
	// Send message to CRP
	if ( iCRPGraphic->Initialize(iBitmap1->Handle(), KBadBitmapHandle) != KErrArgument )
		{
		__MM_HEAP_MARKEND;
		ERR_PRINTF1(_L("iCRPGraphic->Initialize(iBitmap1->Handle(), KBadBitmapHandle))"));
		return EFail;
		}
		
	// Send message to CRP
	if ( iCRPGraphic->Initialize(KBadBitmapHandle, iBitmap1->Handle()) != KErrArgument )
		{
		__MM_HEAP_MARKEND;
		ERR_PRINTF1(_L("iCRPGraphic->Initialize(KBadBitmapHandle, iBitmap1->Handle())"));
		return EFail;
		}	
		
	// Send message to CRP
	if ( iCRPGraphic->Initialize(KBadBitmapHandle, KBadBitmapHandle) != KErrArgument )
		{
		__MM_HEAP_MARKEND;
		ERR_PRINTF1(_L("iCRPGraphic->Initialize(KBadBitmapHandle, KBadBitmapHandle)"));
		return EFail;
		}
	
	__MM_HEAP_MARKEND;
	return EPass;
	}
	
/* 
MM-MMF-SUBTITLE-CRP-0104
Test sending a 'swapframe' message to the CRP using an invalid dirty region
Test sending a 'swapframe' message to the CRP after an incorrect initialise funtion call
*/
RSubtitleGraphicTestNegSwapFrame1* RSubtitleGraphicTestNegSwapFrame1::NewL(const TDesC& aStepName)
	{
	RSubtitleGraphicTestNegSwapFrame1* self = new (ELeave) RSubtitleGraphicTestNegSwapFrame1(aStepName);
	return self;
	}

RSubtitleGraphicTestNegSwapFrame1::RSubtitleGraphicTestNegSwapFrame1(const TDesC& aStepName)
	:RSubtitleGraphicTestStep(aStepName)
	{
	
	}

TVerdict RSubtitleGraphicTestNegSwapFrame1::DoTestStepPreambleL()
	{
	RSubtitleGraphicTestStep::DoTestStepPreambleL();
	
	if ( !CreateBitmap1L() )
		{
		return EFail;
		}
		
	if ( !CreateBitmap2L() )
		{
		return EFail;
		}
	
	// Send message to CRP
	TEST(iCRPGraphic->Initialize(iBitmap1->Handle(), iBitmap2->Handle()) == KErrNone);
		
	return EPass;
	}

TVerdict RSubtitleGraphicTestNegSwapFrame1::DoTestStepPostambleL()
	{
	DestroyBitmap1();
	DestroyBitmap2();
	return RSubtitleGraphicTestStep::DoTestStepPostambleL();
	}

TVerdict RSubtitleGraphicTestNegSwapFrame1::DoTestStepL()
	{
	__MM_HEAP_MARK;
	
	RSubtitleGraphicTestStep::StartCrpDrawL();
	
	TRect dirtyRegion(iBitmap1->SizeInPixels());
	TRect invalidDirtyRegion(0,0,0,0);
	
	if ( iCRPGraphic->SwapFrame(KExpectedFrame, invalidDirtyRegion, KDisplayDuration) != KErrArgument )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
		
	// Test sending a swapframe message after using the wrong initialize function
	iCRPGraphic->Initialize();
	
	if ( iCRPGraphic->SwapFrame(KExpectedFrame, dirtyRegion, TTimeIntervalMicroSeconds(0)) != KErrNotReady )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}

	__MM_HEAP_MARKEND;
	return EPass;
	}

/* 
MM-MMF-SUBTITLE-CRP-0105
Test sending a 'swapframe' message to the CRP using an invalid expected buffer
*/
RSubtitleGraphicTestNegSwapFrame2* RSubtitleGraphicTestNegSwapFrame2::NewL(const TDesC& aStepName)
	{
	RSubtitleGraphicTestNegSwapFrame2* self = new (ELeave) RSubtitleGraphicTestNegSwapFrame2(aStepName);
	return self;
	}

RSubtitleGraphicTestNegSwapFrame2::RSubtitleGraphicTestNegSwapFrame2(const TDesC& aStepName)
	:RSubtitleGraphicTestStep(aStepName)
	{
	
	}

TVerdict RSubtitleGraphicTestNegSwapFrame2::DoTestStepPreambleL()
	{
	RSubtitleGraphicTestStep::DoTestStepPreambleL();
	
	if ( !CreateBitmap1L() )
		{
		return EFail;
		}
		
	if ( !CreateBitmap2L() )
		{
		return EFail;
		}
	
	// Send message to CRP
	TEST(iCRPGraphic->Initialize(iBitmap1->Handle(), iBitmap2->Handle()) == KErrNone);
	
	return EPass;
	}

TVerdict RSubtitleGraphicTestNegSwapFrame2::DoTestStepPostambleL()
	{
	DestroyBitmap1();
	DestroyBitmap2();
	return RSubtitleGraphicTestStep::DoTestStepPostambleL();
	}

TVerdict RSubtitleGraphicTestNegSwapFrame2::DoTestStepL()
	{
	__MM_HEAP_MARK;
	
	RSubtitleGraphicTestStep::StartCrpDrawL();
		
	if ( iCRPGraphic->SwapFrame(KBadExpectedFrame, TRect(iBitmap1->SizeInPixels()), KDisplayDuration) != KErrArgument )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
		
	__MM_HEAP_MARKEND;
	
	return EPass;
	}

/* 
MM-MMF-SUBTITLE-CRP-0106
Test sending a 'swapframe' message to the CRP using an invalid (negative) display duration
Test sending a 'drawframe' message to the CRP using an invalid (negative) display duration
*/
RSubtitleGraphicTestNegDuration* RSubtitleGraphicTestNegDuration::NewL(const TDesC& aStepName)
	{
	RSubtitleGraphicTestNegDuration* self = new (ELeave) RSubtitleGraphicTestNegDuration(aStepName);
	return self;
	}

RSubtitleGraphicTestNegDuration::RSubtitleGraphicTestNegDuration(const TDesC& aStepName)
	:RSubtitleGraphicTestStep(aStepName)
	{
	
	}

TVerdict RSubtitleGraphicTestNegDuration::DoTestStepPreambleL()
	{
	RSubtitleGraphicTestStep::DoTestStepPreambleL();
	
	if ( !CreateBitmap1L() )
		{
		return EFail;
		}
		
	if ( !CreateBitmap2L() )
		{
		return EFail;
		}
	
	// Send message to CRP
	TEST(iCRPGraphic->Initialize(iBitmap1->Handle(), iBitmap2->Handle()) == KErrNone);
	
	return EPass;
	}

TVerdict RSubtitleGraphicTestNegDuration::DoTestStepPostambleL()
	{
	DestroyBitmap1();
	DestroyBitmap2();
	return RSubtitleGraphicTestStep::DoTestStepPostambleL();
	}

TVerdict RSubtitleGraphicTestNegDuration::DoTestStepL()
	{
	__MM_HEAP_MARK;
	
	RSubtitleGraphicTestStep::StartCrpDrawL();
		
	if ( iCRPGraphic->SwapFrame(KExpectedFrame1, TRect(iBitmap1->SizeInPixels()), KInvalidDisplayDuration) != KErrArgument )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
		
	__MM_HEAP_MARKEND;
	
	return EPass;
	}
