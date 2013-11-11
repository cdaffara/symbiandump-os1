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
// Description: Custom command to intialise the RAM drive.
//

#include "cmdinitramdrive.h"

//RAM drive directories created on every boot
_LIT(KRAMDriveTempPath, "\\system\\temp\\");
_LIT(KWAPCachePath, "\\wap\\");
_LIT(KCBSCachePath, "\\cbs\\");

CCustomCmdInitRamDrive* CCustomCmdInitRamDrive::NewL()
	{
	return new (ELeave) CCustomCmdInitRamDrive();
	}

CCustomCmdInitRamDrive::~CCustomCmdInitRamDrive()
    {
    }

CCustomCmdInitRamDrive::CCustomCmdInitRamDrive()
    {
    }

TInt CCustomCmdInitRamDrive::Initialize(CSsmCustomCommandEnv* aCmdEnv)
    {
    //Using the RFs from the command environment
    iFs.SetHandle(aCmdEnv->Rfs().Handle());
    return KErrNone;
    }

void CCustomCmdInitRamDrive::Execute(const TDesC8& /*aParams*/, TRequestStatus& aRequest)
    {
    aRequest = KRequestPending;
    TChar ramDriveLetter;
    // Ignore errors
    iFs.DriveToChar( EDriveD, ramDriveLetter );

    //Buffer containing drive id (drive letter + :)
    TBuf<2> driveName;
    driveName.Append(ramDriveLetter);
    driveName.Append(TChar(':'));

    TParse parse;
    // Ignore errors.
    parse.SetNoWild(KRAMDriveTempPath, &driveName, NULL);
    iFs.MkDirAll(parse.DriveAndPath());

    parse.SetNoWild(KWAPCachePath, &driveName, NULL);
    iFs.MkDirAll(parse.DriveAndPath());

    parse.SetNoWild(KCBSCachePath, &driveName, NULL);
    iFs.MkDirAll(parse.DriveAndPath());

    TRequestStatus* request = &aRequest;
    User::RequestComplete(request, KErrNone);
    }

void CCustomCmdInitRamDrive::ExecuteCancel()
    {
    //Nothing to cancel
    }

void CCustomCmdInitRamDrive::Close()
    {
    }

void CCustomCmdInitRamDrive::Release()
    {
	delete this;
    }
