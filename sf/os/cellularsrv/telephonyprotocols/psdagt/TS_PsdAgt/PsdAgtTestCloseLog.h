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
// This is the header file for PsdAgt test 12.1
// 
//


#if (!defined __PSDAGTTEST_CLOSELOG_H__)
#define __PSDAGTTEST_CLOSELOG_H__


class CPsdAgtTest12_1 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest12_1() : CTestStepPsdAgt(_L("Test12.1")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	
protected:
	// Override AgentProgress function
	virtual void AgentProgress(TInt aStage, TInt aError);
	};


#endif //(__PSDAGTTEST_CLOSELOG_H__)
