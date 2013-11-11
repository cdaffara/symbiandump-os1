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
//

/**
 @file
 @internalComponent - Internal Symbian test code 
*/


#ifndef __TSUPLASN1SERVER_H__
#define __TSUPLASN1SERVER_H__

#include <test/testexecutestepbase.h>
#include <test/testexecuteserverbase.h>
#include <test/testexecutelog.h>

class CSuplAsn1Server : public CTestServer
	{
public:
	static CSuplAsn1Server* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs(){return iFs;};

private:
	RFs iFs;

	};
#endif


