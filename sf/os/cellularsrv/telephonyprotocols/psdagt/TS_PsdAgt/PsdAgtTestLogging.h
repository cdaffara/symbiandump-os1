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
// This is the header file for PsdAgt test 5.1 - 5.3
// 
//


#if (!defined __PSDAGTTEST_LOGGING_H__)
#define __PSDAGTTEST_LOGGING_H__


class CPsdAgtTest5_1 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest5_1() : CTestStepPsdAgt(_L("Test5.1")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	
protected:
	// Override log volume request function
	virtual void PsdAgtGetDataVolumeRequest(TUint32 &aSent, TUint32 &aReceived);
	};

class CPsdAgtTest5_2 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest5_2() : CTestStepPsdAgt(_L("Test5.2")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	
protected:
	// Override log volume request function
	virtual void PsdAgtGetDataVolumeRequest(TUint32 &aSent, TUint32 &aReceived);
	};

class CPsdAgtTest5_3 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest5_3() : CTestStepPsdAgt(_L("Test5.3")) {};
	
	virtual enum TVerdict doPsdAgtTestStepL( void );
	
protected:
	// Override log volume request function
	virtual void PsdAgtGetDataVolumeRequest(TUint32 &aSent, TUint32 &aReceived);
	};

#endif //(__PSDAGTTEST_LOGGING_H__)
