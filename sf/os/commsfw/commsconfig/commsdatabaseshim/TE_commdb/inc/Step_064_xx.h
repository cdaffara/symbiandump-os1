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
// This is the header file for CommDb test 064.XX group of scenarios 


#if (!defined __STEP_064_XX_H__)
#define __STEP_064_XX_H__


////////////////////
//	Test step 064.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest064_01) : public CTestStepCommDb
{
public:
	CCommDbTest064_01();
	~CCommDbTest064_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();

private:

};

#endif //(__STEP_064_XX_H__)
