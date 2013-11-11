// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _TE_ETELSATPCMDS1F_H_
#define _TE_ETELSATPCMDS1F_H_

#include <etel.h>

class CTestSatPCmds1f : public CRSatTestStep
	{
public:
	CTestSatPCmds1f() ;
	~CTestSatPCmds1f(){} ;
	virtual enum TVerdict doTestStepL();
	
private:
	void Release6Test1L();
	void Release6Test2L();
	void Release6Test3L();
	void Release6Test4L();
	void Release6Test5L();
	void Release6Test6L();
	void Release6Test7L();
	}; // class CTestSatPCmds1f

#endif // _TE_ETELSATPCMDS1F_H_

