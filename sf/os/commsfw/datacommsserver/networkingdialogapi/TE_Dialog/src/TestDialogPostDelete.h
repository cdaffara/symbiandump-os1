/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* POSTDELETE.H
* Ini file preparation
* 
*
*/



/**
 @file
 @internalComponent
*/

//Test Framework Header
#include <test/testexecutestepbase.h>

_LIT(KFilePath, "c:\\private\\101F7989\\esock\\agentdialog.ini");

///////////////////////////////////////////////
// Test Post Delete agentdialog.ini
class CTestPostDeleteIni : public CTestStep
{
public:
	CTestPostDeleteIni();
	virtual ~CTestPostDeleteIni();

	virtual enum TVerdict doTestStepL( void );
	void deleteFileL (const TDesC& aFileName);

private:
};

_LIT(KPostDeleteDialog,"PostDeleteDialog");

