// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Anim DLL to animate a bitmap
// 
//

#include "MBMCMD.H"
#include "MBMANIM.H"

#define DEFAUlT_LINE_WIDTH 4
#define DEFAUlT_MASK_WIDTH_FACTOR 3
#define DEFAUlT_END_POINT_FACTOR 2
#define BLACK TRgb::Gray2(0)
#define WHITE TRgb::Gray2(1)


EXPORT_C CAnimDll *CreateCAnimDllL()
	{
	return(new(ELeave) CAnimateMbmAnimDll());
	}


/*CAnimateMbmAnimDll*/

CAnim *CAnimateMbmAnimDll::CreateInstanceL(TInt /*aType*/)
	{
	return new(ELeave) CAnimateMbm();
	}


/*CAnimTimer*/

void CAnimTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

void CAnimTimer::DoCancel()
	{}

void CAnimTimer::RunL()
	{
	iAnimator->Animate();
	}


/*CAnimateMbm*/

CAnimateMbm::~CAnimateMbm()
	{
	delete iTimer;
	delete iBitmap;
	}

void CAnimateMbm::ConstructL(TAny *, TBool )
	{
	iTimer=new(ELeave) CAnimTimer(this);
	iTimer->ConstructL();
	CActiveScheduler::Add(iTimer);
	iInterval=100000;		//0.1 secs
	iBitmap=new(ELeave) CFbsBitmap();
	iWindowFunctions->SetRect(TRect(0,0,20,20));
	}

void CAnimateMbm::Animate()
	{
	if (!iRunning)
		return;
	iTimer->After(iInterval);
	++iIndex;
	while(LoadBitmap()!=KErrNone)
		iIndex=0;
	iWindowFunctions->ActivateGc();
	Redraw();
	WindowFunctions()->DeactivateGc();
	WindowFunctions()->Update();
	}

TBool CAnimateMbm::OfferRawEvent(const TRawEvent& /*aRawEvent*/)
	{
	return EFalse;
	}

void CAnimateMbm::Animate(TDateTime* /*aDateTime*/)
	{
	}

void CAnimateMbm::Redraw()
	{
	iGc->BitBlt(TPoint(),iBitmap);
	/*if (1>0)
		{
		iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		iGc->SetBrushColor(TRgb::Gray16(iIndex));
		iGc->DrawRect(TRect(5,5,300,300));
		}*/
	}

void CAnimateMbm::Command(TInt aOpcode,TAny* /*aParams*/)
	{
	switch (aOpcode)
		{
	case EMbmOpDeactivate:
		//iFunctions->SetSync(MAnimGeneralFunctions::ESyncNone);
		if (iRunning)
			{
			WindowFunctions()->ActivateGc();
			WindowFunctions()->DeactivateGc();
			iRunning=EFalse;
			}
		break;
	case EMbmOpFaster:
		iInterval=(19*iInterval.Int())/20;
		break;
	case EMbmOpSlower:
		iInterval=(21*iInterval.Int())/20;
		break;
	default:
		iFunctions->Panic();
		}
	}

void CAnimateMbm::FocusChanged(TBool )
	{
	}

TInt CAnimateMbm::CommandReplyL(TInt aOpcode,TAny* aParams)
	{
	switch (aOpcode)
		{
	case EMbmOpActivate:
		//iFunctions->SetSync(MAnimGeneralFunctions::ESyncSecond);
		if (!iRunning)
			{
			iRunning=ETrue;
			}
		iTimer->After(iInterval);
		break;
	case EMbmOpSetFileName:
		iName=*STATIC_CAST(TBuf<32>*,aParams);
		iIndex=0;
		User::LeaveIfError(LoadBitmap());
		break;
	default:
		iFunctions->Panic();
		}
	return KErrNone;
	}
