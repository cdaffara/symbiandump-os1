// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MVSSavaAeDialog.cpp
// Part of the MVS Application for TechView
//

#include "MVSSaveAsDialog.h"
#include "MVSApp.hrh"
#include <techview/eikcfdlg.h>
#include <techview/eikon.hrh>
#include <techview/eikfbrow.h>
#include <techview/eikedwin.h>
#include <eikenv.h>
#include <coeutils.h>
#include <f32file.h>
#include <techview/eikfsel.h>
#include <apparc.h>
#include <eikproc.h>
#include <techview/eikinfo.h>
#include <eikfutil.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikpanic.h>
#include <techview/eikinfo.h>
#include <uiklafgt/eikcore.rsg>
#include <eikfile.rsg>

_LIT(KAnyFile,"*");
const TInt KShortErrorMessageLength = 64;
const TInt KMaxDisplayedFullNameLen	= 35;

TInt CMVSSaveAsDialog::SetupDialogLD(TDes& aFileName, TBool* aReplace)
	{
    CMVSSaveAsDialog* dialog = new(ELeave) CMVSSaveAsDialog(aFileName, aReplace);
    TInt val = dialog->ExecuteLD(R_EIK_DIALOG_FILE_SAVEAS);
    return val;
    }

CMVSSaveAsDialog::CMVSSaveAsDialog(TDes& aFilename, TBool* aReplace):CEikFileSaveAsDialog(&aFilename),iFilename(aFilename)
    {
    iReplace = aReplace;
    }

TBool CMVSSaveAsDialog::OkToExitL(TInt aButtonId)

    {
	if (aButtonId==EEikBidBrowse)
		{
		HandleBrowseButtonL();
		return EFalse;
		}
	else
		{
		CEikFileNameEditor* fileNameEditor=(CEikFileNameEditor*)(Control(EEikCidFileNameEd));
		TFileName* fullName=new(ELeave) TFileName;
		CleanupStack::PushL(fullName);
		fileNameEditor->GetFullNameL(*fullName);
		TUint attributes=0;
		if (!ConeUtils::FileExists(*fullName))
			{
			ConeUtils::EnsurePathExistsL(*fullName);	
			}
		else
			{
			TBuf<32> infoDialogTitle;
			iEikonEnv->ReadResource(infoDialogTitle,R_EIK_TBUF_DIALOG_TITLE_CONFIRM_FILE_REPLACE);
			TBuf<KShortErrorMessageLength> formatStr;
			User::LeaveIfError(iEikonEnv->FsSession().Att(*fullName,attributes));
			if (attributes&KEntryAttReadOnly)
				{
				iEikonEnv->LeaveWithInfoMsg(R_EIK_TBUF_CANNOT_REPLACE_READONLY_FILE);
				}		
			else
				{
				iEikonEnv->ReadResource(formatStr,R_EIK_TBUF_FILE_REPLACE_CONFIRM1);
				}
			TBuf<KMaxDisplayedFullNameLen> abbrevName;
			User::LeaveIfError(EikFileUtils::Parse(*fullName));
			TParsePtr parse(*fullName);
			EikFileUtils::AbbreviateFileName(parse.NameAndExt(),abbrevName);
			TBuf<KShortErrorMessageLength + KMaxDisplayedFullNameLen> textMsg;
			textMsg.Format(formatStr,&abbrevName);
			CEikDialog* infoDialog=new(ELeave) CEikInfoDialog(infoDialogTitle,textMsg,CEikInfoDialog::EIgnoreEnter);
			*iReplace = infoDialog->ExecuteLD(R_EIK_DIALOG_SINGLE_FILE_REPLACE);
			}
		// check disk is present in selected drive
		TParsePtrC parse(*fullName);
		TPtrC drv=parse.Drive();
		TBuf<4> root(drv);
		root.Append(TChar(KPathDelimiter));
		root.Append(KAnyFile);
		RDir dir;
		const TInt ret=dir.Open(iEikonEnv->FsSession(),root,EFileRead|EFileShareAny);
		if (ret==KErrNotReady)
			{
			iEikonEnv->LeaveWithInfoMsg(R_EIK_TBUF_DISK_NOT_PRESENT);
			}
		if (ret==KErrNone)
			{
			dir.Close();
			}
		//
		iFilename=*fullName;
		CleanupStack::PopAndDestroy(); // fullName
		return ETrue;
		}
    }
