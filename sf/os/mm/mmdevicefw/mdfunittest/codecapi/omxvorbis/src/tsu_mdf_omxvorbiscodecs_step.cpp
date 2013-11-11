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

/**
 @file
 @internalComponent
*/

#include <e32base.h>
#include "tsu_mdf_omxvorbiscodecs_step.h"

RTestStepOmxVorbisCodecs::RTestStepOmxVorbisCodecs()
	{
	}
	
RTestStepOmxVorbisCodecs::~RTestStepOmxVorbisCodecs()
	{
	// nothing here just yet
	}
	
TVerdict RTestStepOmxVorbisCodecs::OpenL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( iActiveScheduler );
	CActiveScheduler::Install(iActiveScheduler);
	CleanupStack::Pop();
	return EPass;
	}
	
void RTestStepOmxVorbisCodecs::CleanupAfterOpenFail()
	{
	// nothing here just yet
	}
	
void RTestStepOmxVorbisCodecs::Close()
	{
	delete iActiveScheduler;
	iActiveScheduler = NULL;
	}


