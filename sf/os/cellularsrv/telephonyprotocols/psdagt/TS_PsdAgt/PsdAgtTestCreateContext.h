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
// This is the header file for PsdAgt test 8.1
// 
//


#if (!defined __PSDAGTTEST_CREATE_CONTEXT_H__)
#define __PSDAGTTEST_CREATE_CONTEXT_H__


class CPsdAgtTest8_1 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest8_1() : CTestStepPsdAgt(_L("Test8.1")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

#endif //(__PSDAGTTEST_CREATE_CONTEXT_H__)

