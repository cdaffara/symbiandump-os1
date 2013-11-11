/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* __EDIT_ME__ Create your own class definition based on this
* 
*
*/



/**
 @file
*/

#ifndef __SAMPLE_SERVER_H__
#define __SAMPLE_SERVER_H__

#include <test/testexecuteserverbase.h>

class CTgsmuServer : public CTestServer
	{
public:
	static CTgsmuServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs(){return iFs;};

private:
	RFs iFs;
	};

#endif // __SAMPLE_SERVER_H__
