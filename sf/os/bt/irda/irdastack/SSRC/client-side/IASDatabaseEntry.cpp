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

/**
 @file
 @internalAll
*/

#include <ir_sock.h>

EXPORT_C TIASDatabaseEntryV001::TIASDatabaseEntryV001()
	:iData()
/** Default constructor. Sets the type of the response data to missing. */
	{
	}

EXPORT_C void TIASDatabaseEntry::SetClassName(const TDesC8& aClassName)
/** Sets the class name in the IAS entry.

@param aClassName The class name. The maximum length of this descriptor is 
the value of the constant KIASClassNameMax. */
	{
	operator()().iClassName=aClassName;
	}

EXPORT_C void TIASDatabaseEntry::SetAttributeName(const TDesC8& anAttributeName)
/** Sets the attribute in the IAS entry.

@param anAttributeName The attribute. The maximum length of this descriptor 
is the value of the constant KIASAttributeNameMax. */
	{
	operator()().iAttributeName=anAttributeName;
	}

EXPORT_C void TIASDatabaseEntry::SetToInteger(const TUint anInteger)
/** Sets the response type corresponding to the class and attribute defined in 
this record, as an integer.

@param anInteger The response integer. */
	{
	operator()().iData.SetToInteger(anInteger);
	}

EXPORT_C void TIASDatabaseEntry::SetToCharString(const TDesC8& aCharString)
/** Sets the response type corresponding to the class and attribute defined in 
this record, as a character string.

@param aCharString The response string. The length of this string can never 
be greater than (KMaxQueryStringLength - 3). */
	{
	operator()().iData.SetToCharString(aCharString);
	}

EXPORT_C void TIASDatabaseEntry::SetToOctetSeq(const TDesC8& aLumpOfBinaryData)
/** Sets the response type corresponding to the class and attribute defined in 
this record, as an Octet sequence.

@param aLumpOfBinaryData The response sequence. */
	{
	operator()().iData.SetToOctetSeq(aLumpOfBinaryData);
	}

#ifdef _UNICODE
EXPORT_C void TIASDatabaseEntry::SetToCharString(const TDesC16& aWideString)
/** Sets the response type corresponding to the class and attribute defined in 
this record, as a wide character string.

@param aWideString The response string. */
	{
	operator()().iData.SetToCharString(aWideString);
	}
#endif

// EOF
