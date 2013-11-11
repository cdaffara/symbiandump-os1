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
// This is the header file for CsdAgx test 4.1
// 
//


#if (!defined __CSDAGTTEST_OOM_H__)
#define __CSDAGTTEST_OOM_H__


class CCsdAgtTest4_1 : public CTestStepCsdAgt
	{
public:
	CCsdAgtTest4_1() : CTestStepCsdAgt(_L("Test4.1")) {};

	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

#endif //(__CSDAGTTEST_OOM_H__)
