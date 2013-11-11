/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file OOMStep.h
 @internalTechnology
*/
#ifndef __OOM_STEP_H__
#define __OOM_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_defproxySuiteStepBase.h"
/*
OOM
*
* To test OOM situation, copy paste the following code to the begining of 
* CPosDefaultPositioner::NewL and compile default proxy again.
    //Debug code
    TInt fr=0;
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RFile file;
    User::LeaveIfError(file.Open(fs, _L("c:\\fr.txt"), EFileRead));
    CleanupClosePushL(file);
    TFileText fileText;
    fileText.Set(file);
    TBuf<32> buf;
    fileText.Read(buf);
    TLex lex(buf);
    User::LeaveIfError(lex.Val(fr));
    CleanupStack::PopAndDestroy(2, &fs);

    __UHEAP_FAILNEXT(fr);

*/
class COOMStep : public CTe_defproxySuiteStepBase
	{
public:
	COOMStep();
	~COOMStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
private:
    void MakeLRL();
	};

_LIT(KOOMStep,"OOMStep");

#endif
