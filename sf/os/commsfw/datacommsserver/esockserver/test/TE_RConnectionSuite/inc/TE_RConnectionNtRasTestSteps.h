// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file with the declartions of the RConnection test step classes that predominately 
// require that NT RAS is the default interface specified in commdb. These test TCP sockets and 
// host resolvers. Each class encapsulates a complete test case and is derived from TE_RConnectionStep
// 
//

/**
 @file
*/

#if (!defined __TE_RConnectionNTRASTESTSTEPS_H__)
#define __TE_RConnectionNTRASTESTSTEPS_H__

#include "TE_RConnectionStep.h"

/*
 * TCP DOMINATED TESTS...........
 */

class TE_RConnectionTest101 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest101();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest102 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest102();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest103 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest103();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest104 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest104();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest105 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest105();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest106 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest106();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest107 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest107();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest108 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest108();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest109 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest109();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest110 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest110();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest111 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest111();

	virtual enum TVerdict doTestStepL(void);
};

// Base classes containing common Idle Timer test code

class TE_RConnectionTestIdle1Base : public TE_RConnectionStep
{
public:
	TE_RConnectionTestIdle1Base();
	enum TVerdict doBaseTestStepL(TInt aStep, TBool aTestSocket, TBool aTestHostResolver);
};

class TE_RConnectionTestIdle2Base : public TE_RConnectionStep
{
public:
	// TE_RConnectionTestIdle2Base();
	enum TVerdict doBaseTestStepL(TInt aStep);
};

class TE_RConnectionTestIdle3Base : public TE_RConnectionStep
{
public:
	TE_RConnectionTestIdle3Base();
	enum TVerdict doBaseTestStepL(TInt aStep, TBool aTestSocket, TBool aTestHostResolver);
};

// (end)

class TE_RConnectionTest112 : public TE_RConnectionTestIdle1Base
{
public:
	virtual ~TE_RConnectionTest112();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest113 : public TE_RConnectionTestIdle1Base
{
public:
	virtual ~TE_RConnectionTest113();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest114 : public TE_RConnectionTestIdle2Base
{
public:
	virtual ~TE_RConnectionTest114();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest115 : public TE_RConnectionTestIdle3Base
{
public:
	virtual ~TE_RConnectionTest115();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionSnapNotSupported : public TE_RConnectionTestIdle3Base
{
public:
	virtual ~TE_RConnectionSnapNotSupported();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest116 : public TE_RConnectionTestIdle3Base
{
public:
	virtual ~TE_RConnectionTest116();

	virtual enum TVerdict doTestStepL(void);
};

/*
 * HOST RESOLVER DOMINATED TESTS..........
 */

class TE_RConnectionTest201 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest201();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest202 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest202();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest203 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest203();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest204 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest204();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest205 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest205();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest206 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest206();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest207 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest207();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest208 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest208();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest209 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest209();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest210 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest210();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest211 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest211();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest212 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest212();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest213 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest213();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest214 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest214();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest215 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest215();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest216 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest216();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest217 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest217();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest218 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest218();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest219 : public TE_RConnectionStep
{ // uses tcp and hr
public:
	virtual ~TE_RConnectionTest219();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest220 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest220();

	virtual enum TVerdict doTestStepL(void);
};

NONSHARABLE_CLASS (TE_RConnectionTest221) : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest221();

	virtual enum TVerdict doTestStepL(void);
};

#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT

NONSHARABLE_CLASS (TE_RConnectionTest222) : public TE_RConnectionSimTsyDataStep
{
public:
	virtual ~TE_RConnectionTest222();
	virtual enum TVerdict doTestStepL();
};

NONSHARABLE_CLASS (TE_RConnectionTest223) : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest223();
	virtual enum TVerdict doTestStepL();	
};

NONSHARABLE_CLASS (TE_RConnectionTest224) : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest224();
	virtual enum TVerdict doTestStepL();
};

#endif // SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT
#endif

