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
// Global entry point for the LBS root process
// 
//

/**
 @file
 @internalTechnology
 @released
*/


#include <e32base.h>
#include <f32file.h>

#include "csecureasbase.h"
#include "claunchedprocess.h"
#include "tprocessstartparams.h"
#include "lbsdevloggermacros.h"
#include "lbsrootcenrepdefs.h"

#include <e32debug.h>
     
TInt E32Main()
   	{
   	LBSLOG_BEGIN();
   	
   	//We need to implement a lock for protecting against starting several instances of lbsroot.exe
   	//Several running instances of lbsroot may cause various problems. For example, the LBS shutdown 
   	//is done incorrectly - see DEF133108.
   	//The only safe way to implement locking, which was found, is by opening a file in the lbsroot
   	//private directory with EFileShareExclusive mode.
   	//This solution is a temporary solution. In the future it may not be needed when we convert the lbsroot
   	//into a server. Then its uniqueness will be guaranteed by the kernel. 
   	RFs fs;
   	TInt r = fs.Connect();
   	if(r!=KErrNone)
   	   	{
   	   	return r;
   	   	}
   	
   	_LIT(KLockFile,"c:\\private\\10282253\\lock");
   	RFile lockFile;
   	r = lockFile.Open(fs, KLockFile, EFileShareExclusive);
   	
   	if(r==KErrPathNotFound)
   		{
   		r = fs.CreatePrivatePath(EDriveC);
   		if(r==KErrNone)
   			{
   			r = lockFile.Create(fs, KLockFile, EFileShareExclusive);
   			}
   		else
   			{
   			fs.Close();
   			return r;
   			}
   		}
   	else if(r==KErrNotFound)
   		{
   		r = lockFile.Create(fs, KLockFile, EFileShareExclusive);
   		}
   		
   	
   	if(r!=KErrNone)
   		{
   		fs.Close();
   		if(r==KErrInUse) //That means that the file already locked by another lbsroot process
   			{
   			return 0;
   			}
   		else
   			{
   			return r;
   			}
   		}
   	
   	// do we need to process any startline params?
   	if(User::CommandLineLength()>0)
   		{
   		// yes, so assume these are TProcessStartParams that we understand
   		TProcessStartParams params;
   		TPtr ptr(reinterpret_cast<TText*>(&params), 0, sizeof(TProcessStartParams)/sizeof(TText16));
   		User::CommandLine(ptr);
 		r = CLaunchedProcess::CompleteProcessLaunch(params);
   		}
   	else
   		{
   		// no - so somebody and this should ONLY be the test team!! - has passed
   		// us a null command line, presumably by directly calling RProcess::Create()
   		// NB in this case you have no way of determining directly that the process
   		// creation has failed, or if the process is ready to start processing events!
 		r = CLaunchedProcess::CompleteProcessLaunch();
   		}
 
    lockFile.Close();
    fs.Close();
    
   	return r;
   	}

