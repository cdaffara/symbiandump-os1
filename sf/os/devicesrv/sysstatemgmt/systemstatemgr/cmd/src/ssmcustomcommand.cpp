// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <ssm/ssmcustomcommand.h>

/** 
Constructor
*/
CSsmCustomCommandEnv::CSsmCustomCommandEnv(RFs& aFs)
	: iFs(aFs)
	{
	}

/** 
Destructor
*/
CSsmCustomCommandEnv::~CSsmCustomCommandEnv()
	{
	}

/** 
Safe constructor
*/
CSsmCustomCommandEnv* CSsmCustomCommandEnv::NewL(RFs& aRfs)
	{
	CSsmCustomCommandEnv* self = new (ELeave) CSsmCustomCommandEnv(aRfs);
	return self;
	}

	
/** 
Provides a connected RFs session to custom commands.

@return const reference to a filesystem object.

@publishedPartner
@released
*/
EXPORT_C const RFs& CSsmCustomCommandEnv::Rfs() const
	{
	return iFs;
	}

/** 
Copy constructor
*/
CSsmCustomCommandEnv::CSsmCustomCommandEnv(const CSsmCustomCommandEnv& aEnv)
	: iFs(aEnv.iFs)
	{
	}

/** 
Assignment operator
*/
CSsmCustomCommandEnv& CSsmCustomCommandEnv::operator= (const CSsmCustomCommandEnv& aEnv)
	{
	// protect against self-assignment
 	if(&aEnv != this)
	 	{
		 iFs = aEnv.iFs;
	 	}
	 	
	return (*this);
	}
