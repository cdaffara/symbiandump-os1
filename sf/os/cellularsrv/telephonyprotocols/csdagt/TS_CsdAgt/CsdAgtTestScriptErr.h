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
// This is the header file for CsdAgt test 5.1 - 5.15
// 
//


#if (!defined __CSDAGTTEST_SCRIPT_ERR_H__)
#define __CSDAGTTEST_SCRIPT_ERR_H__

class CDbCsdTable;

class CCsdAgtTest5_xx : public CTestStepCsdAgt
	{
public:
	CCsdAgtTest5_xx(TPtrC aName) : CTestStepCsdAgt(aName) {};

protected:
    enum TVerdict DoScriptErrTestL(TInt aTestNo, const TDesC& aLoginScript);
	void CustomiseLoginScriptL(const TDesC16& aLoginScript);
	void RevertCommdbChangesL();
private:
    CDbCsdTable *csd;
	};


class CCsdAgtTest5_1 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_1() : CCsdAgtTest5_xx(_L("Test5.1")) {};

	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_2 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_2() : CCsdAgtTest5_xx(_L("Test5.2")) {};

	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_3 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_3() : CCsdAgtTest5_xx(_L("Test5.3")) {};

	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_4 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_4() : CCsdAgtTest5_xx(_L("Test5.4")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};


class CCsdAgtTest5_5 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_5() : CCsdAgtTest5_xx(_L("Test5.5")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_6 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_6() : CCsdAgtTest5_xx(_L("Test5.6")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_7 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_7() : CCsdAgtTest5_xx(_L("Test5.7")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_8 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_8() : CCsdAgtTest5_xx(_L("Test5.8")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_9 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_9() : CCsdAgtTest5_xx(_L("Test5.9")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_10 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_10() : CCsdAgtTest5_xx(_L("Test5.10")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_11 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_11() : CCsdAgtTest5_xx(_L("Test5.11")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_12 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_12() : CCsdAgtTest5_xx(_L("Test5.12")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_13 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_13() : CCsdAgtTest5_xx(_L("Test5.13")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_14 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_14() : CCsdAgtTest5_xx(_L("Test5.14")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_15 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_15() : CCsdAgtTest5_xx(_L("Test5.15")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};
class CCsdAgtTest5_16 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_16() : CCsdAgtTest5_xx(_L("Test5.16")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest5_17 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_17() : CCsdAgtTest5_xx(_L("Test5.17")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};
class CCsdAgtTest5_18 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_18() : CCsdAgtTest5_xx(_L("Test5.18")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};
class CCsdAgtTest5_19 : public CCsdAgtTest5_xx
	{
public:
	CCsdAgtTest5_19() : CCsdAgtTest5_xx(_L("Test5.19")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};
#endif //(__CSDAGTTEST_SCRIPT_ERR_H__)
