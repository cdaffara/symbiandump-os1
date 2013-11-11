/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file backuprestoreStep.h
*/
#if (!defined BACKUPRESTORE_STEP_H)
#define BACKUPRESTORE_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

class CbackuprestoreStep : public CTe_EsockStepBase
	{
public:
	CbackuprestoreStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

private:
    TPtrC iOriginalFileName;
    TPtrC iBackupFileName;
    TPtrC iNewFileName;
    TBool iIsBackup;
    };

_LIT(KbackuprestoreStep,"backuprestoreStep");

#endif //BACKUPRESTORE_STEP_H

