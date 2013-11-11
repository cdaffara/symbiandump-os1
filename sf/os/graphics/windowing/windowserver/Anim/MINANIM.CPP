// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Template for writing Anim DLL's
// 
//

#include "mincmd.h"
#include "minanim.h"


GLDEF_C TInt E32Dll(TDllReason)
	{
	return(KErrNone);
	}

EXPORT_C CAnimDll *CreateCAnimDllL()
	{
	return(new(ELeave) CMinAnimDll());
	}


/*CMinAnimDll*/

CAnim *CMinAnimDll::CreateInstanceL(TInt aType)
	{
    switch (aType)
        {
    case EMinAnimWindow:
	    return new(ELeave) CMinWindowAnim();
	    
    case EMinAnimHandwriting:
	    return new(ELeave) CMinHandAnim();
	    
    default:         
        User::Leave(KErrArgument);
        }
    return NULL;    // dummy return to prevent compiler error
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
	}

void CAnimateMbm::Command(TInt aOpcode,TAny *aParams)
	{
	switch (aOpcode)
		{
	case EMbmOpActivate:
		//iFunctions->SetSync(MAnimGeneralFunctions::ESyncSecond);
		iTimer->After(iInterval);
		iRunning=ETrue;
		break;
	case EMbmOpDeactivate:
		//iFunctions->SetSync(MAnimGeneralFunctions::ESyncNone);
		iRunning=EFalse;
		break;
	case EMbmOpFaster:
		iInterval=(19*iInterval.Int())/20;
		break;
	case EMbmOpSlower:
		iInterval=(21*iInterval.Int())/20;
		break;
	case EMbmOpSetFileName:
		iName=*STATIC_CAST(TBuf<32>*,aParams);
		iIndex=0;
		break;
	default:
		iFunctions->Panic();
		}
	}

void CAnimateMbm::FocusChanged(TBool )
	{
	}

TInt CAnimateMbm::CommandReplyL(TInt /*aOpcode*/,TAny* /*aParams*/)
	{
	return KErrNone;
	}
