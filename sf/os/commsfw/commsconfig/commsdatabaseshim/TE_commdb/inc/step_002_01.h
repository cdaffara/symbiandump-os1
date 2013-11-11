/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
// This is the header file for CommDb test 001.01


#if (!defined __STEP_002_01_H__)
#define __STEP_002_01_H__

NONSHARABLE_CLASS(CCommDbTest002_01) : public CTestStepCommDb
{
public:
	CCommDbTest002_01();
	~CCommDbTest002_01();

	virtual enum TVerdict doTestStepL( void );

};

#endif //(__STEP_002_01_H__)
