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
//


//************************************************************************************************************
// System
#include <e32cons.h>

#include "testapproot.h"
#include <f32file.h>


//************************************************************************************************************
//   CTestAppRoot
//************************************************************************************************************
CTestAppRoot* CTestAppRoot::NewL(RTest& aTest, TDesC& aFrom, TDesC& aTo)
   {
	CTestAppRoot* self = new(ELeave) CTestAppRoot(aTest, aFrom, aTo);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
   };


//------------------------------------------------------------------------------------------------------------
CTestAppRoot::CTestAppRoot(RTest& aTest, TDesC& aFrom, TDesC& aTo)
	:CTimer(CActive::EPriorityStandard),
	iTest(aTest),
	iFrom(aFrom),
	iTo(aTo)
	{
	CActiveScheduler::Add(this);
	}


//------------------------------------------------------------------------------------------------------------
void CTestAppRoot::ConstructL()
	{
	CTimer::ConstructL();
	CompleteSelf();
	}


//------------------------------------------------------------------------------------------------------------
CTestAppRoot::~CTestAppRoot()
	{
	}

//------------------------------------------------------------------------------------------------------------
void CTestAppRoot::RunL()
	{
	const TInt chunkSize = 1024;

	RFs filesys;
	RFile file;
	TBuf8<chunkSize> buf;
	TInt ret 	 = 0;
	TInt counter = 0;
	
	ret = filesys.Connect();
	if (KErrNone != ret)
		{
		iTest.Printf(_L("\nError connecting to the file system: %d\n"), ret);
		}
	else
		{
		ret = file.Replace(filesys, iTo, EFileWrite);
		if (KErrNone != ret)
			{
			iTest.Printf(_L("\nError replacing the destination file: %d\n"), ret);
			}
		else
			{
			iTest.Printf(_L("Copying:\n"));
			
			while (1)
				{
				// KErrNone returned if EOF
				ret = filesys.ReadFileSection(iFrom, counter*chunkSize, buf, chunkSize);
				if (KErrNone != ret) 
					{
					iTest.Printf(_L("\nError reading from file: %d\n"), ret);
					break;			
					}
				
				ret = file.Write(buf);
				if (KErrNone != ret) 
					{
					iTest.Printf(_L("\nError writing to file: %d\n"), ret);
					break;			
					}
					
				iTest.Printf(_L("#"));
					
				// Break if EOF
				if (chunkSize > buf.Length()) break;
				counter++;
				}
			}
		}
					
	file.Close();
	filesys.Close();
	CActiveScheduler::Stop();
	}

//------------------------------------------------------------------------------------------------------------
void CTestAppRoot::DoCancel()
	{
	}

//------------------------------------------------------------------------------------------------------------	
void CTestAppRoot::CompleteSelf(TInt aReason)
	{
	TRequestStatus* pStat = &iStatus;
	   
	User::RequestComplete(pStat, aReason);
	SetActive();
	}
