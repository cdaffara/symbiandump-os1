//a3ffourcclookup.h

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
//

/**
 @file
 @internalTechnology 
*/

 
#ifndef A3FFOURCCLOOKUP_H
#define A3FFOURCCLOOKUP_H

//INCLUDES
#include <ecom/ecom.h>
#include <mmf/common/mmfutilities.h>
#include <e32hashtab.h> 
#include <a3f/a3fbase.h>

//CLASS DECLARATION
/*
CFourCCConvertor class
This is a class used to load the format Uids and fourCC codes in to Cache Table.
This class is used for resolution of the fourCC code corresponding to format UID and vice-versa.
*/ 
class CFourCCConvertor : public CBase
	{
public: 
	/*
	Create a Single instance of a class.If the instance is already created, It
	returns that instance.
	*/
	IMPORT_C static CFourCCConvertor* NewL();
	
	/*
	This method is used to find fourCC code corresponding to format Uid from cache table.
	@param TUid aFormat a unique format Uid.
	@param TFourCC &aFourCC The unique FourCC code.
	@return an error code. KErrNone if successful, otherwise one of the system wide error codes.
	*/
	IMPORT_C TInt FormatToFourCC(TUid aFormat,TFourCC &aFourCC);

	/*
	This method is used to find format Uid corresponding to fourCC Code from cache table.
	@param TFourCC &aFourCC The unique FourCC code.
	@param TUid aFormat a unique format Uid.
	@return an error code. KErrNone if successful, otherwise one of the system wide error codes.
	*/
	IMPORT_C TInt FourCCToFormat(TFourCC aFourCC, TUid &aFormat) const;

	/*
 	Destructor    
	*/
	virtual ~CFourCCConvertor();

private:
	/*
	Constructor
	*/
	CFourCCConvertor();

	/*
	Second Phase Constructor
	*/
	void ConstructL();

	/*
	This method is used to initialize the cache tables. This method uses two cache
	tables. It Stores Format Uids corresponding to fourCC Codes in one cache table
	and fourCC codes corresponding to Format Uids in another cache table. In the First 
	cache table,it uses format Uid as key and in the second cache table, it uses fourCC code as key. 
	*/
	void LoadL();

    // DATA DECLARATION

	/* Cache Table that stores FourCC codes corresponding to Format Uids */
	RHashMap<TUint32,TUint32> *iHashFourCCToFormat;
	
	/* Cache Table that stores format Uids corresponding to FourCC codes */
	RHashMap<TUint32,TUint32> *iHashFormatToFourCC;

	};

#endif // A3FFOURCCLOOKUP_H
