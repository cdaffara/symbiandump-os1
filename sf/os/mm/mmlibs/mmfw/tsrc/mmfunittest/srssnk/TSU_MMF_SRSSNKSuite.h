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
// This contains CTestSuiteWP7
// 
//

#ifndef __TSU_MMF_SRSSNK_SUITE_H_
#define __TSU_MMF_SRSSNK_SUITE_H_

#include <testframework.h>
//#include <Log.h>
//#include <TestSuite.h>

class CTestStep_MMF_SRSSNK; //forward reference

/**
 *
 * CTestSuiteBaseClasses, which is the base class for 
 * the sources and sinks WP7 suite.
 *
 * @lib "TSU_MMF_SRSSNK.lib"
 *
 * @xxxx
 *
 */
class  CTestSuite_MMF_SRSSNK : public CTestSuite
	{
public:
	void InitialiseL();
	virtual ~CTestSuite_MMF_SRSSNK();
	void AddTestStepL(CTestStep_MMF_SRSSNK * ptrTestStep);
	virtual TPtrC GetVersion() const;
};

#endif /* TSU_MMF_SRSSNK_SUITE_H_ */

