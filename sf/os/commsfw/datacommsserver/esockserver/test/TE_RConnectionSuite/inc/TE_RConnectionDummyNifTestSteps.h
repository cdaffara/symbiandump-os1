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
// require that the dummy nif is the default interface specified in commdb. These test UDP sockets mainly
// and lots of functionality where it isn't important which interface is used. Each class encapsulates
// a complete test case and is derived from TE_RConnectionStep
// 
//

/**
 @file
*/

#if (!defined __TE_RConnectionDUMMYNIFTESTSTEPS_H__)
#define __TE_RConnectionDUMMYNIFTESTSTEPS_H__

#include "TE_RConnectionStep.h"

class TE_RConnectionTest1 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest1();
	virtual enum TVerdict doTestStepL(void);
};

/**
Testing closing a connection during its startup.

@internalTechnology
*/
class TE_RConnectionTest1b : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest1b();
	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest2 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest2();
	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest3 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest3();
	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest4 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest4();
	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest5 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest5();
	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest6 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest6();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest7 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest7();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest8 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest8();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest9 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest9();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest10 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest10();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest11 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest11();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest12 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest12();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest13 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest13();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest14 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest14();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest15 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest15();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest16 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest16();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest17 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest17();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest18 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest18();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest19 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest19();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest20 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest20();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest21 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest21();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest22 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest22();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest23 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest23();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest24 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest24();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest25 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest25();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest26 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest26();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest27 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest27();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest28 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest28();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest29 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest29();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest30 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest30();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest31 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest31();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest32 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest32();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest33 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest33();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest34 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest34();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest37 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest37();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest38 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest38();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest39 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest39();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest40 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest40();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest41 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest41();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest42 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest42();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest43 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest43();
	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest44 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest44();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest45 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest45();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest46 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest46();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest47 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest47();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest48 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest48();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest49 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest49();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest50 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest50();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest51 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest51();

	virtual enum TVerdict doTestStepL(void);
};


// enumeration tests

class TE_RConnectionTest52 : public TE_RConnectionStep
	{
public:
	virtual ~TE_RConnectionTest52();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest53 : public TE_RConnectionStep
	{
public:
	virtual ~TE_RConnectionTest53();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest54 : public TE_RConnectionStep
	{
public:
	virtual ~TE_RConnectionTest54();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest55 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest55();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest56 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest56();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest57 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest57();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest58 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest58();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest59 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest59();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest60 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest60();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest61 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest61();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest63 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest63();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest64 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest64();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest65 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest65();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest66 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest66();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest67 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest67();

	virtual enum TVerdict doTestStepL(void);
};

/**
Interface for the base class used to test runtime update mechanism
of Nifman Idle timeouts.
This class implements common functionality.

@internalTechnology
*/
class TE_RConnectionIdleTimeoutUpdateTest : public TE_RConnectionStep
{
public:
	/**
	Specifies the timeout to be tested

	@internalTechnology */
	enum TTimeoutType
		{
		ELastSessionClosedTimeout,
		ELastSocketClosedTimeout,
		ELastSocketActivityTimeout
		};

protected:
	void DoTestSequenceL(enum TTimeoutType aTimeoutType, TInt aTimeoutChangeFactor, TInt aExpectedUpdateErr);
};

/**
Interface for LastSessionClosed runtime update testing.

@internalTechnology
*/
class TE_RConnectionTest68 : public TE_RConnectionIdleTimeoutUpdateTest
{
public:
	virtual ~TE_RConnectionTest68();

	virtual enum TVerdict doTestStepL(void);
};

/**
Interface for tests for LastSocketClosed timeout runtime update testing.

@internalTechnology
*/
class TE_RConnectionTest69 : public TE_RConnectionIdleTimeoutUpdateTest
{
public:
	virtual ~TE_RConnectionTest69();

	virtual enum TVerdict doTestStepL(void);
};

/**
Interface for LastSocketActivity timeout runtime update testing.

@internalTechnology
*/
class TE_RConnectionTest70 : public TE_RConnectionIdleTimeoutUpdateTest
{
public:
	virtual ~TE_RConnectionTest70();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest71 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest71();

	virtual enum TVerdict doTestStepL(void);
};

/**
Interface for testing proper deletion of selectors when connection is closed after selection is complete

@internalTechnology
*/
NONSHARABLE_CLASS(TE_RConnectionTest72) : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest72();

	virtual enum TVerdict doTestStepL(void);
};

/**
Interface for testing proper deletion of selectors when connection is closed after selection is complete

@internalTechnology
*/
class TE_RConnectionTest73 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest73();

	virtual enum TVerdict doTestStepL(void);
};

/**

@internalTechnology
*/
class TE_RConnectionTest74 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest74();

	virtual enum TVerdict doTestStepL(void);
};

/**
@internalTechnology
*/
class TE_RConnectionTest75 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest75();

	virtual enum TVerdict doTestStepL(void);
};


/**

@internalTechnology
*/
class TE_RConnectionTest76 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest76();

	virtual enum TVerdict doTestStepL(void);
};

/**

@internalTechnology
*/
class TE_RConnectionTest77 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest77();

	virtual enum TVerdict doTestStepL(void);
};

/**

@internalTechnology
*/
class TE_RConnectionTest78 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest78();

	virtual enum TVerdict doTestStepL(void);
};

/**

@internalTechnology
*/
class TE_RConnectionTest79 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest79();

	virtual enum TVerdict doTestStepL(void);
};

/**

@internalTechnology
*/
class TE_RConnectionTest80 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest80();

	virtual enum TVerdict doTestStepL(void);
};

/**

@internalTechnology
*/
class TE_RConnectionTest81 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest81();

	virtual enum TVerdict doTestStepL(void);
};


#endif

