/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* Provides the base class for CEmsInformationElements and a factory with which
* to create Ems Elements.
*
*/


/**
 *  @file
 *  @internalComponent
 *  Defines  CEmsInformationElement  class
 */


#ifndef __EMSInformationElement__
#define __EMSInformationElement__

#include <gsmuelem.h>

/**
 *  @internalComponent
 */
#define SHIFT_BOOL_LEFT(a,x) ((TUint8) ((a ? 1 : 0) <<x))
/**
 *  @internalComponent
 */
#define BOOL_AT(a,x)		 ((TBool) ((a >> x) & 0x01))

/**
 *  @internalComponent
 */
#define HI_BYTE(a)	(TUint8) ((a&0xFF00) >> 8)
/**
 *  @internalComponent
 */
#define LO_BYTE(a)	(TUint8) (a&0x00FF)



class CEmsInformationElement : public CSmsInformationElement
/**
 *  Abstract base class used to allow ems objects to be passed around, and to be
 *  handled as a generic group of objects.
 *  @internalComponent
 */
	{
	friend class CEmsFactory;

public:
	// Externally available and exported functions
	IMPORT_C virtual CEmsInformationElement* DuplicateL() const = 0;
	IMPORT_C void SetStartPosition(TUint aValue);
	IMPORT_C TUint StartPosition() const;

	// Length of the generated encoded information element, INCLUDING
	// the IE header
	IMPORT_C TInt Length() const;

public:
	// Encodes the CSmsInformationElement part and decodes to from an
	// external CSmsInformationElement.
	IMPORT_C void EncodeInformationElementL();
	static CEmsInformationElement* DecodeInformationElementL(const CSmsInformationElement& aSmsIe);

	// Serialisation operations
	void ExternalizeL(RWriteStream& aStream) const;
	static CEmsInformationElement* InternalizeL(RReadStream& aStream);

protected:
	// constructor which takes an ems id and a boolean indicating whether or
	// not the start position is encoded in the pdu (true by default,
	// but needs to false for UserPrompt
	inline CEmsInformationElement(TSmsId aId, TBool aStartPosEncoded = ETrue);
	void CopyL(const CEmsInformationElement& aSrc);

	// Encodes and decodes the body of an information element, EXCLUDING the
	// start position. The boolean flag indicates whether or not it is for
	// serialisation or PDU transmission. This impacts in particular the
	// EmsFormatIE object, which stores the length in 16-bits if serialised
	// but only 8-bits when encoded.
	virtual void EncodeBodyL(TPtr8 aPtr, TBool aIsForSerialisation) const = 0;
	virtual void DecodeBodyL(const TPtrC8 aPtr, TBool aIsFromSerialisation) = 0;

	// This virtual function returns the length of object body when it is to
	// be serialised - note that this is generally the same as the encoded
	// body length, except for the Format object
	virtual TInt SerialisedBodyLength() const;

protected:
	// The start position in relation to the entire text
	TUint iStartPosition;

	// Length of the encoded body, EXCLUDING the start position and IE header
	TUint iEncodedBodyLength;

private:
	// boolean to indicate if the start position needs to be encoded in the
	// pdu
	const TBool iStartPosEncoded;

protected:
	// the scope of the following functions of CSmsInformationElement have
	// been downgraded to protected to prevent them from being used when
	// accessed as an EMS element. The point of transition from being
	// a CEmsInformationElement to a CSmsInformation element should occur
	// in the EncodeInformationElement function
	using CSmsInformationElement::Data;
	using CSmsInformationElement::EncodeL;
	using CSmsInformationElement::DecodeL;
	};


class CEmsFactory : public CBase
/**
 *  Factory class used to create an EMS object from the given data.
 *  @internalComponent
 */
	{
public:
	static TBool Supported(TSmsId aId);

	// Creates an information element with a given ID
	static CEmsInformationElement* CreateIeL(TSmsId aId);
	static void InternalizeL(RPointerArray<CEmsInformationElement>& aObjectStore, RReadStream& aStream);
	static void ExternalizeL(RPointerArray<CEmsInformationElement>& aObjectStore, RWriteStream& aStream);
	static CEmsInformationElement* CreateReceivedEmsIEL(const CSmsInformationElement& aIE,TInt aBaseAddr);
	};

#include <emsinformationelement.inl>
#endif // __EMSInformationElement__
