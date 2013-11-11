// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _TE_ETELMULTICOMPLETION_H_
#define _TE_ETELMULTICOMPLETION_H_

#include <etel.h>

class CTestMultiCompletion : public CEtelTestStep
	{
public:
	CTestMultiCompletion() ;
	~CTestMultiCompletion(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestMultiCompletionDif : public CEtelTestStep
	{
public:
	CTestMultiCompletionDif() ;
	~CTestMultiCompletionDif(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif
