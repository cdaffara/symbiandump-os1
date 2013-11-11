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

#include "SystemStartupDllInfo.h"

//
// Standard Symbian factory functions/destructor
//
 
CSystemStartupDllInfo* CSystemStartupDllInfo::NewL( )
	{
	CSystemStartupDllInfo* self = CSystemStartupDllInfo::NewLC( );
	CleanupStack::Pop(self);
	return self;
	}

CSystemStartupDllInfo* CSystemStartupDllInfo::NewLC( )
	{
	CSystemStartupDllInfo* self = new (ELeave) CSystemStartupDllInfo();
	CleanupStack::PushL(self);
	self->Construct( );
	return self;
	}

CSystemStartupDllInfo::~CSystemStartupDllInfo()
	{
 	delete iDllBuffer;
	delete iDllName;
	}
	
CSystemStartupDllInfo::CSystemStartupDllInfo()  
	{}

void CSystemStartupDllInfo::Construct()
	{}


//
// Public accessor functions
//
 
// Set methods

/**
Set the DLL name. 
*/ 
void CSystemStartupDllInfo::SetDllNameL(const TDesC& aDllName)
	{
	iDllName = aDllName.AllocL();
	}

/** 
Set the ordinal number of the function in the DLL 
to be executed	
*/
void CSystemStartupDllInfo::SetOrdinal(TUint8 aOrdinal)
	{
	iOrdinal = aOrdinal;
	}

 
/** Set a boolean indicating whether startup should fail or 
continue if DLL fails to load or DLL function returns an error code
*/	
void CSystemStartupDllInfo::SetFailOnError(TBool aFail)
	{
	iFailOnError = aFail;
	}

/**    
Set the number of times an attempt should be made to load a DLL/execute a 
DLL function in case of failure
*/ 
void CSystemStartupDllInfo::SetNoOfRetries(TUint8 aNoOfRetries)
	{
	iNoOfRetries = aNoOfRetries;
	}

/**  Set a pointer to the buffer containing data to be
used by the DLL function
*/	
void CSystemStartupDllInfo::SetDllBuffer( HBufC8* aDllBuffer)
 	{
 	iDllBuffer = aDllBuffer;
 	}
  
  
// Get methods
  
/**
Retrieve the DLL name. 
*/
TPtrC CSystemStartupDllInfo::DllName() const
	{
	return (*iDllName);
	}

/** 
Retrieve the ordinal number of the function in the DLL 
to be executed	
*/
TUint8 CSystemStartupDllInfo::Ordinal() const
	{
	return iOrdinal;
	}

/**    
Retrieve the number of times an attempt should be made to 
load a DLL/execute a DLL function in case of failure
*/
TUint8 CSystemStartupDllInfo::NoOfRetries() const
	{
	return iNoOfRetries;
	}	

/** Returns boolean indicating whether startup should fail or 
continue if DLL fails to load or DLL function returns an error 
code
*/
TBool CSystemStartupDllInfo::FailOnError() const
	{
	return iFailOnError;
	}

/** Retrieve a pointer to a buffer containing data to be
used by the DLL function
*/ 
const TDesC8& CSystemStartupDllInfo::DllBuffer() const
	{
	return *iDllBuffer ;
	} 
