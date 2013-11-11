/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: DRM Player
*
*/

#include "MediaEvent.h"

//void CMediaEvent::hi() {}

CMediaEvent* CMediaEvent::NewL(CTestModuleIf &aTestModuleIf , CStifLogger &aLogger,
											TTimeIntervalMicroSeconds32 aDelay , TTimeIntervalMicroSeconds32 aInterval,
											MEventTarget *aTarget , CParameters *aParameters , TInt aPriority)
	{
		CMediaEvent *__self = CMediaEvent::NewLC(aTestModuleIf , aLogger,
											aDelay , aInterval , aTarget,
											aParameters, aPriority);
		CleanupStack::Pop(__self);
		return __self;
	}

CMediaEvent* CMediaEvent::NewLC(CTestModuleIf &aTestModuleIf , CStifLogger &aLogger,
											TTimeIntervalMicroSeconds32 aDelay , TTimeIntervalMicroSeconds32 aInterval,
											MEventTarget *aTarget, CParameters *aParameters , TInt aPriority)
	{
		CMediaEvent *__self = new(ELeave) CMediaEvent(aTestModuleIf , aLogger , aDelay , aInterval , aTarget , aParameters);
		CleanupStack::PushL(__self);
		__self->ConstructL(aPriority);
		return __self;
	}

CMediaEvent::CMediaEvent (CTestModuleIf &aTestModuleIf , CStifLogger &aLogger,
						TTimeIntervalMicroSeconds32 aDelay, TTimeIntervalMicroSeconds32 aInterval,
						MEventTarget *aTarget , CParameters *aParameters)
					: iTestModuleIf(aTestModuleIf), iLogger(aLogger),
						iDelay(aDelay) , iInterval(aInterval) , iEventTarget(aTarget),
						iParameters(aParameters) , iCount(0)
	{}

void CMediaEvent::ConstructL(TInt aPriority=CActive::EPriorityStandard)
	{
	iLogger.Log(_L("Creating event with priority (%d)") , aPriority);
	iPeriodic=CPeriodic::NewL(aPriority);
	iPeriodic->Start(iDelay , iInterval , TCallBack(Tick, this));
	}

CMediaEvent::~CMediaEvent ()
	{
	iPeriodic->Cancel();
	delete iPeriodic;
	delete iParameters;
	}

TInt CMediaEvent::Tick(TAny *aObject)
	{
	return (static_cast<CMediaEvent*>(aObject) )->DoTick(); // cast, and call non-static function
	}

TInt CMediaEvent::DoTick()
	{
	iCount++;
	if (! iEventTarget->ExecuteL(iParameters)) {
		iPeriodic->Cancel();
	}
	return 0;
	}

TInt CMediaEvent::GetCount() const
	{
	return iCount;
	}
