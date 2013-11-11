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
// This is the header file for ROOTSERVER test section 1
// 
//


#if (!defined __ROOTSERVERTEST_3_H__)
#define __ROOTSERVERTEST_3_H__


class CRootServerTest3_2 : public CTestStepRootServer
{
public:
	CRootServerTest3_2(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};


class CRootServerTest3_DblLoad : public CTestStepRootServer
	{
public:
	CRootServerTest3_DblLoad(CTE_RootServerServer *aSuite);
protected:
	TVerdict LoadSameModuleTwiceL(TThreadPriority aPriority);
	};

class CRootServerTest3_3 : public CRootServerTest3_DblLoad
{
public:
	CRootServerTest3_3(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_4 : public CRootServerTest3_DblLoad
{
public:
	CRootServerTest3_4(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_5 : public CTestStepRootServer
{
public:
	CRootServerTest3_5(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_6 : public CTestStepRootServer
{
public:
	CRootServerTest3_6(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_7 : public CTestStepRootServer
{
public:
	CRootServerTest3_7(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_8 : public CTestStepRootServer
{
public:
	CRootServerTest3_8(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_9 : public CTestStepRootServer
{
public:
	CRootServerTest3_9(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_10 : public CTestStepRootServer
{
public:
	CRootServerTest3_10(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_11 : public CTestStepRootServer
{
public:
	CRootServerTest3_11(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_12 : public CTestStepRootServer
{
public:
	CRootServerTest3_12(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_13 : public CTestStepRootServer
{
public:
	CRootServerTest3_13(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_14 : public CTestStepRootServer
{
public:
	CRootServerTest3_14(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_15 : public CTestStepRootServer
{
public:
	CRootServerTest3_15(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_16 : public CTestStepRootServer
{
public:
	CRootServerTest3_16(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_17 : public CTestStepRootServer
{
public:
	CRootServerTest3_17(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest3_18 : public CTestStepRootServer
{
public:
	CRootServerTest3_18(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};


#endif //(__ROOTSERVERTEST_3_H__)
