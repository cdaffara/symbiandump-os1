/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_lbsprivfwtestchanneltestserver.h
*/

#ifndef _TE_LBS_PRIVFW_TESTCHANNEL_TEST_SERVER_H_
#define _TE_LBS_PRIVFW_TESTCHANNEL_TEST_SERVER_H_

#include <test/testexecuteserverbase.h>


class CTe_LbsPrivfwTestChannelTestServer : public CTestServer
	{
public:
	static CTe_LbsPrivfwTestChannelTestServer* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

// Please Add/modify your class members
private:
	};

#endif //_TE_LBS_PRIVFW_TESTCHANNEL_TEST_SERVER_H_
