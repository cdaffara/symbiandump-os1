// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "tlog.h"





RLog::RLog()
{
	// Does nothing
}



RLog::RLog(const TDesC &aLogFileName, RTest *atest)
{
	Construct(aLogFileName, atest);
}



void RLog::Construct(const TDesC &aLogFileName, RTest *atest)
{
	TInt pos;
	
	fileServer.Connect();
	file.Replace(fileServer, aLogFileName, EFileStreamText | EFileShareAny);
	file.Seek(ESeekEnd, pos);

	test = atest;
}


void RLog::Close()
{
	file.Close();
}


void RLog::Start(const TDesC &aHeading)
{
	// Logs in the log file and calls the "Start" function from RTest
	
	TBuf8 <256> Buffer;
	TBuf8 <1024> pHeading;

	pHeading.Copy(aHeading);

	

	if(aHeading.Length()){
		Buffer.Copy(aHeading);
		if(pHeading.Mid(pHeading.Length() - 1, 1) == _L8("\n")){
			Buffer.Insert(pHeading.Length() - 1, _L8("\r"));
		}
		else
		{
			Buffer.Append(_L("\r\n"));
		}
		file.Write(Buffer);
	}
		
		test->Start(aHeading);
		

}



void RLog::Next(const TDesC &aHeading)
{
	// Logs in the file and calls the "Next" function from RTest
	
	TBuf8 <256> Buffer;
	TBuf8 <1024> pHeading;

	pHeading.Copy(aHeading);


	if(aHeading.Length()){
			Buffer.Copy(aHeading);
			if(pHeading.Mid(pHeading.Length() - 1, 1) == _L8("\n")){
				Buffer.Insert(pHeading.Length() - 1, _L8("\r"));
			}
			else
			{
				Buffer.Append(_L("\r\n"));
			}

			file.Write(Buffer);
	}

	test->Next(aHeading);
	
	
}


void RLog::Printf(TRefByValue<const TDesC> aFmt,...)
{
	// Logs in the file and calls the Printf Function from the attached  RTest object

	VA_LIST list;
	VA_START(list,aFmt);

	TBuf<0x100> aBuf;
	TBuf8<1024> fBuf;
	aBuf.AppendFormatList(aFmt,list);
	
	if(aBuf.Mid(aBuf.Length() - 1, 1) == _L("\n")){
		aBuf.Insert(aBuf.Length() - 1, _L("\r"));
	}
	
	

	test->Printf(aBuf);
	fBuf.Copy(aBuf);
	TInt r = file.Write(fBuf);	
	
	__ASSERT_ALWAYS(r==KErrNone,User::Panic(_L("Write-Console"),0));

}


void RLog::Log(TRefByValue<const TDesC> aFmt,...)
{
	// Logs in the file and calls the Printf Function from the attached  RTest object

	VA_LIST list;
	VA_START(list,aFmt);

	TBuf<0x100> aBuf;
	TBuf8 <1024> fBuf;
	aBuf.AppendFormatList(aFmt,list);
	
	if(aBuf.Mid(aBuf.Length() - 1, 1) == _L("\n")){
		aBuf.Insert(aBuf.Length() - 1, _L("\r"));
	}
	
	
	fBuf.Copy(aBuf);
	file.Write(fBuf);	
	

}

void RLog::SeekEnd()
{
	TInt pos;

	file.Seek(ESeekEnd, pos);

}
