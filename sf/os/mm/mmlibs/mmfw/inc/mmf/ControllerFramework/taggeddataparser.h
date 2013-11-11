// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// mmf\common\mmfcontrollerpluginresolver.h
// 
//

#ifndef TAGGEDDATAPARSER_H
#define TAGGEDDATAPARSER_H

#include <ecom/ecom.h>
#include <e32base.h>
#include <apmstd.h>

class MTaggedDataParserClient;
/**
@internalComponent

Utility class used to parse data separated by xml-style tags.
*/
class TaggedDataParser
	{
public:

	/**
	Splits aData into xml-style tags and values, and gets aClient to process each tag/value pair.
	*/
	static void ParseTaggedDataL(const TDesC8& aData, MTaggedDataParserClient& aClient);

	/**
	Converts a string to a uid. If the string begin with "0x" it will be parsed as Hex, else Decimal.
	*/
	static void ConvertTextToUidL(const TDesC8& aData, TUid& aUid);

	/**
	Converts a string to a TUint. The string must begin with "0x" and be 10 characters long,
	otherwise it is considered corrupt.
	*/
	static void ConvertTextToTUintL(const TDesC8& aData, TUint& aUid);

	};

/**
@internalAll

MMF utility class used by MMF recognizer
Maintains an array of CMMFControllerImplementationInformation objects
so that data headers can be speedily matched against.
Updates the array when notified by ECOM of a change to the global
interface implementation registration data.
*/
NONSHARABLE_CLASS( CMmfRecognizerUtil ): public CBase
	{
public:
	enum TMatchLevel
		{
		EMatchNone, //no match
		EMatchData, //data match only
		EMatchName  //suffix and data match
		};
public:
	IMPORT_C static void GetMimeTypesL(CDesC8Array* aMimeTypes);

	IMPORT_C static CMmfRecognizerUtil* NewL();

	~CMmfRecognizerUtil();
	IMPORT_C TMatchLevel GetMimeTypeL(const TDesC& aFileName, const TDesC8& aImageData, TDes8& aMimeType);

private:
	CMmfRecognizerUtil();
	void ConstructL();

private:
	class CBody;
	CBody* iBody;
	};

#endif //TAGGEDDATAPARSER_H
