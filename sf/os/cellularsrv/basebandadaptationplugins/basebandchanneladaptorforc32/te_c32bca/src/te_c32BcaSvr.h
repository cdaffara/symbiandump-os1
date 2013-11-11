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
// C32Bca TestExecute Server definition
// 
//

/**
 @file
*/

#ifndef TE_C32BCASVR_H
#define TE_C32BCASVR_H


#include <test/testexecuteserverbase.h>

/** Encapsulates all C32Bca tests */
namespace te_c32BcaUnitTests
	{
	
	/**
	 C32Bca test server class.
	 
	 @internalComponent
	*/
	class CTE_C32BcaSvr : public CTestServer
		{
		public:
			static CTE_C32BcaSvr* NewL(void);
			virtual CTestStep* CreateTestStep(const TDesC& aStepName);
		};
	}
	
#endif  
