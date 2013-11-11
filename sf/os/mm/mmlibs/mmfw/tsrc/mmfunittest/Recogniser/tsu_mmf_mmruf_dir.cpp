// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include "tsu_mmf_mmruf_step.h"


// These directories contain files that RUF shouldn't recognise.
static const TText* const KDirPaths[] = 
	{
	_S("c:\\recog\\common\\")
	};

#define KDirPathsCount	sizeof(KDirPaths) / sizeof(const TText*)
	
//
//
//
RMMRUFDirTestStep::RMMRUFDirTestStep()
	{
	iTestStepName = _L("MM-MMF-MMRUF-U-0400-HP");
	}

//
//
//
RMMRUFDirTestStep::~RMMRUFDirTestStep()
	{
	Close();
	}

//
//
//
RMMRUFDirTestStep* RMMRUFDirTestStep::NewL()
	{
	return new(ELeave) RMMRUFDirTestStep();
	}

//
//
//
TVerdict RMMRUFDirTestStep::DoTestStepL()
	{
	TInt err;
	TBool failed;
	RAppArc appArc;
	TVerdict verdict = EPass;

	err = appArc.Connect();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("RAppArc.Connect() failed with: %d"), err);
		return EInconclusive;
		}
	CleanupClosePushL(appArc);

	for (TInt i = 0; i < KDirPathsCount; i++)
		{
		const TDesC& curDirectory = TPtrC(KDirPaths[i]);
		INFO_PRINTF2(_L("Checking files in directory: %S"), &curDirectory);
		
		TRAP(err, CheckFilesInDirectoryL(appArc, curDirectory, failed));
		switch (err)
			{
			case KErrNone:
				break;
				
			case KErrPathNotFound:
				// The test hasn't really failed.
				INFO_PRINTF1(_L(" - This directory cannot be found"));
				break;
				
			default:
				INFO_PRINTF2(_L("CheckFilesInDirectoryL left with %d"), err);
				User::Leave(err);
			}
			
		if (failed)
			{
			// Even though a failure was seen, keep going until all files have been checked.
			verdict = EFail;
			}
		}
		
	CleanupStack::PopAndDestroy(&appArc);
	return verdict;
	}

//
//
//
void RMMRUFDirTestStep::CheckFilesInDirectoryL(RAppArc& aAppArc, const TDesC& aDirPath, TBool& aFailure)
	{
	CDir* pDir = NULL;
	CDirScan* pDirScan = CDirScan::NewLC(aAppArc.FileServer());
	pDirScan->SetScanDataL(aDirPath, KEntryAttNormal, CDirScan::EScanDownTree);
	pDirScan->NextL(pDir);
	CleanupStack::PushL(pDir);
	
	TBuf<KMaxPath> path;
	aFailure = EFalse;
	TInt i;
	for (i = 0; i < pDir->Count(); i++)
		{
		const TEntry& entry = (*pDir)[i];
		
		path.Zero();
		path.Append(aDirPath);
		path.Append(entry.iName);
		if (DoRecogniseL(path, aAppArc) != EPass)
			{
			// Even though this file was wrongly recognised
			// keep checking the other files in the directory.
			aFailure = ETrue;
			}
		}
	INFO_PRINTF2(_L("Checked %d files"), i);
		
	CleanupStack::PopAndDestroy(2, pDirScan);
	}

//
// This function returns EPass if aFileName was not recognised
// as an audio or video mime-type, EFail otherwise.
//
TVerdict RMMRUFDirTestStep::DoRecogniseL(const TDesC& aFileName, RAppArc& aAppArc)
	{
	TDataType dataType;
	RFile file;
	_LIT8(KAudio, "audio/*");
	_LIT8(KVideo, "video/*");

	TInt err = file.Open(aAppArc.FileServer(), aFileName, EFileRead | EFileShareAny);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("RFile.Open() failed: %d"), err);
		return EInconclusive;
		}
		
	err = aAppArc.Recognise(file, dataType);
	file.Close();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("RAppArc.Recognise() failed: %d"), err);
		return EInconclusive;
		}

	// Check that the mime-type is either zero length,
	// or doesn't begin with "audio/" or "video/"
	TPtrC8 mimeType = dataType.Des8();
	if (mimeType.Length() > 0)
		{
		if (mimeType.MatchF(KAudio) != KErrNotFound)
			{
			TBuf<KMaxDataTypeLength> returned;
			returned.Copy(dataType.Des());
			INFO_PRINTF3(_L("File \"%S\" has been recognised as %S"), &aFileName, &returned);
			return EFail; // It has been recognised.
			}

		if (mimeType.MatchF(KVideo) != KErrNotFound)
			{
			TBuf<KMaxDataTypeLength> returned;
			returned.Copy(dataType.Des());
			INFO_PRINTF3(_L("File \"%S\" has been recognised as %S"), &aFileName, &returned);
			return EFail; // It has been recognised.
			}
		}

	return EPass;
	}
