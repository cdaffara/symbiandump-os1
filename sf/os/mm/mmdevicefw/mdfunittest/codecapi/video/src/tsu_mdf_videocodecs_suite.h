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
// This contains CTestSuiteDevSound
// 
//

#ifndef TSU_MDF_VIDEOCODECS_SUITE_H
#define TSU_MDF_VIDEOCODECS_SUITE_H

#include <testframework.h>

/**
 *
 * CTestSuiteDevSound defines the test suite for DevSound tests
 * 
 */
class  CTestSuiteVideoCodecs : public CTestSuite 
	{
public:
	CTestSuiteVideoCodecs();
	void InitialiseL(void);
	virtual ~CTestSuiteVideoCodecs();
	virtual TPtrC GetVersion( void ) const;
	};

#endif // TSU_MDF_VIDEOCODECS_SUITE_H
