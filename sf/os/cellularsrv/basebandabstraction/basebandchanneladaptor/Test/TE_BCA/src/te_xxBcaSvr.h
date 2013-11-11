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
// xxBca TestExecute Server definition
// 
//

/**
 @file
*/

#ifndef TE_xxBCASVR_H
#define TE_xxBCASVR_H


#include <test/TestExecuteServerBase.h>

/** Encapsulates all xxBca tests */
namespace te_xxBcaUnitTests
	{
	
	/**
	 xxBca test server class.
	 
	 @internalComponent
	*/
	class CTE_xxBcaSvr : public CTestServer
		{
		public:
			static CTE_xxBcaSvr* NewL(void);
			virtual CTestStep* CreateTestStep(const TDesC& aStepName);
		};
	}
	
#endif  
