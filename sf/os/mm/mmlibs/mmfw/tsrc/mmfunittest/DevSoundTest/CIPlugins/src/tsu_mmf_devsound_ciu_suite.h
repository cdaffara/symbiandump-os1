// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSU_MMF_DEVSOUND_CIU_SUITE_H
#define TSU_MMF_DEVSOUND_CIU_SUITE_H

#include <testframework.h>


/**
CTestSuiteCustomInterfaceUnificationUnit, which is the base class for 
the Custom Interface Unification Unit suite.


 */
class CTestStepCustominterfaceUnificationUnit;

class CTestSuiteCustomInterfaceUnificationUnit : public CTestSuite
	{
public:
	void InitialiseL();
	virtual ~CTestSuiteCustomInterfaceUnificationUnit();
	void AddTestStepL( CTestStepCustominterfaceUnificationUnit* aTestStep );
	TPtrC GetVersion() const;
	
private: 

	};

#endif
