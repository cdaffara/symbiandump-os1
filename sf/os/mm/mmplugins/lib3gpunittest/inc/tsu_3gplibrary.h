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

#ifndef TSU_3GPLIBRARY_H
#define TSU_3GPLIBRARY_H

#include <test/testexecuteserverbase.h>

/** 
 Test server for 3GPLibrary unit tests
 */
class C3GPLibraryServer : public CTestServer
	{
	public:
		static C3GPLibraryServer* NewL();
		CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif
