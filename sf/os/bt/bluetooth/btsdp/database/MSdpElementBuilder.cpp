// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <btsdp.h>
#include "mignorer.h"

EXPORT_C MSdpElementBuilder* MSdpElementBuilder::BuildUnknownL(TUint8 /*aType*/, TUint8 /*aSizeDesc*/, const TDesC8& /*aData*/)
/** Adds an element of any type.

@param aType Type descriptor
@param aSizeDesc Size descriptor
@param aData Data field
@return Builder with added element */
	{
	User::Leave(KErrGeneral);
	return 0;
	}

EXPORT_C MSdpElementBuilder* MSdpElementBuilder::BuildNilL()
/** Adds a null element.

@return Builder with added element */
	{
	User::Leave(KErrGeneral);
	return 0;
	}

EXPORT_C MSdpElementBuilder* MSdpElementBuilder::BuildUintL(const TDesC8& /*aUint*/)
/** Adds an unsigned integer element.

@param aUint Element to add
@return Builder with added element */
	{
	User::Leave(KErrGeneral);
	return 0;
	}

EXPORT_C MSdpElementBuilder* MSdpElementBuilder::BuildIntL(const TDesC8& /*aInt*/)
/** Adds a signed integer element.

@param aInt Element to add
@return Builder with added element */
	{
	User::Leave(KErrGeneral);
	return 0;
	}

EXPORT_C MSdpElementBuilder* MSdpElementBuilder::BuildUUIDL(const TUUID& /*aUUID*/)
/** Adds a UUID element.

@param aUUID Element to add
@return Builder with added element */
	{
	User::Leave(KErrGeneral);
	return 0;
	}

EXPORT_C MSdpElementBuilder* MSdpElementBuilder::BuildBooleanL(TBool /*aBool*/)
/** Adds a Boolean element.

@param aBool Element to add
@return Builder with added element */
	{
	User::Leave(KErrGeneral);
	return 0;
	}

EXPORT_C MSdpElementBuilder* MSdpElementBuilder::BuildStringL(const TDesC8& /*aString*/)
/** Adds a Text String element.

@param aString Element to add
@return Builder with added element */
	{
	User::Leave(KErrGeneral);
	return 0;
	}

EXPORT_C MSdpElementBuilder* MSdpElementBuilder::BuildDESL()
/** Adds a (header for a) Data element sequence (DES), a data element 
whose data field is a sequence of data elements.

This should be followed by a call to StartListL(), and then calls to add elements 
to the list.

@return Attribute value with added element */
	{
	User::Leave(KErrGeneral);
	return 0;
	}

EXPORT_C MSdpElementBuilder* MSdpElementBuilder::BuildDEAL()
/** Adds a (header for a) Data element alternative (DEA), a data element 
whose data field is a sequence of data elements from which one data element 
is to be selected.

This should be followed by a call to StartListL(), and then calls to add elements 
to the list.

@return Attribute value with added element */
	{
	User::Leave(KErrGeneral);
	return 0;
	}

/** Tell builder you are about to start a list

@return Builder */
EXPORT_C MSdpElementBuilder* MSdpElementBuilder::StartListL()
/** Indicates that subsequent elements added belong to a DES or DEA.

The end of the list should be indicated by a call to EndList().

@return Builder */
	{
	User::Leave(KErrGeneral);
	return 0;
	}

EXPORT_C MSdpElementBuilder* MSdpElementBuilder::EndListL()
/** Indicates that subsequent elements no longer belong to current DES or DEA

Building of current DES or DEA is terminated.
@return Builder */
	{
	User::Leave(KErrGeneral);
	return 0;
	}

EXPORT_C MSdpElementBuilder* MSdpElementBuilder::BuildURLL(const TDesC8& /*aString*/)
/** Adds a URL element.

@param aURL Element to add
@return Builder with added element */
	{
	User::Leave(KErrGeneral);
	return 0;
	}
	
/**
This virtual function allows the M- classes to be extended in future in a binary
compatible way by providing a method that clients can override in future to
allow extra callbacks to be made via aObject.
*/	
EXPORT_C void MSdpElementBuilder::MSEB_ExtensionInterfaceL(TUid /*aInterface*/, void*& aObject)
	{
	aObject = NULL;
	}

// opposite, ignores everything

MSdpElementBuilder* MIgnorer::BuildUnknownL(TUint8 /*aType*/, TUint8 /*aSizeDesc*/, const TDesC8& /*aData*/)
	{
	return this;
	}

MSdpElementBuilder* MIgnorer::BuildNilL()
	{
	return this;
	}

MSdpElementBuilder* MIgnorer::BuildUintL(const TDesC8& /*aUint*/)
	{
	return this;
	}

MSdpElementBuilder* MIgnorer::BuildIntL(const TDesC8& /*aInt*/)
	{
	return this;
	}

MSdpElementBuilder* MIgnorer::BuildUUIDL(const TUUID& /*aUUID*/)
	{
	return this;
	}

MSdpElementBuilder* MIgnorer::BuildBooleanL(TBool /*aBool*/)
	{
	return this;
	}

MSdpElementBuilder* MIgnorer::BuildStringL(const TDesC8& /*aString*/)
	{
	return this;
	}

MSdpElementBuilder* MIgnorer::BuildDESL()
	{
	return this;
	}

MSdpElementBuilder* MIgnorer::BuildDEAL()
	{
	return this;
	}

MSdpElementBuilder* MIgnorer::StartListL()
	{
	return this;
	}

MSdpElementBuilder* MIgnorer::EndListL()
	{
	return this;
	}

MSdpElementBuilder* MIgnorer::BuildURLL(const TDesC8& /*aString*/)
	{
	return this;
	}

//MSdpElementBuilder* MIgnorer::BuildEncodedL(const TDesC8& /*aString*/)
//	{
//	return this;
//	}
