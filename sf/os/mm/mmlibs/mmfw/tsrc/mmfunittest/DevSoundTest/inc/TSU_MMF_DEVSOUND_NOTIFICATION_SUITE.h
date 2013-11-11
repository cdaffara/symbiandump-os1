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
// This contains CTestSuiteDevSoundNotification
// 
//

#ifndef __TSU_MMF_DEVSOUND_NOTIFICATION_SUITE_H__
#define __TSU_MMF_DEVSOUND_NOTIFICATION_SUITE_H__

#include <testframework.h>
#include <mmf/server/sounddevice.h>

class CTestStepDevSoundNotification;
class CTestStepFsmDevSoundNotification;

/**
 *
 * CTestSuiteDevSoundNotification defines the test suite for DevSound Notification tests
 * 
 *
 * @lib "TSU_MMF_DEVSOUND_NOTIFICATION_SUITE.lib"
 *
 *
 */
class  CTestSuiteDevSoundNotification : public CTestSuite 
	{
public:
	void InitialiseL(void);
	virtual ~CTestSuiteDevSoundNotification();
	void AddTestStepL(CTestStepDevSoundNotification * ptrTestStep);
	void AddTestStepL(CTestStepFsmDevSoundNotification *ptrTestStep);
	virtual TPtrC GetVersion( void ) const;
	};

#endif // __TSU_MMF_DEVSOUND_NOTIFICATION_SUITE_H__
