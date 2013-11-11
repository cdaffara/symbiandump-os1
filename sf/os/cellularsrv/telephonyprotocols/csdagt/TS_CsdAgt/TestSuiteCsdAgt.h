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
// This contains CTestSuiteCsdAgt 
// 
//

#if (!defined __TESTSUITCSDAgt_H__)
#define __TESTSUITCSDAgt_H__

#include "testsuite.h"
#include <es_sock.h>

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

class  CTestSuiteCsdAgt : public CTestSuite
	{
public:
	void InitialiseL( void );
	~CTestSuiteCsdAgt();

	void AddTestStepL( CTestStepCsdAgt * ptrTestStep );

	// this should be a pure virtual so every test ddl
	// has to provide a version but for now defaults to ?.?
	virtual TPtrC GetVersion( void );
private:
	};


#endif /* __TESTSUITCSDAGT_H__ */
