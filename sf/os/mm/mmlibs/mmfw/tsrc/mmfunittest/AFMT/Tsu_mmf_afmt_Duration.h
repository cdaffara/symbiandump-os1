
// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_AFMT_DURATION_H__
#define __TSU_MMF_AFMT_DURATION_H__

#include <testframework.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/server/mmfdatasink.h>




class CTestStep_MMF_AFMT_DurationL : public CTestStep_MMF_AFMT
	{
public: // Constructor/Destructor
	CTestStep_MMF_AFMT_DurationL(){}
	virtual ~CTestStep_MMF_AFMT_DurationL(){}

public: //	New function
	/**
	 * This function runs the duration test.  You should call SetupL of the base
	 * class before running this function to set up the test file and test objects.
	 * @param aDuration - The expected duration of the test file.
	 * @return TVerdict EPass on success, else EFail.
	 */
	TVerdict RunDurationTest( const TInt64& aDuration );

	};




#endif

	















