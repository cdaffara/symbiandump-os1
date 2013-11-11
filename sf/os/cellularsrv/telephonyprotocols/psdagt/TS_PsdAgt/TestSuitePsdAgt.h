// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestSuitePsdAgt 
// 
//

#if (!defined __TESTSUITPSDAGT_H__)
#define __TESTSUITPSDAGT_H__

#include "networking/testsuite.h"
#include <es_sock.h>

class  CTestSuitePsdAgt : public CTestSuite
{
public:
	
	void InitialiseL( void );
	~CTestSuitePsdAgt();

	void AddTestStepL( CTestStepPsdAgt * ptrTestStep );

	// this should be a pure virtual so every test ddl
	// has to provide a version but for now defaults to ?.?
	virtual TPtrC GetVersion( void );

};


#endif /* __TESTSUITPSDAGT_H__ */
