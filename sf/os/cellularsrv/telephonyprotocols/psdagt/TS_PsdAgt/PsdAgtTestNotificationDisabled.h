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
// This is the header file for PsdAgt test 4.1
// 
//


#if (!defined __PSDAGTTEST_NOTIFICATION_DISABLED_H__)
#define __PSDAGTTEST_NOTIFICATION_DISABLED_H__


class CPsdAgtTest4_1 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest4_1() : CTestStepPsdAgt(_L("Test4.1")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};


#endif //(__PSDAGTTEST_NOTIFICATION_DISABLED_H__)
