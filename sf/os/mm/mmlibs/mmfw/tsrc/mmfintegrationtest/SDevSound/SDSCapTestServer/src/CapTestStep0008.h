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

#ifndef __SECUREDEVSOUNDTESTSTEP0008_H__
#define __SECUREDEVSOUNDTESTSTEP0008_H__

#include "CapTestStep.h"

class CSecDevSndTS0008 : public CDevSoundTestStepBase
	{
public:
	static CSecDevSndTS0008* NewL();
	void ConstructL();
	virtual void StartProcessing(TRequestStatus& aStatus);
	virtual ~CSecDevSndTS0008();

protected:
	TVerdict DoPlayData();
	TVerdict PlayDataFile(TDesC& aFilename);

	TVerdict TestInitialize(TMMFState aMode);
	TVerdict TestPlayInit();
	TVerdict TestPlayData();

	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError (TInt aError);

	TBool iIsFirst;

	};

#endif	// __SECUREDEVSOUNDTESTSTEP0008_H__
