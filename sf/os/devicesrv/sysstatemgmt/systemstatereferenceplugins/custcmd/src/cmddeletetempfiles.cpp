// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalComponent
 @released
*/
#include "cmddeletetempfiles.h"
#include "ssmdebug.h"
#include "ssmuiproviderdll.h"

//Temp directory to be cleared
_LIT( KTempFiles, "\\System\\Temp\\*" );

CCustomCmdDeleteTempFiles* CCustomCmdDeleteTempFiles::NewL()
	{
	return new (ELeave) CCustomCmdDeleteTempFiles();
	}

CCustomCmdDeleteTempFiles::~CCustomCmdDeleteTempFiles()
    {
    }

CCustomCmdDeleteTempFiles::CCustomCmdDeleteTempFiles()
    {
    }

TInt CCustomCmdDeleteTempFiles::Initialize(CSsmCustomCommandEnv* aCmdEnv)
    {
    //Using the RFs from the command environment
    iFs.SetHandle(aCmdEnv->Rfs().Handle());
    return KErrNone;
    }

void CCustomCmdDeleteTempFiles::DeleteTempFilesL()
    {
    TInt driveId = CSsmUiSpecific::PhoneMemoryRootDriveId();
    TChar driveLetter;
    User::LeaveIfError(iFs.DriveToChar(driveId, driveLetter));
    TBuf<2> driveName;
    driveName.Append(driveLetter);
    driveName.Append(TChar(':'));

    TParse parse;
    //Ignore errors
    parse.Set(KTempFiles, &driveName, NULL);

    CFileMan* fileManager = CFileMan::NewL(iFs);
    fileManager->Delete(parse.FullName(), CFileMan::ERecurse);
    delete fileManager;
    }
        
void CCustomCmdDeleteTempFiles::Execute(const TDesC8& /*aParams*/, TRequestStatus& aRequest)
    {
    aRequest = KRequestPending;
    TRAPD(error, DeleteTempFilesL());
    if(KErrNone != error)
        {
        DEBUGPRINT2A("CCustomCmdDeleteTempFiles: Deleting temporary files failed with error %d", error);
        }

    TRequestStatus* request = &aRequest;
    User::RequestComplete(request, error);
    }

void CCustomCmdDeleteTempFiles::ExecuteCancel()
    {
    //Nothing to cancel
    }

void CCustomCmdDeleteTempFiles::Close()
    {
    }

void CCustomCmdDeleteTempFiles::Release()
    {
	delete this;
    }
