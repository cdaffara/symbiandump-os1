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

#include "tsu_mmf_subtitlegraphicclientpos.h"
#include "subtitlecommonutils.h"

// constants
// common
const TInt KDisplayDuration(3000000);
const TInt KDefaultWaitInterval(3000000);
const TInt KDefaultShortWaitInterval(2000000);

_LIT(KInfoPrintCRPInitSimple, 		"CRP: Init simple");
_LIT(KInfoPrintCRPInit,       		"CRP: Init");
_LIT(KInfoPrintCRPDrawFrame,  		"CRP: DrawFrame");
_LIT(KInfoPrintCRPSwapFrame,  		"CRP: SwapFrame");
_LIT(KInfoPrintCRPSwapFrameNoClear, "CRP: SwapFrame");
_LIT(KInfoPrintCRPClear,      		"CRP: Clear");
_LIT(KInfoPrintCRPHalfSwapFrame,  	"CRP: SwapFrame - 50% frame");

// MM-MMF-SUBTITLE-CRP-0001

// MM-MMF-SUBTITLE-CRP-0002
const TInt KExpectedFrame1(1);
const TInt KExpectedFrame2(2);
const TInt KExpectedFrame3(1);
const TInt KExpectedFrame4(2);
const TInt KExpectedFrame5(1);
const TInt KExpectedFrame6(1);
const TInt KDefaultNoDisplayDuration(0);

/* 
MM-MMF-SUBTITLE-CRP-U-0001-HP
Test sending a 'drawframe' message to the CRP, this test also includes multiple initialization calls
*/
RSubtitleGraphicTestStepDrawFrame* RSubtitleGraphicTestStepDrawFrame::NewL(const TDesC& aStepName)
	{
	RSubtitleGraphicTestStepDrawFrame* self = new (ELeave) RSubtitleGraphicTestStepDrawFrame(aStepName);
	return self;
	}

RSubtitleGraphicTestStepDrawFrame::RSubtitleGraphicTestStepDrawFrame(const TDesC& aStepName)
	:RSubtitleGraphicTestStep(aStepName)
	{
	
	}

TVerdict RSubtitleGraphicTestStepDrawFrame::DoTestStepPreambleL()
	{
	RSubtitleGraphicTestStep::DoTestStepPreambleL();
	
	// Test initializing multiple times
	
	// Send message to CRP
	User::InfoPrint(KInfoPrintCRPInitSimple);
	iCRPGraphic->Initialize();
	
	User::After(KDefaultWaitInterval);
	
	// Send message to CRP
	User::InfoPrint(KInfoPrintCRPInitSimple);
	iCRPGraphic->Initialize();
	
	User::After(KDefaultWaitInterval);
	
	if ( !CreateBitmap1L() )
		{
		return EFail;
		}
	
	if ( !CreateBitmap2L() )
		{
		return EFail;
		}
		
	if ( !CreateBitmap3L() )
		{
		return EFail;
		}
		
	return EPass;
	}


TVerdict RSubtitleGraphicTestStepDrawFrame::DoTestStepPostambleL()
	{
	DestroyBitmap1();
	DestroyBitmap2();
	DestroyBitmap3();
	return RSubtitleGraphicTestStep::DoTestStepPostambleL();
	}

TVerdict RSubtitleGraphicTestStepDrawFrame::DoTestStepL()
	{
	__MM_HEAP_MARK;
	
	TRect dirtyRegion1(iBitmap1->SizeInPixels());
	TRect dirtyRegion2(iBitmap2->SizeInPixels());
	TRect dirtyRegion3(iBitmap3->SizeInPixels());
	TTimeIntervalMicroSeconds displayDuration(KDisplayDuration);
	
	RSubtitleGraphicTestStep::StartCrpDrawL();
	
	User::InfoPrint(KInfoPrintCRPInitSimple);
	iCRPGraphic->Initialize();
	User::After(KDefaultWaitInterval);
	
	// Send message to CRP
	User::InfoPrint(KInfoPrintCRPInitSimple);
	iCRPGraphic->Initialize();
	User::After(KDefaultWaitInterval);
	
	User::InfoPrint(KInfoPrintCRPDrawFrame);
	if ( iCRPGraphic->DrawFrame(iBitmap1->Handle(), dirtyRegion1, displayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
		
	User::After(KDefaultWaitInterval);
	
	User::InfoPrint(KInfoPrintCRPDrawFrame);
	if ( iCRPGraphic->DrawFrame(iBitmap2->Handle(), dirtyRegion2, displayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
	
	User::After(KDefaultWaitInterval);
	
	// Draw a new frame before the current one expires
	User::InfoPrint(KInfoPrintCRPDrawFrame);
	if ( iCRPGraphic->DrawFrame(iBitmap3->Handle(), dirtyRegion3, displayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
	
	User::After(KDefaultShortWaitInterval);
	
	if ( iCRPGraphic->DrawFrame(iBitmap1->Handle(), dirtyRegion1, displayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
	
	__MM_HEAP_MARKEND;
	return EPass;
	}
    
/* 
MM-MMF-SUBTITLE-CRP-U-0002-HP
Test sending a 'swapframe' message to the CRP, this test also includes multiple initialization
calls, and multiple variations of Display Duration
Test sending a 'swapframe' message, reinitialize and send a 'drawframe' message to ensure the CRP
can change rendering modes.
*/
RSubtitleGraphicTestStepSwapFrame* RSubtitleGraphicTestStepSwapFrame::NewL(const TDesC& aStepName)
	{
	RSubtitleGraphicTestStepSwapFrame* self = new (ELeave) RSubtitleGraphicTestStepSwapFrame(aStepName);
	return self;
	}

RSubtitleGraphicTestStepSwapFrame::RSubtitleGraphicTestStepSwapFrame(const TDesC& aStepName)
	:RSubtitleGraphicTestStep(aStepName)
	{
	
	}

TVerdict RSubtitleGraphicTestStepSwapFrame::DoTestStepPreambleL()
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
		
	if ( !CreateBitmap3L() )
		{
		return EFail;
		}
	
	// Test initializing multiple times.
	
	// Send message to CRP
	User::InfoPrint(KInfoPrintCRPInit);
	if (iCRPGraphic->Initialize(iBitmap1->Handle(), iBitmap2->Handle()) != KErrNone )
		{
		return EFail;
		}
	
	// Send message to CRP
	User::InfoPrint(KInfoPrintCRPInit);
	if ( iCRPGraphic->Initialize(iBitmap1->Handle(), iBitmap2->Handle()) != KErrNone )
		{
		return EFail;
		}
		
			
	return EPass;
	}

TVerdict RSubtitleGraphicTestStepSwapFrame::DoTestStepPostambleL()
	{
	DestroyBitmap1();
	DestroyBitmap2();
	DestroyBitmap3();
	return RSubtitleGraphicTestStep::DoTestStepPostambleL();
	}

TVerdict RSubtitleGraphicTestStepSwapFrame::DoTestStepL()
	{
	__MM_HEAP_MARK;
	
	RSubtitleGraphicTestStep::StartCrpDrawL();
	
	TRect dirtyRegion1(iBitmap1->SizeInPixels());
	TRect dirtyRegion2(iBitmap2->SizeInPixels());
	
	//User::InfoPrint(KInfoPrintCRPSwapFrame);
	if ( iCRPGraphic->SwapFrame(KExpectedFrame1, dirtyRegion1, KDisplayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}

	// Wait for the CRP to process the swap frame message
	User::After(KDefaultWaitInterval);
	
	User::InfoPrint(KInfoPrintCRPSwapFrame);
	if ( iCRPGraphic->SwapFrame(KExpectedFrame2, dirtyRegion2, KDisplayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
		
	// Wait for the CRP to process the swap frame message
	User::After(KDefaultWaitInterval);
	
	User::InfoPrint(KInfoPrintCRPSwapFrame);
	if ( iCRPGraphic->SwapFrame(KExpectedFrame3, dirtyRegion1, KDisplayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}

	// Wait for the CRP to process the swap frame message
	User::After(KDefaultWaitInterval);
	
	// Test multiple duration states
	User::InfoPrint(KInfoPrintCRPSwapFrameNoClear);
	if ( iCRPGraphic->SwapFrame(KExpectedFrame4, dirtyRegion2, KDisplayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
		
	// Wait for the CRP to clear the subtitle frame
	User::After(KDisplayDuration*1.2);
	
	// Test multiple duration states - Duration set to 0 frame should never be cleared
	User::InfoPrint(KInfoPrintCRPSwapFrame);
	if ( iCRPGraphic->SwapFrame(KExpectedFrame5, dirtyRegion1, KDefaultNoDisplayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}	
	
	// Wait for the CRP to process the swap frame message
	User::After(KDefaultWaitInterval);
	
	// Test dirty region code path
	
	// Render frame 1
	User::InfoPrint(KInfoPrintCRPSwapFrame);
	if ( iCRPGraphic->SwapFrame(KExpectedFrame6, dirtyRegion1, KDefaultNoDisplayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
		
	User::After(KDefaultWaitInterval);
	
	// Render frame 2 with 50% of the frame to illustrate the dirtyregion cleaup within the CRP
	TRect halfDirtyRegion(TSize(dirtyRegion2.Width()*0.5, dirtyRegion2.Height()));
	User::InfoPrint(KInfoPrintCRPHalfSwapFrame);
	if ( iCRPGraphic->SwapFrame(KExpectedFrame6, halfDirtyRegion, KDefaultNoDisplayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
		
	User::After(KDefaultWaitInterval);
	
	// Reinitialize for drawframe() rendering
	User::InfoPrint(KInfoPrintCRPInitSimple);
	iCRPGraphic->Initialize();
	User::After(KDefaultWaitInterval);
	
	TRect dirtyRegion(iBitmap1->SizeInPixels());
	User::InfoPrint(KInfoPrintCRPDrawFrame);
	if ( iCRPGraphic->DrawFrame(iBitmap3->Handle(), dirtyRegion, KDisplayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
	
	// Reinitialize for swapframe() rendering
	User::InfoPrint(KInfoPrintCRPInit);
	if ( iCRPGraphic->Initialize(iBitmap1->Handle(), iBitmap2->Handle()) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
		
	User::After(KDefaultWaitInterval);
	
	User::InfoPrint(KInfoPrintCRPSwapFrame);
	if ( iCRPGraphic->SwapFrame(KExpectedFrame6, dirtyRegion, KDisplayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
	
	User::After(KDefaultWaitInterval);	
	
	// Reinitialize for drawframe() rendering
	User::InfoPrint(KInfoPrintCRPInitSimple);
	iCRPGraphic->Initialize();
	User::After(KDefaultWaitInterval);
	
	User::InfoPrint(KInfoPrintCRPDrawFrame); 
	if ( iCRPGraphic->DrawFrame(iBitmap3->Handle(), dirtyRegion, KDisplayDuration) != KErrNone ) 
		{
		__MM_HEAP_MARKEND;
		return EFail; 
		}
	
	__MM_HEAP_MARKEND;
	return EPass; 
	}
	
/* 
MM-MMF-SUBTITLE-CRP-U-0003-HP
Test requests the current CRP UID 
*/
RSubtitleGraphicTestStepGetUid* RSubtitleGraphicTestStepGetUid::NewL(const TDesC& aStepName)
	{
	RSubtitleGraphicTestStepGetUid* self = new (ELeave) RSubtitleGraphicTestStepGetUid(aStepName);
	return self;
	}

RSubtitleGraphicTestStepGetUid::RSubtitleGraphicTestStepGetUid(const TDesC& aStepName)
	:RSubtitleGraphicTestStep(aStepName)
	{
	
	}

TVerdict RSubtitleGraphicTestStepGetUid::DoTestStepL()
	{
	__MM_HEAP_MARK;
	
	RSubtitleGraphicTestStep::StartCrpDrawL();
	
	if ( iCRPGraphic->Id().Id() == NULL )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
		
	if ( !(iCRPGraphic->Id().IsId()) )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
		
	if ( iCRPGraphic->Id().IsUid() )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
	
	__MM_HEAP_MARKEND;
	return EPass;
	}
	
/* 
MM-MMF-SUBTITLE-CRP-U-0004-HP Test sending a 'clear' message to the CRP, the clear message is 
sent multiple times
*/
RSubtitleGraphicTestStepClear* RSubtitleGraphicTestStepClear::NewL(const TDesC& aStepName)
	{
	RSubtitleGraphicTestStepClear* self = new (ELeave) RSubtitleGraphicTestStepClear(aStepName);
	return self;
	}

RSubtitleGraphicTestStepClear::RSubtitleGraphicTestStepClear(const TDesC& aStepName)
	:RSubtitleGraphicTestStep(aStepName)
	{
	
	}

TVerdict RSubtitleGraphicTestStepClear::DoTestStepPreambleL()
	{
	RSubtitleGraphicTestStep::DoTestStepPreambleL();
	
	if ( !CreateBitmap1L() )
		{
		return EFail;
		}
	
	// Send message to CRP
	User::InfoPrint(KInfoPrintCRPInitSimple);
	iCRPGraphic->Initialize();
			
	return EPass;
	}

TVerdict RSubtitleGraphicTestStepClear::DoTestStepPostambleL()
	{
	DestroyBitmap1();
	return RSubtitleGraphicTestStep::DoTestStepPostambleL();
	}

TVerdict RSubtitleGraphicTestStepClear::DoTestStepL()
	{
	__MM_HEAP_MARK;
	
	RSubtitleGraphicTestStep::StartCrpDrawL();
	
	TRect dirtyRegion(iBitmap1->SizeInPixels());
	
	User::InfoPrint(KInfoPrintCRPDrawFrame);
	
	if ( iCRPGraphic->DrawFrame(iBitmap1->Handle() ,dirtyRegion, KDisplayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
	
	// Wait for CRP to process the DrawFrame message
	User::After(KDefaultWaitInterval);
	
	// Ask the CRP to clear the still active frame from the screen.
	User::InfoPrint(KInfoPrintCRPClear);
	
	// Ask the CRP to clear the still active frame from the screen.
	iCRPGraphic->Clear();
	
	User::After(KDefaultWaitInterval);
	
	//
	// Draw again to ensure the Crp is still functioning
	User::InfoPrint(KInfoPrintCRPDrawFrame);
	if ( iCRPGraphic->DrawFrame(iBitmap1->Handle() ,dirtyRegion, KDisplayDuration) != KErrNone )
		{
		__MM_HEAP_MARKEND;
		return EFail;
		}
	
	__MM_HEAP_MARKEND;
	return EPass;
	}
