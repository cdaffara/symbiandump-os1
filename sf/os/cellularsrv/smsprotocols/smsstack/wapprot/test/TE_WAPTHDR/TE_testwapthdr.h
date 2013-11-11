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
// @file
// 
//

#ifndef _TE_TESTWAPTHDR_H_
#define _TE_TESTWAPTHDR_H_

#include "TE_wapthdrbase.h"

class CTestParseWapTextMessage : public CWapThdrTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestEncodeWapTextMessage : public CWapThdrTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestLoggerOverflow : public CWapThdrTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


#endif
