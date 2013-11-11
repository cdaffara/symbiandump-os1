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

#ifndef CHAR_VIDEOPLAY_TESTSUITE_H
#define CHAR_VIDEOPLAY_TESTSUITE_H

#include <testframework.h>
#include <w32std.h>

class  CTestSuiteCharVideoPlay : public CTestSuite
	{
public:
	static CTestSuiteCharVideoPlay* NewL();
	void InitialiseL(void);
	~CTestSuiteCharVideoPlay();
	
#ifdef __WINS__
protected:
    // TO create the Window Server related objects
    TBool WaitUntilWindowServerReadyL();
    void InitWservL();
    void DeInitWserv();
protected:
    CWsScreenDevice* iScreen;
    RWindow* iWindow;
    RWsSession iWs;
    RWindowGroup iRootWindow;
#endif // __WINS__

private:
	CTestSuiteCharVideoPlay();
	};


#endif /* CHAR_VIDEOPLAY_TESTSUITE_H  */
