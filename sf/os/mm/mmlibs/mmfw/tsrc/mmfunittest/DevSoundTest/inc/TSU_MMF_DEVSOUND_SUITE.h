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
// This contains CTestSuiteDevSound
//

#ifndef __TSU_MMF_DEVSOUND_SUITE_H__
#define __TSU_MMF_DEVSOUND_SUITE_H__

#include <testframework.h>
#include <mmf/server/sounddevice.h>

class CTestStepDevSound;

/**
 *
 * CTestSuiteDevSound defines the test suite for DevSound tests
 * 
 *
 * @lib "TSU_MMF_DEVSOUND_SUITE.lib"
 *
 *
 */
class  CTestSuiteDevSound : public CTestSuite 
	{
public:
		void InitialiseL(void);
		virtual ~CTestSuiteDevSound();
		void AddTestStepL(CTestStepDevSound * ptrTestStep);
		virtual TPtrC GetVersion( void ) const;
	};

#endif // __TSU_MMF_DEVSOUND_SUITE_H__
