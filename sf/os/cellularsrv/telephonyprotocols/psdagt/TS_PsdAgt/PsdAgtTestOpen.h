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
// This is the header file for PsdAgt test 11.1 - 11.7
// 
//


#if (!defined __PSDAGTTEST_OPEN_H__)
#define __PSDAGTTEST_OPEN_H__


class CPsdAgtTest11_1 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest11_1() : CTestStepPsdAgt(_L("Test11.1")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	
protected:
	// Override AgentProgress function
	virtual void AgentProgress(TInt aStage, TInt aError);
	};

class CPsdAgtTest11_2 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest11_2() : CTestStepPsdAgt(_L("Test11.2")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	
protected:
	// Override AgentProgress function
	virtual void AgentProgress(TInt aStage, TInt aError);
	};

class CPsdAgtTest11_3 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest11_3() : CTestStepPsdAgt(_L("Test11.3")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	
protected:
	// Override AgentProgress function
	virtual void AgentProgress(TInt aStage, TInt aError);
	};

class CPsdAgtTest11_4 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest11_4() : CTestStepPsdAgt(_L("Test11.4")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

class CPsdAgtTest11_5 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest11_5() : CTestStepPsdAgt(_L("Test11.5")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	
protected:
	// Override AgentProgress function
	virtual void AgentProgress(TInt aStage, TInt aError);
	};

class CPsdAgtTest11_6 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest11_6() : CTestStepPsdAgt(_L("Test11.6")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

class CPsdAgtTest11_7 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest11_7() : CTestStepPsdAgt(_L("Test11.7")) {};
	
	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

class CPsdAgtTest11_8 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest11_8() : CTestStepPsdAgt(_L("Test11.8")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );

protected:
	// Override AgentProgress function
	virtual void AgentProgress(TInt aStage, TInt aError);

	static TInt Callback(TAny* aParam);

	TInt iProgress;
	};

#endif //(__PSDAGTTEST_OPEN_H__)
