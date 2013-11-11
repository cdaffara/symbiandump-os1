// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSU_MMF_MMRUF_STEP_H
#define TSU_MMF_MMRUF_STEP_H

#include <apgcli.h>
#include <testframework.h>
#include "definitions.h"

/*
**
*/
class RMMRUFTestStep : public RTestStep
	{
public:
	static RMMRUFTestStep* NewL(const TTestSpec* aTestSpec);
	~RMMRUFTestStep();

	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepL();
	TVerdict DoTestStepPostambleL();


protected:
	RMMRUFTestStep(const TTestSpec* aTestSpec);
	void ConstructL();

protected:
	const TTestSpec* iTestSpec;
	RAppArc iAppArc;
	};

/*
** This test examines some non-audio/video files in a directory
** to make sure they're not recognised accidentally by RUF.
*/
class RMMRUFDirTestStep : public RTestStep
	{
public:
	static RMMRUFDirTestStep* NewL();
	~RMMRUFDirTestStep();
	TVerdict DoTestStepL();

protected:
	RMMRUFDirTestStep();
	TVerdict DoRecogniseL(const TDesC& aFileName, RAppArc& aAppArc);
	void CheckFilesInDirectoryL(RAppArc& aAppArc, const TDesC& aDirPath, TBool& aFailure);
	};

/**
 *
 * DoRecogniseL OOM Tests
 *
 */
class RTestStepMMRUFOOM : public RTestStep
	{
public:
	~RTestStepMMRUFOOM();
	static RTestStepMMRUFOOM* NewL(const TTestSpec* aTestSpec);

	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepL();
	TVerdict DoTestStepPostambleL();

protected:
	RTestStepMMRUFOOM(const TTestSpec* aTestSpec);

protected:
	const TTestSpec* iTestSpec;
	RAppArc iAppArc;
	};

#endif
