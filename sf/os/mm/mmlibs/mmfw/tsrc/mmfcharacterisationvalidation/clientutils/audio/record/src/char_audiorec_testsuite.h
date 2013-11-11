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

#ifndef CHAR_AUDIOREC_TESTSUITE_H
#define CHAR_AUDIOREC_TESTSUITE_H

#include <testframework.h>

class  CTestSuiteCharAudioRec : public CTestSuite
	{
private:
	CTestSuiteCharAudioRec();
public:
	static CTestSuiteCharAudioRec* NewL();
	void     InitialiseL( void );
	
	~CTestSuiteCharAudioRec();
	};


#endif /* CHAR_AUDIOREC_TESTSUITE_H  */
