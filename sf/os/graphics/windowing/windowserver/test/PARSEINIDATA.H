// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class definition to parse the ini file
// 
//

/**
@file
@test
@internalComponent
*/

#ifndef __CINIDATA_H__
#define __CINIDATA_H__

#include <e32base.h>

class CIniData : public CBase 
/**
 *
 * @publishedPartner
 * @test
 *
 * Defines the interface to acess to ini data file
 *
 * The basic functions, FindVar(), SetValue(), AddValue() and WriteToFileL()
 * Compulsory to call WriteToFileL() after calling any SetValue() or AddValue()
 */
	{
public:
	// Constructor, pass in name of ini file to open
	// Default search path is 'c:\system\data' on target filesystem
	// ie. 'NewL(_L("c:\\system\\data\\ttools.ini"))' is equivalent
	// to 'NewL(_L("ttools.ini"))'
	static CIniData* NewL(const TDesC& aName, char aDelimiter);
	virtual ~CIniData();

	TBool FindVar(const TDesC& aKeyName,	// Key to look for
				  TPtrC& aResult);			// Buffer to store value

	TBool FindVar(const TDesC& aKeyName, 	// Key to look for
	    		  TInt& aResult);			// Int ref to store result

	TBool FindVar(const TDesC& aSection,	// Section to look under
			      const TDesC& aKeyName,	// Key to look for
				  TPtrC& aResult);			// Buffer to store result

	TBool FindVar(const TDesC& aSection,	// Section to look under
				  const TDesC& aKeyName,	// Key to look for
				  TInt& aResult);			// Int ref to store result
protected:
	CIniData();
	void ConstructL(const TDesC& aName, char aDelimiter);
private:
	HBufC* iName;
	HBufC* iToken;
	TPtr iPtr;
	char iDelimiter;
	};

#endif
