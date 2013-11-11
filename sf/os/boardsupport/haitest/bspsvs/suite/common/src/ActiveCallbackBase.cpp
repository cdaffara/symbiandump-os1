/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "ActiveCallbackBase.h"

CActiveCallbackBase::CActiveCallbackTimer* CActiveCallbackBase::CActiveCallbackTimer::NewL(CActive& aActive, TInt aPriority)
	{
	CActiveCallbackTimer*	self=new (ELeave) CActiveCallbackTimer(aActive, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CActiveScheduler::Add(self);
	CleanupStack::Pop(self);
	return self;
	}

CActiveCallbackBase::CActiveCallbackTimer::CActiveCallbackTimer(CActive& aActive, TInt aPriority)
:	CTimer(aPriority)
,	iActive(aActive)
	{
	}

void CActiveCallbackBase::CActiveCallbackTimer::RunL()
	{
	if ( iStatus.Int()==KErrNone )
		{
		//	Timer termiated without error
		if ( iActive.IsActive() )
			{
			iActive.Cancel();
			}
		}
	}

CActiveCallbackBase* CActiveCallbackBase::NewLC(CDataWrapperBase& aDataWrapperBase, TInt aPriority)
	{
	CActiveCallbackBase*    self=new(ELeave) CActiveCallbackBase(aDataWrapperBase, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CActiveCallbackBase* CActiveCallbackBase::NewL(CDataWrapperBase& aDataWrapperBase, TInt aPriority)
	{
	CActiveCallbackBase*	self=CActiveCallbackBase::NewLC(aDataWrapperBase, aPriority);
	CleanupStack::Pop();
	return self;
	}

CActiveCallbackBase::CActiveCallbackBase(CDataWrapperBase& aDataWrapperBase, TInt aPriority)
:	CActiveCallback(aDataWrapperBase, aPriority)
,	iDataWrapperBase(aDataWrapperBase)
,	iTimer(NULL)
	{
	}

void CActiveCallbackBase::ConstructL()
	{
	CActiveCallback::ConstructL();
	iTimer=CActiveCallbackTimer::NewL(*this);
	}

CActiveCallbackBase::~CActiveCallbackBase()
/**
 * Public destructor
 */
	{
	delete iTimer;
	iTimer=NULL;
	}

void CActiveCallbackBase::Activate(TInt aIndex, TInt aTimeout)
	{
	CActiveCallback::Activate(aIndex);
	if ( aTimeout!=0 )
		{
		iTimer->After(aTimeout);
		}
	}

void CActiveCallbackBase::KillTimer()
	{
	if ( iTimer->IsActive() )
		{
		iDataWrapperBase.INFO_PRINTF1(_L("CActiveCallbackBase::KillTimer"));
		iTimer->Cancel();
		}
	}
