// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SECUREDEVSOUNDTESTSTEP0006_H__
#define __SECUREDEVSOUNDTESTSTEP0006_H__

#include "CapTestStep.h"

class CSecDevSndTS0006 : public CDevSoundTestStepBase
	{
public:
	static CSecDevSndTS0006* NewL();
	void ConstructL();
	virtual void StartProcessing(TRequestStatus& aStatus);
	virtual ~CSecDevSndTS0006();

protected:
	virtual void ToneFinished (TInt aError);

	TVerdict DoPlayDualTone();
	TVerdict TestPlayTone(TInt aFreq1, TInt aFreq2, TTimeIntervalMicroSeconds aDur);

private:
	TBool iIsFirstPlayed;

	};

#endif	// __SECUREDEVSOUNDTESTSTEP0006_H__
