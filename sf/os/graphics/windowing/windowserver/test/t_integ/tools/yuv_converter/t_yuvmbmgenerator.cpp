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

#include "t_yuvmbmgenerator.h"

LOCAL_D TUint32* destAddress;
LOCAL_D TSize srcBitmapSize;

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("T_MbmYuvGenerator::E32Main - entry"));

	__UHEAP_MARK; // mark heap state

	CTrapCleanup* TheTrapCleanup = CTrapCleanup::New();

	TRAPD(err, StartGeneratorL());
	if (err)
		{
		User::Panic(KTMbmYuvGeneratorAppPanic, err);
		}

    delete TheTrapCleanup;

	__UHEAP_MARKEND; // check no memory leak

	RDebug::Print(_L("T_MbmYuvGenerator::E32Main - exit"));
	return KErrNone;
	}

LOCAL_C void StartGeneratorL()
	{
	RDebug::Print(_L("Create Window server session"));
	RWsSession session;
	User::LeaveIfError(session.Connect());

	/************* Create file server for saving a bitmap ***********/
	RFs fs;
	User::LeaveIfError(fs.Connect());
	fs.SetNotifyUser(EFalse);

	TDesC* bmps[KMultiBitmapFileSize];
	TBufC<32> yuvBmp[KMultiBitmapFileSize];

	CFbsBitmap* bitmapSource = new(ELeave) CFbsBitmap();
	User::LeaveIfError(bitmapSource->Load(KMultiRgbFrame, 0));
	srcBitmapSize = bitmapSource->SizeInPixels();

	CFbsBitmap* bitmapDest = new(ELeave) CFbsBitmap();
	TSize destSize(srcBitmapSize.iWidth/2, srcBitmapSize.iHeight);
	User::LeaveIfError(bitmapDest->Create(destSize, EColor16MA));
	destAddress = bitmapDest->DataAddress();

	for(TInt i=0; i<KMultiBitmapFileSize; i++)
		{
		bitmapSource->LockHeap();
		User::LeaveIfError(bitmapSource->Load(KMultiRgbFrame, i));
		bitmapSource->UnlockHeap();

		//Convert the bitmap
		ConvertToYuv422(*bitmapSource);

		yuvBmp[i] = (TDesC&)KYuvFrame;
		TPtr ptrYuvBmp(yuvBmp[i].Des());
		ptrYuvBmp.AppendNumFixedWidth(i, EDecimal, KFixedNumWidth);
		ptrYuvBmp.Append(KBmpSuffix);
		bmps[i] = &(yuvBmp[i]);

		//Allow overwriting if file already exists
		TInt ret = fs.MkDir(yuvBmp[i]);
		if (ret != KErrNone && ret != KErrAlreadyExists && ret != KErrPathNotFound)
			{
			User::Leave(ret);
			}

		bitmapDest->Save(yuvBmp[i]);
		User::After(10000);
		}

	TBufC<40> multiYuvBmp(KMultiYuvFrame);
	TInt ret = fs.MkDir(KMultiYuvFrame);
	if (ret != KErrNone && ret != KErrAlreadyExists && ret != KErrPathNotFound)
		{
		User::Leave(ret);
		}

	TInt32 ids[KMultiBitmapFileSize] = {0};
	TRAP(ret,CFbsBitmap::StoreL(multiYuvBmp, KMultiBitmapFileSize, (const TDesC**)bmps,ids));

	delete bitmapDest;
	delete bitmapSource;
	fs.Close();
	session.Close();
	}

LOCAL_C void ConvertToYuv422(CFbsBitmap& bitmapSource)
	{	
	TUint16 Y0 = 0; TUint16 U0 = 0; TUint16 V0 = 0; TUint16 Y1 = 0;
	TUint16 U1 = 0; TUint16 V1 = 0; TUint16 U = 0;  TUint16 V = 0;

	TInt blue=0; TInt red=0; TInt green=0;
	
	TRgb pixel;
	
	/* YUV data packed into 16 bits */
	TInt counter=0;

	for(TInt i = 0; i < srcBitmapSize.iHeight; i++)
		{
		for(TInt j = 0; j < srcBitmapSize.iWidth; j+=2)
			{
			bitmapSource.GetPixel(pixel, TPoint(j,i));

			blue = pixel.Blue();
			red = pixel.Red();
			green = pixel.Green();

			Y0 = 0.2126*red + 0.7152*green + 0.0722*blue;
			U0 = -0.11457*red - 0.38543*green + 0.5*blue;
			V0 =  0.5*red - 0.45415*green - 0.04586*blue;

			bitmapSource.GetPixel(pixel, TPoint(j+1,i));

			blue = pixel.Blue();
			red = pixel.Red();
			green = pixel.Green();

			Y1 = 0.2126*red + 0.7152*green + 0.0722*blue;
			U1 = -0.11457*red - 0.38543*green + 0.5*blue;
			V1 =  0.5*red - 0.45415*green - 0.04586*blue;

			U = (U0 + U1)/2 + 127;
			V = (V0 + V1)/2 + 127;

			destAddress[counter++] = (Y1 << 24) + (V << 16) + (Y0 << 8) + U;
			}
		}
	}
