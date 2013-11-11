// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// RStartProcess Client side header
// Declares the R class to start a process,
// the class covers EPOC platform & emulator diferences
// 
//

/**
 @file 
 @internalTechnology
*/

#ifndef __CS_STARTSERVER_H__
#define __CS_STARTSERVER_H__

#include <e32base.h>

const TUint KMaxFileNameLength = 55;

/**
 Starts a process and synchronise with it.
 
 The process must support synchronisation via Randezvous
 @internalTechnology
**/
class RStartProcess
	{
public:
   	IMPORT_C TInt Start(const TDesC& aName, TRequestStatus& aStatus);
   	IMPORT_C TInt Start(const TDesC& aName);
   	IMPORT_C void Cancel();
   	TDesC& Name();
   	TInt ErrorCode() const;

private:
   	TBufC<KMaxFileNameLength> iName;
      TInt iErrorCode;


   	RProcess iProcess;

private:
   	TInt StartProcess();


   	static TInt StartProcessThreadFn(TAny* aStartProcess);
   	TInt WaitForCompletion();
	};


inline TInt RStartProcess::ErrorCode() const
   {
      return iErrorCode;
   }

inline TDesC& RStartProcess::Name()
	{
   	return iName;
	}

#endif

