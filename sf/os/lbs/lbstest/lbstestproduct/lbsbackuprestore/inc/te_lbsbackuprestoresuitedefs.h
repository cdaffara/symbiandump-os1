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
* This file define all the common values thoughout your test project
* 
*
*/



/**
 @file Te_lbsbackuprestoreSuiteDefs.h
 @internalTechnology
*/
#ifndef __TE_LBSBACKUPRESTORE_SUITEDEFS_H__
#define __TE_LBSBACKUPRESTORE_SUITEDEFS_H__

const TUint32 KBackupKey = 0x01234567;
const TUint32 KMetatagBackupAndRestoreValidValue = 0x10000000;
const TInt KValidValue = 0x01221122;
const TInt KCorruptedValue = 0x01001001;


// For test step panics
_LIT(KTe_lbsbackuprestoreSuitePanic, "Te_lbsbackuprestoreSuite");
_LIT(KImpossibleCreateTestStep, "The creation of the test step failed with error code:%d");

_LIT(KTe_iniFileName, "inifilename");
_LIT(KPSYList, "psylist");
_LIT(KPSYListSize, "listsize");
_LIT(KUIDString, "UID%d");
_LIT(KNameString, "name%d=");
_LIT(KLogPSYListSize, "[psylist]listsize=%d");
_LIT(KLogUIDString, "UID%d=%X");
_LIT(KLogNameString, "name%d=%S");

#endif
