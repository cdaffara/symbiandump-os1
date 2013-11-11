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

/**
 @file
 @test
 @internalComponent
*/

#include <e32math.h>
#include <fbs.h>		//CFbsBitmap, CFbsFont, CFbsTypefaceStore
#include <bitstd.h>		//CFbsBitGc
#include <bitdev.h>		//CFbsBitmapDevice
#include <f32file.h>	//RFs
#include <gdi.h>		//TFontSpec

#include "t_clockmbmgenerator.h"

//Hand lengths are for one particular 216 x 216 pixel bitmap
clockhand secondHand = {90, 9, 45, 255, 0, 0, 1, 60};
clockhand minuteHand = {90, 9, 45, 0,   0, 0, 2, 60*60};
clockhand hourHand   = {60, 9, 45, 0,   0, 0, 2, 60*60*60};
//

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("T_ClockMbmGenerator::E32Main - entry"));

//	__UHEAP_MARK; // mark heap state

	CTrapCleanup* TheTrapCleanup = CTrapCleanup::New();

	TRAPD(err, StartGeneratorL());
	if (err)
		{
		User::Panic(KTClockMbmGeneratorAppPanic, err);
		}

    delete TheTrapCleanup;

//	__UHEAP_MARKEND; // check no memory leak

	RDebug::Print(_L("T_ClockMbmGenerator::E32Main - exit"));
	return KErrNone;
	}

LOCAL_C void StartGeneratorL(void)
	{
	RDebug::Print(_L("Create Window server session"));
	RWsSession session;
	User::LeaveIfError(session.Connect());

	/************* Create file server for saving a bitmap ***********/
	RFs fs;
	User::LeaveIfError(fs.Connect());
	fs.SetNotifyUser(EFalse);

	TBufC<32> names[KMultiBitmapFileSize];
	TDesC* bmps[KMultiBitmapFileSize];
	TBufC<32> clockBmp[KMultiBitmapFileSize];

	CFbsBitmap* bitmapSource = new(ELeave) CFbsBitmap();
	User::LeaveIfError(bitmapSource->Create(TSize(0,0), EColor64K));

	for(TInt i=0; i<KMultiBitmapFileSize; i++)
		{
		bitmapSource->LockHeap();
		User::LeaveIfError(bitmapSource->Load(KClockFace, 0));
		bitmapSource->UnlockHeap();

		//Graphic Context to draw to bitmap
		CFbsBitGc* context = CFbsBitGc::NewL();
		CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmapSource);
		context->Activate(bitmapDevice);

		AddHand(i, secondHand, context);
		AddHand(i, minuteHand, context);
		AddHand(i, hourHand, context);
		AddFrameCount(i, context);

		clockBmp[i] = (TDesC&)KClockFrame;
		TPtr ptrClockBmp(clockBmp[i].Des());
		ptrClockBmp.AppendNumFixedWidth(i, EDecimal, KFixedNumWidth);
		ptrClockBmp.Append(KBmpSuffix);
		bmps[i] = &(clockBmp[i]);

		//Allow overwriting if file already exists
		TInt ret = fs.MkDir(clockBmp[i]);
		if (ret != KErrNone && ret != KErrAlreadyExists && ret != KErrPathNotFound)
			{
			User::Leave(ret);
			}

		bitmapSource->Save(clockBmp[i]);
		User::After(10000);

		delete bitmapDevice;
		delete context;
		}

	TBufC<40> multiClockBmp(KMultiClockFrame);
	TInt ret = fs.MkDir(KMultiClockFrame);
	if (ret != KErrNone && ret != KErrAlreadyExists && ret != KErrPathNotFound)
		{
		User::Leave(ret);
		}

	TInt32 ids[KMultiBitmapFileSize] = {0};
	TRAP(ret,CFbsBitmap::StoreL(multiClockBmp, KMultiBitmapFileSize, (const TDesC**)bmps,ids));

	delete bitmapSource;
	fs.Close();
	session.Close();
	}

LOCAL_C void AddHand(TInt aSecondsAfterMidnight, clockhand aHand, CFbsBitGc* context)
	{
	const TReal pi = 3.14159;

	//Calculate "bearing" of hand - 6 degrees is a 60ieth of 360, convert to radians
	TReal angle = (TReal)((aSecondsAfterMidnight % aHand.divisor)*2*pi/aHand.divisor);
	TReal result = 0;

	//Calculate deltas
	Math::Cos(result, angle + 2*pi*aHand.arrowAngle/360);
	TInt delta1X = (TInt)(aHand.arrowLength*result);

	Math::Sin(result, angle + 2*pi*aHand.arrowAngle/360);
	TInt delta1Y = (TInt)(aHand.arrowLength*result);

	Math::Cos(result, angle - 2*pi*aHand.arrowAngle/360);
	TInt delta2X = (TInt)(aHand.arrowLength*result);

	Math::Sin(result, angle - 2*pi*aHand.arrowAngle/360);
	TInt delta2Y = (TInt)(aHand.arrowLength*result);

	//Calculate absolute values of nodes
	Math::Cos(result, angle);
	TInt handEndY = 108 - (TInt)aHand.handLength*result;
	Math::Sin(result, angle);
	TInt handEndX = 108 + (TInt)aHand.handLength*result;

	//Draw complete hand
	context->SetPenColor(TRgb(aHand.red, aHand.green, aHand.blue));
	context->SetPenSize(TSize(aHand.handSize, aHand.handSize));
	context->DrawLine(TPoint(108,108), TPoint(handEndX, handEndY));
	context->DrawLine(TPoint(handEndX,handEndY), TPoint(handEndX + delta1X, handEndY + delta1Y));
	context->DrawLine(TPoint(handEndX,handEndY), TPoint(handEndX - delta2X, handEndY - delta2Y));
	}

LOCAL_C void AddFrameCount(TInt aFrameCount, CFbsBitGc* context)
	{
	CFbsFont* font;
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);

	//Font size determined visually
	TFontSpec spec(_L("Swiss"), 40);

	TInt err = ts->GetNearestFontToDesignHeightInPixels((CFont*&)font, spec);

	if(err != KErrNone)
		{
   		RDebug::Print(_L("Failed to obtain font, aborting"));
   		User::Exit(0);
   		}

    context->UseFont(font);

	//Append frame count to empty string
	TBufC<32> clockBmp;
	clockBmp = (TDesC&)KEmpty;
	TPtr ptrClockBmp(clockBmp.Des());
	ptrClockBmp.AppendNumFixedWidth(aFrameCount, EDecimal, KFixedNumWidth);

	//Draw position is determined visually
	context->DrawText(clockBmp, TPoint(118, 121));

	CleanupStack::PopAndDestroy(ts);
	context->DiscardFont();
	}