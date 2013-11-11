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

#if !defined (__SYSTEMSTARTUPDLLINFO_H__)
#define __SYSTEMSTARTUPDLLINFO_H__

#include <e32base.h>
 
/**
This class holds all the information specific to a particular DLL. 
The information is extracted from the static start-up configuration
file (SSC). Information stored includes the DLL name and the ordinal 
number of the function to be called.  

@internalComponent
@deprecated
*/
class CSystemStartupDllInfo: public CBase 
	{
public:
	static CSystemStartupDllInfo* NewL( );
	static CSystemStartupDllInfo* NewLC( );
	~CSystemStartupDllInfo();
	 
	// Set Functions
	void SetDllNameL(const TDesC& aDllName);
	void SetOrdinal(TUint8 aOrdinal);
	void SetFailOnError(TBool aFailOnError);
	void SetNoOfRetries(TUint8 aNoOfRetries);
	void SetDllBuffer(HBufC8* aDllBuffer);
 	
	// Get Functions		
	TPtrC DllName() const;
	TUint8 Ordinal() const;
	TBool FailOnError() const;
	TUint8 NoOfRetries() const;
 	const TDesC8& DllBuffer() const;
  
private:
	CSystemStartupDllInfo();
	void Construct( );
	
private:
 	HBufC* iDllName;
 	TUint8 iOrdinal;
   	TBool iFailOnError;
 	TUint8 iNoOfRetries;
 	HBufC8*  iDllBuffer;
	};
 
#endif	// __SYSTEMSTARTUPDLLINFO_H__
