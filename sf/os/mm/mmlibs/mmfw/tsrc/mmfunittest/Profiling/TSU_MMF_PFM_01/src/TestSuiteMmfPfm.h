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
// TestSuiteMmfPerformance.h
// 
//

#if (!defined __TEST_SUITE_MMF_PERFORMANCE_H_)
#define __TEST_SUITE_MMF_PERFORMANCE_H_

#include <TestFramework.h>


/**
 *
 * RTestSuiteMmfPerformance is the test suite
 *
 * @lib "TSU_MMF_PFM_01.lib"
 *
 */
class  CTestSuiteMmfPerformance : public CTestSuite
	{
public:
	EXPORT_C static CTestSuiteMmfPerformance* NewL();

	//From CTestSuite
	void InitialiseL( void );
	TPtrC GetVersion( void );
	const TDesC& DefaultPath() const;

private:
	void InitSystemPath();		
	
private:
	TFileName iDefaultPath;	
	};

#endif /* __TEST_SUITE_MMF_PERFORMANCE_H_  */
