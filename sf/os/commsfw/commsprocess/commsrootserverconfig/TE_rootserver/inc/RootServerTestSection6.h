// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// // Root Server Test Section 6 - Loading modules in Root Server
// 
//


#if (!defined __ROOTSERVERTEST_6_H__)
#define __ROOTSERVERTEST_6_H__


class CRootServerTest6_1 : public CTestStepRootServer
{
public:
	CRootServerTest6_1(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_2 : public CTestStepRootServer
{
public:
	CRootServerTest6_2(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_BindWithinModule : public CTestStepRootServer
{
public:
	CRootServerTest6_BindWithinModule(CTE_RootServerServer *aSuite);
protected:
	TVerdict BindWithinModuleL(TThreadPriority aPriority);
};

class CRootServerTest6_3 : public CRootServerTest6_BindWithinModule
{
public:
	CRootServerTest6_3(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_4 : public CRootServerTest6_BindWithinModule
{
public:
	CRootServerTest6_4(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_5 : public CTestStepRootServer
{
public:
	CRootServerTest6_5(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_6 : public CTestStepRootServer
{
public:
	CRootServerTest6_6(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_7 : public CTestStepRootServer
{
public:
	CRootServerTest6_7(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_UnbindWithinModule : public CTestStepRootServer
{
public:
	CRootServerTest6_UnbindWithinModule(CTE_RootServerServer *aSuite);
protected:
	TVerdict UnbindWithinModuleL(TThreadPriority aPriority);
};

class CRootServerTest6_8 : public CRootServerTest6_UnbindWithinModule
{
public:
	CRootServerTest6_8(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_9 : public CRootServerTest6_UnbindWithinModule
{
public:
	CRootServerTest6_9(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_10 : public CTestStepRootServer
{
public:
	CRootServerTest6_10(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_11 : public CTestStepRootServer
{
public:
	CRootServerTest6_11(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_12 : public CTestStepRootServer
{
public:
	CRootServerTest6_12(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_13 : public CTestStepRootServer
{
public:
	CRootServerTest6_13(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_14 : public CTestStepRootServer
{
public:
	CRootServerTest6_14(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_15 : public CTestStepRootServer
{
public:
	CRootServerTest6_15(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest6_16 : public CTestStepRootServer
{
public:
	CRootServerTest6_16(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

#endif //(__ROOTSERVERTEST_6_H__)
