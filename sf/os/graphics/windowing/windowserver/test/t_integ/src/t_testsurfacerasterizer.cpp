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

#include <e32debug.h>
#include <hal.h>
#include <limits.h>	//UINT_MAX

#include "t_testsurfacerasterizer.h"

EXPORT_C CTestSurfaceRasterizer* CTestSurfaceRasterizer::NewL(const TSurfaceId& aSurfaceId, TInt aSurfaceScreenNumber, RSurfaceManager* aSurfaceManager)
	{
	RDebug::Print(_L("Creating CTestSurfaceRasterizer class\n"));
    CTestSurfaceRasterizer* self = new (ELeave) CTestSurfaceRasterizer();
    CleanupStack::PushL(self);
    TInt ret = self->ConstructL(aSurfaceId, aSurfaceScreenNumber, aSurfaceManager);
    CleanupStack::Pop(); // self;

 	if(ret)
 		{
	 	return self;
		}
	else
		{
		return 0;
		}
	}

TBool CTestSurfaceRasterizer::ConstructL(const TSurfaceId& aSurfaceId, TInt aSurfaceScreenNumber, RSurfaceManager* aSurfaceManager)
	{
	RDebug::Print(_L("Constructing CTestSurfaceRasterizer\n"));

	iSurfaceId = aSurfaceId;

	iSurfaceScreenNumber  = aSurfaceScreenNumber;

	iSurfaceManager = aSurfaceManager;

	TInt err = iSurfaceManager->MapSurface(iSurfaceId, iChunk);

	if(err)
		{
		RDebug::Print(_L("Error mapping to surface\n"));
	 	User::Exit(0);
		}

	iInfoBuf.FillZ();

	iSurfaceManager->SurfaceInfo(iSurfaceId, iInfoBuf);

	return ETrue;
	}

EXPORT_C CTestSurfaceRasterizer::CTestSurfaceRasterizer()
	{
	}

CTestSurfaceRasterizer::~CTestSurfaceRasterizer()
	{
	iChunk.Close();
	}

EXPORT_C void CTestSurfaceRasterizer::DrawRect(TUint32* aBuffer, SurfaceDetails& aSurfDetails)
	{
	//Send the fullscreen bitmap to the test rasterizer

	RSurfaceManager::TSurfaceInfoV01& info = iInfoBuf();

	aSurfDetails.aSurfaceId = iSurfaceId;
	aSurfDetails.aSurfaceSize = info.iSize;
	aSurfDetails.aBufferNumber = iSurfaceCurrentBuffer;


	TInt offsetToCurrentBuffer;
	TInt err = iSurfaceManager->GetBufferOffset(iSurfaceId, iSurfaceCurrentBuffer, offsetToCurrentBuffer);
	if(err != KErrNone)
		{
		RDebug::Print(_L("Could not open buffer number\n"));
		return;
		}
	TUint32* surfaceBufferPtr = reinterpret_cast<TUint32*>(iChunk.Base() + offsetToCurrentBuffer);

	TInt noOfBytes =  (info.iStride * info.iSize.iHeight);
	Mem::Move(surfaceBufferPtr, aBuffer, noOfBytes);

	iSurfaceCurrentBuffer++;
	if(iSurfaceCurrentBuffer == info.iBuffers)
		{
		iSurfaceCurrentBuffer = 0;
		}
	}

 EXPORT_C void CTestSurfaceRasterizer::PerfTestL(TInt aFrameNumber)
 	{
 	//Send the fullscreen bitmap to the test rasterizer
 	RChunk aChunk;
 	RSurfaceManager aManager;
 	User::LeaveIfError(aManager.Open());

 	TInt err = aManager.OpenSurface(iSurfaceId);
  	if(err!=KErrNone)
 		{
 		RDebug::Print(_L("Error opening a surface\n"));
 		User::Exit(0);
 		}

 	err = aManager.MapSurface(iSurfaceId, aChunk);
 	if(err!=KErrNone)
 		{
 		RDebug::Print(_L("Error mapping to surface\n"));
 		User::Exit(0);
 		}

 	RSurfaceManager::TInfoBuf InfoBuf;
 	RSurfaceManager::TSurfaceInfoV01& info = InfoBuf();
 	InfoBuf.FillZ();

 	aManager.SurfaceInfo(iSurfaceId, InfoBuf);

 	TInt noOfPixels =  (info.iSize.iWidth * info.iSize.iHeight);

 	TInt offsetToCurrentBuffer;
	err = iSurfaceManager->GetBufferOffset(iSurfaceId, iSurfaceCurrentBuffer,offsetToCurrentBuffer);
	if(err != KErrNone)
		{
		RDebug::Print(_L("Could not open buffer number\n"));
		User::Exit(0);
		}
	TUint32* surfaceBufferPtr = reinterpret_cast<TUint32*>(aChunk.Base() + offsetToCurrentBuffer);

	if(aFrameNumber < info.iBuffers)
		{
		TInt sector = aFrameNumber % info.iBuffers;
		TInt offset = (noOfPixels * sector)/info.iBuffers;
		Mem::Fill((surfaceBufferPtr + offset), noOfPixels, TChar(0xFF));
		}

 	//Submit update
	TRect rc[1] =
			{
			TRect(0, 0, info.iSize.iWidth, info.iSize.iHeight)
			};

	RRegion region(1, rc);
	RSurfaceUpdateSession aSurfaceUpdateSession;

	err = aSurfaceUpdateSession.Connect();
	if (err!=KErrNone)
		{
		RDebug::Print(_L("error in connecting surface update session\n"));
	 	User::Exit(0);
		}

	TInt ret = aSurfaceUpdateSession.SubmitUpdate(KAllScreens, iSurfaceId, iSurfaceCurrentBuffer, &region);

	RDebug::Print(_L("Frame %d done\n"), aFrameNumber);

	if(ret != KErrNone)
		{
		RDebug::Print(_L("Error submitting update request\n"));
		}

	iSurfaceCurrentBuffer++;
	if(iSurfaceCurrentBuffer == info.iBuffers)
		{
		iSurfaceCurrentBuffer = 0;
		}

 	aChunk.Close();

 	err = aManager.CloseSurface(iSurfaceId);
 	if(err)
 		{
 		RDebug::Print(_L("Error closing the surface\n"));
 		}
 	aManager.Close();
	}
