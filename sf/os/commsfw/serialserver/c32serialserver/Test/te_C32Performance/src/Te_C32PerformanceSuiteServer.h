/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file
 @internalComponent
*/

#if (!defined TE_C32PERFORMANCE_SERVER_H)
#define TE_C32PERFORMANCE_SERVER_H

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <testexecuteserverbase.h>
#else
#include <test/testexecuteserverbase.h>
#endif


class CTe_C32PerformanceSuite : public CTestServer
	{
public:
	static CTe_C32PerformanceSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

// Please Add/modify your class members
private:
	};

#endif

