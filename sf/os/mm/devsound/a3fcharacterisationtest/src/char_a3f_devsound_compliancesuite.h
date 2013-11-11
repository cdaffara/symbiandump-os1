// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CHAR_A3F_DEVSOUND_COMPLIANCESUITE_H
#define CHAR_A3F_DEVSOUND_COMPLIANCESUITE_H

#include <testframework.h>

_LIT(KTestSuiteName, "CHAR_A3F_DEVSOUND");


/*
 The class creates a test suite that defines and includes A3F DevSound 
 tests to be run using the Symbian Multimedia Test Framework 
 */
class  CA3FDevSoundComplianceSuite : public CTestSuite
	{
public:
	void InitialiseL();
	static CA3FDevSoundComplianceSuite* NewL();
	~CA3FDevSoundComplianceSuite();
private:
	CA3FDevSoundComplianceSuite();
	};


#endif  // CHAR_A3F_DEVSOUND_COMPLIANCESUITE_H

