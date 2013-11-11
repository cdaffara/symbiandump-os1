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

#ifndef __MEDIA_EVENT_H
#define __MEDIA_EVENT_H

#include <e32std.h>
#include <e32base.h>
#include "Parameters.h"
#include "EventTarget.h"
#include <StifTestModule.h>
#include <StifLogger.h>
#include "SimpleSoundPlayer.h"

//typedef TInt (*TMediaCommand)(const CParameters *params) ;
//typedef TInt (CSimpleSoundPlayer::*TcspMediaCommand)(const CParameters *params) ;

class CMediaEvent: public CBase
	{
public:
	static CMediaEvent *NewL(CTestModuleIf &aTestModuleIf, CStifLogger &aLogger, TTimeIntervalMicroSeconds32 aDelay, TTimeIntervalMicroSeconds32 aInterval, MEventTarget *aTarget, CParameters *aParameters, TInt aPriority=CActive::EPriorityStandard);
	static CMediaEvent *NewLC(CTestModuleIf &aTestModuleIf, CStifLogger &aLogger, TTimeIntervalMicroSeconds32 aDelay, TTimeIntervalMicroSeconds32 aInterval, MEventTarget *aTarget, CParameters *aParameters, TInt aPriority=CActive::EPriorityStandard);

	///static CMediaEvent *NewLC(CTestModuleIf &aTestModuleIf, CStifLogger &aLogger, TTimeIntervalMicroSeconds32 aDelay, TTimeIntervalMicroSeconds32 aInterval, MEventTarget *aTarget, tmc aCommand, CParameters *aParameters);
	TInt GetCount() const;

private:
	void ConstructL(TInt aPriority);
	~CMediaEvent();

	static TInt Tick(TAny* aObject);
	TInt DoTick();

protected:
	CMediaEvent(CTestModuleIf &aTestModuleIf , CStifLogger &aLogger, TTimeIntervalMicroSeconds32 aDelay, TTimeIntervalMicroSeconds32 aInterval, MEventTarget *aTarget , /*TMediaCommand aCommand,*/ CParameters *aParameters);
	//-///CMediaEvent(CTestModuleIf &aTestModuleIf , CStifLogger &aLogger, TTimeIntervalMicroSeconds32 aDelay, TTimeIntervalMicroSeconds32 aInterval, MEventTarget *aTarget , tmc aCommand , CParameters *aParameters);

private:
	CTestModuleIf &iTestModuleIf;	//Test Module Interface
	CStifLogger &iLogger;	//Test Module Logger

	TTimeIntervalMicroSeconds32 iDelay;
	TTimeIntervalMicroSeconds32 iInterval;
	MEventTarget *iEventTarget;
	//TMediaCommand iCommand;
	CParameters *iParameters;

	CPeriodic *iPeriodic;
	TInt iCount;
	};

#endif	//__MEDIA_EVENT_H
