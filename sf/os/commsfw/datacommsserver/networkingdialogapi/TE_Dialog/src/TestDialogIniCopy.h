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
* INICOPY.H
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


_LIT(KSrcPath, "z:\\testdata\\configs\\agentdialog.ini");

_LIT(KDestPath, "c:\\private\\101F7989\\esock\\agentdialog.ini");

///////////////////////////////////////////////
// Test Pre Copy agentdialog.ini
class CTestPreCopyIni : public CTestStep
{
public:
	CTestPreCopyIni();
	virtual ~CTestPreCopyIni();

	virtual enum TVerdict doTestStepL( void );
	void copyFileL (const TDesC& anOld,const TDesC& aNew);

private:
};

_LIT(KPreCopyDialog,"PreCopyDialog");

