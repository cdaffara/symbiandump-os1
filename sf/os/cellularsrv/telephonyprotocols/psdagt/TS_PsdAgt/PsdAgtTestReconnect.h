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
// This is the header file for PsdAgt test 14.1
// 
//


#if (!defined __PSDAGTTEST_RECONNECT_H__)
#define __PSDAGTTEST_RECONNECT_H__


class CPsdAgtTest14_1 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest14_1() : CTestStepPsdAgt(_L("Test14.1")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

class CPsdAgtTest14_2 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest14_2() : CTestStepPsdAgt(_L("Test14.2")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

class CPsdAgtTest14_3 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest14_3() : CTestStepPsdAgt(_L("Test14.3")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );

protected:
	virtual void PsdAgtGetDataVolumeRequest(TUint32 &aSent, TUint32 &aReceived);

	};

class CPsdAgtTest14_4 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest14_4() : CTestStepPsdAgt(_L("Test14.4")) {};
	
	virtual enum TVerdict doPsdAgtTestStepL( void );

protected:
	virtual void DialogHasAppeared();
	};

class CPsdAgtTest14_5 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest14_5() : CTestStepPsdAgt(_L("Test14.5")) {};
	virtual enum TVerdict doPsdAgtTestStepL( void );

protected:
	virtual void DialogHasAppeared();
	};


#endif //(__PSDAGXTEST_RECONNECT_H__)
