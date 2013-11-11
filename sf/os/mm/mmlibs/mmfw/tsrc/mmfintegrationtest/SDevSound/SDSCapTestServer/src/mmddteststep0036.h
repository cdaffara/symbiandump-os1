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

#ifndef __MMDDTESTSTEP0036_H__
#define __MMDDTESTSTEP0036_H__

#include "CapTestStep.h"

class CSecDevSndTS0036 : public CDevSoundTestStepBase
	{
public:
	static CSecDevSndTS0036* NewL(TThreadId aClientTid);
	virtual void StartProcessing(TRequestStatus& aStatus);
	virtual ~CSecDevSndTS0036();

protected:
	CSecDevSndTS0036(TThreadId aClientTid);
	void ConstructL();
	//TVerdict DoTestSetClientThread();
	virtual void ToneFinished (TInt aError);

	TVerdict DoPlaySimpleTone();
	TVerdict TestPlayTone(TInt aFreq, TTimeIntervalMicroSeconds aDur);

private:
	TThreadId iClientTid;
	};


#endif	// __MMDDTESTSTEP0036_H__
