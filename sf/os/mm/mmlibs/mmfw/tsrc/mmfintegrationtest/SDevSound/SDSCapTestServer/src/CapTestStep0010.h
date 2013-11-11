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

#ifndef __SECUREDEVSOUNDTESTSTEP0010_H__
#define __SECUREDEVSOUNDTESTSTEP0010_H__

#include "CapTestStep.h"

class CSecDevSndTS0010 : public CDevSoundTestStepBase
	{
public:
	static CSecDevSndTS0010* NewL();
	void ConstructL();
	virtual void StartProcessing(TRequestStatus& aStatus);
	virtual ~CSecDevSndTS0010();

protected:
	TVerdict DoRecordData();
	TVerdict RecordDataFile(TDesC& aFilename);

	TVerdict TestInitialize(TMMFState aMode);
	TVerdict TestRecordInit();
	TVerdict TestRecordData();

	void BufferToBeEmptied (CMMFBuffer* aBuffer);
	void RecordError (TInt aError);

	};

#endif	// __SECUREDEVSOUNDTESTSTEP0010_H__
