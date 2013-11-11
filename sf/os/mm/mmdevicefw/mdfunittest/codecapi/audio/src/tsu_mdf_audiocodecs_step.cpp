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

#include <e32base.h>
#include "tsu_mdf_audiocodecs_step.h"

RTestStepAudioCodecs::RTestStepAudioCodecs()
	{	
	}
	
RTestStepAudioCodecs::~RTestStepAudioCodecs()
	{	
	}
	
TVerdict RTestStepAudioCodecs::OpenL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( iActiveScheduler );
	CActiveScheduler::Install(iActiveScheduler);
	CleanupStack::Pop();
	return EPass;
	}
	
void RTestStepAudioCodecs::CleanupAfterOpenFail()
	{	
	}
	
void RTestStepAudioCodecs::Close()
	{
	delete iActiveScheduler;
	iActiveScheduler = NULL;
	}


