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

#ifndef _TE_ETELMEMBADCLIENT_H_
#define _TE_ETELMEMBADCLIENT_H_

#include <etel.h>

class CTestMemBadClient7 : public CEtelTestStep
	{
public:
	CTestMemBadClient7() ;
	~CTestMemBadClient7(){} ;
	virtual enum TVerdict doTestStepL();
	static TInt LeaveSubsessionsOpen(TAny * /* aArg */);
	};

	class CTestMemBadClient7a : public CEtelTestStep
	{
public:
	CTestMemBadClient7a() ;
	~CTestMemBadClient7a(){} ;
	virtual enum TVerdict doTestStepL();
	static TInt OpenSubsessionsFewTimes(TAny * /* aArg */);
	};

	class CTestMemBadClient7b : public CEtelTestStep
	{
public:
	CTestMemBadClient7b() ;
	~CTestMemBadClient7b(){} ;
	virtual enum TVerdict doTestStepL();
	static TInt DoubleOpenThread(TAny * /* anArg */);
	};

#endif
