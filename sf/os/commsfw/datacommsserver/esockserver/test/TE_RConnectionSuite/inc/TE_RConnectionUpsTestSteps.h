// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file with the declartions of the RConnectionUpsStep classes that test the
// IP Networking User Prompting Functionality added in PREQ1116.
// Each class encapsulates a complete test case and is derived from TE_RConnectionUpsStep
// 
//

/**
 @file 
*/


#if (!defined __TE_RCONNECTIONUPSTESTSTEPS_H__)
#define __TE_RCONNECTIONUPSTESTSTEPS_H__

#include "TE_RConnectionUpsStep.h"


class TE_RConnectionUPSCloseInterfaces : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSCloseInterfaces();
	virtual enum TVerdict doTestStepL(void);
	
private:
	virtual enum TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepPostambleL();

};

class TE_RConnectionUPSSetNotifFileOverride : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSSetNotifFileOverride();
	virtual enum TVerdict doTestStepL(void);
	
private:
	virtual enum TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepPostambleL();

};


class TE_RConnectionUPSUnsetNotifFileOverride : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSUnsetNotifFileOverride();
	virtual enum TVerdict doTestStepL(void);
	
private:
	virtual enum TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepPostambleL();

};

class TE_RConnectionUPSTest1a : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest1a();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest1b : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest1b();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest1c : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest1c();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest1d : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest1d();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest2a : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest2a();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest2b : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest2b();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest2c : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest2c();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest2d : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest2d();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest3a : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest3a();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest3b : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest3b();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest3c : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest3c();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest3d : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest3d();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest4a : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest4a();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest4c : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest4c();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest5a : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest5a();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest6a : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest6a();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest6b : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest6b();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest6c : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest6c();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest6d : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest6d();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest7a : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest7a();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest7b : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest7b();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest7c : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest7c();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest7d : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest7d();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest10 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest10();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest11 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest11();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest12 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest12();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest13 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest13();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest14 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest14();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest15 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest15();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest16 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest16();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest17 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest17();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest18 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest18();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest19 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest19();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest20 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest20();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest21 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest21();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest22 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest22();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest23 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest23();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest24 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest24();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest25 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest25();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest26 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest26();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSPreMulti : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSPreMulti();

	virtual enum TVerdict doTestStepL(void);
	
private:
	virtual enum TVerdict doTestStepPreambleL();
};


class TE_RConnectionUPSPostMulti : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSPostMulti();

	virtual enum TVerdict doTestStepL(void);
	
private:
	virtual enum TVerdict doTestStepPreambleL();
};


class TE_RConnectionUPSTest27a : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest27a();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest27b : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest27b();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest27c : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest27c();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest27d : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest27d();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest28 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest28();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest29 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest29();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest30 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest30();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest31 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest31();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest32 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest32();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest33 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest33();

	virtual enum TVerdict doTestStepL(void);
};


class TE_RConnectionUPSTest34 : public TE_RConnectionUpsStep
{
public:
	virtual ~TE_RConnectionUPSTest34();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionUPSTest35 : public TE_RConnectionUpsStep
{
public:
    virtual ~TE_RConnectionUPSTest35();

    virtual enum TVerdict doTestStepL(void);
};

#endif



