// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definitions of logging related replayer functions.
// 
//

/**
 @file
*/

#include "replaylogger.h"

_LIT8(KEtelSequenceHtmlHeader, "<html><body><pre>\r");
_LIT8(KEtelSequenceHtmlFooter, "</pre></body></html>\r");
_LIT8(KEtelSequenceHtmlPreamble, "IPC Sequence Flow\r\r");

CLogger::~CLogger()
	{
	iFile.Write(KEtelSequenceHtmlFooter);
	iFile.Close();
	iFs.Close();
	}	

CLogger* CLogger::NewL()
	{
	CLogger* self = new(ELeave) CLogger();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

void CLogger::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	_LIT(KOutFile,"C:\\Logs\\EtelReplaySequence.htm");


	TInt fileoptions = EFileShareExclusive|EFileWrite|EFileStreamText;
	User::LeaveIfError(iFile.Replace(iFs,KOutFile,fileoptions));
		
	//write html header
	iFile.Write(KEtelSequenceHtmlHeader);
	iFile.Write(KEtelSequenceHtmlPreamble);	
	}

void CLogger::Write(TRefByValue<const TDesC8> aFmt,...)
  	{
	VA_LIST list;
	VA_START(list, aFmt);
	
	iFormatted.FormatList(aFmt,list);

	iFile.Write(iFormatted);
	}
