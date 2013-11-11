// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSU_MDF_VIDEOCODECS_STEP_H
#define TSU_MDF_VIDEOCODECS_STEP_H

#include <testframework.h>
#include "tsu_mdf_videocodecs_constants.h"

/**
 */
class RTestStepVideoCodecs : public RTestStep
	{
public:
	RTestStepVideoCodecs();
	~RTestStepVideoCodecs();
	
	virtual void SetVerdict(const TDesC& aText, TVerdict aVerdict = EPass);
	virtual void InfoMessage(const TDesC& aText);
	virtual TVerdict Verdict();

protected:
	virtual TVerdict OpenL();
	virtual void CleanupAfterOpenFail();
	virtual void Close();
	virtual TVerdict DoTestStepL() = 0;
private:
	CActiveScheduler* iActiveScheduler;
	TVerdict iVerdict;
	};

#endif // TSU_MDF_VIDEOCODECS_STEP_H
