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
// Header file: Test suite definition for PREQ1024 MDF Codec API.
// 
//

/**
 @file tsu_mdf_codecapi_suite.h
*/

#ifndef TSU_MDF_CODECAPI_SUITE_H
#define TSU_MDF_CODECAPI_SUITE_H

#include <testframework.h>

/**
 *
 * @class CTestMmfXvidSuite
 * defines the test suite for CodecApi tests
 *
 */
class  CTestSuiteCodecApi : public CTestSuite 
	{
public:
		CTestSuiteCodecApi();
		void InitialiseL(void);
		virtual ~CTestSuiteCodecApi();
		virtual TPtrC GetVersion( void ) const;
	};

#endif // TSU_MDF_CODECAPI_SUITE_H
