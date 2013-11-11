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

#include <e32uid.h>
#include <e32svr.h>
#include <u32hal.h>
#include <fbs.h>
#include "UTILS.H"
#include "fbshelper.h"
#include "fbsrasterizer.h"
#include "FbsMessage.H"

GLREF_C void Panic(TFbsPanic aPanic);


CFbsSessionHelper::CFbsSessionHelper(RFbsSession& aFbs)
	: CActive(CActive::EPriorityIdle), iFbs(aFbs)
	{
	if (CActiveScheduler::Current() != NULL)
		CActiveScheduler::Add(this);
#ifdef __WINS__
	TUint8* rasterizerSetting = NULL;
	UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalStringProperty, (TAny*)"FBSRASTERIZER_DLL", &rasterizerSetting);
	if (rasterizerSetting)
		{
		TBuf<80> rasterizerName;
		rasterizerName.Copy(TPtrC8(rasterizerSetting));
		const TUidType KFbsRasterizerLibraryUidType(KDynamicLibraryUid, KSharedLibraryUid, KFbsRasterizerLibraryUid);
		TInt err = iRasterizerLib.Load(rasterizerName, KFbsRasterizerLibraryUidType);
		if (err == KErrNone)
			{
			CFbsRasterizer* (*create)() = reinterpret_cast<CFbsRasterizer*(*)()>(iRasterizerLib.Lookup(1));
			iRasterizer = create();
			}
#ifdef _DEBUG
		else
			{
			RDebug::Printf("Failed to load extended bitmap rasterizer %s\r\n", rasterizerSetting);
			}
#endif
		}
	// else do nothing, as rasterizer is optional
#else
	iRasterizer = CFbsRasterizer::New();
#endif
#ifdef SYMBIAN_DEBUG_FBS_LOCKHEAP
	TInt err = iDebugMutex.OpenGlobal(KFBSERVDebugMutexName, EOwnerThread);
	if (err != KErrNone)
		Panic(EFbsPanicChunkError);
#endif
	}


CFbsSessionHelper::~CFbsSessionHelper()
	{
	if (IsAdded())
		Cancel();
	iBitmaps.Reset();
	delete iRasterizer;
#ifdef __WINS__
	iRasterizerLib.Close();
#endif
#ifdef SYMBIAN_DEBUG_FBS_LOCKHEAP
	iDebugMutex.Close();
#endif
	delete iExtraBuffer;
	}


TInt CFbsSessionHelper::AddBitmap(CFbsBitmap& aBitmap)
	{
	if (IsAdded())
		{
		TInt err = iBitmaps.InsertInAddressOrder(&aBitmap);
		if (err == KErrNone && !IsActive())
			{
			SetActive();
			iFbs.SendCommand(EFbsMessBitmapNotifyDirty, TIpcArgs(), iStatus);
			}
		return err;
		}
	return KErrNone;
	}


void CFbsSessionHelper::RemoveBitmap(CFbsBitmap& aBitmap)
	{
	if (IsAdded())
		{
		TInt index = iBitmaps.FindInAddressOrder(&aBitmap);
		if (index != KErrNotFound)
			{
			iBitmaps.Remove(index);
			if (iBitmaps.Count() == 0)
				Cancel();
			}
		}
	}


void CFbsSessionHelper::RunL()
	{
	if (iStatus == KErrNone && iBitmaps.Count() > 0)
		{
		for (TInt i = 0; i < iBitmaps.Count(); ++i)
			(void)iBitmaps[i]->CleanAddress();
		SetActive();
		iFbs.SendCommand(EFbsMessBitmapNotifyDirty, TIpcArgs(), iStatus);
		}
	}


void CFbsSessionHelper::DoCancel()
	{
	iFbs.SendCommand(EFbsMessBitmapCancelNotifyDirty);
	}
