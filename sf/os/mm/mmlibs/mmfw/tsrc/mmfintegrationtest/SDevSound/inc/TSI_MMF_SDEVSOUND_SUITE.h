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


#ifndef __TSI_MMF_SDEVSOUND_SUITE_H__
#define __TSI_MMF_SDEVSOUND_SUITE_H__

#include <testframework.h>
#include <mmf/server/sounddevice.h>

class CTestStepSDevSound;

/**
 *
 * CTestSuiteSDevSound defines the test suite for SDevSound tests
 * 
 *
 * @lib "TSI_MMF_SDEVSOUND_SUITE.lib"
 *
 *
 */
class  CTestSuiteSDevSound : public CTestSuite 
	{
public:
		void InitialiseL();
		virtual ~CTestSuiteSDevSound();
		void AddTestStepL(CTestStepSDevSound* ptrTestStep);
		virtual TPtrC GetVersion() const;
	};

#endif // __TSI_MMF_SDEVSOUND_SUITE_H__
