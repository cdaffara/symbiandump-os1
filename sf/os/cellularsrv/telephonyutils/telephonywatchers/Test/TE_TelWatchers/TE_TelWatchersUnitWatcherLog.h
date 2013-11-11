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
// Declaration for class used for logging in telephony watcher unit tests
// Replaces messaging's CWatcherLog for telephony watcher unit tests
// 
//

/**
 @file
*/

#ifndef __TE_TELWATCHERSUNITWATCHERLOG_H__
#define __TE_TELWATCHERSUNITWATCHERLOG_H__

#include <test/testexecutelogger.h>

class CWatcherLog : public CBase
	{
public:
	CWatcherLog(CTestExecuteLogger& aTestExecuteLogger);

	void Printf(TRefByValue<const TDesC> aFmt, ...);

private:
    void PrintToTestExecuteLogL(TRefByValue<const TDesC> aFmt, VA_LIST aParamList);

	CTestExecuteLogger&  iTestExecuteLogger;
	};

#endif // #ifndef __TE_TELWATCHERSUNITWATCHERLOG_H__
