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

#ifndef __SECUREDEVSOUNDTESTSTEP0004_H__
#define __SECUREDEVSOUNDTESTSTEP0004_H__

#include "CapTestStep.h"

class CSecDevSndTS0004 : public CDevSoundTestStepBase
	{
public:
	static CSecDevSndTS0004* NewL();
	void ConstructL();
	virtual void StartProcessing(TRequestStatus& aStatus);
	virtual ~CSecDevSndTS0004();

protected:
	virtual void ToneFinished (TInt aError);

	TVerdict DoPlayDTMFTone();
	TVerdict TestPlayDTMFString(const TDesC &aDTMFString);

private:
	TBool iIsFirstPlayed;

	};

#endif	// __SECUREDEVSOUNDTESTSTEP0004_H__
