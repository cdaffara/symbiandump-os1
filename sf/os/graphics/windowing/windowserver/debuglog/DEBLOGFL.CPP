// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// code for the F32 derived class 
// 
//

#include "../SERVER/w32cmd.h"
#include "DEBLOGFL.H"

#define __FORCE_ASCII yes

#define DEFAULT_LOG_FILE_NAME _L("C:\\private\\10003b20\\WSERV.LOG")

/*#if defined(__WINS__)
#pragma data_seg(".E32_UID")
__WINS_UID(0, KWservLoggingDllUidValue, 0)
#pragma data_seg()
#endif*/


EXPORT_C CDebugLogBase *CreateDebugLog(TBool aIsFirst, TDesC &aParams)
	{
	CDebugLogFile *device=new(ELeave) CDebugLogFile();
	CDebugLog *log=NULL;
	TRAPD(err,log=new(ELeave) CDebugLog(device));
	if (err!=KErrNone)
		{
		delete device;
		User::Leave(err);
		}
	TRAP(err,log->ConstructL(aIsFirst, aParams));
	if (err!=KErrNone)
		{
		delete log;
		User::Leave(err);
		}
	return(log);
	}

CDebugLogFile::CDebugLogFile()
	{
	__DECLARE_NAME(_S("CDebugLogFile"));
	}

CDebugLogFile::~CDebugLogFile()
	{
	iFile.Close();
	iFs.Close();
	}

void CDebugLogFile::ConstructL(TBool aIsFirst, TDesC &aParams)
	{
	TPtrC defaultFileName(DEFAULT_LOG_FILE_NAME);
	TDesC *fileName=&aParams;
	if (aParams.Length()==0)
		fileName=&defaultFileName;
	User::LeaveIfError(iFs.Connect());
	//_LIT(KLog1,"Connected To File Server");
	//RDebug::Print(KLog1);
	iFs.MkDirAll(*fileName);
	if (aIsFirst)
		{
		User::LeaveIfError(iFile.Replace(iFs,*fileName,EFileStreamText|EFileWrite|EFileShareReadersOrWriters));
		//_LIT(KLog2,"Replaced File 1");
		//RDebug::Print(KLog2);
#if !defined(__FORCE_ASCII)
		TUint16 feffInt=0xFEFF;
		User::LeaveIfError(iFile.Write(TPtrC8((TUint8 *)&feffInt,2)));
#endif
		}
	else
		{
		TInt err=iFile.Open(iFs,*fileName,EFileStreamText|EFileWrite|EFileShareReadersOrWriters);
		//_LIT(KLog3,"Done ReOpen");
		//RDebug::Print(KLog3);
		if (err==KErrNone)
			{
			TInt seekpos = 0;
			User::LeaveIfError(iFile.Seek(ESeekEnd,seekpos));
			}
		else
			User::LeaveIfError(iFile.Replace(iFs,*fileName,EFileStreamText|EFileWrite|EFileShareReadersOrWriters));
		}
	iEol16=TPtrC16((TUint16 *)_S("\r\n"));
	iEol.Set((TUint8 *)iEol16.Ptr(),iEol16.Size());
	iEol8=TPtrC8((TUint8 *)"\r\n");
	}

void CDebugLogFile::WriteToLogL(const TDesC &aDes, const TDesC &aDes2)
	{
#if defined(__FORCE_ASCII)
	TBuf8<128> des1;
	TBuf8<160> des2;
	des1.Copy(aDes);
	des2.Copy(aDes2);
	WriteToLog8L(des1,des2);
#else
	User::LeaveIfError(iFile.Write(TPtrC8((TUint8 *)aDes.Ptr(),aDes.Size())));
	User::LeaveIfError(iFile.Write(iEol));
	User::LeaveIfError(iFile.Write(TPtrC8((TUint8 *)aDes2.Ptr(),aDes2.Size())));
	iFile.Flush();		//Ignore Error
#endif
	}

void CDebugLogFile::WriteToLog8L(const TDesC8 &aDes, const TDesC8 &aDes2)
	{
	User::LeaveIfError(iFile.Write(aDes));
	User::LeaveIfError(iFile.Write(iEol8));
	User::LeaveIfError(iFile.Write(aDes2));
	iFile.Flush();		//Ignore Error
	}

