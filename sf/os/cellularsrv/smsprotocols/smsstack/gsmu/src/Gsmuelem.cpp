// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements the different PDU elements
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "GsmuelemTraces.h"
#endif

#include <gsmuelem.h>
#include <gsmumsg.h>
#include "Gsmumain.h"
#include <gsmusar.h>
#include "gsmupriv.h"
#include <s32strm.h>
#include <etelmm.h>
#include <gsmuetel.h>
#include <exterror.h>
#include <emsinformationelement.h>
#include <emsformatie.h>
#include <charconv.h>

/**
 *  Question Mark character.
 */
const TInt  KReplacementCharacter = 0x003f;


/**
 *  Constructs the object with a descriptor. The extraction mark and next character members are initialised to point to the start of the string.
 *  
 *  @param aSource Descriptor to be assigned by reference
 */
TGsmuLex8::TGsmuLex8(const TDesC8& aSource): TLex8(aSource)
	{
	//NOP
	} // TGsmuLex8::TGsmuLex8


/**
 *  Gets the next character in the string and increments the next character position.
 *  
 *  @return The next character in the string
 *  @leave Leaves with KErrGsmuDecoding if at end of string
 */
TUint8 TGsmuLex8::GetL()
	{
	if (Eos())
		LeaveL();

	return static_cast<TUint8>(Get());
	} // TGsmuLex8::GetL


/**
 *  Increments the next character position by aNumber
 *  
 *  @param aNumber The number of characters to increment the next character position by.
 *  @leave If the increment puts the next character position before the start or beyond the end of the string, the function leaves with KErrGsmuDecoding.
 */
void TGsmuLex8::IncL(TInt aNumber)
	{
	if (aNumber != 0)
		{
		if (aNumber < 0 || Eos() || Remainder().Length() < aNumber)
			LeaveL();
		else
			Inc(aNumber);
		}
	} // TGsmuLex8::IncL


/**
 *  Checks there are enough remaining characters, then returns a TPtrC containing the next aLength characters. Does not increment the current position.
 *  
 *  @param aLength Number of characters to return
 *  @return Remainder().Left(aLength)
 *  @leave Leaves with KErrGsmuDecoding if there are insufficient characters remaining
 */
TPtrC8 TGsmuLex8::NextWithNoIncL(TInt aLength) const
	{
	return NextWithNoIncL(aLength, EFalse);
	}

/**
 * Checks there are enough remaining characters, then returns a TPtrC containing the next aLength characters. Does not increment the current position.
 *
 * @param aLength Number of characters to return
 * @param aAcceptTruncation ETrue if a mismatch between length indicator and actual length is acceptable. EFalse otherwise.
 * @return Remainder().Left(aLength) if aAcceptTruncation is EFalse. Remainder() otherwise.
 * @leave Leaves with KErrGsmuDecoding if there are insufficient characters remaining, unless aAcceptTruncation is ETrue.
 */
TPtrC8 TGsmuLex8::NextWithNoIncL(TInt aLength, TBool aAcceptTruncation) const
	{
	const TPtrC8 remainder(Remainder());

	if ( (aLength < 0) || (remainder.Length()< aLength && !aAcceptTruncation) )
		{
		LeaveL();
		}
	
  if (!aAcceptTruncation)
		{
		return Remainder().Left(aLength);
		}
	// aAcceptTruncation is ETrue
	else if (remainder.Length() < aLength)
		{
		return Remainder();
		}
	// otherwise, no need for truncation:
  
	return Remainder().Left(aLength);
	}


/**
 *  Calls TGsmuLeax8::NextWithNoIncL then increments the next character position by aLength
 *  
 *  @param aLength Number of characters to return and increment the next character position
 *  @return Remainder().Left(aLength)
 *  @leave If the increment puts the next character position before the start or beyond the end of the string, the function leaves with KErrGsmuDecoding.
 */
TPtrC8 TGsmuLex8::NextAndIncL(TInt aLength)
	{
	const TPtrC8 next(NextWithNoIncL(aLength));
	IncL(aLength);
	return next;
	} // TGsmuLeax8::NextWithNoIncL


const TSmsOctet& TSmsOctet::operator = (TInt aValue)
	{
	iValue = static_cast<TUint8>(aValue);
	return *this;
	} // TSmsOctet::operator


TUint8* TSmsOctet::EncodeL(TUint8* aPtr) const
	{
	*aPtr=iValue;
	return aPtr+1;
	} // TSmsOctet::EncodeL


TSmsFirstOctet::TSmsFirstOctet(TInt aValue):
	TSmsOctet(aValue)
	{
	} // TSmsFirstOctet::TSmsFirstOctet


const TSmsFirstOctet& TSmsFirstOctet::operator = (TInt aValue)
	{
	TSmsOctet::operator = (aValue);
	return *this;
	} // TSmsFirstOctet::operator


TSmsFailureCause::TSmsFailureCause():
	TSmsOctet(ESmsErrorUnspecified)
	{
	} // TSmsFailureCause::TSmsFailureCause


TSmsStatus::TSmsStatus():
	TSmsOctet(ESmsShortMessageReceivedBySME)
	{
	} // TSmsStatus::TSmsStatus


CSmsCommandData* CSmsCommandData::NewL(TSmsFirstOctet& aFirstOctet)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_NEWL_1, "CSmsCommandData::NewL()");

	CSmsCommandData* commanddata=new(ELeave) CSmsCommandData(aFirstOctet);
	CleanupStack::PushL(commanddata);
	TPtrC8 ptr;
	commanddata->SetDataL(ptr);
	CleanupStack::Pop();
	return commanddata;
	} // CSmsCommandData::NewL


CSmsCommandData::~CSmsCommandData()
	{
	iInformationElementArray.ResetAndDestroy();
	delete iBuffer;
	} // CSmsCommandData::NewL


/**
 *  Duplicates this CSmsCommandData object.
 * 
 *  @return  Pointer to the newly created CSmsCommandData object.
 */
CSmsCommandData* CSmsCommandData::DuplicateL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_DUPLICATEL_1, "CSmsCommandData::DuplicateL()");

	CSmsCommandData*  smsCommandData = CSmsCommandData::NewL(iFirstOctet);
	CleanupStack::PushL(smsCommandData);

	smsCommandData->SetDataL(Data());

	for (TInt ie = 0;  ie < iInformationElementArray.Count();  ie++)
		{
		smsCommandData->AddInformationElementL(iInformationElementArray[ie]->Identifier(),
											   iInformationElementArray[ie]->Data());
		}

	CleanupStack::Pop(smsCommandData);

	return smsCommandData;
	} // CSmsCommandData::DuplicateL


CSmsInformationElement& CSmsCommandData::InformationElement(TInt aIndex) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_INFORMATIONELEMENT_1, "CSmsCommandData::InformationElement()");

	CSmsInformationElement* ie=iInformationElementArray[aIndex];
	return *ie;
	} // CSmsCommandData::InformationElement


CSmsInformationElement*& CSmsCommandData::InformationElementPtr(TInt aIndex)
    {
    // Ignore in code coverage - not used in SMS stack and not exported
    // but cannot be removed as impacts public header.
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_INFORMATIONELEMENTPTR_1, "CSmsCommandData::InformationElementPtr()");
    return iInformationElementArray[aIndex];
    BULLSEYE_RESTORE
    }

TBool CSmsCommandData::InformationElementIndex(CSmsInformationElement::TSmsInformationElementIdentifier aIdentifier,
		TInt& aIndex) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_INFORMATIONELEMENTINDEX_1, "CSmsCommandData::InformationElementIndex()");

	TBool found=EFalse;
	TInt count=NumInformationElements();
	for (TInt i=0; (!found) && (i<count); i++)
		if (InformationElement(i).Identifier()==aIdentifier)
			{
			found=ETrue;
			aIndex=i;
			}
	return found;
	} // CSmsCommandData::InformationElementIndex


void CSmsCommandData::AddInformationElementL(const TSmsId aIdentifier,const TDesC8& aData)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_ADDINFORMATIONELEMENTL_1, "CSmsCommandData::AddInformationElementL()");

	//
	// Currently there is no restriction on how many instances of an information element can be
	// placed in the collection.
	// No restriction will be placed on the number of Special SMS Message Indications that can be
	// added in order to maintain FC.
	// (cf CSmsUserData::AddInformationElementL(const TSmsId aIdentifier,const TDesC8& aData);
	//
	CSmsInformationElement* informationelement=CSmsInformationElement::NewL(aIdentifier,aData);
	CleanupStack::PushL(informationelement);
	iInformationElementArray.AppendL(informationelement);
	CleanupStack::Pop();
	if (NumInformationElements()>=1)
		SetHeaderPresent(ETrue);
	} // CSmsCommandData::AddInformationElementL


void CSmsCommandData::RemoveInformationElement(TInt aIndex)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_REMOVEINFORMATIONELEMENT_1, "CSmsCommandData::RemoveInformationElement()");
	// Since iInformationElementArray[aIndex] is removed from iInformationElementArray, no double free issue.
	// coverity[double_free]
	delete iInformationElementArray[aIndex];
	iInformationElementArray[aIndex] = NULL;
	iInformationElementArray.Delete(aIndex);

	if (NumInformationElements()==0)
		{
		SetHeaderPresent(EFalse);
		}
	} // CSmsCommandData::RemoveInformationElement


TPtrC8 CSmsCommandData::Data() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_DATA_1, "CSmsCommandData::Data()");

	TPtrC8 ptr;
	ptr.Set(iBuffer->Des());
	return ptr;
	} // CSmsCommandData::Data


void CSmsCommandData::SetDataL(const TDesC8& aData)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_SETDATAL_1, "CSmsCommandData::SetDataL()");

	TInt length=aData.Length();
	__ASSERT_DEBUG(length<=KSmsMaxDataSize,Panic(KGsmuPanicCommandDataLengthTooLong));
	HBufC8* buffer=HBufC8::NewL(length);
	delete iBuffer;
	iBuffer=buffer;
	iBuffer->Des().SetLength(length);
	iBuffer->Des().Copy(aData);
	} // CSmsCommandData::SetDataL


TUint8* CSmsCommandData::EncodeL(TUint8* aPtr) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_ENCODEL_1, "CSmsCommandData::EncodeL()");

	__ASSERT_DEBUG(iBuffer->Length()<=MaxDataLength(),Panic(KGsmuPanicCommandDataBufferTooLong));
	TSmsOctet datalength=iBuffer->Length()+TSmsOctet(TotalHeaderLengthInUDLUnits());
	aPtr=datalength.EncodeL(aPtr);
	TPtr8 ptr((TUint8*) aPtr,datalength);

    if (HeaderPresent())
        {
		TSmsOctet headerLength(HeaderLength());
		aPtr=headerLength.EncodeL(aPtr);
		for (TInt i=0; i<NumInformationElements(); i++)
		    {
			aPtr=iInformationElementArray[i]->EncodeL(aPtr);
		    }
        }

	ptr.Copy(iBuffer->Des());
	aPtr+=TInt(iBuffer->Length());
	return aPtr;
	} // CSmsCommandData::EncodeL


void CSmsCommandData::DecodeL(TGsmuLex8& aPdu)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_DECODEL_1, "CSmsCommandData::DecodeL()");

	iInformationElementArray.ResetAndDestroy();
	const TBool headerPresent=HeaderPresent();
	TSmsOctet dataLength;
	dataLength.DecodeL(aPdu);

	if (headerPresent)
		{
		TSmsOctet headerLength;
		headerLength.DecodeL(aPdu);
		if ((1+headerLength)>KSmsMaxDataSize)
			User::Leave(KErrGsmSMSTpduNotSupported);
		while (HeaderLength()<headerLength)
			{
			CSmsInformationElement* informationElement=CSmsInformationElement::NewL();
			CleanupStack::PushL(informationElement);
			informationElement->DecodeL(aPdu);
			iInformationElementArray.AppendL(informationElement);
			CleanupStack::Pop(informationElement);
			}
		if (HeaderLength()!=headerLength)
			User::Leave(KErrGsmSMSTpduNotSupported);
		}
	const TInt totalHeaderLength=TotalHeaderLengthInUDLUnits();
	const TInt totalDataLength=dataLength-totalHeaderLength;
	const TPtrC8 next(aPdu.NextAndIncL(totalDataLength));
	SetDataL(next);
	} // CSmsCommandData::DecodeL


void CSmsCommandData::InternalizeL(RReadStream& aStream)
	{
	iInformationElementArray.ResetAndDestroy();
	TInt numInformationElements=aStream.ReadInt32L();
	for (TInt i=0; i<numInformationElements; i++)
		{
		CSmsInformationElement* informationElement=CSmsInformationElement::NewL();
		CleanupStack::PushL(informationElement);
		aStream >> *informationElement;
		iInformationElementArray.AppendL(informationElement);
		CleanupStack::Pop();
		}
	HBufC8* buffer=HBufC8::NewL(aStream,KSmsMaxDataSize);
	delete iBuffer;
	iBuffer=buffer;
	} // CSmsCommandData::InternalizeL


void CSmsCommandData::ExternalizeL(RWriteStream& aStream) const
	{
	TInt numInformationElements=iInformationElementArray.Count();
	aStream.WriteInt32L(numInformationElements);
	for (TInt i=0; i<numInformationElements; i++)
		aStream << *iInformationElementArray[i];
	aStream << *iBuffer;
	} // CSmsCommandData::ExternalizeL


CSmsCommandData::CSmsCommandData(TSmsFirstOctet& aFirstOctet):
	iFirstOctet(aFirstOctet),
	iInformationElementArray(8)
	{
	} // CSmsCommandData::CSmsCommandData


TInt CSmsCommandData::HeaderLength() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_HEADERLENGTH_1, "CSmsCommandData::HeaderLength()");

	TInt headerLength=0;
	for (TInt i=0; i<NumInformationElements(); i++)
		headerLength+=iInformationElementArray[i]->Length();
	return headerLength;
	} // CSmsCommandData::HeaderLength


TInt CSmsCommandData::TotalHeaderLengthInUDLUnits() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_TOTALHEADERLENGTHINUDLUNITS_1, "CSmsCommandData::TotalHeaderLengthInUDLUnits()");

	if (iInformationElementArray.Count()==0)
		return 0;
	else
		return (HeaderLength()+1);   // +1 stands for  UDHL
	} // CSmsCommandData::TotalHeaderLengthInUDLUnits


TBool CSmsCommandData::HeaderPresent() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_HEADERPRESENT_1, "CSmsCommandData::HeaderPresent()");

	return (iFirstOctet&TSmsFirstOctet::ESmsUDHIMask)==TSmsFirstOctet::ESmsUDHIHeaderPresent;
	} // CSmsCommandData::HeaderPresent


void CSmsCommandData::SetHeaderPresent(TBool aHeaderPresent)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMANDDATA_SETHEADERPRESENT_1, "CSmsCommandData::SetHeaderPresent()");

	iFirstOctet=aHeaderPresent? (iFirstOctet&(~TSmsFirstOctet::ESmsUDHIMask))|TSmsFirstOctet::ESmsUDHIHeaderPresent: (iFirstOctet&(~TSmsFirstOctet::ESmsUDHIMask))|TSmsFirstOctet::ESmsUDHIHeaderNotPresent;
	} // CSmsCommandData::SetHeaderPresent


TSmsParameterIndicator::TSmsParameterIndicator():
	TSmsOctet(ESmsPIDProtocolIdentifierPresent|ESmsPIDUserDataPresent|ESmsPIDDataCodingSchemePresent)
	{
	} // TSmsParameterIndicator::TSmsParameterIndicator


TSmsCommandType::TSmsCommandType():
	TSmsOctet(ESmsCommandTypeEnquiry)
	{
	} // TSmsCommandType::TSmsCommandType


TSmsProtocolIdentifier::TSmsProtocolIdentifier():
	TSmsOctet((TInt)ESmsPIDTelematicInterworking|(TInt)ESmsNoTelematicDevice)
	{
	} // TSmsProtocolIdentifier::TSmsProtocolIdentifier


TSmsProtocolIdentifier::TSmsTelematicDeviceIndicator TSmsProtocolIdentifier::TelematicDeviceIndicator() const
	{
	__ASSERT_DEBUG(PIDType()==ESmsPIDTelematicInterworking,Panic(KGsmuPanicNoTelematicInterworking));
	return (TSmsTelematicDeviceIndicator) (iValue&EPIDTelematicDeviceIndicatorMask);
	} // TSmsProtocolIdentifier::TSmsTelematicDeviceIndicator


void TSmsProtocolIdentifier::SetTelematicDeviceIndicator(TSmsTelematicDeviceIndicator aIndicator)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSPROTOCOLIDENTIFIER_SETTELEMATICDEVICEINDICATOR_1, "TSmsProtocolIdentifier::SetTelematicDeviceIndicator()");

	__ASSERT_DEBUG(PIDType()==ESmsPIDTelematicInterworking,Panic(KGsmuPanicNoTelematicInterworking));

	//iValue=(TUint8) ((iValue&ESmsPIDTypeMask)|aIndicator);
	iValue=(TUint8) ((iValue&(~EPIDTelematicDeviceIndicatorMask))|aIndicator);
	} // TSmsProtocolIdentifier::SetTelematicDeviceIndicator


TSmsProtocolIdentifier::TSmsTelematicDeviceType TSmsProtocolIdentifier::TelematicDeviceType() const
	{
	__ASSERT_DEBUG(TelematicDeviceIndicator()==ESmsTelematicDevice,Panic(KGsmuPanicNoTelematicDevice));
	return (TSmsTelematicDeviceType) (iValue&ESmsTelematicDeviceTypeMask);
	} // TSmsProtocolIdentifier::TSmsTelematicDeviceType


void TSmsProtocolIdentifier::SetTelematicDeviceType(TSmsTelematicDeviceType aDeviceType)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSPROTOCOLIDENTIFIER_SETTELEMATICDEVICETYPE_1, "TSmsProtocolIdentifier::SetTelematicDeviceType()");

	__ASSERT_DEBUG(TelematicDeviceIndicator()==ESmsTelematicDevice,Panic(KGsmuPanicNoTelematicDevice));
	iValue=(TUint8) ((iValue&(~ESmsTelematicDeviceTypeMask))|aDeviceType);
	} // TSmsProtocolIdentifier::SetTelematicDeviceType


TInt TSmsProtocolIdentifier::ShortMessageALProtocol() const
    {
    // Ignore in code coverage - not used in SMS stack and not exported
    // but cannot be removed as impacts public header.
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSPROTOCOLIDENTIFIER_SHORTMESSAGEALPROTOCOL_1, "TSmsProtocolIdentifier::ShortMessageALProtocol()");
    
    __ASSERT_DEBUG(TelematicDeviceIndicator()==ESmsNoTelematicDevice,Panic(KGsmuPanicNoTelematicDevice));
    return (TSmsShortMessageALProtocol) (iValue&ESmsShortMessageALProtocolMask);
    BULLSEYE_RESTORE
    }

void TSmsProtocolIdentifier::SetShortMessageALProtocol(TSmsShortMessageALProtocol aProtocol)
    {
    // Ignore in code coverage - not used in SMS stack and not exported
    // but cannot be removed as impacts public header.
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSPROTOCOLIDENTIFIER_SETSHORTMESSAGEALPROTOCOL_1, "TSmsProtocolIdentifier::SetShortMessageALProtocol()");
    
    __ASSERT_DEBUG(TelematicDeviceIndicator()==ESmsNoTelematicDevice,Panic(KGsmuPanicNoTelematicDevice));
    iValue=(TUint8) ((iValue&(~ESmsShortMessageALProtocolMask))|aProtocol);
    BULLSEYE_RESTORE
    }

TInt TSmsProtocolIdentifier::ShortMessageType() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSPROTOCOLIDENTIFIER_SHORTMESSAGETYPE_1, "TSmsProtocolIdentifier::ShortMessageType()");

	__ASSERT_DEBUG(PIDType()==ESmsPIDShortMessageType,Panic(KGsmuPanicNoShortMessageType));
	return (TSmsShortMessageType) (iValue&ESmsShortMessageTypeMask);
	} // TSmsProtocolIdentifier::ShortMessageType


void TSmsProtocolIdentifier::SetShortMessageType(TSmsShortMessageType aShortMessageType)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSPROTOCOLIDENTIFIER_SETSHORTMESSAGETYPE_1, "TSmsProtocolIdentifier::SetShortMessageType()");

	__ASSERT_DEBUG(PIDType()==ESmsPIDShortMessageType,Panic(KGsmuPanicNoShortMessageType));
	//iValue=(TUint8) ((iValue&(~ESmsPIDTypeMask))|aShortMessageType);
	iValue=(TUint8) ((iValue&(~ESmsShortMessageTypeMask))|aShortMessageType);
	} // TSmsProtocolIdentifier::SetShortMessageType


TSmsDataCodingScheme::TSmsDataCodingScheme():
	TSmsOctet((TInt)ESmsDCSTextUncompressedWithNoClassInfo|(TInt)ESmsAlphabet7Bit|(TInt)ESmsClass0)
	// Constructor is expected to set the octet = 0, This is needed by
	// CSmsDeliverReport::DecodeL, CSmsSubmitReport::DecodeL and CSmsStatusReport::DecodeL, per
	// 23.040 v6.5.0 section 9.2.3.27.
	{
	} // CSmsDeliverReport::DecodeL


TBool TSmsDataCodingScheme::TextCompressed() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_TEXTCOMPRESSED_1, "TSmsDataCodingScheme::TextCompressed()");

	TInt bits7to4=Bits7To4();
	return (bits7to4==ESmsDCSTextCompressedWithNoClassInfo)    || (bits7to4==ESmsDCSTextCompressedWithClassInfo)      ||
           (bits7to4==ESmsDCSAutoDelNoClassInfoCompressedText) || (bits7to4==ESmsDCSAutoDelClassInfoTextCompressedText);
	} // TSmsDataCodingScheme::TextCompressed


void TSmsDataCodingScheme::SetTextCompressed(TBool aCompressed)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_SETTEXTCOMPRESSED_1, "TSmsDataCodingScheme::SetTextCompressed()");

	TInt bits7to4=Bits7To4();
	if (aCompressed)
		{
		switch (bits7to4)
			{
			case (ESmsDCSTextUncompressedWithNoClassInfo):
				{
				iValue=(TUint8) (ESmsDCSTextCompressedWithNoClassInfo|(iValue&(~ESmsDCSBits7To4Mask)));
				break;
				}
			case (ESmsDCSAutoDelNoClassInfoUncompressedText):
				{
				iValue=(TUint8) (ESmsDCSAutoDelNoClassInfoCompressedText|(iValue&(~ESmsDCSBits7To4Mask)));
				break;
				}
			case (ESmsDCSAutoDelClassInfoUncompressedText):
				{
				iValue=(TUint8) (ESmsDCSAutoDelClassInfoTextCompressedText|(iValue&(~ESmsDCSBits7To4Mask)));
				break;
				}
			case (ESmsDCSTextUncompressedWithClassInfo):
			case (ESmsDCSTextUncompressed7BitOr8Bit):
				{
				iValue=(TUint8) (ESmsDCSTextCompressedWithClassInfo|(iValue&(~ESmsDCSBits7To4Mask)));
				break;
				}
			case (ESmsDCSTextCompressedWithNoClassInfo):
			case (ESmsDCSTextCompressedWithClassInfo):
			case (ESmsDCSAutoDelNoClassInfoCompressedText):
			case (ESmsDCSAutoDelClassInfoTextCompressedText):
				break;
			default:
                break;
                // has to be tested, what happens in this default case
				//Panic(KGsmuPanicNotSupportedWithDCSBits7To4);
			}
		}
	else
		{
		switch (bits7to4)
			{
			case (ESmsDCSTextCompressedWithNoClassInfo):
				{
				iValue=(TUint8) (ESmsDCSTextUncompressedWithNoClassInfo|(iValue&(~ESmsDCSBits7To4Mask)));
				break;
				}
			case (ESmsDCSTextCompressedWithClassInfo):
				{
				iValue=(TUint8) (ESmsDCSTextUncompressedWithClassInfo|(iValue&(~ESmsDCSBits7To4Mask)));
				break;
				}
			case (ESmsDCSAutoDelNoClassInfoCompressedText):
				{
				iValue=(TUint8) (ESmsDCSAutoDelNoClassInfoUncompressedText|(iValue&(~ESmsDCSBits7To4Mask)));
				break;
				}
			case (ESmsDCSAutoDelClassInfoTextCompressedText):
				{
				iValue=(TUint8) ( ESmsDCSAutoDelClassInfoUncompressedText|(iValue&(~ESmsDCSBits7To4Mask)));
				break;
				}
			case (ESmsDCSTextUncompressedWithNoClassInfo):
			case (ESmsDCSTextUncompressedWithClassInfo):
            case (ESmsDCSAutoDelClassInfoUncompressedText):
            case (ESmsDCSAutoDelNoClassInfoUncompressedText):
			case (ESmsDCSTextUncompressed7BitOr8Bit):
			default:
				{
				}
			}
		}
	} // TSmsDataCodingScheme::SetTextCompressed


TSmsDataCodingScheme::TSmsAlphabet TSmsDataCodingScheme::Alphabet() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_ALPHABET_1, "TSmsDataCodingScheme::TSmsAlphabet()");

	TInt bits7to4=Bits7To4();
	TInt alphabet=ESmsAlphabet7Bit;
	switch (bits7to4)
		{
		case (ESmsDCSTextUncompressedWithNoClassInfo):
		case (ESmsDCSTextUncompressedWithClassInfo):
		case (ESmsDCSTextCompressedWithNoClassInfo):  //  Alphabet not used in these cases
		case (ESmsDCSTextCompressedWithClassInfo):
		case (ESmsDCSAutoDelNoClassInfoUncompressedText):
		case (ESmsDCSAutoDelClassInfoUncompressedText):
		case (ESmsDCSAutoDelNoClassInfoCompressedText):
		case (ESmsDCSAutoDelClassInfoTextCompressedText):
			{
			alphabet=iValue&ESmsAlphabetMask;
			break;
			}
		case (ESmsDCSTextUncompressed7BitOr8Bit):
			{
			alphabet=iValue&ESmsAlphabet8Bit;  //  N.B. only one bit used to code alphabet
			break;
			}
		case (ESmsDCSMessageWaitingIndicationDiscardMessage):
		case (ESmsDCSMessageWaitingIndication7Bit):
			{
			alphabet=ESmsAlphabet7Bit;
			break;
			}
		case (ESmsDCSMessageWaitingIndicationUCS2):
			{
			alphabet=ESmsAlphabetUCS2;
			break;
			}
		default:
		    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_ALPHABET_2, "TSmsDataCodingScheme::Alphabet() WARNING! default case has been reached");
            break;
 		}
	return (TSmsAlphabet) alphabet;
	} // TSmsDataCodingScheme::TSmsAlphabet

void TSmsDataCodingScheme::SetAlphabet(TSmsAlphabet aAlphabet)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_SETALPHABET_1, "TSmsDataCodingScheme::SetAlphabet()");

	TInt bits7to4=Bits7To4();
	switch (bits7to4)
		{
		case (ESmsDCSTextUncompressedWithNoClassInfo):
		case (ESmsDCSTextUncompressedWithClassInfo):
		case (ESmsDCSTextCompressedWithNoClassInfo):  //  Alphabet not used in these cases
		case (ESmsDCSTextCompressedWithClassInfo):
		case (ESmsDCSAutoDelNoClassInfoUncompressedText):
		case (ESmsDCSAutoDelClassInfoUncompressedText):
		case (ESmsDCSAutoDelNoClassInfoCompressedText):
		case (ESmsDCSAutoDelClassInfoTextCompressedText):
			{
			iValue=(TUint8) ((iValue&(~ESmsAlphabetMask))|aAlphabet);
			break;
			}
		case (ESmsDCSTextUncompressed7BitOr8Bit):
			{
			if ((aAlphabet==ESmsAlphabet7Bit) || (aAlphabet==ESmsAlphabet8Bit))
				{
				iValue=(TUint8) ((iValue&(~ESmsAlphabet8Bit))|aAlphabet);  //  N.B. only one bit used to code alphabet
				}
			break;
			}
		case (ESmsDCSMessageWaitingIndicationDiscardMessage):
			{
            if (aAlphabet!=ESmsAlphabet7Bit)
                {
                OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_SETALPHABET_2, "TSmsDataCodingScheme::SetAlphabet() WARNING! Not Supported With Discard Message  [Bits7To4=%d], [aAlphabet=%d]", bits7to4, aAlphabet);
                }
			break;
			}
		case (ESmsDCSMessageWaitingIndication7Bit):
			{
 			if (aAlphabet==ESmsAlphabetUCS2)
 			    {
				iValue=(TUint8) (ESmsDCSMessageWaitingIndicationUCS2|(iValue&(~ESmsDCSBits7To4Mask)));
 			    }
 			else 
                {
        		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_SETALPHABET_3, "TSmsDataCodingScheme::SetAlphabet() WARNING! Not Supported With Discard Message  [Bits7To4=%d], [aAlphabet=%d]", bits7to4, aAlphabet);
			    }	     
			break;
			}
		case (ESmsDCSMessageWaitingIndicationUCS2):
			{
  			if (aAlphabet==ESmsAlphabet7Bit)
			    {
				iValue=(TUint8) (ESmsDCSMessageWaitingIndication7Bit|(iValue&(~ESmsDCSBits7To4Mask)));
			    }
			else 
			    {
				OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_SETALPHABET_4, "TSmsDataCodingScheme::SetAlphabet() WARNING! Not Supported With Discard Message  [Bits7To4=%d], [aAlphabet=%d]", bits7to4, aAlphabet);
			    }
			break;
			}
		default:
		    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_SETALPHABET_5, "TSmsDataCodingScheme::SetAlphabet() WARNING! default case has been reached");
            break;
      		}
	} // TSmsDataCodingScheme::SetAlphabet


TBool TSmsDataCodingScheme::Class(TSmsClass& aClass) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_CLASS_1, "TSmsDataCodingScheme::Class()");

	switch (Bits7To4())
		{
		case (ESmsDCSTextUncompressedWithClassInfo):
		case (ESmsDCSTextCompressedWithClassInfo):
		case (ESmsDCSAutoDelClassInfoUncompressedText):
		case (ESmsDCSAutoDelClassInfoTextCompressedText):
		case (ESmsDCSTextUncompressed7BitOr8Bit):
			aClass=(TSmsClass) (iValue&ESmsClassMask);
			return ETrue;
		default:
			return EFalse;
		}
	} // TSmsDataCodingScheme::Class


void TSmsDataCodingScheme::SetClass(TBool aClassDefined,TSmsDataCodingScheme::TSmsClass aClass)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_SETCLASS_1, "TSmsDataCodingScheme::SetClass()");

	TInt bits7to4=Bits7To4();
	if (aClassDefined)
		{
		switch (bits7to4)
			{
			case (ESmsDCSTextUncompressedWithNoClassInfo):
				{
				iValue=(TUint8) (ESmsDCSTextUncompressedWithClassInfo|(iValue&ESmsAlphabetMask|aClass));
				break;
				}
			case (ESmsDCSTextCompressedWithNoClassInfo):
				{
				iValue=(TUint8) (ESmsDCSTextCompressedWithClassInfo|(iValue&ESmsAlphabetMask|aClass));
				break;
				}
			case (ESmsDCSAutoDelNoClassInfoUncompressedText):
				{
				iValue=(TUint8) (ESmsDCSAutoDelClassInfoUncompressedText|(iValue&ESmsAlphabetMask|aClass));
				break;
				}
			case (ESmsDCSAutoDelNoClassInfoCompressedText):
				{
				iValue=(TUint8) (ESmsDCSAutoDelClassInfoTextCompressedText|(iValue&ESmsAlphabetMask|aClass));
				break;
				}
			case (ESmsDCSTextUncompressedWithClassInfo):
			case (ESmsDCSTextCompressedWithClassInfo):
			case (ESmsDCSAutoDelClassInfoUncompressedText):
			case (ESmsDCSAutoDelClassInfoTextCompressedText):
			case (ESmsDCSTextUncompressed7BitOr8Bit):

				{
				iValue=(TUint8) (iValue&(~ESmsClassMask)|aClass);
				break;
				}
			default:
		        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_SETCLASS_2, "WARNING! default case has been reached");
                break;
			}
		}
	else
		{
		switch (bits7to4)
			{
			case (ESmsDCSTextUncompressedWithClassInfo):
				{
				iValue=(TUint8) (ESmsDCSTextUncompressedWithNoClassInfo|(iValue&ESmsAlphabetMask)|aClass);
				break;
				}
			case (ESmsDCSTextCompressedWithClassInfo):
				{
				iValue=(TUint8) (ESmsDCSTextCompressedWithNoClassInfo|(iValue&ESmsAlphabetMask)|aClass);
				break;
				}
			case (ESmsDCSAutoDelClassInfoUncompressedText):
				{
				iValue=(TUint8) (ESmsDCSAutoDelNoClassInfoUncompressedText|(iValue&ESmsAlphabetMask)|aClass);
				break;
				}
			case (ESmsDCSAutoDelClassInfoTextCompressedText):
				{
				iValue=(TUint8) (ESmsDCSAutoDelNoClassInfoCompressedText|(iValue&ESmsAlphabetMask)|aClass);
				break;
				}
			case (ESmsDCSTextUncompressed7BitOr8Bit):
				{
				iValue=(TUint8) (ESmsDCSTextUncompressedWithNoClassInfo|(iValue&ESmsAlphabetMask)|aClass);
				break;
				}
			case (ESmsDCSTextUncompressedWithNoClassInfo):
			case (ESmsDCSTextCompressedWithNoClassInfo):
			case (ESmsDCSAutoDelNoClassInfoUncompressedText):
			case (ESmsDCSAutoDelNoClassInfoCompressedText):
			case (ESmsDCSMessageWaitingIndicationDiscardMessage):
			case (ESmsDCSMessageWaitingIndication7Bit):
			case (ESmsDCSMessageWaitingIndicationUCS2):
			default:
				{
				}
			}
		}
	} // TSmsDataCodingScheme::SetClass


TSmsDataCodingScheme::TSmsIndicationState TSmsDataCodingScheme::IndicationState() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_INDICATIONSTATE_1, "TSmsDataCodingScheme::IndicationState()");

	TInt bits7to4=Bits7To4();
	TSmsIndicationState state=ESmsIndicationInactive;
	switch (bits7to4)
		{
		case (ESmsDCSMessageWaitingIndicationDiscardMessage):
		case (ESmsDCSMessageWaitingIndication7Bit):
		case (ESmsDCSMessageWaitingIndicationUCS2):
			{
			state=(TSmsIndicationState) (iValue&ESmsIndicationStateMask);
			break;
			}
		default:
		    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_INDICATIONSTATE_2, "WARNING! default case has been reached");
            break;
  		}
	return state;
	} // TSmsDataCodingScheme::TSmsIndicationState


void TSmsDataCodingScheme::SetIndicationState(TSmsIndicationState aState)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_SETINDICATIONSTATE_1, "TSmsDataCodingScheme::SetIndicationState()");

	TInt bits7to4=Bits7To4();
	switch (bits7to4)
		{
		case (ESmsDCSMessageWaitingIndicationDiscardMessage):
		case (ESmsDCSMessageWaitingIndication7Bit):
		case (ESmsDCSMessageWaitingIndicationUCS2):
			{
			iValue=(TUint8) (aState | (iValue&(~ESmsIndicationStateMask)));
			break;
			}
		default:
		    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_SETINDICATIONSTATE_2, "TSmsDataCodingScheme::SetIndicationState() WARNING! default case has been reached");
            break;
  		}
	} // TSmsDataCodingScheme::SetIndicationState


TSmsDataCodingScheme::TSmsIndicationType TSmsDataCodingScheme::IndicationType() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_INDICATIONTYPE_1, "TSmsDataCodingScheme::IndicationType()");

	TInt bits7to4=Bits7To4();
	TSmsIndicationType type=ESmsVoicemailMessageWaiting;
	switch (bits7to4)
		{
		case (ESmsDCSMessageWaitingIndicationDiscardMessage):
		case (ESmsDCSMessageWaitingIndication7Bit):
		case (ESmsDCSMessageWaitingIndicationUCS2):
			{
			type=(TSmsIndicationType) (iValue&ESmsIndicationTypeMask);
			break;
			}
		default:
		    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_INDICATIONTYPE_2, "TSmsDataCodingScheme::IndicationType() WARNING default case has been reached");
            break;
		}
	return type;
	} // TSmsDataCodingScheme::TSmsIndicationType


void TSmsDataCodingScheme::SetIndicationType(TSmsIndicationType aType)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_SETINDICATIONTYPE_1, "TSmsDataCodingScheme::SetIndicationType()");

	TInt bits7to4=Bits7To4();
	switch (bits7to4)
		{
		case (ESmsDCSMessageWaitingIndicationDiscardMessage):
		case (ESmsDCSMessageWaitingIndication7Bit):
		case (ESmsDCSMessageWaitingIndicationUCS2):
			{
			iValue=(TUint8) (aType | (iValue&(~ESmsIndicationTypeMask)));
			break;
			}
		default:
		    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSDATACODINGSCHEME_SETINDICATIONTYPE_2, "TSmsDataCodingScheme::SetIndicationType() WARNING! default case has been reached");
            break;
 		}
	} // TSmsDataCodingScheme::SetIndicationType


/**
 *  Allocates and creates a CSmsAlphabetConverter object, specifying an Alphabet
 *  Coding scheme and a Binary flag.
 *  
 *  @param aCharacterSetConverter Pre-initialised character set converter
 *  @param aFs File system handle
 *  @param aSmsAlphabet Data coding scheme alphabet
 *  @param aIsBinary Set to true for WAP or compressed data
 *  @return New CSmsAlphabetConverter object
 *  @capability None
 */
EXPORT_C CSmsAlphabetConverter* CSmsAlphabetConverter::NewLC(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,TSmsDataCodingScheme::TSmsAlphabet aSmsAlphabet,TBool aIsBinary)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSALPHABETCONVERTER_NEWLC_1, "CSmsAlphabetConverter::NewLC()");

	CSmsAlphabetConverter* converter=new (ELeave)CSmsAlphabetConverter(aCharacterSetConverter,aFs,aSmsAlphabet,aIsBinary);
	CleanupStack::PushL(converter);
	converter->ConstructL();
	return converter;
	} // CSmsAlphabetConverter::NewLC


/**
 *  Destructor.
 *  @capability None
 */
EXPORT_C CSmsAlphabetConverter::~CSmsAlphabetConverter()
    {
    delete iConvertedNativeCharacters;
    delete iConvertedUDElements;
    delete iUnconvertedNativeCharacters;
    delete iUnconvertedUDElements;
    } // CSmsAlphabetConverter::NewLC


//
// C'tor - standard stuff
//
CSmsAlphabetConverter::CSmsAlphabetConverter(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,TSmsDataCodingScheme::TSmsAlphabet aSmsAlphabet,TBool aIsBinary)
    : iCharacterSetConverter(aCharacterSetConverter),
    iFs(aFs),
    iSmsAlphabet(aSmsAlphabet),
    iIsBinary(aIsBinary),
    iUnconvertedNativeCharactersPtr(NULL,0),
    iUnconvertedUDElementsPtr(NULL,0)
    {
    } // CSmsAlphabetConverter::CSmsAlphabetConverter


//
// Ensures this is a supported character set if not binary conversion
//
void CSmsAlphabetConverter::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_CONSTRUCTL_1, "CSmsAlphabetConverter::ConstructL()");


	if (!iIsBinary)
		{
		switch (iSmsAlphabet)
			{
			case TSmsDataCodingScheme::ESmsAlphabet7Bit:
			case TSmsDataCodingScheme::ESmsAlphabet8Bit:
			case TSmsDataCodingScheme::ESmsAlphabetUCS2:
				{
				// Supported
				break;
				}
			default:
				{
				// Not supported
				User::Leave(KErrGsmSMSDataCodingSchemeNotSupported);
				break;
				}
			}
		}
	} // CSmsAlphabetConverter::ConstructL


//
// Returns whether the character set converter is invoked.  Provided to allow
// clients to provided efficient converted length calculation where no
// conversion is required.
//
void CSmsAlphabetConverter::ConversionPropertiesL(TSmsAlphabetConversionProperties& aConversionProperties) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_CONVERSIONPROPERTIESL_1, "CSmsAlphabetConverter::ConversionPropertiesL()");


	// Set defaults
	aConversionProperties.iWidthConversion=ESmsAlphabetWidthConversionFixed;
	aConversionProperties.iUDElementsPerNativeCharacter=1;
	// Modify if different
	if (iIsBinary)
		return;
	switch (iSmsAlphabet)
		{
		case TSmsDataCodingScheme::ESmsAlphabet7Bit:
		case TSmsDataCodingScheme::ESmsAlphabet8Bit:
			{
			aConversionProperties.iWidthConversion=ESmsAlphabetWidthConversionVariable;
			break;
			}
		case TSmsDataCodingScheme::ESmsAlphabetUCS2:
			{
			aConversionProperties.iUDElementsPerNativeCharacter=sizeof(TText);
			break;
			}
		default:
			{
			User::Leave(KErrGsmSMSDataCodingSchemeNotSupported);
			}
		}
	} // CSmsAlphabetConverter::ConversionPropertiesL


/**
 *  Converts from the native character set to unpacked user data elements of the
 *  desired character set.
 *  
 *  The function stores the converted data internally.
 *  
 *  @param aNativeCharacters The native character set data (Unicode only)
 *  @return Converted characters
 *  @capability None
 */
EXPORT_C TPtrC8 CSmsAlphabetConverter::ConvertFromNativeL(const TDesC& aNativeCharacters)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSALPHABETCONVERTER_CONVERTFROMNATIVEL_1, "CSmsAlphabetConverter::ConvertFromNativeL()");

	TInt  numberOfUnconvertibleCharacters, numberOfDowngradedCharacters;

	return ConvertFromNativeL(aNativeCharacters, ESmsEncodingNone,
			                  numberOfUnconvertibleCharacters,
			                  numberOfDowngradedCharacters);
	} // CSmsAlphabetConverter::ConvertFromNativeL


/**
 *  Converts from the native character set to unpacked user data elements of the
 *  desired character set.
 *  
 *  The function stores the converted data internally.
 *  
 *  @param aNativeCharacters                 The native character set data (Unicode only)
 *  @param aNumberOfUnconvertibleCharacters  Number of characters unconverted
 *  @param aNumberOfDowngradedCharacters     Number of characters downgraded
 *  @param aEncoding                         Alternative 7bit encoding to used (if needed)
 * 
 *  @return Converted characters
 * 
 *  @capability None
 */
EXPORT_C TPtrC8 CSmsAlphabetConverter::ConvertFromNativeL(const TDesC& aNativeCharacters,
                                                          TSmsEncoding aEncoding,
                                                          TInt& aNumberOfUnconvertibleCharacters,
			                                              TInt& aNumberOfDowngradedCharacters)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSALPHABETCONVERTER_CONVERTFROMNATIVEL1_1, "CSmsAlphabetConverter::ConvertFromNativeL(): aEncoding=%d", aEncoding);
	
	aNumberOfUnconvertibleCharacters = 0;
	aNumberOfDowngradedCharacters    = 0;

	// Check for some shortcuts
	if (iIsBinary ||  iSmsAlphabet == TSmsDataCodingScheme::ESmsAlphabet8Bit)
		{
		// Binary data stored as padded unicode
		TPtr8 outputPtr=CheckAllocBufferL(&iConvertedUDElements,aNativeCharacters.Length(),0);
		outputPtr.Copy(aNativeCharacters);
		iUnconvertedNativeCharactersPtr.Zero();
		return outputPtr;
		}
	else if (iSmsAlphabet==TSmsDataCodingScheme::ESmsAlphabetUCS2)
		{
		TInt nativeCharactersLength = aNativeCharacters.Length();
		// 16-bit copy with possible endianess correction
		TInt elementCount=nativeCharactersLength*2;
		TPtr8 outputPtr(CheckAllocBufferL(&iConvertedUDElements,elementCount,elementCount));
	    for (TInt i=0;i<nativeCharactersLength;i++)
		    {
		    outputPtr[2*i]=(TUint8)(aNativeCharacters[i]>>8);
		    outputPtr[2*i+1]=(TUint8)aNativeCharacters[i];
		    }
		iUnconvertedNativeCharactersPtr.Zero();
		return outputPtr;
		}
	else // No shortcuts, do proper conversion
		{
		PrepareForConversionFromNativeL(aEncoding);
		
		// Create input buffer
		TInt newInputLength=iUnconvertedNativeCharactersPtr.Length()+aNativeCharacters.Length();
		iUnconvertedNativeCharactersPtr.Set(CheckAllocBufferL(&iUnconvertedNativeCharacters,newInputLength,iUnconvertedNativeCharactersPtr.Length()));
		iUnconvertedNativeCharactersPtr.Append(aNativeCharacters);

		// Ensure buffer is at least the length of the input buffer
		TPtr8 outputPtr=CheckAllocBufferL(&iConvertedUDElements,iUnconvertedNativeCharactersPtr.Length(),0);

		TInt retryCount=0;
		TInt unconvertedCount=iUnconvertedNativeCharactersPtr.Length();
		while (unconvertedCount)
			{
			TInt  tempNumberOfUnconvertibleCharacters = 0;
			TInt  tempNumberOfDowngradedCharacters    = 0;
			
			// Get a pointer to unfilled area of output buffer
			TPtr8 fillPtr((TUint8*)outputPtr.Ptr()+outputPtr.Length(),0,outputPtr.MaxLength()-outputPtr.Length());
			// Try the conversion & get number of unconverted characters
			TInt newUnconvertedCount=iCharacterSetConverter.ConvertFromUnicode(fillPtr,iUnconvertedNativeCharactersPtr);
			if (newUnconvertedCount<0)
				break;

			// Compare what is converted with the original, to get downgrade count
			TPtr  tempBufPtr((TUint16*)iUnconvertedNativeCharactersPtr.Ptr(), 0, fillPtr.Length());
			TInt  offset      = aNativeCharacters.Length() - unconvertedCount;
			TInt  state       = CCnvCharacterSetConverter::KStateDefault;
			TInt  notRestored = iCharacterSetConverter.ConvertToUnicode(tempBufPtr, fillPtr, state);
			
			if (notRestored > 0)
				{
				tempNumberOfUnconvertibleCharacters += notRestored;
				}
			
			for (TInt pos = 0;  pos < tempBufPtr.Length();  pos++)
				{
				if (tempBufPtr[pos] != aNativeCharacters[offset + pos])
					{
					if (tempBufPtr[pos] != KReplacementCharacter)
						{
						tempNumberOfDowngradedCharacters++;
						}
					else
						{
						tempNumberOfUnconvertibleCharacters++;
						}
					}
				}

			//
			// If characters were downgraded or unconverted then replace them
			// with downgrades from the PREQ2090 converter.
			//
			if (tempNumberOfUnconvertibleCharacters > 0  &&
				aEncoding != ESmsEncodingNone)
				{
				HBufC8*  downgradesBuf = HBufC8::NewLC(iUnconvertedNativeCharactersPtr.Length());
				HBufC*  nativeBuf = HBufC::NewLC(iUnconvertedNativeCharactersPtr.Length());
				TPtr8  downgradesPtr = downgradesBuf->Des();
				TPtr  nativePtr = nativeBuf->Des();

				PrepareForConversionFromNativeL(ESmsEncodingNone);

				// Attempt to convert the text
	 			TInt ret = iCharacterSetConverter.ConvertFromUnicode(downgradesPtr, iUnconvertedNativeCharactersPtr);
		 		if (ret >= 0)
		 			{
					// Compare what is converted with the original...
					state       = CCnvCharacterSetConverter::KStateDefault;
					notRestored = iCharacterSetConverter.ConvertToUnicode(nativePtr, downgradesPtr, state);
						
					if (notRestored >= 0)
						{
						// Merge in the downgrades
						TInt  pos;
						
						for (pos = 0;  pos < tempBufPtr.Length();  pos++)
							{
							if (tempBufPtr[pos] != aNativeCharacters[offset + pos])
								{
								if (tempBufPtr[pos] != KReplacementCharacter)
									{
									tempBufPtr[pos] = nativePtr[pos];
									}
								}
							}

						// Reconvert...
						PrepareForConversionFromNativeL(aEncoding);

						newUnconvertedCount=iCharacterSetConverter.ConvertFromUnicode(fillPtr,iUnconvertedNativeCharactersPtr);
						if (newUnconvertedCount<0)
							break;

						// Recount the changed characters...
						tempNumberOfUnconvertibleCharacters = 0;
						tempNumberOfDowngradedCharacters    = 0;

						for (pos = 0;  pos < tempBufPtr.Length();  pos++)
							{
							if (tempBufPtr[pos] != aNativeCharacters[offset + pos])
								{
								if (tempBufPtr[pos] != KReplacementCharacter)
									{
									tempNumberOfDowngradedCharacters++;
									}
								else
									{
									tempNumberOfUnconvertibleCharacters++;
									}
								}
							}
		 				}
					}
				
				CleanupStack::PopAndDestroy(2, downgradesBuf);
				}

			//
			// Store these downgraded/unconvertible character counts...
			//
			aNumberOfDowngradedCharacters    += tempNumberOfDowngradedCharacters;
			aNumberOfUnconvertibleCharacters += tempNumberOfUnconvertibleCharacters;
			
			// Update original buffer length, check retry count and realloc if req'd
			outputPtr.SetLength(outputPtr.Length()+fillPtr.Length());
			if (newUnconvertedCount==unconvertedCount)
				{
				if (++retryCount>KMaxSmsAlphabetConversionRetries)
					{
					__ASSERT_DEBUG(EFalse,Panic(KGsmuPanicConversionRetriedOut));
					break;
					}
				}
			else
				{
				iUnconvertedNativeCharactersPtr.Delete(0,unconvertedCount-newUnconvertedCount);
				retryCount=0;
				}
			unconvertedCount=newUnconvertedCount;
			// Check for realloc
			if (unconvertedCount)
				outputPtr.Set(CheckAllocBufferL(&iConvertedUDElements,iConvertedUDElements->Length()+Max(unconvertedCount,KMinSmsAlphabetConversionAllocIncrement),outputPtr.Length()));
			}

		return outputPtr;
		}
	} // CSmsAlphabetConverter::ConvertFromNativeL


/**
 *  Converts the user data elements of the specified character set to the native
 *  character set.
 *  
 *  @param aUDElements The converted character set data
 * 
 *  @return Native character set data (Unicode only)
 * 
 *  @capability None
 */
EXPORT_C TPtrC CSmsAlphabetConverter::ConvertToNativeL(const TDesC8& aUDElements)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSALPHABETCONVERTER_CONVERTTONATIVEL_1, "CSmsAlphabetConverter::ConvertToNativeL()");

	return ConvertToNativeL(aUDElements, ESmsEncodingNone);
	} // CSmsAlphabetConverter::ConvertToNativeL


/**
 *  Converts the user data elements of the specified character set to the native
 *  character set.
 *  
 *  @param aUDElements The converted character set data
 *  @param aEncoding   Alternative 7bit encoding to used (if needed)
 * 
 *  @return Native character set data (Unicode only)
 * 
 *  @capability None
 */
EXPORT_C TPtrC CSmsAlphabetConverter::ConvertToNativeL(const TDesC8& aUDElements,
													   TSmsEncoding aEncoding)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSALPHABETCONVERTER_CONVERTTONATIVEL1_1, "CSmsAlphabetConverter::ConvertToNativeL(): aEncoding=%d", aEncoding);

	// Check for some shortcuts
	if (iIsBinary ||  iSmsAlphabet == TSmsDataCodingScheme::ESmsAlphabet8Bit)
		{
		// Binary data stored as padded unicode
		TPtr16 outputPtr=CheckAllocBufferL(&iConvertedNativeCharacters,aUDElements.Length(),0);
		outputPtr.Copy(aUDElements);
		iUnconvertedUDElementsPtr.Zero();
		return outputPtr;
		}
	else if (iSmsAlphabet==TSmsDataCodingScheme::ESmsAlphabetUCS2)
		{
		// 16-bit copy with possible endianess correction
		TInt charCount=aUDElements.Length()/2;
		TPtr16 outputPtr=CheckAllocBufferL(&iConvertedNativeCharacters,charCount,charCount);
	    for (TInt i=0; i<charCount; i++)
		    {
		    outputPtr[i]=(TText)(aUDElements[2*i]<<8);
		    outputPtr[i]=(TText)(outputPtr[i]+(aUDElements[2*i+1]));
		    }
		iUnconvertedUDElementsPtr.Zero();
		return outputPtr;
		}
	else
		{
		PrepareForConversionToNativeL(aEncoding);

		// Create input buffer with unconverted characters prepended
		TInt newInputLength=iUnconvertedUDElementsPtr.Length()+aUDElements.Length();
		iUnconvertedUDElementsPtr.Set(CheckAllocBufferL(&iUnconvertedUDElements,newInputLength,iUnconvertedUDElementsPtr.Length()));
		iUnconvertedUDElementsPtr.Append(aUDElements);

		// Ensure buffer is at least the length of the input buffer
		TPtr outputPtr=CheckAllocBufferL(&iConvertedNativeCharacters,iUnconvertedUDElementsPtr.Length(),0);

		TInt retryCount=0;
		TInt unconvertedCount=iUnconvertedUDElementsPtr.Length();
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		while (unconvertedCount)
			{
			// Get a pointer to unfilled area of output buffer
			TPtr16 fillPtr((TUint16*)outputPtr.Ptr()+outputPtr.Length(),0,outputPtr.MaxLength()-outputPtr.Length());
			// Try the conversion & get number of unconverted characters
			TInt newUnconvertedCount=iCharacterSetConverter.ConvertToUnicode(fillPtr,iUnconvertedUDElementsPtr,state);
			if (newUnconvertedCount<0)
				break;
			// Update original buffer length & check retry count
			outputPtr.SetLength(outputPtr.Length()+fillPtr.Length());
			if (newUnconvertedCount==unconvertedCount)
				{
				if (++retryCount>KMaxSmsAlphabetConversionRetries)
					{
					__ASSERT_DEBUG(EFalse,Panic(KGsmuPanicConversionRetriedOut));
					break;
					}
				}
			else
				{
				iUnconvertedUDElementsPtr.Delete(0,unconvertedCount-newUnconvertedCount);
				retryCount=0;
				}
			unconvertedCount=newUnconvertedCount;
			// Check for realloc
			if (unconvertedCount)
				outputPtr.Set(CheckAllocBufferL(&iConvertedNativeCharacters,iConvertedNativeCharacters->Length()+Max(unconvertedCount,KMinSmsAlphabetConversionAllocIncrement),outputPtr.Length()));
			}
		return outputPtr;
		}
	} // CSmsAlphabetConverter::ConvertToNativeL


/**
 *  Tests if the descriptor text is supported by the current character set.
 *  This function can be used with 7bit and 8bit alphabets.
 * 
 *  @param aDes                                 Text string to check.
 *  @param aNumberOfUnconvertibleCharacters     Exit param for the number of
 *                                              characters unconvertible.
 *  @param aIndexOfFirstUnconvertibleCharacter  Exit param for the first
 *                                              unconverted character.
 * 
 *  @return  ETrue if the character is supported.
 */
TBool CSmsAlphabetConverter::IsSupportedL(const TDesC& aDes, TInt& aNumberOfUnconvertibleCharacters,
                                          TInt& aIndexOfFirstUnconvertibleCharacter)
	{
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_ISSUPPORTEDL_1, "[1] CSmsAlphabetConverter::IsSupportedL(aDes=\"%S\")", aDes);

	TInt desLength = aDes.Length();

    //
    // Create buffer for restored text string...
    //
    HBufC*  backToUnicodeAfterStdBuf = HBufC::NewLC(desLength);
    TPtr  backToUnicodeAfterStd(backToUnicodeAfterStdBuf->Des());
    
    //
    // Convert the string...
    //   
	TInt numberOfDowngradedCharacters;
	TBool isCountDowngrade = EFalse; // Don't count downgrades
	ConvertWith7BitEncodingL(aDes, backToUnicodeAfterStd, 
            aNumberOfUnconvertibleCharacters, numberOfDowngradedCharacters, 
            aIndexOfFirstUnconvertibleCharacter, isCountDowngrade);
	
    CleanupStack::PopAndDestroy(backToUnicodeAfterStdBuf);
	
	TBool  supported = (aNumberOfUnconvertibleCharacters == 0);

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_ISSUPPORTEDL_2, "CSmsAlphabetConverter::IsSupportedL(): aNumberOfUnconvertibleCharacters=%d.", aNumberOfUnconvertibleCharacters);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_ISSUPPORTEDL_3, "CSmsAlphabetConverter::IsSupportedL(): aIndexOfFirstUnconvertibleCharacter=%d.", aIndexOfFirstUnconvertibleCharacter);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_ISSUPPORTEDL_4, "CSmsAlphabetConverter::IsSupportedL(): supported=%d.", supported);

	return supported;
	} // CSmsAlphabetConverter::IsSupportedL


/**
 *  Tests if the descriptor text is supported by the current character set.
 *  This function can be used with 7bit and 8bit alphabets.
 * 
 *  @param aDes                                 Text string to check.
 *  @param aEncoding                            Alternative 7bit encoding (if used).
 *  @param aNumberOfUnconvertibleCharacters     Exit param for the number of
 *                                              characters unconvertible.
 *  @param aNumberOfDowngradedCharacters        Exit param for the number of
 *                                              downgraded characters.
 *  @param aNumberRequiringAlternativeEncoding  Exit param for the number of
 *                                              characters requiring use of
 *                                              the alternative encoder.
 *  @param aIndexOfFirstUnconvertibleCharacter  Exit param for the first
 *                                              unconverted character.
 * 
 *  @return  ETrue if the character is supported.
 */
TBool CSmsAlphabetConverter::IsSupportedL(const TDesC& aDes, TSmsEncoding aEncoding,
										  TInt& aNumberOfUnconvertibleCharacters,
                                          TInt& aNumberOfDowngradedCharacters,
                                          TInt& aNumberRequiringAlternativeEncoding,
                                          TInt& aIndexOfFirstUnconvertibleCharacter)
	{
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_ISSUPPORTEDL1_1, "[2] CSmsAlphabetConverter::IsSupportedL(aDes=\"%S\")", aDes);

	TInt desLength = aDes.Length();
	//
	// Initialise the exit param...
	//
	aNumberRequiringAlternativeEncoding = 0;
	
	//
	// Create buffer for restored text string...
	//
    HBufC*  backToUnicodeAfterStdBuf = HBufC::NewLC(desLength);
    TPtr  backToUnicodeAfterStd(backToUnicodeAfterStdBuf->Des());
    TBool  isCountDowngrade = ETrue; // Count downgraded chars
    
    //
    // Convert the character...
    //   
    ConvertWith7BitEncodingL(aDes, backToUnicodeAfterStd, 
            aNumberOfUnconvertibleCharacters, aNumberOfDowngradedCharacters, 
            aIndexOfFirstUnconvertibleCharacter, isCountDowngrade);
    
    TInt  totalCharFaultsSoFar = aNumberOfUnconvertibleCharacters + 
                                    aNumberOfDowngradedCharacters;
    
    //
    // If the total unconvertible plus downgrades is zero, then there is nothing
    // more to do as the string can be converted to 7bit with no issues and no
    // other encoder is needed.
    //
    // Otherwise we have to look at the alternative encoder...
    //
    if (totalCharFaultsSoFar == 0)
        {
        //
        // We are done (the return counts are already zero and therefore
        // correct at this point too!).
        //
        }
    else if (iSmsAlphabet != TSmsDataCodingScheme::ESmsAlphabet7Bit  ||
             aEncoding == ESmsEncodingNone)
        {
        //
        // The string was not perfectly converted, but there is no alternative
        // encoder to try.  We are done.
        //
        }
    else
        {
        //
        // Initialise the params...
        //
        TInt  tmpDowngradedCharacters        = 0;
        TInt  tmpUnconvertibleCharacters     = 0;
        TInt  tmpIndexOfFirstUnconvertibleCharacter = desLength;
        
        ConvertWithAlternativeEncodingL(aDes, backToUnicodeAfterStd, aEncoding,
                tmpUnconvertibleCharacters, tmpDowngradedCharacters,
                aNumberRequiringAlternativeEncoding,
                tmpIndexOfFirstUnconvertibleCharacter);       
        
        // Is this better?  
        if ( totalCharFaultsSoFar >= (tmpUnconvertibleCharacters + tmpDowngradedCharacters) )
            {
            // Best conversion is the alternative conversion
            aNumberOfUnconvertibleCharacters     = tmpUnconvertibleCharacters;
            aNumberOfDowngradedCharacters        = tmpDowngradedCharacters;
            aIndexOfFirstUnconvertibleCharacter  = tmpIndexOfFirstUnconvertibleCharacter;
            }
        else
            {
            // Best conversion is the standard conversion
            aNumberRequiringAlternativeEncoding = 0;
            }
        }
    
    CleanupStack::PopAndDestroy(backToUnicodeAfterStdBuf);   
 
	//
	// Useful logging...
	//
	TBool  supported = (aNumberOfUnconvertibleCharacters == 0);

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_ISSUPPORTEDL1_2, "CSmsAlphabetConverter::IsSupportedL(): aNumberOfUnconvertibleCharacters=%d.", aNumberOfUnconvertibleCharacters);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_ISSUPPORTEDL1_3, "CSmsAlphabetConverter::IsSupportedL(): aNumberOfDowngradedCharacters=%d.", aNumberOfDowngradedCharacters);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_ISSUPPORTEDL1_4, "CSmsAlphabetConverter::IsSupportedL(): aNumberRequiringAlternativeEncoding=%d.", aNumberRequiringAlternativeEncoding);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_ISSUPPORTEDL1_5, "CSmsAlphabetConverter::IsSupportedL(): aIndexOfFirstUnconvertibleCharacter=%d.", aIndexOfFirstUnconvertibleCharacter);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_ISSUPPORTEDL1_6, "CSmsAlphabetConverter::IsSupportedL(): supported=%d.", supported);

	return supported;
	} // CSmsAlphabetConverter::IsSupportedL


/**
 *  Tests if the descriptor text is supported by the current character set.
 * 
 *  @param aDes                                 Text string to check.
 *  @param aRestoredDes                         Exit restored text string after conversion.
 *  @param aNumberOfUnconvertibleCharacters     Exit param for the number of
 *                                              characters unconvertible.
 *  @param aNumberOfDowngradedCharacters        Exit param for the number of
 *                                              downgraded characters.
 *  @param aIndexOfFirstUnconvertibleCharacter  Exit param for the first
 *                                              unconverted character.
 *  @param aIsCountDowngrade                    Flag for counting downgrades.
 */
void CSmsAlphabetConverter::ConvertWith7BitEncodingL(const TDesC& aDes, TDes& aRestoredDes,
                                          TInt& aNumberOfUnconvertibleCharacters,
                                          TInt& aNumberOfDowngradedCharacters,
                                          TInt& aIndexOfFirstUnconvertibleCharacter,
                                          TBool aIsCountDowngrade)
    {
 	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_CONVERTWITH7BITENCODINGL_1, "CSmsAlphabetConverter::ConvertWith7BitEncodingL(aDes=\"%S\")", aDes);

    TInt desLength = aDes.Length();
    //
    // Initialise the exit params...
    //
    aNumberOfUnconvertibleCharacters    = 0;
    aNumberOfDowngradedCharacters       = 0;
    aIndexOfFirstUnconvertibleCharacter = desLength;
    
    //
    // Create buffer for the input converted to 7Bit.
    //
    HBufC8* encodedBuf       = HBufC8::NewLC(desLength*2); // worse case
    TPtr8  encoded(encodedBuf->Des());

    //
    // Convert the input string to standard 7bit (with downgrades if needed)...
    // 
    PrepareForConversionFromNativeL(ESmsEncodingNone);

    TInt  notConverted = iCharacterSetConverter.ConvertFromUnicode(encoded, aDes);

    if (notConverted > 0)
        {
        aNumberOfUnconvertibleCharacters += notConverted;
        }
    else if (notConverted < 0)
        {
        aNumberOfUnconvertibleCharacters = desLength;
        }
    
    //
    // Convert it back again to the native format...
    //
    TInt  state       = CCnvCharacterSetConverter::KStateDefault;
    TInt  notRestored = iCharacterSetConverter.ConvertToUnicode(aRestoredDes, encoded, state);

    if (notRestored > 0)
        {
        aNumberOfUnconvertibleCharacters += notRestored;
        }
    else if (notRestored < 0)
        {
        aNumberOfUnconvertibleCharacters = desLength;
        }

    //
    // Work out if the string is acceptable as it is (e.g. no unconvertible
    // and no downgrades).
    //
    for (TInt pos = desLength-1;  pos >= 0;  --pos)
        {
        if (aRestoredDes[pos] != aDes[pos])
            {
            if (aRestoredDes[pos] != KReplacementCharacter
                    && aIsCountDowngrade)
                {
                ++aNumberOfDowngradedCharacters;
                }
            else
                {
                ++aNumberOfUnconvertibleCharacters;
                aIndexOfFirstUnconvertibleCharacter = pos;
                }
            }
        }
    
    CleanupStack::PopAndDestroy(encodedBuf);
    
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_CONVERTWITH7BITENCODINGL_2, "CSmsAlphabetConverter::ConvertWith7BitEncodingL(): aNumberOfUnconvertibleCharacters=%d.", aNumberOfUnconvertibleCharacters);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_CONVERTWITH7BITENCODINGL_3, "CSmsAlphabetConverter::ConvertWith7BitEncodingL(): aNumberOfDowngradedCharacters=%d.", aNumberOfDowngradedCharacters);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_CONVERTWITH7BITENCODINGL_4, "CSmsAlphabetConverter::ConvertWith7BitEncodingL(): aIndexOfFirstUnconvertibleCharacter=%d.", aIndexOfFirstUnconvertibleCharacter);
    } // CSmsAlphabetConverter::ConvertWith7BitEncodingL    

/**
 *  Tests if the descriptor text is supported by given encoding.
 *  This function can be used with 7bit and 8bit alphabets.
 * 
 *  @param aDes                                 Text string to check.
 *  @param aRestoredStdDes                      Text string restored from 7bit encoding.
 *  @param aEncoding                            Alternative 7bit encoding.
 *  @param aNumberOfUnconvertibleCharacters     Exit param for the number of
 *                                              characters unconvertible.
 *  @param aNumberOfDowngradedCharacters        Exit param for the number of
 *                                              downgraded characters.
 *  @param aNumberRequiringAlternativeEncoding  Exit param for the number of
 *                                              characters requiring use of
 *                                              the alternative encoder.
 *  @param aIndexOfFirstUnconvertibleCharacter  Exit param for the first
 */
void CSmsAlphabetConverter::ConvertWithAlternativeEncodingL(const TDesC& aDes, const TDesC& aRestoredStdDes, 
                                          TSmsEncoding aEncoding,
                                          TInt& aNumberOfUnconvertibleCharacters,
                                          TInt& aNumberOfDowngradedCharacters,
                                          TInt& aNumberRequiringAlternativeEncoding,
                                          TInt& aIndexOfFirstUnconvertibleCharacter)
    {
  	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_CONVERTWITHALTERNATIVEEENCODINGL_1, "CSmsAlphabetConverter::ConvertWithAlternativeEncodingL(aDes=\"%S\")", aDes);
  
    TInt desLength = aDes.Length();
    //
    // Initialise the exit params...
    //
    aNumberOfUnconvertibleCharacters    = 0;
    aNumberOfDowngradedCharacters       = 0;
    aNumberRequiringAlternativeEncoding = 0;
    aIndexOfFirstUnconvertibleCharacter = desLength;
        
    //
    // Create buffer for the input converted to 7Bit.
    //
    HBufC8* encodedBuf = HBufC8::NewLC(desLength*2); // worse case
    TPtr8  encoded(encodedBuf->Des());
 
    //
    // Convert the input string to the alternative encoding...
    //
    PrepareForConversionFromNativeL(aEncoding);

    TInt notConverted = iCharacterSetConverter.ConvertFromUnicode(encoded, aDes);
    if (notConverted > 0)
        {
        aNumberOfUnconvertibleCharacters = notConverted;
        }
    else if (notConverted < 0)
        {
        aNumberOfUnconvertibleCharacters = desLength;
        }

    //
    // Create buffer for restored text...
    //
    HBufC*  backToUnicodeBuf = HBufC::NewLC(desLength);
    TPtr  backToUnicode(backToUnicodeBuf->Des());

    //
    // Convert it back again to the native format...
    //
    TInt  state       = CCnvCharacterSetConverter::KStateDefault;
    TInt  notRestored = iCharacterSetConverter.ConvertToUnicode(backToUnicode, encoded, state);

    if (notRestored > 0)
        {
        aNumberOfUnconvertibleCharacters += notRestored;
        }
    else if (notRestored < 0)
        {
        aNumberOfUnconvertibleCharacters = desLength;
        }

    //
    // Now work out which characters are downgrades, require alternative encoding
    // or are unsupported.
    //
    for (TInt pos = desLength-1;  pos >= 0;  --pos)
        {
        if (aRestoredStdDes[pos] != aDes[pos])
            {
            // Not supported by standard encoder...
            if (backToUnicode[pos] == aDes[pos])
                {
                // Supported by alternative encoder...
                ++aNumberRequiringAlternativeEncoding;
                }
            else if (aRestoredStdDes[pos] != KReplacementCharacter)
                {
                // Downgraded by standard encoder...
                ++aNumberOfDowngradedCharacters;
                }
            else if (backToUnicode[pos] != KReplacementCharacter)
                {
                // Downgraded by alternative encoder...
                ++aNumberOfDowngradedCharacters;
                ++aNumberRequiringAlternativeEncoding;
                }
            else
                {
                // Unconvertible...
                ++aNumberOfUnconvertibleCharacters;
                aIndexOfFirstUnconvertibleCharacter = pos;
                }
            }
        }

    CleanupStack::PopAndDestroy(2, encodedBuf); // backToUnicode, encodedBuf
    
 	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_CONVERTWITHALTERNATIVEEENCODINGL_2, "CSmsAlphabetConverter::ConvertWithAlternativeEncodingL(): aNumberOfUnconvertibleCharacters=%d.", aNumberOfUnconvertibleCharacters);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_CONVERTWITHALTERNATIVEEENCODINGL_3, "CSmsAlphabetConverter::ConvertWithAlternativeEncodingL(): aNumberOfDowngradedCharacters=%d.", aNumberOfDowngradedCharacters);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_CONVERTWITHALTERNATIVEEENCODINGL_4, "CSmsAlphabetConverter::ConvertWithAlternativeEncodingL(): aNumberRequiringAlternativeEncoding=%d.", aNumberRequiringAlternativeEncoding);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_CONVERTWITHALTERNATIVEEENCODINGL_5, "CSmsAlphabetConverter::ConvertWithAlternativeEncodingL(): aIndexOfFirstUnconvertibleCharacter=%d.", aIndexOfFirstUnconvertibleCharacter);
    } // CSmsAlphabetConverter::ConvertWithAlternativeEncodingL    

/**
 *  Given a piece of text and an alternative encoding, this function works out
 *  which encoding is best to use and returns the ID of that converter.
 * 
 *  @param aNativeCharacters      Text to use as a sample.
 *  @param aEncoding              Suggested alternative 7bit encoding method.
 * 
 *  @return Encoding that should be used.
 */
TSmsEncoding CSmsAlphabetConverter::FindBestAlternativeEncodingL(const TDesC& aNativeCharacters,
									                     		 TSmsEncoding aSuggestedEncoding)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_FINDBESTALTERNATIVEENCODINGL_1, "CSmsAlphabetConverter::FindBestAlternativeEncodingL(): aSuggestedEncoding=%d",aSuggestedEncoding);

	TSmsEncoding  encodingToUse = ESmsEncodingNone;

	//
	// If this is not 7bit or the alternative encoding is not set then do
	// nothing...
	//
	if (aSuggestedEncoding != ESmsEncodingNone  &&
		iSmsAlphabet == TSmsDataCodingScheme::ESmsAlphabet7Bit)
		{
        TInt desLength = aNativeCharacters.Length();
		TInt  numberOfUnconvertibleCharacters, numberOfDowngradedCharacters;
		TInt  numberRequiringAlternativeEncoding, indexOfFirstUnconvertibleCharacter;
		
		//
		// First try the default encoding (but in this case treat downgrades
		// as unconverted, since later encoders might do better)...
		//
		HBufC*  backToUnicodeAfterStdBuf = HBufC::NewLC(desLength);
	    TPtr  backToUnicodeAfterStd(backToUnicodeAfterStdBuf->Des());
	    TBool  isCountDowngrade = ETrue; // Count downgraded chars
		
	    ConvertWith7BitEncodingL(aNativeCharacters, backToUnicodeAfterStd, 
	            numberOfUnconvertibleCharacters, numberOfDowngradedCharacters, 
	            indexOfFirstUnconvertibleCharacter, isCountDowngrade);
		

		TInt leastUnconvertibleCharacters = numberOfUnconvertibleCharacters + numberOfDowngradedCharacters;

		if (leastUnconvertibleCharacters > 0)
		    {
            //
            // Create a list of alternative encodings to try...
            //
            TSmsEncoding  encodingList[8];
            TInt          encodingCount = 0;
            
            if (aSuggestedEncoding == ESmsEncodingTurkishLockingAndSingleShift)
                {
                encodingList[encodingCount++] = ESmsEncodingTurkishSingleShift;
                encodingList[encodingCount++] = ESmsEncodingTurkishLockingShift;
                }
            else if (aSuggestedEncoding == ESmsEncodingPortugueseLockingAndSingleShift)
                {
                encodingList[encodingCount++] = ESmsEncodingPortugueseSingleShift;
                encodingList[encodingCount++] = ESmsEncodingPortugueseLockingShift;
                }
            
            encodingList[encodingCount++] = aSuggestedEncoding;

            //
            // Now try the all the alternatives...
            //
            for (TInt  encoder = 0;  encoder < encodingCount && leastUnconvertibleCharacters > 0;  ++encoder)
                {
                ConvertWithAlternativeEncodingL(aNativeCharacters, backToUnicodeAfterStd, 
                        encodingList[encoder], 
                        numberOfUnconvertibleCharacters, 
                        numberOfDowngradedCharacters,
                        numberRequiringAlternativeEncoding,
                        indexOfFirstUnconvertibleCharacter);       

                if (numberOfUnconvertibleCharacters + numberOfDowngradedCharacters < leastUnconvertibleCharacters)
                    {
                    encodingToUse = encodingList[encoder];
                    leastUnconvertibleCharacters = numberOfUnconvertibleCharacters + numberOfDowngradedCharacters;
                    }
                }
		    }
		
            CleanupStack::PopAndDestroy(backToUnicodeAfterStdBuf);
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_FINDBESTALTERNATIVEENCODINGL_2, "CSmsAlphabetConverter::FindBestAlternativeEncodingL(): encodingToUse=%d", encodingToUse);

	return encodingToUse;
	} // CSmsAlphabetConverter::FindBestAlternativeEncoding


/**
 *  Confirms that the requested encoding method is present (e.g. the converter
 *  plug-in can be loaded).
 * 
 *  @param aEncoding  Alternative 7bit encoding method.
 * 
 *  @leave KErrNotSupported if the encoder is not available.
 */
void CSmsAlphabetConverter::ConfirmAlternativeEncoderL(TSmsEncoding aEncoding) const
	{
	//
	// Check the ID for the encoder exists...
	//
	if (aEncoding != ESmsEncodingNone)
		{
		TUint  encoderID;
		
		GetAlternativeEncoderIDL(aEncoding, encoderID);
		
		//
		// Confirm it can be loaded...
		//
		if (iCharacterSetConverter.PrepareToConvertToOrFromL(encoderID, iFs) != CCnvCharacterSetConverter::EAvailable)
			{
			User::Leave(KErrNotSupported);
			}
		}
	} // CSmsAlphabetConverter::ConfirmAlternativeEncoderL


/**
 *  Prepares the converted for conversion from native charset.
 *  Character set specific preparation is performed here.
 * 
 *  @param aEncoding  Alternative 7bit encoding to use if required.
 */
void CSmsAlphabetConverter::PrepareForConversionFromNativeL(TSmsEncoding aEncoding)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_PREPAREFORCONVERSIONFROMNATIVEL_1, "CSmsAlphabetConverter::PrepareForConversionFromNativeL(): aEncoding=%d",aEncoding);

	__ASSERT_DEBUG(iIsBinary==EFalse,Panic(KGsmuPanicUnsupportedAlphabet));
	switch (iSmsAlphabet)
		{
		case TSmsDataCodingScheme::ESmsAlphabet7Bit:
			{
			//
			// If an alternative encoding has been specified then try and
			// load that converter...
			//
			if (aEncoding != ESmsEncodingNone)
				{
				TUint  alternativeEncoderID;
				
				GetAlternativeEncoderIDL(aEncoding, alternativeEncoderID);
				if (alternativeEncoderID != 0)
					{
					CCnvCharacterSetConverter::TAvailability  availability;
		
					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_PREPAREFORCONVERSIONFROMNATIVEL_2, "CSmsAlphabetConverter::PrepareForConversionFromNativeL(): Converter 0x%08x",alternativeEncoderID);
					
					availability = iCharacterSetConverter.PrepareToConvertToOrFromL(alternativeEncoderID, iFs);
					if (availability == CCnvCharacterSetConverter::EAvailable)
						{
			 			// Force unicode line termination characters to simple line feed
			 			iCharacterSetConverter.SetDowngradeForExoticLineTerminatingCharacters(
				 				CCnvCharacterSetConverter::EDowngradeExoticLineTerminatingCharactersToJustLineFeed);
					
						// Job done, return
			 			return;
						}
					
					// Plug-in could not be loaded, so drop through and load the default!
					}
				}
			
			//
			// Check for the PREQ2090 7bit converter with Eastern European
			// downgrade support first, otherwise if the plug-in is not found
			// use the standard internal converter.
			//
			CCnvCharacterSetConverter::TAvailability  availability;
			
			availability = iCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetIdentifierExtendedSms7Bit, iFs);
			if (availability == CCnvCharacterSetConverter::ENotAvailable)
				{
				availability = iCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit, iFs);
				if (availability == CCnvCharacterSetConverter::ENotAvailable)
					{
					User::Leave(KErrNotFound);
					}
				}

 			// Force unicode line termination characters to simple line feed
 			iCharacterSetConverter.SetDowngradeForExoticLineTerminatingCharacters(
 				CCnvCharacterSetConverter::EDowngradeExoticLineTerminatingCharactersToJustLineFeed);
			break;
			}
		case TSmsDataCodingScheme::ESmsAlphabet8Bit:
			{
			CCnvCharacterSetConverter::TAvailability availability=iCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252,iFs);
			if (availability==CCnvCharacterSetConverter::ENotAvailable)
				{
				User::Leave(KErrNotFound);
				}

 			// Force unicode line termination characters to simple line feed
 			iCharacterSetConverter.SetDowngradeForExoticLineTerminatingCharacters(
 				CCnvCharacterSetConverter::EDowngradeExoticLineTerminatingCharactersToJustLineFeed);
			break;
			}
		case TSmsDataCodingScheme::ESmsAlphabetUCS2:
		default:
			{
            User::Leave(KErrNotSupported);
			}
		}
	} // CSmsAlphabetConverter::PrepareForConversionFromNativeL


/**
 *  Prepares the converted for conversion to unicode.  Character set
 *  specific preparation is performed here.
 * 
 *  @param aEncoding  Alternative 7bit encoding to use if required.
 */
void CSmsAlphabetConverter::PrepareForConversionToNativeL(TSmsEncoding aEncoding)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_PREPAREFORCONVERSIONTONATIVEL_1, "CSmsAlphabetConverter::PrepareForConversionToNativeL(): aEncoding=%d",aEncoding);

	switch (iSmsAlphabet)
		{
		case TSmsDataCodingScheme::ESmsAlphabet7Bit:
			{
			//
			// If an alternative encoding has been specified then try and
			// load that converter...
			//
			if (aEncoding != ESmsEncodingNone)
				{
				TUint  alternativeEncoderID;
				
				GetAlternativeEncoderIDL(aEncoding, alternativeEncoderID);
				if (alternativeEncoderID != 0)
					{
					CCnvCharacterSetConverter::TAvailability  availability;
		
					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_PREPAREFORCONVERSIONTONATIVEL_2, "CSmsAlphabetConverter::PrepareForConversionFromNativeL(): Converter 0x%08x",alternativeEncoderID);
					
					availability = iCharacterSetConverter.PrepareToConvertToOrFromL(alternativeEncoderID, iFs);
					if (availability == CCnvCharacterSetConverter::EAvailable)
						{
						// Job done, return
			 			return;
						}
					
					// Plug-in could not be loaded, so drop through and load the default!
					}
				}
			
			//
			// Always use the internal converter, as it is quicker to prepare
			// and the PREQ2090 7bit converter with Eastern European downgrade
			// offers no benefit when converting to native.
			//
			CCnvCharacterSetConverter::TAvailability availability=iCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit,iFs);
			if (availability==CCnvCharacterSetConverter::ENotAvailable)
				{
				User::Leave(KErrNotFound);
				}
			break;
			}
		case TSmsDataCodingScheme::ESmsAlphabet8Bit:
			{
			CCnvCharacterSetConverter::TAvailability availability=iCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252,iFs);
			if (availability==CCnvCharacterSetConverter::ENotAvailable)
				{
				User::Leave(KErrNotFound);
				}
			break;
			}
		case TSmsDataCodingScheme::ESmsAlphabetUCS2:
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	} // CSmsAlphabetConverter::PrepareForConversionToNativeL


/**
 *  This function returns the alternative encoding converters that are used
 *  incase the default GSM encoding cannot encode the message completely
 *  without data loss.
 * 
 *  @param aEncoding   Encoding to obtain the converter for.
 *  @param aEncoderID  Returned converter UID if present.
 * 
 *  @leave KErrArgument if the encoding enum is invalid or
 */
void CSmsAlphabetConverter::GetAlternativeEncoderIDL(TSmsEncoding aEncoding, TUint& aEncoderID) const
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_GETALTERNATIVEENCODERIDL_1, "CSmsAlphabetConverter::GetAlternativeEncoderIDL(%d)", aEncoding);

	aEncoderID = 0;
	
	//
	// Decide on appropriate encoders.
	//
	switch (aEncoding)
		{
		case ESmsEncodingNone:
			{
			// Nothing to set.
			}
			break;
		
		case ESmsEncodingTurkishSingleShift:
			{
			aEncoderID = KCharacterSetIdentifierTurkishSingleSms7Bit;
			}
			break;
		
		case ESmsEncodingTurkishLockingShift:
			{
			aEncoderID = KCharacterSetIdentifierTurkishLockingSms7Bit;
			}
			break;
		
		case ESmsEncodingTurkishLockingAndSingleShift:
			{
			aEncoderID = KCharacterSetIdentifierTurkishLockingAndSingleSms7Bit;
			}
			break;
		
		case ESmsEncodingSpanishSingleShift:
			{
			aEncoderID = KCharacterSetIdentifierSpanishSingleSms7Bit;
			}
			break;
		
		case ESmsEncodingPortugueseSingleShift:
			{
			aEncoderID = KCharacterSetIdentifierPortugueseSingleSms7Bit;
			}
			break;
		
		case ESmsEncodingPortugueseLockingShift:
			{
			aEncoderID = KCharacterSetIdentifierPortugueseLockingSms7Bit;
			}
			break;
		
		case ESmsEncodingPortugueseLockingAndSingleShift:
			{
			aEncoderID = KCharacterSetIdentifierPortugueseLockingAndSingleSms7Bit;
			}
			break;

		default:
			{
			//
			// Invalid encoder method!
			//
			User::Leave(KErrArgument);
			}
		};
	} // CSmsAlphabetConverter::GetAlternativeEncoderIDL


//
// Ensures the allocated 16 bit buffer is at least of the specified length
//
TPtr16 CSmsAlphabetConverter::CheckAllocBufferL(HBufC16** aBuffer,TInt aMaxLength,TInt aUsedLength)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_CHECKALLOCBUFFERL_1, "CSmsAlphabetConverter::CheckAllocBufferL()");

	if (*aBuffer!=NULL)
		{
		if ((*aBuffer)->Length()<aMaxLength)
			{
			*aBuffer=(*aBuffer)->ReAllocL(aMaxLength);
			(*aBuffer)->Des().SetLength(aMaxLength);
			}
		}
	else
		{
		*aBuffer=HBufC16::NewMaxL(aMaxLength);
		}
	return TPtr16((TUint16*)(*aBuffer)->Des().Ptr(),aUsedLength,(*aBuffer)->Length());
	} // CSmsAlphabetConverter::CheckAllocBufferL


//
// Ensures the allocated 8 bit buffer is at least of the specified length
//
TPtr8 CSmsAlphabetConverter::CheckAllocBufferL(HBufC8** aBuffer,TInt aMaxLength,TInt aUsedLength)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSALPHABETCONVERTER_CHECKALLOCBUFFERL1_1, "CSmsAlphabetConverter::CheckAllocBufferL()");

	if (*aBuffer!=NULL)
		{
		if ((*aBuffer)->Length()<aMaxLength)
			{
			*aBuffer=(*aBuffer)->ReAllocL(aMaxLength);
			(*aBuffer)->Des().SetLength(aMaxLength);
			}
		}
	else
		{
		*aBuffer=HBufC8::NewMaxL(aMaxLength);
		}
	return TPtr8((TUint8*)(*aBuffer)->Des().Ptr(),aUsedLength,(*aBuffer)->Length());
	} // CSmsAlphabetConverter::CheckAllocBufferL


/**
 *  @internalComponent
 *  
 *  Determines whether the address format matches the specified type.
 *  
 *  @param aType Specifies an indicator type, as defined in the Common PCN Handset Specification
 *  @return returns ETrue if address is of specified type, EFalse otherwise 
 */
EXPORT_C TBool TGsmSmsTelNumber::IsInstanceOf(TTypeOfIndicator aType)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TGSMSMSTELNUMBER_ISINSTANCEOF_1, "TGsmSmsTelNumber::IsInstanceOf()");

    TBool rc = EFalse;

    (void) aType;

    if((aType == EVoiceMessageWaitingIndicator)
     && ((iTypeOfAddress & TGsmSmsTypeOfAddress::EGsmSmsTONMask)==EGsmSmsTONAlphaNumeric)
     && (iTelNumber.Length()==ECPHSSizeOfAddressField)
     && ((iTelNumber[ECPHSAddressIndicatorType] & ECPSHIndicatorTypeBitMask) == ECPSHVoiceMailId)
     && ((iTelNumber[ECPHSAddressIndicatorId] & ECPSHIndicatorIdBitMask) == ECPSHIndicatorId ))
      rc = ETrue;

    return rc;
    } // TGsmSmsTelNumber::IsInstanceOf


//
// CSmsAddress
//

CSmsAddress* CSmsAddress::NewL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSADDRESS_NEWL_1, "CSmsAddress::NewL()");

	CSmsAddress* address=new(ELeave) CSmsAddress(aCharacterSetConverter,aFs);
	CleanupStack::PushL(address);
	TPtrC ptr;
	address->SetAddressL(ptr);
	CleanupStack::Pop();
	return address;
	} // CSmsAddress::NewL


CSmsAddress::~CSmsAddress()
    {
    delete iBuffer;
    } // CSmsAddress::NewL


/**
 *  Duplicates this CSmsAddress object.
 * 
 *  @return  Pointer to the newly created CSmsAddress object.
 */
CSmsAddress* CSmsAddress::DuplicateL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSADDRESS_DUPLICATEL_1, "CSmsAddress::DuplicateL()");

	CSmsAddress*  address = CSmsAddress::NewL(iCharacterSetConverter, iFs);
	CleanupStack::PushL(address);

	address->SetRawAddressL(iTypeOfAddress, *iBuffer);
	
	CleanupStack::Pop();

	return address;
	} // CSmsAddress::DuplicateL


TPtrC CSmsAddress::Address() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSADDRESS_ADDRESS_1, "CSmsAddress::Address()");

	TPtrC ptr;
	if (iBuffer)
		ptr.Set(iBuffer->Des());
	return ptr;
	} // CSmsAddress::Address


void CSmsAddress::SetRawAddressL(TGsmSmsTypeOfAddress aTypeOfAddress, TPtrC aBufferPtr)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSADDRESS_SETRAWADDRESSL_1, "CSmsAddress::SetRawAddressL()");

    iTypeOfAddress = aTypeOfAddress;

    NewBufferL(aBufferPtr.Length());

	*iBuffer=aBufferPtr;
    } // CSmsAddress::SetRawAddressL


TGsmSmsTypeOfAddress& CSmsAddress::TypeOfAddress()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSADDRESS_TYPEOFADDRESS_1, "CSmsAddress::TypeOfAddress()");

    return iTypeOfAddress;
    } // CSmsAddress::TypeOfAddress


void CSmsAddress::SetAddressL(const TDesC& aAddress)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSADDRESS_SETADDRESSL_1, "CSmsAddress::SetAddressL()");

	TInt length=aAddress.Length();
	NewBufferL(length);
	iBuffer->Des().Copy(aAddress);

	const TGsmSmsTypeOfNumber typeofnumber=length && (iBuffer->Des()[0]=='+')? EGsmSmsTONInternationalNumber: EGsmSmsTONUnknown;
	iTypeOfAddress.SetTON(typeofnumber);
	} // CSmsAddress::SetAddressL


void CSmsAddress::ParsedAddress(TGsmSmsTelNumber& aParsedAddress) const
	{
	aParsedAddress.iTypeOfAddress = iTypeOfAddress;

	TInt maxparsedlength=aParsedAddress.iTelNumber.MaxLength();

	if (iTypeOfAddress.TON()==EGsmSmsTONAlphaNumeric)
		{
		TInt parsedlength=Address().Length();
		if (parsedlength>maxparsedlength)
			parsedlength=maxparsedlength;
		aParsedAddress.iTelNumber.Copy(Address().Mid(0,parsedlength));
		}
	else
		{
		aParsedAddress.iTelNumber.SetLength(maxparsedlength);

		TInt length=iBuffer->Des().Length();
		TInt parsedlength=0;
		for (TInt i=0; (i<length) && (parsedlength<maxparsedlength); i++)
			{
			TText ch=iBuffer->Des()[i];
            switch(ch)
                {
                case '*':
                case '#':
                case 'a':
                case 'b':
                case 'c':
                    aParsedAddress.iTelNumber[parsedlength]=(TUint8) ch;
                    parsedlength++;
                    break;                    
                default:
                    if ((ch>='0') && (ch<='9'))
                        {
                        aParsedAddress.iTelNumber[parsedlength]=(TUint8) ch;
                        parsedlength++;
                        }
                    break;
                }			
			}

		aParsedAddress.iTelNumber.SetLength(parsedlength);
		}
	}


void CSmsAddress::SetParsedAddressL(const TGsmSmsTelNumber& aParsedAddress)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSADDRESS_SETPARSEDADDRESSL_1, "CSmsAddress::SetParsedAddressL()");

	iTypeOfAddress=aParsedAddress.iTypeOfAddress;
	DoSetParsedAddressL(aParsedAddress.iTelNumber);
	} // CSmsAddress::SetParsedAddressL


TUint8 CSmsAddress::SizeL()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSADDRESS_SIZEL_1, "CSmsAddress::SizeL()");

    TUint8 size = 0;

    TBuf8<KSmsAddressMaxAddressLength> testBuffer;
    testBuffer.SetLength(KSmsAddressMaxAddressLength);
    TUint8* startPtr = &testBuffer[0];
    TUint8* endPtr = NULL;

    TRAPD(err,endPtr = EncodeL(startPtr));

    if (err != KErrNone)
        {
        User::Leave(KErrArgument);
        }
    else
        {
        // handle architectures whose address space increments or whose address space decrements.
        (endPtr > startPtr) ? (size = endPtr - startPtr) : (size = startPtr - endPtr);
        }

	return size;
    } // CSmsAddress::SizeL


TUint8* CSmsAddress::EncodeL(TUint8* aPtr) const
	{
	TGsmSmsTelNumber parsedaddress;
	ParsedAddress(parsedaddress);

	switch (iTypeOfAddress.TON())
		{
		case EGsmSmsTONAlphaNumeric:
			{
			// Mark buffer for length encoding after conversion
			TUint8* lengthPtr=aPtr++;
			// Encode type
			aPtr=iTypeOfAddress.EncodeL(aPtr);
			// 7-bit conversion
			TPtrC address=Address();
			TInt convertedNumUDUnits=0;
			TPtr8 packedPtr(aPtr,0,KSmsAddressMaxAddressValueLength);
			TSmsAlphabetPacker packer(TSmsDataCodingScheme::ESmsAlphabet7Bit,EFalse,0);
			aPtr+=packer.ConvertAndPackL(iCharacterSetConverter,iFs,packedPtr,address,convertedNumUDUnits);
			// Now encode length
			TSmsOctet length=(convertedNumUDUnits*7+3)/4;
			length.EncodeL(lengthPtr);
			break;
			}
		case EGsmSmsTONInternationalNumber:
		default:
			{
			TSmsOctet length=parsedaddress.iTelNumber.Length();
			if (length > KSmsAddressMaxAddressValueLength * 2)
			// each address value occupies one nibble.
                {
                User::Leave(KErrArgument);
                }
			aPtr=length.EncodeL(aPtr);
			aPtr=iTypeOfAddress.EncodeL(aPtr);
			TSmsOctet octet;
			for (TInt i=0; i<length; i++)
				{
                if ((i%2)==0)  //  even
                    {
                    switch(parsedaddress.iTelNumber[i])
                        {
                        case '*':
                            {
                            octet=10; // 1010 as a binary, according to 23.040 …
                            }
                            break;

                        case '#':
                            {
                            octet=11; // 1011 as a binary, according to 23.040…
                            }
                            break; 

                        case 'a':
                            {
                            octet=12; // 1100 as a binary, according to 23.040...
                            }
                            break;

                        case 'b':
                            {
                            octet=13; // 1101 as a binary, according to 23.040…
                            }
                            break;

                        case 'c':
                            {
                            octet=14; // 1110 as a binary, according to 23.040…
                            }
                            break;

                        default:
                            octet=(parsedaddress.iTelNumber[i])-'0';
                            break;
                        }
                    } 
                else
                    {
                    TInt tempOctet = 0;
       
                    switch(parsedaddress.iTelNumber[i])
                        {
                        case '*':
                            {
                            tempOctet=10; // 1010 as a binary, according to 23.040…
                            }
                            break;

                        case '#':
                            {
                            tempOctet=11; // 1011 as a binary, according to 23.040…
                            }
                            break;

                        case 'a':
                            {
                            tempOctet=12; // 1100 as a binary, according to 23.040…
                            }
                            break;

                        case 'b':
                            {
                            tempOctet=13; // 1101 as a binary, according to 23.040…
                            }
                            break;

                        case 'c':
                            {
                            tempOctet=14; // 1110 as a binary, according to 23.040…
                            }
                            break;

                        default:
                            tempOctet=(parsedaddress.iTelNumber[i])-'0';
                            break;
                        }
                    octet=octet|tempOctet<<4;
                    aPtr=octet.EncodeL(aPtr);				
				    }
                
			    } // end for loop

			if (length%2)   // odd number of semioctets
				{
				octet=octet|0xF0;
				aPtr=octet.EncodeL(aPtr);
				}
						
		    } // end default case
	    }
	return aPtr;
	}


void CSmsAddress::DecodeL(TGsmuLex8& aPdu)
	{
	TSmsOctet length;  //represents the number of valid semi-octets
	length.DecodeL(aPdu);
	iTypeOfAddress.DecodeL(aPdu);

	switch (iTypeOfAddress.TON())
		{
		case EGsmSmsTONAlphaNumeric:
			{
			const TUint cphs1 = aPdu.GetL();
			const TUint cphs2 = aPdu.GetL();
			aPdu.UnGet();
			aPdu.UnGet();

			const TPtrC8 remainder(aPdu.Remainder());
			// we assume that it is a cphs message waiting indicator
            // we might want to do further tests tobe sure that it is a CPHS message
			if(    (length==4)
				&& ((cphs1 & 0x7E) == 0x10) // x001 000x constant value
				&& ((cphs2 & 0x7E) == 0x00) // x000 000x constant value
			  )
				{
				TGsmSmsTelNumber parsedaddress;
				parsedaddress.iTelNumber.SetLength(length);
				//Copy the two bytes of the cphs message waiting address into the parsed address
				parsedaddress.iTelNumber[0]=(TUint8)length;
				parsedaddress.iTelNumber[1]= (TUint8)iTypeOfAddress;
				parsedaddress.iTelNumber[2]= (TUint8)aPdu.GetL();
				parsedaddress.iTelNumber[3]= (TUint8)aPdu.GetL();
				DoSetParsedAddressL(parsedaddress.iTelNumber);
				}
			else if(length==11 && remainder.Left(6).CompareF(KMOSES) ==KErrNone ) //check for MOSES
				{
					DoSetParsedAddressL(KNETWORK);
					aPdu.IncL(6);
				}
			else
				{
				// Encoded length is number of semi-octets used to store address using
				// 7-bit char set - determine number of user data units required
				const TInt numUDUnits=length*4/7;
				// Unpack the data - assume max converted length twice the unconverted length
				// VEP Why this assumption, the length will be doubled at the client because of that??
				// EXT-568BMW
				// Fix is not to multiply by 2
				//NewBufferL(2*numUDUnits);
				NewBufferL(numUDUnits);
				TPtr unpackedPtr((TText*)iBuffer->Des().Ptr(),0,iBuffer->Length());

				if (remainder.Length() < KSmsAddressMaxAddressValueLength)
					User::Leave(KErrGsmuDecoding);

				TPtrC8 packedPtr(remainder.Ptr(), KSmsAddressMaxAddressValueLength);
				TSmsAlphabetPacker unpacker(TSmsDataCodingScheme::ESmsAlphabet7Bit,EFalse,0);
				unpacker.UnpackAndConvertL(iCharacterSetConverter,iFs,packedPtr,unpackedPtr,numUDUnits);
				aPdu.IncL(length / 2);
				if ((length % 2) != 0)
					aPdu.IncL();
				}
			break;
			}
		case EGsmSmsTONInternationalNumber:
		default:
			{
			TGsmSmsTelNumber parsedaddress;
			if (length>parsedaddress.iTelNumber.MaxLength())
				User::Leave(KErrGsmuDecoding);
			parsedaddress.iTelNumber.SetLength(length);
			TSmsOctet octet;
			for (TInt i=0; i<length; i++)
				{
				if ((i%2)==0)  //  even
					{
                    TUint8 tempOctet = 0;
                    octet.DecodeL(aPdu);
                    tempOctet = (TUint8)octet&0x0F; // four topmost bits set to zero
                    
                    switch(tempOctet)
                        {
                        case 10: // 1010
                            {
                            parsedaddress.iTelNumber[i] = '*';
                            }
                            break;

                        case 11: // 1011
                            {
                            parsedaddress.iTelNumber[i] = '#';
                            }
                            break; 

                        case 12: // 1100
                            {
                            parsedaddress.iTelNumber[i] = 'a';
                            }
                            break;

                        case 13: //1101
                            {
                            parsedaddress.iTelNumber[i] = 'b'; 
                            }
                            break;

                        case 14: //1110
                            {
                            parsedaddress.iTelNumber[i] = 'c';
                            }
                            break;

                        case 15: 
                            // Skip if 1111 is received 
                            break;

                        default:
                            parsedaddress.iTelNumber[i]=(TUint8) (tempOctet+'0');
                            break;
                        }
					}
				else
                    {
                    TUint8 tempOctet = 0;
                    octet&0xF0; // four least significant bits…
                    tempOctet = (TUint8)octet>>4;

                    switch(tempOctet)
                        {
                        case 10: // 1010
                            {
                            parsedaddress.iTelNumber[i] = '*';
                            }
                            break;

                        case 11: // 1011
                            {
                            parsedaddress.iTelNumber[i] = '#';
                            }
                            break;

                        case 12: // 1100
                            {
                            parsedaddress.iTelNumber[i] = 'a';
                            }
                            break;

                        case 13: // 1101
                            {
                            parsedaddress.iTelNumber[i] = 'b';
                            }
                            break;

                        case 14: // 1110
                            {
                            parsedaddress.iTelNumber[i] = 'c';
                            }
                            break;

                        case 15: 
                            // Skip if 1111 is received 
                            break;

                        default:
                            parsedaddress.iTelNumber[i]=(TUint8) (tempOctet+'0'); 
                            // unwanted bits zeroed in the beginning
       
                            break;
                        }
                    }
				}
			DoSetParsedAddressL(parsedaddress.iTelNumber);
			}
		}
	}

void CSmsAddress::InternalizeL(RReadStream& aStream)
	{
	aStream >> iTypeOfAddress;
	HBufC* buffer=HBufC::NewL(aStream,256);
	delete iBuffer;
	iBuffer=buffer;
	} // CSmsAddress::InternalizeL


void CSmsAddress::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iTypeOfAddress;
	aStream << *iBuffer;
	} // CSmsAddress::ExternalizeL


CSmsAddress::CSmsAddress(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs):
	iCharacterSetConverter(aCharacterSetConverter),
	iFs(aFs),
	iTypeOfAddress(EGsmSmsTONUnknown, EGsmSmsNPIISDNTelephoneNumberingPlan)
	{
	} // CSmsAddress::CSmsAddress


void CSmsAddress::NewBufferL(TInt aLength)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSADDRESS_NEWBUFFERL_1, "CSmsAddress::NewBufferL()");

    HBufC* buffer=HBufC::NewL(aLength);
    delete iBuffer;
    iBuffer=buffer;
    iBuffer->Des().SetLength(aLength);
    iBuffer->Des().FillZ();
	}


void CSmsAddress::DoSetParsedAddressL(const TDesC& aAddress)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSADDRESS_DOSETPARSEDADDRESSL_1, "CSmsAddress::DoSetParsedAddressL() the length of the Address [Length = %d", aAddress.Length());

	TInt length=aAddress.Length();
	if ((iTypeOfAddress.TON()==EGsmSmsTONInternationalNumber) &&
	    (length && (aAddress[0]!='+')))
		{
		NewBufferL(length+1);
		iBuffer->Des()[0]='+';
		TPtr ptr((TText*) (iBuffer->Des().Ptr()+1),length,length);
		ptr.Copy(aAddress);
		}
	else
		{
		NewBufferL(length);
		iBuffer->Des().Copy(aAddress);
		}
	} // CSmsAddress::DoSetParsedAddressL


TSmsServiceCenterTimeStamp::TSmsServiceCenterTimeStamp()
	{
	iTimeZoneNumQuarterHours = 0;
	} // TSmsServiceCenterTimeStamp::TSmsServiceCenterTimeStamp


void TSmsServiceCenterTimeStamp::SetTimeOffset(TInt aNumQuarterHours)
	{
	__ASSERT_DEBUG((aNumQuarterHours>=-KSmsMaxTimeZoneNumQuarterHours) && (aNumQuarterHours<=KSmsMaxTimeZoneNumQuarterHours),Panic(KGsmuPanicNumQuarterHoursOutOfRange));
	iTimeZoneNumQuarterHours=aNumQuarterHours;
	} // TSmsServiceCenterTimeStamp::SetTimeOffset


TUint8* TSmsServiceCenterTimeStamp::EncodeL(TUint8* aPtr) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSSERVICECENTERTIMESTAMP_ENCODEL_1, "TSmsServiceCenterTimeStamp::EncodeL()");

    TInt numquarterhours=iTimeZoneNumQuarterHours;

    TInt timeZoneOffsetInSeconds = numquarterhours * CSmsMessage::E15MinutesRepresentedInSeconds;
    TTimeIntervalSeconds offsetFromUTCToLocal(timeZoneOffsetInSeconds);

    TTime time = iTime;
    time += offsetFromUTCToLocal;

    TDateTime datetime=time.DateTime();

    TSmsOctet octet;
    octet.FillSemiOctets(datetime.Year()%100);
    aPtr=octet.EncodeL(aPtr);
    octet.FillSemiOctets(datetime.Month()+1);
    aPtr=octet.EncodeL(aPtr);
    octet.FillSemiOctets(datetime.Day()+1);
    aPtr=octet.EncodeL(aPtr);
    octet.FillSemiOctets(datetime.Hour());
    aPtr=octet.EncodeL(aPtr);
    octet.FillSemiOctets(datetime.Minute());
    aPtr=octet.EncodeL(aPtr);
    octet.FillSemiOctets(datetime.Second());
    aPtr=octet.EncodeL(aPtr);

    TInt signbit=ESmsTimeZonePositive;
    if (numquarterhours<0)
        {
        numquarterhours=-numquarterhours;
        signbit=ESmsTimeZoneNegative;
        }

    TSmsOctet timezone;
    timezone.FillSemiOctets(numquarterhours);
    timezone=timezone|signbit;
    return timezone.EncodeL(aPtr);
    } // TSmsServiceCenterTimeStamp::EncodeL


void TSmsServiceCenterTimeStamp::DecodeL(TGsmuLex8& aPdu, TInt& aTimeError)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSSERVICECENTERTIMESTAMP_DECODEL_1, "TSmsServiceCenterTimeStamp::DecodeL()");

	TSmsOctet octet;
	octet.DecodeL(aPdu);
	TInt year=octet.SemiOctetsToNum();
	if (year>95)
		year+=1900;
	else
		year+=2000;
	octet.DecodeL(aPdu);
	TInt month=octet.SemiOctetsToNum();
	octet.DecodeL(aPdu);

	TInt day=octet.SemiOctetsToNum();
	octet.DecodeL(aPdu);
	TInt hour=octet.SemiOctetsToNum();
	octet.DecodeL(aPdu);
	TInt minute=octet.SemiOctetsToNum();
	octet.DecodeL(aPdu);
	TInt second=octet.SemiOctetsToNum();

    TDateTime datetime;
    aTimeError = datetime.Set(year,(TMonth) (month-1),day-1,hour,minute,second,0);

    TSmsOctet timezone;
    timezone.DecodeL(aPdu);

    TInt signBit = (timezone&ESmsTimeZoneSignBitMask) ?  ESmsTimeZoneNegative : ESmsTimeZonePositive;
    timezone=timezone &(~ESmsTimeZoneSignBitMask); // clear sign bit

    TInt offsetToUTCInSeconds = 0;
    if (timezone.SemiOctetsToNum() <= KSmsMaxTimeZoneNumQuarterHours)
        {
        offsetToUTCInSeconds = ((TInt) timezone.SemiOctetsToNum()) * CSmsMessage::E15MinutesRepresentedInSeconds;
        if (signBit == ESmsTimeZoneNegative)    
            {
            offsetToUTCInSeconds = -offsetToUTCInSeconds;
            }

        TTimeIntervalSeconds offsetToUTC(offsetToUTCInSeconds) ;      

        if (aTimeError == KErrNone)
            {
            TTime time = datetime;    	
            time -= offsetToUTC;
            iTime = time;
    	
            iTimeZoneNumQuarterHours=(signBit == ESmsTimeZonePositive) ? timezone.SemiOctetsToNum(): -timezone.SemiOctetsToNum();
            }
            
        }
    else // Time zone out of range, set to 0 per 23.040 4.4.0 Section 9.2.3.11
        {
    	if (aTimeError == KErrNone)
            {    		
            iTime = datetime;    	
    	    iTimeZoneNumQuarterHours=0;
    	    }            
        }
	} // TSmsServiceCenterTimeStamp::DecodeL


void TSmsServiceCenterTimeStamp::InternalizeL(RReadStream& aStream)
	{
	TInt64 time;
	aStream >> time;
	iTime=time;
	iTimeZoneNumQuarterHours=aStream.ReadInt32L();
	} // TSmsServiceCenterTimeStamp::InternalizeL


void TSmsServiceCenterTimeStamp::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iTime.Int64();
	aStream.WriteInt32L(iTimeZoneNumQuarterHours);
	} // TSmsServiceCenterTimeStamp::ExternalizeL


TSmsValidityPeriod::TSmsValidityPeriod(TSmsFirstOctet& aFirstOctet):
	iFirstOctet(aFirstOctet),
	iTimeIntervalMinutes(EOneDayUnitInMinutes)
	{
	} // TSmsValidityPeriod::TSmsValidityPeriod


TTime TSmsValidityPeriod::Time() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSVALIDITYPERIOD_TIME_1, "TSmsValidityPeriod::Time()");

	TTime time;
	time.UniversalTime();
	time+=iTimeIntervalMinutes;
	return time;
	} // TSmsValidityPeriod::Time


TUint8* TSmsValidityPeriod::EncodeL(TUint8* aPtr) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSVALIDITYPERIOD_ENCODEL_1, "TSmsValidityPeriod::EncodeL()");

	TInt validityperiodformat=ValidityPeriodFormat();
	switch (validityperiodformat)
		{
		case (TSmsFirstOctet::ESmsVPFNone):
			break;
		case (TSmsFirstOctet::ESmsVPFInteger):
			{
			TInt timeintervalminutes=iTimeIntervalMinutes.Int();
			__ASSERT_DEBUG((timeintervalminutes>=EFiveMinuteUnitInMinutes) && (timeintervalminutes<=63*EOneWeekUnitLimitInMinutes),Panic(KGsmuPanicValidityPeriodOutOfRange));
			TSmsOctet octet;
			if (timeintervalminutes<=EFiveMinuteUnitLimitInMinutes)
                octet=((timeintervalminutes/EFiveMinuteUnitInMinutes)-1);
			else if (timeintervalminutes<=EHalfHourUnitLimitInMinutes)
                octet=(((timeintervalminutes-(EHalfHourUnitLimitInMinutes/2))/EHalfHourUnitInMinutes)+EFiveMinuteUnitLimit);

			else if (timeintervalminutes<=EOneDayUnitLimitInMinutes)
                octet=((timeintervalminutes/EOneDayUnitInMinutes)+166);
			else
                octet=((timeintervalminutes/EOneWeekUnitInMinutes)+192);

			aPtr=octet.EncodeL(aPtr);
			break;
			}
		case (TSmsFirstOctet::ESmsVPFSemiOctet):
			{
			TSmsServiceCenterTimeStamp timestamp;
			timestamp.SetTime(Time());

	 		TTimeIntervalSeconds timeIntervalInSeconds(User::UTCOffset());
			timestamp.SetTimeOffset(timeIntervalInSeconds.Int() / CSmsMessage::E15MinutesRepresentedInSeconds);

			aPtr=timestamp.EncodeL(aPtr);
			break;
			}
		default:
            __ASSERT_DEBUG(EFalse,Panic(KGsmuPanicUnsupportedValidityPeriodFormat));
			User::Leave(KErrGsmSMSTPVPFNotSupported);
            break;
		};
	return aPtr;
	} // TSmsValidityPeriod::EncodeL
	
TUint8* TSmsValidityPeriod::EncodeL(TUint8* aPtr, const TEncodeParams* aEncodeParams) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSVALIDITYPERIOD_ENCODEL1_1, "TSmsValidityPeriod::EncodeL()");

	TInt validityperiodformat=ValidityPeriodFormat();
	switch (validityperiodformat)
		{
		case (TSmsFirstOctet::ESmsVPFNone):
			break;
		case (TSmsFirstOctet::ESmsVPFInteger):
			{
			TInt timeintervalminutes=iTimeIntervalMinutes.Int();
			__ASSERT_DEBUG((timeintervalminutes>=EFiveMinuteUnitInMinutes) && (timeintervalminutes<=63*EOneWeekUnitLimitInMinutes),Panic(KGsmuPanicValidityPeriodOutOfRange));
			TSmsOctet octet;
			if (timeintervalminutes<=EFiveMinuteUnitLimitInMinutes)
                octet=((timeintervalminutes/EFiveMinuteUnitInMinutes)-1);
			else if (timeintervalminutes<=EHalfHourUnitLimitInMinutes)
                octet=(((timeintervalminutes-(EHalfHourUnitLimitInMinutes/2))/EHalfHourUnitInMinutes)+EFiveMinuteUnitLimit);

			else if (timeintervalminutes<=EOneDayUnitLimitInMinutes)
                octet=((timeintervalminutes/EOneDayUnitInMinutes)+166);
			else
                octet=((timeintervalminutes/EOneWeekUnitInMinutes)+192);

			aPtr=octet.EncodeL(aPtr);
			break;
			}
		case (TSmsFirstOctet::ESmsVPFSemiOctet):
			{			
			//The reason TSmsValidityPeriod::EncodeL(TUint8* aPtr, const TEncodeParams* aEncodeParams) was
			//created was to allow the CSmsMessage's time stamp to be used when generating the validity time.
			//The CSmsMessage's time stamp is typically created when the message is constructed by the SMS Stack client.
			//This means the validity time is based from the point the SMS Stack client actually sends the message, rather
			//than the SMS Stack encodes it
						
			TSmsServiceCenterTimeStamp timestamp;
			timestamp.SetTime( *aEncodeParams->iTimeStamp + iTimeIntervalMinutes ); 									    		    		    		    		    

	 		TTimeIntervalSeconds timeIntervalInSeconds( *aEncodeParams->iTimeIntervalInSeconds );
			timestamp.SetTimeOffset(timeIntervalInSeconds.Int() / CSmsMessage::E15MinutesRepresentedInSeconds);

			aPtr=timestamp.EncodeL(aPtr);
			break;
			}
		default:
            __ASSERT_DEBUG(EFalse,Panic(KGsmuPanicUnsupportedValidityPeriodFormat));
			User::Leave(KErrGsmSMSTPVPFNotSupported);
            break;
		};
	return aPtr;
	} // TSmsValidityPeriod::EncodeL	

void TSmsValidityPeriod::DecodeL(TGsmuLex8& aPdu)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSVALIDITYPERIOD_DECODEL_1, "TSmsValidityPeriod::DecodeL()");

	TInt validityperiodformat=ValidityPeriodFormat();
	switch (validityperiodformat)
		{
		case (TSmsFirstOctet::ESmsVPFNone):
			break;
		case (TSmsFirstOctet::ESmsVPFInteger):
			{
			TSmsOctet octet;
			octet.DecodeL(aPdu);
            if (octet<=EFiveMinuteUnitLimit)
                iTimeIntervalMinutes=(octet+1)*EFiveMinuteUnitInMinutes;
			else if (octet<=EHalfHourUnitLimit)
                iTimeIntervalMinutes=((EOneDayUnitInMinutes/2)+((octet-EFiveMinuteUnitLimit)*EHalfHourUnitInMinutes));
			else if (octet<=EOneDayUnitLimit)
                iTimeIntervalMinutes=(octet-166)*EOneDayUnitInMinutes;
			else
                iTimeIntervalMinutes=(octet-192)*EOneWeekUnitInMinutes;
			break;
			}
		case (TSmsFirstOctet::ESmsVPFSemiOctet):
			{
			TSmsServiceCenterTimeStamp timestamp;
			TInt timeError;
			timestamp.DecodeL(aPdu, timeError);
			User::LeaveIfError(timeError);
			TTime time;
			time.UniversalTime();
			timestamp.Time().MinutesFrom(time,iTimeIntervalMinutes);
			break;
			}
		default:
            __ASSERT_DEBUG(EFalse,Panic(KGsmuPanicUnsupportedValidityPeriodFormat));
			User::Leave(KErrGsmSMSTPVPFNotSupported);
            break;
		};
	} // TSmsValidityPeriod::DecodeL


void TSmsValidityPeriod::InternalizeL(RReadStream& aStream)
	{
	TInt timeintervalinminutes=aStream.ReadInt32L();
	iTimeIntervalMinutes=timeintervalinminutes;
	} // TSmsValidityPeriod::InternalizeL


void TSmsValidityPeriod::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iTimeIntervalMinutes.Int());
	} // TSmsValidityPeriod::ExternalizeL


CSmsInformationElement* CSmsInformationElement::NewL(TSmsInformationElementIdentifier aIdentifier,const TDesC8& aData)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSINFORMATIONELEMENT_NEWL_1, "CSmsInformationElement::NewL()");

	CSmsInformationElement* informationelement=new(ELeave) CSmsInformationElement(aIdentifier);
	CleanupStack::PushL(informationelement);
	informationelement->ConstructL(aData);
	CleanupStack::Pop();
	return informationelement;
	} // CSmsInformationElement::NewL


CSmsInformationElement* CSmsInformationElement::NewL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSINFORMATIONELEMENT_NEWL1_1, "CSmsInformationElement::NewL()");

	CSmsInformationElement* informationelement=new(ELeave) CSmsInformationElement(ESmsIEIConcatenatedShortMessages8BitReference);
	CleanupStack::PushL(informationelement);
	TPtrC8 data;
	informationelement->ConstructL(data);
	CleanupStack::Pop();
	return informationelement;
	} // CSmsInformationElement::NewL


/**
 *  Destructor.
 */
CSmsInformationElement::~CSmsInformationElement()
	{
	delete iData;
	} // CSmsInformationElement::NewL


/**
 *  Gets the Information Element data.
 *  
 *  @return Information Element data
 *  @capability None
 */
EXPORT_C TPtr8 CSmsInformationElement::Data()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSINFORMATIONELEMENT_DATA_1, "CSmsInformationElement::Data()");

	return iData->Des();
	} // CSmsInformationElement::Data


/**
 *  Gets the (const) Information Element data.
 *  
 *  @return Information Element data
 *  @capability None
 */
EXPORT_C const TDesC8& CSmsInformationElement::Data() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSINFORMATIONELEMENT_DATA1_1, "CSmsInformationElement::Data()");

	return *iData;
	} // CSmsInformationElement::Data


/**
 *  Gets the Information Element Identifier.
 *  
 *  @return Information Element Identifier
 *  @capability None
 */
EXPORT_C CSmsInformationElement::TSmsInformationElementIdentifier CSmsInformationElement::Identifier() const
	{
	return TSmsId(TInt(iIdentifier));
	} // CSmsInformationElement::TSmsInformationElementIdentifier


TUint8* CSmsInformationElement::EncodeL(TUint8* aPtr) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSINFORMATIONELEMENT_ENCODEL_1, "CSmsInformationElement::EncodeL()");

	TSmsOctet id=iIdentifier;
	aPtr=id.EncodeL(aPtr);
	TSmsOctet informationelementlength=iData->Des().Length();
	aPtr=informationelementlength.EncodeL(aPtr);
	Mem::Copy(aPtr,iData->Des().Ptr(),informationelementlength);
	aPtr+=TInt(informationelementlength);
	return aPtr;
	} // CSmsInformationElement::EncodeL


void CSmsInformationElement::DecodeL(TGsmuLex8& aPdu)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSINFORMATIONELEMENT_DECODEL_1, "CSmsInformationElement::DecodeL()");

	TSmsOctet id;
	id.DecodeL(aPdu);
	iIdentifier=static_cast<TSmsInformationElementIdentifier>((TInt)id);

	TSmsOctet informationelementlength;
	informationelementlength.DecodeL(aPdu);

 	switch(iIdentifier)
 	{
 	case (ESmsIEIConcatenatedShortMessages8BitReference):
 		if(informationelementlength !=3)
 			User::Leave(KErrGsmSMSTpduNotSupported);
 		break;
 	case (ESmsIEISpecialSMSMessageIndication):
 		if(informationelementlength !=2)
 			User::Leave(KErrGsmSMSTpduNotSupported);
 		break;
 	case (ESmsIEIApplicationPortAddressing8Bit):
 		if(informationelementlength !=2)
 			User::Leave(KErrGsmSMSTpduNotSupported);
 		break;
 	case (ESmsIEIApplicationPortAddressing16Bit):
 		if(informationelementlength !=4)
 			User::Leave(KErrGsmSMSTpduNotSupported);
 		break;
 	case (ESmsIEISMSCControlParameters):
 		if(informationelementlength !=1)
 			User::Leave(KErrGsmSMSTpduNotSupported);
 		break;
 	case (ESmsIEIUDHSourceIndicator):
 		if(informationelementlength !=1)
 			User::Leave(KErrGsmSMSTpduNotSupported);
 		break;
 	case (ESmsIEIConcatenatedShortMessages16BitReference):
 		if(informationelementlength !=4)
 			User::Leave(KErrGsmSMSTpduNotSupported);
 		break;
 	case (ESmsIEIRFC822EmailHeader):
 		if(informationelementlength !=1)
 			User::Leave(KErrGsmSMSTpduNotSupported);
 		break;
  	case (ESmsHyperLinkFormat):
 		if(informationelementlength !=4)
 			User::Leave(KErrGsmSMSTpduNotSupported);
 		break;
  	case (ESmsNationalLanguageSingleShift):
 		if(informationelementlength != 1)
 			User::Leave(KErrGsmSMSTpduNotSupported);
 		break;
  	case (ESmsNationalLanguageLockingShift):
 		if(informationelementlength != 1)
 			User::Leave(KErrGsmSMSTpduNotSupported);
 		break;
 	case (ESmsIEISIMToolkitSecurityHeaders1):
 	case (ESmsIEISIMToolkitSecurityHeaders2):
 	case (ESmsIEISIMToolkitSecurityHeaders3):
 	case (ESmsIEISIMToolkitSecurityHeaders4):
 	case (ESmsIEISIMToolkitSecurityHeaders5):
 	case (ESmsIEISIMToolkitSecurityHeaders6):
 	case (ESmsIEISIMToolkitSecurityHeaders7):
 	case (ESmsIEISIMToolkitSecurityHeaders8):
 	case (ESmsIEISIMToolkitSecurityHeaders9):
 	case (ESmsIEISIMToolkitSecurityHeaders10):
 	case (ESmsIEISIMToolkitSecurityHeaders11):
 	case (ESmsIEISIMToolkitSecurityHeaders12):
 	case (ESmsIEISIMToolkitSecurityHeaders13):
 	case (ESmsIEISIMToolkitSecurityHeaders14):
 	case (ESmsIEISIMToolkitSecurityHeaders15):
 	case (ESmsIEISIMToolkitSecurityHeaders16):
 	if(informationelementlength !=0)
 			User::Leave(KErrGsmSMSTpduNotSupported);
 		break;
 	default:
 		break;
 	}

	const TPtrC8 data(aPdu.NextAndIncL(informationelementlength));
	NewDataL(informationelementlength);
	TPtr8 ptr(iData->Des());
	ptr.Copy(data);
	} // CSmsInformationElement::DecodeL


void CSmsInformationElement::InternalizeL(RReadStream& aStream)
	{
	TSmsOctet id;
	aStream >> id;
	iIdentifier=static_cast<TSmsInformationElementIdentifier>((TInt)id);
	delete iData;
	iData=NULL;
	iData=HBufC8::NewL(aStream,CSmsUserData::KSmsMaxUserDataSize);
	} // CSmsInformationElement::InternalizeL


void CSmsInformationElement::ExternalizeL(RWriteStream& aStream) const
	{
	TSmsOctet id;
	id=(TInt)iIdentifier;
	aStream << id;
	aStream << *iData;
	} // CSmsInformationElement::ExternalizeL


void CSmsInformationElement::ConstructL(const TDesC8& aData)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSINFORMATIONELEMENT_CONSTRUCTL_1, "CSmsInformationElement::ConstructL()");

	NewDataL(aData.Length());
	iData->Des().Copy(aData);
	} // CSmsInformationElement::ConstructL


void CSmsInformationElement::NewDataL(TInt aLength)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSINFORMATIONELEMENT_NEWDATAL_1, "CSmsInformationElement::NewDataL()");

	HBufC8* data=HBufC8::NewL(aLength);
	delete iData;
	iData=data;
	iData->Des().SetLength(aLength);
	} // CSmsInformationElement::NewDataL


TUint CSmsInformationElement::Length()const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSINFORMATIONELEMENT_LENGTH_1, "CSmsInformationElement::Length()");

	return 2+iData->Length();  // 2 stands for IEID and IEDL
	} // CSmsInformationElement::Length


/**
 *  @internalComponent
 *  
 *  This method maps an information element to an index into an array of categories.
 *  
 *  @param aId
 *  The information Element Identifier.
 *  @param aIndex
 *  The index into the array of categories
 *  @return
 *  True if the information element can be mapped.
 *  False otherwise.
 */
TBool TSmsInformationElementCategories::TranslateCategoryToIndex(TInformationElementId aId, TInt& aIndex)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSINFORMATIONELEMENTCATEGORIES_TRANSLATECATEGORYTOINDEX_1, "CSmsMessage::TranslateCategoryToIndex");

    TBool rc = ETrue;

    if (aId < CSmsInformationElement::ESmsIEMaximum)
        {
        switch (aId)
            {
            case CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference:
            case CSmsInformationElement::ESmsIEIConcatenatedShortMessages16BitReference:
            case CSmsInformationElement::ESmsIEISMSCControlParameters:
            case CSmsInformationElement::ESmsIEIRFC822EmailHeader:
                {
                aIndex = EIndexCtrlMandatoryInEveryPDUButWithValueSpecificToPDU;
                break;
                }
            case CSmsInformationElement::ESmsIEISpecialSMSMessageIndication:
                {
                aIndex = EIndexCtrlMandatoryInEveryPDUMultipleInstancesPerPDU;
                break;
                }
            case CSmsInformationElement::ESmsHyperLinkFormat:
                {
                aIndex = EIndexCtrlMultipleInstancesAllowed;
                break;
                }
            case CSmsInformationElement::ESmsNationalLanguageSingleShift:
            case CSmsInformationElement::ESmsNationalLanguageLockingShift:
                {
                aIndex = EIndexCtrlOptionalInEveryPDUWithValueSpecificToPDU;
                break;
                }
            case CSmsInformationElement::ESmsReplyAddressFormat:
                {
                aIndex = EIndexCtrlMandatoryIn1stPDUOnly;
                break;
                }
            case CSmsInformationElement::ESmsEnhanceVoiceMailInformation:
                {
                aIndex = EIndexCtrlSingleInstanceOnly;
                break;
                }
            case CSmsInformationElement::ESmsEnhancedTextFormatting:
            case CSmsInformationElement::ESmsEnhancedPredefinedSound:
            case CSmsInformationElement::ESmsEnhancedUserDefinedSound:
            case CSmsInformationElement::ESmsEnhancedPredefinedAnimation:
            case CSmsInformationElement::ESmsEnhancedLargeAnimation:
            case CSmsInformationElement::ESmsEnhancedSmallAnimation:
            case CSmsInformationElement::ESmsEnhancedLargePicture:
            case CSmsInformationElement::ESmsEnhancedSmallPicture:
            case CSmsInformationElement::ESmsEnhancedVariablePicture:
            case CSmsInformationElement::ESmsEnhancedUserPromptIndicator:
            case CSmsInformationElement::ESmsEnhancedExtendedObject:
            case CSmsInformationElement::ESmsEnhancedReusedExtendedObject:
            case CSmsInformationElement::ESmsEnhancedCompressionControl:
            case CSmsInformationElement::ESmsEnhancedODI:
            case CSmsInformationElement::ESmsEnhancedStandardWVG:
            case CSmsInformationElement::ESmsEnhancedCharacterSizeWVG:
            case CSmsInformationElement::ESmsEnhancedextendedObjectDataRequest:
                {
                aIndex = EIndexEmsInformationElement;
                break;
                }
            case CSmsInformationElement::ESmsIEIReserved:
            case CSmsInformationElement::ESmsIEIValueNotUsed:
                {
                rc = EFalse;
                break;
                }
            default:
                {
                aIndex = EIndexCtrlMandatoryInEveryPDUAndWithIdenticalValues;
                break;
                }
            }
        }
    else
        {
        rc = EFalse;
        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSINFORMATIONELEMENTCATEGORIES_TRANSLATECATEGORYTOINDEX_2, "CSmsMessage::TranslateCategoryToIndex id = %d, found = %d", aId, rc);
        }
    return rc;
    } // TSmsInformationElementCategories::TranslateCategoryToIndex


/**
 *  @internalComponent
 *  
 *  This method gets an information element identifier's category.
 *  
 *  @param aId
 *  The information Element Identifier.
 *  @param aCategory
 *  The category of information element.
 *  @return
 *  ETrue if successful, EFalse if an information identifier is unknown or cannot
 *  be mapped.
 */
TBool TSmsInformationElementCategories::GetCategoryDefinition(TInformationElementId aId, TInformationElementCategory& aCategory)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSINFORMATIONELEMENTCATEGORIES_GETCATEGORYDEFINITION_1, "TSmsInformationElementCategories::GetCategoryDefinition");
    TInt index;

    if (TranslateCategoryToIndex(aId,index))
        {
        aCategory = categories[index];
        }
    else
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSINFORMATIONELEMENTCATEGORIES_GETCATEGORYDEFINITION_2, "TSmsInformationElementCategories::GetCategoryDefinition, Failure, aId = %d", aId);
        return EFalse;
        }

    return ETrue;
    } // TSmsInformationElementCategories::GetCategoryDefinition


const TSmsInformationElementCategories::TInformationElementCategory TSmsInformationElementCategories::categories[TSmsInformationElementCategories::ENumberOfIndices] =
    {
    TSmsInformationElementCategories::EEmsInformationElement,                           // EDefaultEMSIndex
    TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUAndWithIdenticalValues,   // EDefaultControlIndex
    TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUMultipleInstancesPerPDU,  // EIndexForSpecialSMSMessageIndication, Concatenated Short Messages
    TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUButWithValueSpecificToPDU,// EIndexForIRFC822EmailHeader, Application Port Addresses
    TSmsInformationElementCategories::ECtrlMandatoryIn1stPDUOnly,                       // EIndexReplyAddressFormat
    TSmsInformationElementCategories::ECtrlSingleInstanceOnly,                          // EIndexEnhanceVoiceMailInformation
    TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed                     // EIndexForHyperLinkFormat
    };


CSmsUserData* CSmsUserData::NewL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,TSmsFirstOctet& aFirstOctet,const TSmsDataCodingScheme& aDataCodingScheme)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_NEWL_1, "CSmsUserData::NewL()");

	CSmsUserData* userdata=new(ELeave) CSmsUserData(aCharacterSetConverter,aFs,aFirstOctet,aDataCodingScheme);
	CleanupStack::PushL(userdata);
	userdata->ConstructL();
	CleanupStack::Pop();
	return userdata;
	} // CSmsUserData::NewL


/**
 *  Destructor.
 */
CSmsUserData::~CSmsUserData()
    {
    iInformationElementArray.ResetAndDestroy();
    delete iBody;
    } // CSmsUserData::NewL


/**
 *  Gets an information element by index.
 *  
 *  @param aIndex Index of the information element within the User Data
 *  @return Information element
 *  @capability None
 */
EXPORT_C  CSmsInformationElement& CSmsUserData::InformationElement(TInt aIndex) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUSERDATA_INFORMATIONELEMENT_1, "CSmsUserData::InformationElement()");

	return *iInformationElementArray[aIndex];
	} // CSmsUserData::InformationElement


CSmsInformationElement*& CSmsUserData::InformationElementPtr(TInt aIndex)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_INFORMATIONELEMENTPTR_1, "CSmsUserData::InformationElementPtr()");

    return iInformationElementArray[aIndex];
    } // CSmsUserData::InformationElementPtr


/**
 *  Gets the index of an information element.
 *  
 *  @param aIdentifier An information element Identifier to search for
 *  @param aIndex The index within the User Data of the information element (if
 *  found). If more than 1 instance of this information element exists, then
 *  the 1st instance will be returned.
 *  
 *  Use InformationELementIndexL to get all elements.
 *  @return True if aIdentifier is found in the User Data
 *  @capability None
 */
EXPORT_C TBool CSmsUserData::InformationElementIndex(CSmsInformationElement::TSmsInformationElementIdentifier aIdentifier,TInt& aIndex) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUSERDATA_INFORMATIONELEMENTINDEX_1, "CSmsUserData::InformationElementIndex()");

	TBool found=EFalse;
	TInt count=NumInformationElements();
	for (TInt i=0; (!found) && (i<count); i++)
		if (InformationElement(i).Identifier()==aIdentifier)
			{
			found=ETrue;
			aIndex=i;
			}
	return found;
	} // CSmsUserData::InformationElementIndex


/**
 *  Gets the last index of an information element.
 *  
 *  @param aIdentifier An information element Identifier to search for
 *  @param aIndex The index within the User Data of the information element (if
 *  found). If more than 1 instance of this information element exists, then
 *  the last instance will be returned.
 *  
 *  Use InformationELementIndexL to get all elements.
 *  @return True if aIdentifier is found in the User Data
 *  @capability None
 */
EXPORT_C TBool CSmsUserData::InformationElementLastIndex(CSmsInformationElement::TSmsInformationElementIdentifier aIdentifier,TInt& aIndex) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUSERDATA_INFORMATIONELEMENTLASTINDEX_1, "CSmsUserData::InformationElementLastIndex()");

	TBool found=EFalse;
	TInt count=NumInformationElements();
	for (TInt i=count-1; (!found) && (i>=0); i--)
		if (InformationElement(i).Identifier()==aIdentifier)
			{
			found=ETrue;
			aIndex=i;
			}
	return found;
	} // CSmsUserData::InformationElementLastIndex


/**
 *  @internalComponent
 *  
 *  Locates every information element of type aIdentifier and
 *  stores its location index in the output array aIndices.
 *  
 *  @param aIdentifer
 *  The information element Identifier to search for
 *  @param aIndices
 *  A collection containing the location index for each information element of this type.
 */
void CSmsUserData::InformationElementIndicesL(CSmsInformationElement::TSmsInformationElementIdentifier aIdentifier, CArrayFixFlat<TInt>& aIndices) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_INFORMATIONELEMENTINDICESL_1, "CSmsUserData::InformationElementIndicesL()");

    aIndices.Reset();
    
	TInt count=NumInformationElements();
	for (TInt i=0; i<count; i++)
	    {
    	if (InformationElement(i).Identifier()==aIdentifier)
            {
            aIndices.AppendL(i);
            }
        }
    } // CSmsUserData::InformationElementIndicesL


/**
 *  Note that whilst a pointer to the aIe is passed as an
 *  input argument, the information element is still owned
 *  by the calling function, regardless of the return code.
 *  @param aInformationElement An EMS Information Element
 */
TBool CSmsUserData::EmsInformationElementWillFitL(CEmsInformationElement* aIe,CSmsEMSBufferSegmenter& aSeg,TUint& aCharsAddedToCurrentPDU)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_EMSINFORMATIONELEMENTWILLFITL_1, "CSmsUserData::EmsInformationElementWillFitL()");

	// Before using an EmsInformationElement polymorphically as an SmsIE,
	// we need to make sure that the IE has been encoded
	aIe->EncodeInformationElementL();
	iInformationElementArray.AppendL(aIe);

	TInt sizeLeft=MaxPackedUDUnitsInBodyRemaining();
	iInformationElementArray.Delete(iInformationElementArray.Count()-1);
	if(sizeLeft==0 && iBody && (iBody->Length() > 0) && (*iBody)[iBody->Length()-1] == KSms7BitAlphabetEscapeChar)
	{
		--aCharsAddedToCurrentPDU;
		--aSeg.iElementsExtracted;
		TPtr8 ptr(iBody->Des());
		ptr.Delete(iBody->Length()-1,1);
		--sizeLeft;
	}
	return sizeLeft>=0;
	} // CSmsUserData::EmsInformationElementWillFitL


/**
 *  Tests whether the control information element will fit in the current PDU.
 *  
 *  Note that whilst a pointer to the aIe is passed as an
 *  input argument, the information element is still owned
 *  by the calling function, regardless of the return code.
 *  
 *  @param aInformationElement A pointer to an information Element
 *  @capability None
 */
TBool CSmsUserData::ControlInformationElementWillFitL(CSmsInformationElement* aIe)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_CONTROLINFORMATIONELEMENTWILLFITL_1, "CSmsUserData::ControlInformationElementWillFitL()");

    if (aIe == NULL)
        {
        User::Leave(KErrGeneral);
        }

    TSmsInformationElementCategories::TInformationElementCategory category;

    if (TSmsInformationElementCategories::GetCategoryDefinition(aIe->Identifier(), category) == EFalse  ||
    	category == TSmsInformationElementCategories::EEmsInformationElement)
        {
        User::Leave(KErrArgument);
        }

    iInformationElementArray.AppendL(aIe);

    TInt sizeLeft=MaxPackedUDUnitsInBodyRemaining();
    iInformationElementArray.Delete(iInformationElementArray.Count()-1);

    // Not considering whether escape characters exist in the buffer
    // as control information elements do not add text to the buffer.

    return sizeLeft>=0;
    } // CSmsUserData::ControlInformationElementWillFitL


/**
 *  Adds an information element.
 *  This method can be used to create information elements in the range:
 *  00 - 09 - i.e. Control Information Elements
 *  It can be used to create information elements in the following ranges
 *  (on the assumption that the information element is intended to be encoded into
 *  every PDU):
 *  24 - 25 - i.e. National Language Encodings
 *  70 - 7F - i.e. SIM Tool Kit Security Headers
 *  80 - 9F - i.e  SMS to SME specific use
 *  C0 - CF - i.e. SC specific use
 *  
 *  Information elements in the following ranges should not be
 *  added using this interface. Instead they should be added using
 *  the following identified interfaces:
 *  0A-1F - Use the interfaces provided to support EMS Elements
 *  20 - Use the interfaces provided for Email in CSmsMessage
 *  21 - Use the interface provided by CSmsHyperLinkOperations
 *  22 - Use the interface provided by CSmsReplyAddressOperations
 *  23 - Use the interface provided by CSmsEnhancedVoiceMailOperations
 *  
 *  This interface should not be used for information element in the following
 *  ranges, these are reserved for future use in TS23.040 V6.5.0 and their
 *  characteristics and repeatability have not yet been defined.
 *  26-6F
 *  A0-BF
 *  E0-FF
 *  
 *  @param aIdentifier An information element Identifier for aData
 *  @param aData The information element to add to the User Data
 *  @leave
 *  KErrNotSupported if the information element is not supported via this interface.
 *  @capability None
 */
EXPORT_C void CSmsUserData::AddInformationElementL(TSmsId aIdentifier,const TDesC8& aData)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUSERDATA_ADDINFORMATIONELEMENTL_1, "CSmsUserData::AddInformationElementL");

    if  ((aIdentifier >= 0x21) && (aIdentifier <= 0x23) ||
         (aIdentifier >= 0x26) && (aIdentifier <= 0x6F) ||
         (aIdentifier >= 0xA0) && (aIdentifier <= 0xBF) ||
         (aIdentifier >= 0xE0) && (aIdentifier <= 0xFF))
        {
        User::Leave(KErrNotSupported);
        }
    UpdateInformationElementArrayL(aIdentifier, aData);

    } // CSmsUserData::AddInformationElementL


/**
 *  @internalComponent
 *  
 *  Either adds an information element to
 *  iInformationElementArray.
 *  
 *  @param aIdentifier An information element Identifier for aData
 *  @param aData The information element to add to the User Data
 */
void CSmsUserData::UpdateInformationElementArrayL(TSmsId aIdentifier,const TDesC8& aData)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_UPDATEINFORMATIONELEMENTARRAYL_1, "CSmsUserData::UpdateInformationElementsL");

    TInt count=NumInformationElements();
    if(!CEmsFactory::Supported(aIdentifier))
        {
        for (TInt i=0; i<count; i++)
            {
 	        if (InformationElement(i).Identifier()==aIdentifier)
                {
                TSmsInformationElementCategories::TInformationElementCategory category;
                
                if (TSmsInformationElementCategories::GetCategoryDefinition(aIdentifier, category) == EFalse)
                	{
                	User::Leave(KErrArgument);
                	}

                switch (category)
                    {
                    case TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUMultipleInstancesPerPDU:
                        {
                        if (InformationElement(i).Identifier() == CSmsInformationElement::ESmsIEISpecialSMSMessageIndication)
                            {
                            OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_UPDATEINFORMATIONELEMENTARRAYL_2, "CSmsUserData::AddInformationElementL1 category = %d, identifier = %d",category,aIdentifier);

                            //if Msg type is the same, swap with the most recent value
                            if ((InformationElement(i).Data()[0] & ((TUint8) EGsmSmsSpecialMessageIndicationTypeMask)) ==
                                (aData[0] & ((TUint8) EGsmSmsSpecialMessageIndicationTypeMask)))
                                {
                                User::Leave(KErrAlreadyExists);
                                }
                            }
                        else
                            {
                            OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_UPDATEINFORMATIONELEMENTARRAYL_3, "CSmsUserData::AddInformationElementL3 category = %d, identifier = %d, data = %s",category,aIdentifier, aData);
                            User::Leave(KErrArgument);
                            }
                        break;
                        }
                    case TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUAndWithIdenticalValues:
                    case TSmsInformationElementCategories::ECtrlMandatoryIn1stPDUOnly:
                    case TSmsInformationElementCategories::ECtrlSingleInstanceOnly:
                        {
                        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_UPDATEINFORMATIONELEMENTARRAYL_4, "CSmsUserData::AddInformationElementL4 category = %d, identifier = %d",category,aIdentifier);
                        User::Leave(KErrAlreadyExists);
                        break;
                        }
                    case TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed:
                        {
                        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_UPDATEINFORMATIONELEMENTARRAYL_5, "CSmsUserData::AddInformationElementL5 category = %d, identifier = %d",category,aIdentifier);
                        break;
                        }
                    case TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUButWithValueSpecificToPDU:
                        {
                        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_UPDATEINFORMATIONELEMENTARRAYL_6, "CSmsUserData::AddInformationElementL6 category = %d, identifier = %d",category,aIdentifier);
                        User::Leave(KErrAlreadyExists);
                        // currently the email header is updated in:
                        // void CSmsMessage::DecodeBufferL(CArrayPtr<CSmsPDU>& aSmsPDUArray,CSmsBufferBase& aBuffer)
                        break;
                        }
                    default:
                        {
                        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_UPDATEINFORMATIONELEMENTARRAYL_7, "CSmsUserData::AddInformationElementL8 category = %d, identifier = %d",category,aIdentifier);
                        User::Leave(KErrNotSupported);
                        break;
                        }
                    }
                }
            }
        }

    	CSmsInformationElement* informationElement=CSmsInformationElement::NewL(aIdentifier,aData);
    	CleanupStack::PushL(informationElement);
    	iInformationElementArray.AppendL(informationElement);
    	CleanupStack::Pop();
    	SetHeaderPresent(ETrue);
    } // CSmsUserData::UpdateInformationElementArrayL


void CSmsUserData::AddEmsInformationElementL(CEmsInformationElement* aIe)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_ADDEMSINFORMATIONELEMENTL_1, "CSmsUserData::AddEmsInformationElementL()");

	// Before using an EmsInformationElement polymorphically as an SmsIE,
	// we need to make sure that the IE has been encoded
	aIe->EncodeInformationElementL();
	iInformationElementArray.AppendL(aIe);
	SetHeaderPresent(ETrue);
	} // CSmsUserData::AddEmsInformationElementL


/**
 *  Removes an information element at the specified index.
 *  
 *  @param aIndex Information element index
 *  @capability None
 */
EXPORT_C void CSmsUserData::RemoveInformationElement(TInt aIndex)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUSERDATA_REMOVEINFORMATIONELEMENT_1, "CSmsUserData::RemoveInformationElement()");
	// Since iInformationElementArray[aIndex] pointer is removed from iInformationElementArray, there is no double free issue.
	// coverity[double_free]
	delete iInformationElementArray[aIndex];
	iInformationElementArray[aIndex] = NULL;
	iInformationElementArray.Delete(aIndex);

	if (NumInformationElements()==0)
		{
		SetHeaderPresent(EFalse);
		}
	} // CSmsUserData::RemoveInformationElement


TInt CSmsUserData::MaxPackedUDUnitsInBodyRemaining() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_MAXPACKEDUDUNITSINBODYREMAINING_1, "CSmsUserData::MaxPackedUDUnitsInBodyRemaining()");

	TInt totalHeaderLengthInUDLUnits=TotalHeaderLengthInUDLUnits();
	TInt maxPackedUDUnitsInBody=0;
	if (iDataCodingScheme.TextCompressed()||(iDataCodingScheme.Alphabet()!=TSmsDataCodingScheme::ESmsAlphabet7Bit))
		{
		maxPackedUDUnitsInBody=KSmsMaxUserDataSize-totalHeaderLengthInUDLUnits;
		if (iDataCodingScheme.Alphabet()==TSmsDataCodingScheme::ESmsAlphabetUCS2)
			{
			// Cannot split unicode character across PDU boundary
			maxPackedUDUnitsInBody&=~1;
			}
		}
	else // 7-bit
		{
		maxPackedUDUnitsInBody=(8*KSmsMaxUserDataSize)/7-totalHeaderLengthInUDLUnits;
		}

	if (iBody)
		return maxPackedUDUnitsInBody-=iBody->Length();
	return maxPackedUDUnitsInBody;
	} // CSmsUserData::MaxPackedUDUnitsInBodyRemaining


TInt CSmsUserData::MaxPackedUDUnitsInBodyRemaining(TUint aIELen) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_MAXPACKEDUDUNITSINBODYREMAINING1_1, "CSmsUserData::MaxPackedUDUnitsInBodyRemaining()");

	TInt totalHeaderLengthInUDLUnits=TotalHeaderLengthInUDLUnits(aIELen);
	TInt maxPackedUDUnitsInBody=0;
	if (iDataCodingScheme.TextCompressed()||(iDataCodingScheme.Alphabet()!=TSmsDataCodingScheme::ESmsAlphabet7Bit))
		{
		maxPackedUDUnitsInBody=KSmsMaxUserDataSize-totalHeaderLengthInUDLUnits;
		if (iDataCodingScheme.Alphabet()==TSmsDataCodingScheme::ESmsAlphabetUCS2)
			{
			// Cannot split unicode character across PDU boundary
			maxPackedUDUnitsInBody&=~1;
			}
		}
	else // 7-bit
		{
		maxPackedUDUnitsInBody=(8*KSmsMaxUserDataSize)/7-totalHeaderLengthInUDLUnits;
		}

	if (iBody)
		return maxPackedUDUnitsInBody-=iBody->Length();
	return maxPackedUDUnitsInBody;
	} // CSmsUserData::MaxPackedUDUnitsInBodyRemaining


/**
 *  @capability None
 */
EXPORT_C TInt CSmsUserData::MaxBodyLengthInChars() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUSERDATA_MAXBODYLENGTHINCHARS_1, "CSmsUserData::MaxBodyLengthInChars()");

	TInt totalheaderlengthinudlunits=TotalHeaderLengthInUDLUnits();
	TInt maxbodylengthinchars=0;
	if (iDataCodingScheme.TextCompressed())
		{
		maxbodylengthinchars=KSmsMaxUserDataSize-totalheaderlengthinudlunits;
		}
	else
		{
		switch (iDataCodingScheme.Alphabet())
			{
			case (TSmsDataCodingScheme::ESmsAlphabet7Bit):
				{
				maxbodylengthinchars=(8*KSmsMaxUserDataSize)/7-totalheaderlengthinudlunits;
				break;
				}
			case (TSmsDataCodingScheme::ESmsAlphabet8Bit):
				{
				maxbodylengthinchars=KSmsMaxUserDataSize-totalheaderlengthinudlunits;
				break;
				}
			case (TSmsDataCodingScheme::ESmsAlphabetUCS2):
				{
				maxbodylengthinchars=(KSmsMaxUserDataSize-totalheaderlengthinudlunits)/2;
				break;
				}
			default:
			    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUSERDATA_MAXBODYLENGTHINCHARS_2, "CSmsUserData::MaxBodyLengthInChars() WARNING! default case has been reached");
                break;
 			}
		}
	return maxbodylengthinchars;
	} // CSmsUserData::MaxBodyLengthInChars


/**
 *  Gets the unpacked User Data Elements.
 *  
 *  @return Unpacked User Data Elements
 *  @capability None
 */
EXPORT_C TPtrC8 CSmsUserData::Body() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUSERDATA_BODY_1, "CSmsUserData::Body()");

	return iBody->Des();
	} // CSmsUserData::Body


/**
 *  Sets the User Data (unpacked).
 *  
 *  @param aBody Unpacked User Data Elements
 *  @capability None
 */
EXPORT_C void CSmsUserData::SetBodyL(const TDesC8& aBody)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUSERDATA_SETBODYL_1, "CSmsUserData::SetBodyL()");

	//Some tests fail with this line in, despite it being a valid condition!
	//__ASSERT_DEBUG(aBody.Length() <= MaxBodyLengthInChars(), User::Leave(KErrTooBig));

	NewBodyL(aBody.Length());
	iBody->Des().Copy(aBody);
	} // CSmsUserData::SetBodyL


void CSmsUserData::AppendBodyL(const TDesC8& aBody)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_APPENDBODYL_1, "CSmsUserData::AppendBodyL()");

	if (iBody)
		{
		//Some tests fail with this line in, despite it being a valid condition!
		//__ASSERT_DEBUG(aBody.Length() + iBody->Length() <= MaxBodyLengthInChars(), User::Leave(KErrTooBig));

		iBody = iBody->ReAllocL(aBody.Length()+iBody->Length());
		iBody->Des().Append(aBody);
		}
	else
		{
		SetBodyL(aBody);
		}
	} // CSmsUserData::AppendBodyL


/**
 *  Tests if the character is supported by the current character set.
 *  This function can be used with 7bit and 8bit alphabets.
 * 
 *  @param aChar  Character to investigate.
 * 
 *  @return  ETrue if the character is supported.
 * 
 *  @note Since the function is based on the old behaviour (pre-PREQ2090)
 *        it does not accept a downgraded character or alternative encoding
 *        as being supported.
 *
 *  @capability None
 */
EXPORT_C TBool CSmsUserData::IsSupportedL(TChar aChar)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUSERDATA_ISSUPPORTEDL_1, "CSmsUserData::IsSupportedL()");

	TInt  numberOfUnconvertibleCharacters, numberOfDowngradedCharacters, 
	           numberRequiringAlternativeEncoding, indexOfFirstUnconvertibleCharacter;	
	TBuf<4>   toEncode;
    toEncode.SetLength(1);
    toEncode[0]=(TText)aChar;

	CSmsAlphabetConverter* converter=CSmsAlphabetConverter::NewLC(iCharacterSetConverter,iFs,iDataCodingScheme.Alphabet(),IsBinaryData());
	TBool result=converter->IsSupportedL(toEncode, ESmsEncodingNone, 
                                         numberOfUnconvertibleCharacters,
                                         numberOfDowngradedCharacters,
                                         numberRequiringAlternativeEncoding,
                                         indexOfFirstUnconvertibleCharacter);	
	CleanupStack::PopAndDestroy(converter);

	return result;
	} // CSmsUserData::IsSupportedL


/**
 *  Tests if the descriptor text is supported by the current character set.
 *  This function can be used with 7bit and 8bit alphabets.
 * 
 *  @param aDes                                 Text string to check.
 *  @param aNumberOfUnconvertibleCharacters     Exit param for the number of
 *                                              characters unconvertible.
 *  @param aIndexOfFirstUnconvertibleCharacter  Exit param for the first
 *                                              unconverted character.
 * 
 *  @return  ETrue if the character is supported.
 *
 *  @capability None
 */
EXPORT_C TBool CSmsUserData::IsSupportedL(const TDesC& aDes, TInt& aNumberOfUnconvertibleCharacters,
                                          TInt& aIndexOfFirstUnconvertibleCharacter) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUSERDATA_ISSUPPORTEDL1_1, "[1] CSmsUserData::IsSupportedL()");

	CSmsAlphabetConverter* converter=CSmsAlphabetConverter::NewLC(iCharacterSetConverter,iFs,iDataCodingScheme.Alphabet(),IsBinaryData());
	TBool result=converter->IsSupportedL(aDes, aNumberOfUnconvertibleCharacters,
			                             aIndexOfFirstUnconvertibleCharacter);
	CleanupStack::PopAndDestroy();

	return result;
	} // CSmsUserData::IsSupportedL


/**
 *  Tests if the descriptor text is supported by the current character set.
 *  This function can be used with 7bit and 8bit alphabets.
 * 
 *  @param aDes                                 Text string to check.
 *  @param aEncoding                            Alternative encoding method.
 *  @param aNumberOfUnconvertibleCharacters     Exit param for the number of
 *                                              characters unconvertible.
 *  @param aNumberOfDowngradedCharacters        Exit param for the number of
 *                                              downgraded characters.
 *  @param aNumberRequiringAlternativeEncoding  Exit param for the number of
 *                                              characters requiring use of
 *                                              the alternative encoder.
 *  @param aIndexOfFirstUnconvertibleCharacter  Exit param for the first
 *                                              unconverted character.
 * 
 *  @return  ETrue if the character is supported.
 *
 *  @capability None
 */
EXPORT_C TBool CSmsUserData::IsSupportedL(const TDesC& aDes, TSmsEncoding aEncoding,
										  TInt& aNumberOfUnconvertibleCharacters,
                                          TInt& aNumberOfDowngradedCharacters,
                                          TInt& aNumberRequiringAlternativeEncoding,
                                          TInt& aIndexOfFirstUnconvertibleCharacter) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUSERDATA_ISSUPPORTEDL2_1, "[2] CSmsUserData::IsSupportedL()");

	CSmsAlphabetConverter* converter=CSmsAlphabetConverter::NewLC(iCharacterSetConverter,iFs,iDataCodingScheme.Alphabet(),IsBinaryData());
	TBool result=converter->IsSupportedL(aDes, aEncoding,
			                             aNumberOfUnconvertibleCharacters,
			                             aNumberOfDowngradedCharacters,
			                             aNumberRequiringAlternativeEncoding,
			                             aIndexOfFirstUnconvertibleCharacter);
	CleanupStack::PopAndDestroy();

	return result;
	} // CSmsUserData::IsSupportedL


TUint8* CSmsUserData::EncodeL(TUint8* aPtr) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_ENCODEL_1, "CSmsUserData::EncodeL()");

	__ASSERT_DEBUG(0<=MaxPackedUDUnitsInBodyRemaining(),Panic(KGsmuPanicUserDataBodyTooLong));
	// Encode the user data length
	TInt totalHeaderLengthInUDLUnits=TotalHeaderLengthInUDLUnits();
	TSmsOctet userDataLength=totalHeaderLengthInUDLUnits+TSmsOctet(BodyLengthInUDLUnits());
	aPtr=userDataLength.EncodeL(aPtr);
	// Encode any user data header
	if (HeaderPresent())
		{
		TSmsOctet headerLength=HeaderLength();
		aPtr=headerLength.EncodeL(aPtr);
		TInt numInformationElements=NumInformationElements();
		for (TInt i=0; i<numInformationElements; i++)
			aPtr=iInformationElementArray[i]->EncodeL(aPtr);
		}
	// Pack the user data body
	TInt startBit=0;
	if (iDataCodingScheme.Alphabet()==TSmsDataCodingScheme::ESmsAlphabet7Bit)
		startBit=(totalHeaderLengthInUDLUnits*7)%8;
	TSmsAlphabetPacker packer(iDataCodingScheme.Alphabet(),IsBinaryData(),startBit);
	TPtr8 ptr((TUint8*)aPtr,0,packer.PackedOctetsRequiredL(Body().Length()));
	aPtr+=packer.PackL(ptr,Body());
	return aPtr;
	} // CSmsUserData::EncodeL


void CSmsUserData::DecodeL(TGsmuLex8& aPdu)
	{
	DecodeL(aPdu, EFalse);
	}

void CSmsUserData::DecodeL(TGsmuLex8& aPdu, TBool aAcceptTruncation)
	{	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_DECODEL1_1, "CSmsUserData::DecodeL()");

	// Reset current data
	iInformationElementArray.ResetAndDestroy();
	// Decode the user data
	TSmsOctet userDataLength;
	userDataLength.DecodeL(aPdu);
	TSmsOctet headerLength;
	// Decode any user data header
	TBool headerPresent=HeaderPresent();
	/*
	if (headerPresent || IsHeaderPresent(aPtr,dataLength))
	*/
	if (headerPresent)
		{
		headerLength.DecodeL(aPdu);
		if ((1+headerLength)>KSmsMaxUserDataSize)
			User::Leave(KErrGsmSMSTpduNotSupported);
		while (HeaderLength()<headerLength)
			{
			CSmsInformationElement* informationelement=CSmsInformationElement::NewL();
			CleanupStack::PushL(informationelement);
			informationelement->DecodeL(aPdu);
			iInformationElementArray.AppendL(informationelement);
			CleanupStack::Pop(informationelement);
			}
		if (HeaderLength()!=headerLength)
			User::Leave(KErrGsmSMSTpduNotSupported);
		}
	// Decode the body - make sure we have enough buffer
	TInt headerLengthInUDLUnits=TotalHeaderLengthInUDLUnits();
	TInt bodyLengthInUDLUnits=userDataLength-headerLengthInUDLUnits;

	if(bodyLengthInUDLUnits <=0)
	{
		NewBodyL(0);
		return;
	}

	NewBodyL(bodyLengthInUDLUnits);

	// Unpack the body
	TInt startBit=0;
	if (iDataCodingScheme.Alphabet()==TSmsDataCodingScheme::ESmsAlphabet7Bit)
		startBit=(headerLengthInUDLUnits*7)%8;
	TSmsAlphabetPacker unpacker(iDataCodingScheme.Alphabet(),IsBinaryData(),startBit);
	TInt bodyLengthInOctets=unpacker.PackedOctetsRequiredL(bodyLengthInUDLUnits);
	TPtr8 destPtr((TUint8*)iBody->Des().Ptr(),0,bodyLengthInUDLUnits);

	const TPtrC8 sourcePtr(aPdu.NextWithNoIncL(bodyLengthInOctets,aAcceptTruncation));
	if ( aAcceptTruncation && sourcePtr.Length() < bodyLengthInOctets)
		{
		// field was truncated in an acceptable situation (User Data in Status Report PDU)
		bodyLengthInUDLUnits = unpacker.NumUDUnitsL(sourcePtr.Length());
		}
	unpacker.UnpackL(sourcePtr,destPtr,bodyLengthInUDLUnits);

	//@note No need to call aPdu.IncL() because CSmsUserData is always at the end of a PDU
	} // CSmsUserData::DecodeL


void CSmsUserData::InternalizeL(RReadStream& aStream)
	{
	iInformationElementArray.ResetAndDestroy();
	TInt numiformationelements=aStream.ReadInt32L();
	// The "header present" flag must mirror whether information elements are present, and
	// the parental iFirstOctet should already be set by its InternalizeL()
	__ASSERT_DEBUG(((!HeaderPresent() && numiformationelements == 0) || (HeaderPresent() && numiformationelements > 0)), Panic(KGsmuPanicInformationElementIndexOutOfRange));
	for (TInt i=0; i<numiformationelements; i++)
		{
		CSmsInformationElement* informationelement=CSmsInformationElement::NewL();
		CleanupStack::PushL(informationelement);
		aStream >> *informationelement;
		iInformationElementArray.AppendL(informationelement);
		CleanupStack::Pop();
		}
	delete iBody;
	iBody=NULL;
	iBody=HBufC8::NewL(aStream,KSmsMaxUserDataLengthInChars);
	} // CSmsUserData::InternalizeL


void CSmsUserData::ExternalizeL(RWriteStream& aStream) const
	{
	TInt numiformationelements=iInformationElementArray.Count();
	aStream.WriteInt32L(numiformationelements);
	for (TInt i=0; i<numiformationelements; i++)
		aStream << *iInformationElementArray[i];
	aStream << *iBody;
	} // CSmsUserData::ExternalizeL


CSmsUserData::CSmsUserData(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,TSmsFirstOctet& aFirstOctet,const TSmsDataCodingScheme& aDataCodingScheme):
    iCharacterSetConverter(aCharacterSetConverter),
    iFs(aFs),
    iFirstOctet(aFirstOctet),
    iDataCodingScheme(aDataCodingScheme),
    iInformationElementArray(8)
    {
    } // CSmsUserData::CSmsUserData


void CSmsUserData::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_CONSTRUCTL_1, "CSmsUserData::ConstructL()");

	NewBodyL(0);
	} // CSmsUserData::ConstructL


/**
 *  Duplicates this CSmsUserData object.
 * 
 *  @return  Pointer to the newly created CSmsUserData object.
 */
CSmsUserData* CSmsUserData::DuplicateL(TSmsFirstOctet& aFirstOctet,
									   const TSmsDataCodingScheme& aDataCodingScheme) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_DUPLICATEL_1, "CSmsUserData::DuplicateL()");

	CSmsUserData*  userdata = CSmsUserData::NewL(iCharacterSetConverter, iFs,
	                                             aFirstOctet, aDataCodingScheme);
	CleanupStack::PushL(userdata);

	userdata->SetBodyL(Body());

	for (TInt ie = 0;  ie < iInformationElementArray.Count();  ie++)
		{
		CSmsInformationElement*  oldIE = iInformationElementArray[ie];

		if (CEmsFactory::Supported(oldIE->Identifier()))
			{
			CEmsInformationElement*  newIE = static_cast<CEmsInformationElement*>(oldIE)->DuplicateL();
			
			CleanupStack::PushL(newIE);
			userdata->AddEmsInformationElementL(newIE);
			CleanupStack::Pop(newIE);
			}
		else
			{
			userdata->UpdateInformationElementArrayL(oldIE->Identifier(), oldIE->Data());
			}
		}

	CleanupStack::Pop();

	return userdata;
	} // CSmsUserData::DuplicateL


TInt CSmsUserData::HeaderLength() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_HEADERLENGTH_1, "CSmsUserData::HeaderLength()");

	TInt numinformationelements=NumInformationElements();
	TInt headerlength=0;
	for (TInt i=0; i<numinformationelements; i++)
		headerlength+=iInformationElementArray[i]->Length();
	return headerlength;
	} // CSmsUserData::HeaderLength


TInt CSmsUserData::TotalHeaderLengthInUDLUnits() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_TOTALHEADERLENGTHINUDLUNITS_1, "CSmsUserData::TotalHeaderLengthInUDLUnits()");

	TInt totalheaderlengthinudlunits=0;
	if (iInformationElementArray.Count()>0)
		{
		TInt totalheaderlength=1+HeaderLength();
		if (iDataCodingScheme.TextCompressed())
			{
			totalheaderlengthinudlunits=totalheaderlength;
			}
		else
			{
			switch(iDataCodingScheme.Alphabet())
				{
				case (TSmsDataCodingScheme::ESmsAlphabet7Bit):
					{
					totalheaderlengthinudlunits=((8*totalheaderlength)+6)/7; //  Rounds up
					break;
					}
				case (TSmsDataCodingScheme::ESmsAlphabet8Bit):
				case (TSmsDataCodingScheme::ESmsAlphabetUCS2):
					{
					totalheaderlengthinudlunits=totalheaderlength;
					break;
					}
				default:
				    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_TOTALHEADERLENGTHINUDLUNITS_2, "CSmsUserData::TotalHeaderLengthInUDLUnits() WARNING default case has been reached");
                    break;
				}
			}
		}
	return totalheaderlengthinudlunits;
	} // CSmsUserData::TotalHeaderLengthInUDLUnits


TInt CSmsUserData::TotalHeaderLengthInUDLUnits(TInt aIElen) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_TOTALHEADERLENGTHINUDLUNITS1_1, "CSmsUserData::TotalHeaderLengthInUDLUnits()");

		TInt totalheaderlengthinudlunits=0;
		TInt totalheaderlength=aIElen;

		if (iInformationElementArray.Count()>0)
			totalheaderlength+=HeaderLength();

		if(totalheaderlength)totalheaderlength+=1; //UDHL

		if (iDataCodingScheme.TextCompressed())
			{
			totalheaderlengthinudlunits=totalheaderlength;
			}
		else
			{
			switch(iDataCodingScheme.Alphabet())
				{
				case (TSmsDataCodingScheme::ESmsAlphabet7Bit):
					{
					totalheaderlengthinudlunits=((8*totalheaderlength)+6)/7; //  Rounds up
					break;
					}
				case (TSmsDataCodingScheme::ESmsAlphabet8Bit):
				case (TSmsDataCodingScheme::ESmsAlphabetUCS2):
					{
					totalheaderlengthinudlunits=totalheaderlength;
					break;
					}
				default:
                    break;
				}
			}
		return totalheaderlengthinudlunits;
	} // CSmsUserData::TotalHeaderLengthInUDLUnits


TInt CSmsUserData::BodyLengthInUDLUnits() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_BODYLENGTHINUDLUNITS_1, "CSmsUserData::BodyLengthInUDLUnits()");

	return iBody->Des().Length();
	} // CSmsUserData::BodyLengthInUDLUnits


void CSmsUserData::NewBodyL(TInt aLength)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_NEWBODYL_1, "CSmsUserData::NewBodyL()");


		HBufC8* body=HBufC8::NewL(aLength);
		delete iBody;
		iBody=body;
		iBody->Des().SetLength(aLength);

	} // CSmsUserData::NewBodyL


TBool CSmsUserData::HeaderPresent() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_HEADERPRESENT_1, "CSmsUserData::HeaderPresent()");

	return (iFirstOctet&TSmsFirstOctet::ESmsUDHIMask)==TSmsFirstOctet::ESmsUDHIHeaderPresent;
	} // CSmsUserData::HeaderPresent


void CSmsUserData::SetHeaderPresent(TBool aHeaderPresent)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_SETHEADERPRESENT_1, "CSmsUserData::SetHeaderPresent()");

	iFirstOctet=aHeaderPresent? (iFirstOctet&(~TSmsFirstOctet::ESmsUDHIMask))|TSmsFirstOctet::ESmsUDHIHeaderPresent: (iFirstOctet&(~TSmsFirstOctet::ESmsUDHIMask))|TSmsFirstOctet::ESmsUDHIHeaderNotPresent;
	} // CSmsUserData::SetHeaderPresent


TBool CSmsUserData::IsBinaryData() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUSERDATA_ISBINARYDATA_1, "CSmsUserData::IsBinaryData()");

	TInt index=0;
	return (iDataCodingScheme.TextCompressed()) ||
			((iDataCodingScheme.Alphabet()==TSmsDataCodingScheme::ESmsAlphabet8Bit) &&
			(InformationElementIndex(CSmsInformationElement::ESmsIEIApplicationPortAddressing8Bit,index) ||
	        InformationElementIndex(CSmsInformationElement::ESmsIEIApplicationPortAddressing16Bit,index)));
	} // CSmsUserData::IsBinaryData


/**
 *  Converts type of number and numbering plan identification information
 *  from the type of address parameter to the NMobilePhone::TMobileTON
 *  and NMobilePhone::TMobileNPI format.
 *  
 *  @return aTon The number type
 *  @return aNpi The numbering plan
 *  
 *  @capability None
 */
EXPORT_C void TGsmSmsTypeOfAddress::ConvertToETelMM(NMobilePhone::TMobileTON& aTon,NMobilePhone::TMobileNPI& aNpi) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TGSMSMSTYPEOFADDRESS_CONVERTTOETELMM_1, "TGsmSmsTypeOfAddress::ConvertToETelMM()");

    switch (TON())
    {
		case EGsmSmsTONInternationalNumber:
            {
            aTon = (NMobilePhone::EInternationalNumber);
            break;
            }
        case EGsmSmsTONNationalNumber:
            {
            aTon = (NMobilePhone::ENationalNumber);
            break;
            }
        case EGsmSmsTONNetworkSpecificNumber:
            {
            aTon = (NMobilePhone::ENetworkSpecificNumber);
            break;
            }
        case EGsmSmsTONSubscriberNumber:
            {
            aTon = (NMobilePhone::ESubscriberNumber);
            break;
            }
        case EGsmSmsTONAlphaNumeric:
            {
            aTon = (NMobilePhone::EAlphanumericNumber);
            break;
            }
        case EGsmSmsTONAbbreviatedNumber:
            {
            aTon = (NMobilePhone::EAbbreviatedNumber);
            break;
            }

        default:
            {
            aTon = (NMobilePhone::EUnknownNumber);
            break;
            }
		}

    switch (NPI())
		{
        case EGsmSmsNPIISDNTelephoneNumberingPlan:
            {
            aNpi = (NMobilePhone::EIsdnNumberPlan);
            break;
            }
        case EGsmSmsNPIDataNumberingPlan:
            {
            aNpi = (NMobilePhone::EDataNumberPlan);
            break;
            }
        case EGsmSmsNPITelexNumberingPlan:
            {
            aNpi = (NMobilePhone::ETelexNumberPlan);
            break;
            }
        case EGsmSmsNPINationalNumberingPlan:
            {
            aNpi = (NMobilePhone::ENationalNumberPlan);
            break;
            }
        case EGsmSmsNPIPrivateNumberingPlan:
            {
            aNpi = (NMobilePhone::EPrivateNumberPlan);
            break;
            }
        case EGsmSmsNPIERMESNumberingPlan:
            {
            aNpi = (NMobilePhone::EERMESNumberPlan);
            break;
            }

        default:
            {
            aNpi = (NMobilePhone::EUnknownNumberingPlan);
            break;
            }
		}
	} // NMobilePhone::TMobileNPI


/**
 *  Converts type of number and numbering plan identification information
 *  from the NMobilePhone::TMobileTON and NMobilePhone::TMobileNPI format
 *  to the type of address parameter.
 *  
 *  @param aTon The number type
 *  @param aNpi The numbering plan
 *  
 *  @capability None
 */
EXPORT_C void TGsmSmsTypeOfAddress::SetFromETelMM(NMobilePhone::TMobileTON aTon,NMobilePhone::TMobileNPI aNpi)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TGSMSMSTYPEOFADDRESS_SETFROMETELMM_1, "TGsmSmsTypeOfAddress::SetFromETelMM()");

    switch (aTon)
    {
		case NMobilePhone::EInternationalNumber:
            {
            SetTON( EGsmSmsTONInternationalNumber );
            break;
            }
        case NMobilePhone::ENationalNumber:
            {
            SetTON( EGsmSmsTONNationalNumber );
            break;
            }
        case NMobilePhone::ENetworkSpecificNumber:
            {
            SetTON( EGsmSmsTONNetworkSpecificNumber );
            break;
            }
        case NMobilePhone::ESubscriberNumber:
            {
            SetTON( EGsmSmsTONSubscriberNumber );
            break;
            }
        case NMobilePhone::EAlphanumericNumber:
            {
            SetTON( EGsmSmsTONAlphaNumeric );
            break;
            }
        case NMobilePhone::EAbbreviatedNumber:
            {
            SetTON( EGsmSmsTONAbbreviatedNumber );
            break;
            }

        default:
            {
            SetTON( EGsmSmsTONUnknown );
            break;
            }
		}

    switch (aNpi)
		{
        case NMobilePhone::EIsdnNumberPlan:
            {
            SetNPI( EGsmSmsNPIISDNTelephoneNumberingPlan );
            break;
            }
        case NMobilePhone::EDataNumberPlan:
            {
            SetNPI( EGsmSmsNPIDataNumberingPlan );
            break;
            }
        case NMobilePhone::ETelexNumberPlan:
            {
            SetNPI( EGsmSmsNPITelexNumberingPlan );
            break;
            }
        case NMobilePhone::ENationalNumberPlan:
            {
            SetNPI( EGsmSmsNPINationalNumberingPlan );
            break;
            }
        case NMobilePhone::EPrivateNumberPlan:
            {
            SetNPI( EGsmSmsNPIPrivateNumberingPlan );
            break;
            }
        case NMobilePhone::EERMESNumberPlan:
            {
            SetNPI( EGsmSmsNPIERMESNumberingPlan );
            break;
            }

        default:
            {
            SetNPI( EGsmSmsNPIUnknown );
            break;
            }
		}
	} // NMobilePhone::TMobileTON


/**
 *  @publishedAll
 *  
 *  Indicates whether this message is a Voice Mail Notification
 *  or a Voice Mail Deletion Confirmation.
 *  
 *  @return
 *  TVoiceMailInfoType, indicating whether this message is a Voice Mail
 *  Notification or a Voice Mail Deletion Confirmation.
 *  
 *  @capability None
 */
EXPORT_C TVoiceMailInfoType CEnhancedVoiceMailBoxInformation::Type() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_TYPE_1, "CEnhancedVoiceMailBoxInformation::Type()");

	return iType;
	} // CEnhancedVoiceMailBoxInformation::Type


/**
 *  @publishedAll
 *  
 *  Sets the subscriber profile per 23.040 v6.5 Section 9.2.3.24.13.1.
 *  
 *  @param aProfile
 *  The required subscriber profile
 *  
 *  @capability None
 */
EXPORT_C void CEnhancedVoiceMailBoxInformation::SetProfile(TSmsMessageProfileType aProfile)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_SETPROFILE_1, "CEnhancedVoiceMailBoxInformation::SetProfile()");

	iProfile = aProfile;
	} // CEnhancedVoiceMailBoxInformation::SetProfile


/**
 *  @publishedAll
 *  
 *  Gets the subscriber profile per 23.040 v6.5 Section 9.2.3.24.13.1.
 *  
 *  @param aProfile
 *  The current subscriber profile
 *  
 *  @capability None
 */
EXPORT_C TSmsMessageProfileType CEnhancedVoiceMailBoxInformation::Profile() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_PROFILE_1, "CEnhancedVoiceMailBoxInformation::Profile()");

	return iProfile;
	} // CEnhancedVoiceMailBoxInformation::Profile


/**
 *  @publishedAll
 *  
 *  Configures the storage directive
 *  
 *  @param aIsStored
 *  Set to True if the SM is to be stored in the ME or USIM,
 *  False is the SM is to be discarded.
 *  
 *  @capability None
 */
EXPORT_C void CEnhancedVoiceMailBoxInformation::SetStorage(TBool aIsStored)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_SETSTORAGE_1, "CEnhancedVoiceMailBoxInformation::SetStorage()");

	iStorage = aIsStored;
	} // CEnhancedVoiceMailBoxInformation::SetStorage


/**
 *  @publishedAll
 *  
 *  Indicates whether the SM is to be stored or discarded
 *  
 *  @return
 *  True if the SM is to be stored in the ME or USIM,
 *  False is the SM is to be discarded.
 *  
 *  @capability None
 */
EXPORT_C TBool CEnhancedVoiceMailBoxInformation::Store() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_STORE_1, "CEnhancedVoiceMailBoxInformation::Store()");

	return iStorage;
	} // CEnhancedVoiceMailBoxInformation::Store


/**
 *  @publishedAll
 *  
 *  Used to set or reset the voice mail status to almost full.
 *  
 *  @param aIsStored
 *  Set to True the voice mail system is almost full.
 *  Set to False otherwise.
 *  
 *  @capability None
 */
EXPORT_C void CEnhancedVoiceMailBoxInformation::SetAlmostMaximumCapacity(TBool aIsAlmostFull)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_SETALMOSTMAXIMUMCAPACITY_1, "CEnhancedVoiceMailBoxInformation::SetAlmostMaximumCapacity()");

	iAlmostFull = aIsAlmostFull;
	} // CEnhancedVoiceMailBoxInformation::SetAlmostMaximumCapacity


/**
 *  @publishedAll
 *  
 *  Indicates whether the voice mail system is almost at full capacity.
 *  
 *  @return
 *  True, if the voice mail system is almost full.
 *  False otherwise.
 *  
 *  @capability None
 */
EXPORT_C TBool CEnhancedVoiceMailBoxInformation::AlmostMaximumCapacity() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_ALMOSTMAXIMUMCAPACITY_1, "CEnhancedVoiceMailBoxInformation::AlmostMaximumCapacity()");

	return iAlmostFull;
	} // CEnhancedVoiceMailBoxInformation::AlmostMaximumCapacity


/**
 *  @publishedAll
 *  
 *  Used to set or reset the voice mail status to full.
 *  
 *  @param aIsStored
 *  Set to True the voice mail system is full.
 *  Set to False otherwise.
 *  
 *  @capability None
 */
EXPORT_C void CEnhancedVoiceMailBoxInformation::SetMaximumCapacity(TBool aIsFull)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_SETMAXIMUMCAPACITY_1, "CEnhancedVoiceMailBoxInformation::SetMaximumCapacity()");

	iFull = aIsFull;
	} // CEnhancedVoiceMailBoxInformation::SetMaximumCapacity


/**
 *  @publishedAll
 *  
 *  Indicates whether the voice mail status is full.
 *  
 *  @return
 *  True if the voice mail system is almost full.
 *  False otherwise.
 *  
 *  @capability None
 */
EXPORT_C TBool CEnhancedVoiceMailBoxInformation::MaximumCapacity() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_MAXIMUMCAPACITY_1, "CEnhancedVoiceMailBoxInformation::MaximumCapacity()");

	return iFull;
	} // CEnhancedVoiceMailBoxInformation::MaximumCapacity


/**
 *  @publishedAll
 *  
 *  Indicates whether the message contains extension bytes
 *  
 *  @return
 *  True if the message contains extension bytes.
 *  False otherwise.
 *  
 *  @capability None
 */
EXPORT_C TBool CEnhancedVoiceMailBoxInformation::ExtensionIndicator() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_EXTENSIONINDICATOR_1, "CEnhancedVoiceMailBoxInformation::ExtensionIndicator()");

	return iExtensionIndicator;
	} // CEnhancedVoiceMailBoxInformation::ExtensionIndicator


void CEnhancedVoiceMailBoxInformation::NewBufferL(TInt aLength)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILBOXINFORMATION_NEWBUFFERL_1, "CEnhancedVoiceMailBoxInformation::NewBufferL, length = %d",aLength);

	HBufC* buffer=HBufC::NewL(aLength);
	delete iAccessAddress;
	iAccessAddress=buffer;
	iAccessAddress->Des().SetLength(aLength);
	iAccessAddress->Des().FillZ();
	} // CEnhancedVoiceMailBoxInformation::NewBufferL


/**
 *  @publishedAll
 *  
 *  Used to set the voice mail box number, overwriting any pre-existing number.
 *  
 *  @param aAddress
 *  The voice mail box address.
 *  
 *  @capability None
 */
EXPORT_C void  CEnhancedVoiceMailBoxInformation::SetAccessAddressL(const TDesC& aAddress)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_SETACCESSADDRESSL_1, "CEnhancedVoiceMailBoxInformation::SetAccessAddressL()");

	TInt length=aAddress.Length();
	NewBufferL(length);
	iAccessAddress->Des().Copy(aAddress);

	const TGsmSmsTypeOfNumber typeofnumber=length && (iAccessAddress->Des()[0]=='+')? EGsmSmsTONInternationalNumber: EGsmSmsTONUnknown;
	iTypeOfAddress.SetTON(typeofnumber);
	} // CEnhancedVoiceMailBoxInformation::SetAccessAddressL


/**
 *  @publishedAll
 *  
 *  Retrieves the voice mail box number.
 *  
 *  @return
 *  A pointer to the voice mail box number.
 *  
 *  @capability None
 */
EXPORT_C TPtrC CEnhancedVoiceMailBoxInformation::AccessAddress() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_ACCESSADDRESS_1, "CEnhancedVoiceMailBoxInformation::AccessAddress()");

	TPtrC ptr;
	if (iAccessAddress)
	    ptr.Set(iAccessAddress->Des());
	return ptr;
	} // CEnhancedVoiceMailBoxInformation::AccessAddress


/**
 *  @publishedAll
 *  
 *  Used to set the voice mail box number as a parsed address
 *  
 *  @param aParsedAddress
 *  The parsed address to be used as the voice mail box number.
 *  
 *  @capability None
 */
EXPORT_C void  CEnhancedVoiceMailBoxInformation::SetParsedAccessAddressL(const TGsmSmsTelNumber& aParsedAddress)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_SETPARSEDACCESSADDRESSL_1, "CEnhancedVoiceMailBoxInformation::SetParsedAccessAddressL()");

   	iTypeOfAddress=aParsedAddress.iTypeOfAddress;
	DoSetParsedAddressL(aParsedAddress.iTelNumber);
	} // CEnhancedVoiceMailBoxInformation::SetParsedAccessAddressL


/**
 *  @publishedAll
 *  
 *  Used to get the voice mail box number as a parsed address.
 *  
 *  @param aParsedAddress
 *  An output parameter which is set to the parsed address.
 *  
 *  @capability None
 */
EXPORT_C void  CEnhancedVoiceMailBoxInformation::ParsedAccessAddress(TGsmSmsTelNumber& aParsedAddress) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_PARSEDACCESSADDRESS_1, "CEnhancedVoiceMailBoxInformation::ParsedAccessAddress()");

	aParsedAddress.iTypeOfAddress = iTypeOfAddress;

	TInt maxparsedlength=aParsedAddress.iTelNumber.MaxLength();

	if (iTypeOfAddress.TON()==EGsmSmsTONAlphaNumeric)
		{
		TInt parsedlength=AccessAddress().Length();
		if (parsedlength>maxparsedlength)
			parsedlength=maxparsedlength;
		aParsedAddress.iTelNumber.Copy(AccessAddress().Mid(0,parsedlength));
		}
	else
		{
		aParsedAddress.iTelNumber.SetLength(maxparsedlength);

		TInt length=iAccessAddress->Des().Length();
		TInt parsedlength=0;
		for (TInt i=0; (i<length) && (parsedlength<maxparsedlength); i++)
			{
			TText ch=iAccessAddress->Des()[i];
			if ((ch>='0') && (ch<='9'))
				{
				aParsedAddress.iTelNumber[parsedlength]=(TUint8) ch;
				parsedlength++;
				}
			}

		aParsedAddress.iTelNumber.SetLength(parsedlength);
		}
	} // CEnhancedVoiceMailBoxInformation::ParsedAccessAddress


void CEnhancedVoiceMailBoxInformation::DoSetParsedAddressL(const TDesC& aAddress)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILBOXINFORMATION_DOSETPARSEDADDRESSL_1, "CEnhancedVoiceMailBoxInformation::DoSetParsedAddressL()");

	TInt length=aAddress.Length();
	if ((iTypeOfAddress.TON()==EGsmSmsTONInternationalNumber) &&
	    (length && (aAddress[0]!='+')))
		{
		NewBufferL(length+1);
		iAccessAddress->Des()[0]='+';
		TPtr ptr((TText*) (iAccessAddress->Des().Ptr()+1),length,length);
		ptr.Copy(aAddress);
		}
	else
		{
		NewBufferL(length);
		iAccessAddress->Des().Copy(aAddress);
		}
	} // CEnhancedVoiceMailBoxInformation::DoSetParsedAddressL


/**
 *  @publishedAll
 *  
 *  Sets the number of voice messages to a value in the range 0 to 255.
 *  
 *  @param aNumber
 *  The number of voice messages.
 *  
 *  @capability None
 */
EXPORT_C void   CEnhancedVoiceMailBoxInformation::SetNumberOfVoiceMessages(TUint8 aNumber)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_SETNUMBEROFVOICEMESSAGES_1, "CEnhancedVoiceMailBoxInformation::SetNumberOfVoiceMessages()");

	iNumberOfVoiceMessages=aNumber;
	} // CEnhancedVoiceMailBoxInformation::SetNumberOfVoiceMessages


/**
 *  @publishedAll
 *  
 *  Retrieves the number of voice messages that are unread.
 *  
 *  @return
 *  The number of unread voice messages.
 *  
 *  @capability None
 */
EXPORT_C TUint8 CEnhancedVoiceMailBoxInformation::NumberOfVoiceMessages() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILBOXINFORMATION_NUMBEROFVOICEMESSAGES_1, "CEnhancedVoiceMailBoxInformation::NumberOfVoiceMessages()");

	return iNumberOfVoiceMessages;
	} // CEnhancedVoiceMailBoxInformation::NumberOfVoiceMessages


TUint8* CEnhancedVoiceMailBoxInformation::EncodeL(TUint8* aPtr, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs) const
	{
	*aPtr =  (((TUint8) iType)       & EMask1Bit )       +
	        ((((TUint8) iProfile)    & EMask2Bits) << 2) +
	        ((((TUint8) iStorage)    & EMask1Bit ) << 4) +
	        ((((TUint8) iAlmostFull) & EMask1Bit ) << 5) +
	        ((((TUint8) iFull)       & EMask1Bit ) << 6) +
	         (((TUint8) iExtensionIndicator      ) << 7);

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILBOXINFORMATION_ENCODEL_1, "CEnhancedVoiceMailBoxInformation::EncodeL 1st byte = %d",*aPtr);
	aPtr++;

	// Create an address object to encode the mail box access address into the
	// format required by 23.040 v6.5.0 section 9.1.2.5.
	CSmsAddress* address = CSmsAddress::NewL(aCharacterSetConverter,aFs);
	CleanupStack::PushL(address);
	TPtrC accessAddressPtr;
	if (iAccessAddress)
	    {
		accessAddressPtr.Set(iAccessAddress->Des());
	    }
	address->SetRawAddressL(iTypeOfAddress,accessAddressPtr);
	aPtr = address->EncodeL(aPtr);
	CleanupStack::PopAndDestroy(address);

	*aPtr = (TUint8) iNumberOfVoiceMessages;
	aPtr++;

	return aPtr;
	} // CEnhancedVoiceMailBoxInformation::EncodeL


void CEnhancedVoiceMailBoxInformation::DecodeL(TGsmuLex8& aVoiceMailInfo, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs)
	{
	TUint8 Byte1 = aVoiceMailInfo.GetL();

	iType               = (TVoiceMailInfoType)       (Byte1       & EMask1Bit);
	iProfile            = (TSmsMessageProfileType ) ((Byte1 >> 2) & EMask2Bits);
	iStorage            = (TBool)                   ((Byte1 >> 4) & EMask1Bit);
	iAlmostFull         = (TBool)                   ((Byte1 >> 5) & EMask1Bit);
	iFull               = (TBool)                   ((Byte1 >> 6) & EMask1Bit);
	iExtensionIndicator = (TBool)                   ((Byte1 >> 7) & EMask1Bit);

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILBOXINFORMATION_DECODEL_1, "CEnhancedVoiceMailBoxInformation::DecodeL 1st byte = %d", Byte1);

	// Create an address object to deccode the mail box access address from the
	// format required by 23.040 v6.5.0 section 9.1.2.5.
	CSmsAddress* decodedAddress = CSmsAddress::NewL(aCharacterSetConverter,aFs);
	CleanupStack::PushL(decodedAddress);

	// CSmsAddress::DecodeL is also used to decode source and destination addresses.
	// CSmsAddress::DecodeL expects internally that alphanumeric addresses will be size
	// CSmsAddress::KSmsAddressMaxAddressValueLength=10.
	// Need to add padding bytes to bring the address field up to the maximum size.
	const TPtrC8 remainder(aVoiceMailInfo.Remainder());

	if (remainder.Length() < CSmsAddress::KSmsAddressMaxAddressLength)
	    {
	    TBuf8<CSmsAddress::KSmsAddressMaxAddressLength> data;
	    data = remainder;
	    TInt actualLength = data.Length();
	    data.SetLength(CSmsAddress::KSmsAddressMaxAddressLength);

	    for (TUint8 j = actualLength; j < CSmsAddress::KSmsAddressMaxAddressLength; j++)
	        {
	        data[j] = 0;
	        }

	    TGsmuLex8 encodedAddress(data);
	    decodedAddress->DecodeL(encodedAddress);
	    // might want to check that aVoiceMailInfo can be incremented
	    // by encodedAddress.Offset.
	    aVoiceMailInfo.Inc(encodedAddress.Offset());
	    }
	else
	    {
	    decodedAddress->DecodeL(aVoiceMailInfo);
	    }

	TInt length=(decodedAddress->Address()).Length();
	NewBufferL(length);
	iAccessAddress->Des().Copy((decodedAddress->Address().Ptr()),length);

	iTypeOfAddress = decodedAddress->TypeOfAddress();

	CleanupStack::PopAndDestroy(decodedAddress);

	iNumberOfVoiceMessages = aVoiceMailInfo.GetL();
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILBOXINFORMATION_DECODEL_2, "CEnhancedVoiceMailBoxInformation::DecodeL iNumberOfVoiceMessages = %d", iNumberOfVoiceMessages);
	} // CEnhancedVoiceMailBoxInformation::DecodeL


CEnhancedVoiceMailBoxInformation::CEnhancedVoiceMailBoxInformation()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILBOXINFORMATION_CTOR_1, "CEnhancedVoiceMailBoxInformation::CEnhancedVoiceMailBoxInformation()");

	// Consider changing this over to a Panic.
	iType               = EGsmSmsVoiceMailNotification;
	iOctet1Bit1         = EFalse;
	iProfile            = EGsmSmsProfileId1;
	iStorage            = EFalse;
	iAlmostFull         = EFalse;
	iFull               = EFalse;
	iExtensionIndicator = EFalse;
	iTypeOfAddress      = 0;
	} // CEnhancedVoiceMailBoxInformation::CEnhancedVoiceMailBoxInformation


CEnhancedVoiceMailBoxInformation::CEnhancedVoiceMailBoxInformation(TVoiceMailInfoType aTVoiceMailInfoType)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILBOXINFORMATION_CTOR1_1, "CEnhancedVoiceMailBoxInformation::CEnhancedVoiceMailBoxInformation()");

	iType               = aTVoiceMailInfoType;
	iOctet1Bit1         = EFalse;
	iProfile            = EGsmSmsProfileId1;
	iStorage            = EFalse;
	iAlmostFull         = EFalse;
	iFull               = EFalse;
	iExtensionIndicator = EFalse;
	iTypeOfAddress      = 0;
	} // CEnhancedVoiceMailBoxInformation::CEnhancedVoiceMailBoxInformation


/**
 *  @internalComponent
 *  
 *  Prevent clients from using the copy constructor by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
CEnhancedVoiceMailBoxInformation::CEnhancedVoiceMailBoxInformation(const CEnhancedVoiceMailBoxInformation&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILBOXINFORMATION_CTOR2_1, "CEnhancedVoiceMailBoxInformation::CEnhancedVoiceMailBoxInformation");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Prevent clients from using the equality operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
TBool CEnhancedVoiceMailBoxInformation::operator==(const CEnhancedVoiceMailBoxInformation&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILBOXINFORMATION_OPERATOR_1, "CEnhancedVoiceMailBoxInformation::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    return EFalse;
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
void CEnhancedVoiceMailBoxInformation::operator=(const CEnhancedVoiceMailBoxInformation&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILBOXINFORMATION_OPERATOR1_1, "CEnhancedVoiceMailBoxInformation::operator=");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    BULLSEYE_RESTORE
    }

void CEnhancedVoiceMailBoxInformation::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILBOXINFORMATION_CONSTRUCTL_1, "CEnhancedVoiceMailBoxInformation::ConstructL()");

	NewBufferL(0);
	} // CEnhancedVoiceMailBoxInformation::ConstructL


CEnhancedVoiceMailBoxInformation::~CEnhancedVoiceMailBoxInformation()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILBOXINFORMATION_DTOR_1, "CEnhancedVoiceMailBoxInformation::~CEnhancedVoiceMailBoxInformation");
	delete iAccessAddress;
	} // CEnhancedVoiceMailBoxInformation::ConstructL


CEnhancedVoiceMailBoxInformation* CEnhancedVoiceMailBoxInformation::NewL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILBOXINFORMATION_NEWL_1, "CEnhancedVoiceMailBoxInformation::NewL()");

	CEnhancedVoiceMailBoxInformation* aCEnhancedVoiceMailBoxInformation=new(ELeave) CEnhancedVoiceMailBoxInformation();
	CleanupStack::PushL(aCEnhancedVoiceMailBoxInformation);
	aCEnhancedVoiceMailBoxInformation->ConstructL();
	CleanupStack::Pop(aCEnhancedVoiceMailBoxInformation);
	return aCEnhancedVoiceMailBoxInformation;
	} // CEnhancedVoiceMailBoxInformation::NewL


/**
 *  @publishedAll
 *  
 *  Sets the message id to a value between 0 and 65535.
 *  
 *  @param aNumber
 *  The message id
 *  
 *  @capability None
 */
EXPORT_C void CVoiceMailNotification::SetMessageId(TUint16 aMessageId)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_SETMESSAGEID_1, "CVoiceMailNotification::SetMessageId()");

	iMessageId = aMessageId;
	} // CVoiceMailNotification::SetMessageId


/**
 *  @publishedAll
 *  
 *  Retrieves the message id, a value in the range 0 to 65535.
 *  
 *  @return
 *  The message id,
 *  
 *  @capability None
 */
EXPORT_C TUint16 CVoiceMailNotification::MessageId() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_MESSAGEID_1, "CVoiceMailNotification::MessageId()");

	return iMessageId;
	} // CVoiceMailNotification::MessageId


/**
 *  @publishedAll
 *  
 *  Sets the voice mail message length to a value between 0 and 255
 *  
 *  @param aLength
 *  The voice mail message length.
 *  
 *  @capability None
 */
EXPORT_C void CVoiceMailNotification::SetMessageLength(TUint8 aLength)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_SETMESSAGELENGTH_1, "CVoiceMailNotification::SetMessageLength()");

	iMessageLength=aLength;
	} // CVoiceMailNotification::SetMessageLength


/**
 *  @publishedAll
 *  
 *  Retrieves the voice mail message length, a value in the range 0 to 255.
 *  
 *  @return
 *  The voice mail message length.
 *  
 *  @capability None
 */
EXPORT_C TUint8 CVoiceMailNotification::MessageLength() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_MESSAGELENGTH_1, "CVoiceMailNotification::MessageLength()");

	return iMessageLength;
	} // CVoiceMailNotification::MessageLength


/**
 *  @publishedAll
 *  
 *  Sets the number of days that the voice message will be retained to
 *  a value between 0 and 31. Values in excess of 31 will be capped at
 *  31.
 *  
 *  @param aDays
 *  The number of retention days.
 *  
 *  @capability None
 */
EXPORT_C void CVoiceMailNotification::SetRetentionDays(TUint8 aDays)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_SETRETENTIONDAYS_1, "CVoiceMailNotification::SetRetentionDays()");

	if (aDays > 31)
	    {
	    iRetentionDays = 31;
	    }
	else
	    {
	    iRetentionDays = aDays;
	    }
	} // CVoiceMailNotification::SetRetentionDays


/**
 *  @publishedAll
 *  
 *  Retrieves the number of days the voice message will be retained.
 *  
 *  @return
 *  The number of days the voice message will be retained.
 *  
 *  @capability None
 */
EXPORT_C TUint8 CVoiceMailNotification::RetentionDays() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_RETENTIONDAYS_1, "CVoiceMailNotification::RetentionDays()");

	return iRetentionDays;
	} // CVoiceMailNotification::RetentionDays


/**
 *  @publishedAll
 *  
 *  Sets the message priority to urgent
 *  
 *  @param aPriority
 *  Set to True if the priority is urgent,
 *  Otherwise set to False.
 *  
 *  @capability None
 */
EXPORT_C void CVoiceMailNotification::SetPriorityIndication(TBool aPriority)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_SETPRIORITYINDICATION_1, "CVoiceMailNotification::SetPriorityIndication()");

	iPriorityIndication=aPriority;
	} // CVoiceMailNotification::SetPriorityIndication


/**
 *  @publishedAll
 *  
 *  Retrieves the priority indication.
 *  
 *  @return
 *  True if the priority is urgent,
 *  False otherwise.
 *  
 *  @capability None
 */
EXPORT_C TBool CVoiceMailNotification::PriorityIndication() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_PRIORITYINDICATION_1, "CVoiceMailNotification::PriorityIndication()");

	return iPriorityIndication;
	} // CVoiceMailNotification::PriorityIndication


/**
 *  @publishedAll
 *  
 *  Indicates whether the voice mail notification contains extension bytes.
 *  
 *  @return
 *  True if the voice mail notification contains extension bytes.
 *  False otherwise.
 *  
 *  @capability None
 */
EXPORT_C TBool CVoiceMailNotification::MessageExtensionIndication() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_MESSAGEEXTENSIONINDICATION_1, "CVoiceMailNotification::MessageExtensionIndication()");

	return iMessageExtensionIndicator;
	} // CVoiceMailNotification::MessageExtensionIndication


void CVoiceMailNotification::NewBufferL(TInt aLength)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILNOTIFICATION_NEWBUFFERL_1, "CVoiceMailNotification::NewBufferL()");

	HBufC* buffer=HBufC::NewL(aLength);
	delete iCallingLineIdentity;
	iCallingLineIdentity=buffer;
	iCallingLineIdentity->Des().SetLength(aLength);
	iCallingLineIdentity->Des().FillZ();
	} // CVoiceMailNotification::NewBufferL


/**
 *  @publishedAll
 *  
 *  Used to set the calling line idenity
 *  
 *  @param aLineIdentity
 *  
 *  @capability None
 */
EXPORT_C void  CVoiceMailNotification::SetCallingLineIdentityL(TDesC& aLineIdentity)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_SETCALLINGLINEIDENTITYL_1, "CVoiceMailNotification::SetCallingLineIdentityL()");

	TInt length=aLineIdentity.Length();
	NewBufferL(length);
	iCallingLineIdentity->Des().Copy(aLineIdentity);

	const TGsmSmsTypeOfNumber typeofnumber=length && (iCallingLineIdentity->Des()[0]=='+')? EGsmSmsTONInternationalNumber: EGsmSmsTONUnknown;
	iTypeOfAddress.SetTON(typeofnumber);
	} // CVoiceMailNotification::SetCallingLineIdentityL


/**
 *  @publishedAll
 *  
 *  Retrieves the Calling Line Identity
 *  
 *  @return
 *  A pointer to the Calling Line Identity.
 *  
 *  @capability None
 */
EXPORT_C TPtrC CVoiceMailNotification::CallingLineIdentity() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_CALLINGLINEIDENTITY_1, "CVoiceMailNotification::CallingLineIdentity()");

	TPtrC ptr;
	if (iCallingLineIdentity)
	    ptr.Set(iCallingLineIdentity->Des());
	return ptr;
	} // CVoiceMailNotification::CallingLineIdentity


/**
 *  @publishedAll
 *  
 *  Set the Calling Line Id as a parsed address.
 *  
 *  @param aParsedAddress
 *  The Calling Line Id as a parsed address.
 *  
 *  @capability None
 */
EXPORT_C void CVoiceMailNotification::SetParsedCallingLineIdentityL(TGsmSmsTelNumber& aParsedAddress)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_SETPARSEDCALLINGLINEIDENTITYL_1, "CVoiceMailNotification::SetParsedCallingLineIdentityL()");

	iTypeOfAddress=aParsedAddress.iTypeOfAddress;
	DoSetParsedAddressL(aParsedAddress.iTelNumber);
	} // CVoiceMailNotification::SetParsedCallingLineIdentityL


/**
 *  @publishedAll
 *  
 *  Gets the Calling Line Id as a parsed address.
 *  
 *  @param aParsedAddress
 *  An output parameter which is to be set to the parsed calling line ID.
 *  
 *  @capability None
 */
EXPORT_C void CVoiceMailNotification::ParsedCallingLineIdentity(TGsmSmsTelNumber& aParsedAddress) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_PARSEDCALLINGLINEIDENTITY_1, "CVoiceMailNotification::ParsedCallingLineIdentity()");

	aParsedAddress.iTypeOfAddress = iTypeOfAddress;

	TInt maxparsedlength=aParsedAddress.iTelNumber.MaxLength();

	if (iTypeOfAddress.TON()==EGsmSmsTONAlphaNumeric)
		{
		TInt parsedlength=CallingLineIdentity().Length();
		if (parsedlength>maxparsedlength)
			parsedlength=maxparsedlength;
		aParsedAddress.iTelNumber.Copy(CallingLineIdentity().Mid(0,parsedlength));
		}
	else
		{
		aParsedAddress.iTelNumber.SetLength(maxparsedlength);

		TInt length=iCallingLineIdentity->Des().Length();
		TInt parsedlength=0;
		for (TInt i=0; (i<length) && (parsedlength<maxparsedlength); i++)
			{
			TText ch=iCallingLineIdentity->Des()[i];
			if ((ch>='0') && (ch<='9'))
				{
				aParsedAddress.iTelNumber[parsedlength]=(TUint8) ch;
				parsedlength++;
				}
			}

		aParsedAddress.iTelNumber.SetLength(parsedlength);
		}
	} // CVoiceMailNotification::ParsedCallingLineIdentity


void CVoiceMailNotification::NewExtensionL(TInt aLength)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILNOTIFICATION_NEWEXTENSIONL_1, "CVoiceMailNotification::NewExtensionL()");

	HBufC* buffer=HBufC::NewL(aLength);
	delete iExtension;
	iExtension=buffer;
	iExtension->Des().SetLength(aLength);
	iExtension->Des().FillZ();
	} // CVoiceMailNotification::NewExtensionL

/**
 *  @internalComponent
 *  
 *  Determines the size of the encoded Voice Mail Notification.
 *  
 *  @param aCharacterSetConverter  A reference to a character set converter.
 *  @param aFs  A reference to the file server.
 *  @return The size of the encoded Voice Mail Notification
 *  
 *  @capability None
 */
TUint8 CVoiceMailNotification::SizeL(CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILNOTIFICATION_SIZEL_1, "CVoiceMailNotification::SizeL()");

	const TUint8 KTotalSizeOfFixedLengthAttributes = 4;
	TUint8 size = KTotalSizeOfFixedLengthAttributes;

	// need to find the size of the calling line ID.
	CSmsAddress* address = CSmsAddress::NewL(aCharacterSetConverter,aFs);
	CleanupStack::PushL(address);

	TPtrC callingLineIdentityPtr;
	if (iCallingLineIdentity)
	    {
		callingLineIdentityPtr.Set(iCallingLineIdentity->Des());
	    }

	address->SetRawAddressL(iTypeOfAddress,callingLineIdentityPtr);

	size += address->SizeL();
	CleanupStack::PopAndDestroy(address);

	return size;
	} // CVoiceMailNotification::SizeL


TUint8* CVoiceMailNotification::EncodeL(TUint8* aPtr, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs) const
	{
	// When changes are made to this function that affect the
	// number of bytes that are encoded, this should be reflected in
	// CVoiceMailNotification::SizeL()
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILNOTIFICATION_ENCODEL_1, "CVoiceMailNotification::EncodeL");


	*aPtr =   (TUint8)  (iMessageId                         >> 8);  // Message Id MSB
	aPtr++;
	*aPtr =   (TUint8)   iMessageId;                                // Message Id LSB
	aPtr++;
	*aPtr =   (TUint8)   iMessageLength;
	aPtr++;
	*aPtr =   ((TUint8)  (iRetentionDays & EMask5Bits))             +
	         (((TUint8)   iPriorityIndication)               << 6)  +
	         (((TUint8)   iMessageExtensionIndicator)        << 7);
	aPtr++;

	CSmsAddress* address = CSmsAddress::NewL(aCharacterSetConverter,aFs);
	CleanupStack::PushL(address);

	TPtrC callingLineIdentityPtr;
	if (iCallingLineIdentity)
	    {
		callingLineIdentityPtr.Set(iCallingLineIdentity->Des());
	    }

	address->SetRawAddressL(iTypeOfAddress,callingLineIdentityPtr);

	aPtr = address->EncodeL(aPtr);
	CleanupStack::PopAndDestroy(address);

	return aPtr;
	} // CVoiceMailNotification::EncodeL


void CVoiceMailNotification::DecodeL(TGsmuLex8& aVoiceMailInfo, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILNOTIFICATION_DECODEL_1, "CVoiceMailNotification::DecodeL");

	iMessageId = (((TUint16) aVoiceMailInfo.GetL()) << 8);
	iMessageId += ((TUint16) aVoiceMailInfo.GetL());

	iMessageLength=aVoiceMailInfo.GetL();

	TUint8 currentByte = aVoiceMailInfo.GetL();

	iRetentionDays             =   currentByte       & EMask5Bits;
	iPriorityIndication        = ((currentByte >> 6) & EMask1Bit);
	iMessageExtensionIndicator = ((currentByte >> 7) & EMask1Bit);

	CSmsAddress* decodedAddress = CSmsAddress::NewL(aCharacterSetConverter,aFs);
	CleanupStack::PushL(decodedAddress);

	const TPtrC8 remainder(aVoiceMailInfo.Remainder());
	// The address information element makes an assumption that aVoiceMail must
	// be of size CSmsAddress::KSmsAddressMaxAddressLength
	// Provide padding to ensure that this is the case.
	if (remainder.Length() < CSmsAddress::KSmsAddressMaxAddressLength)
	    {
	    TBuf8<CSmsAddress::KSmsAddressMaxAddressLength> data;
	    data = remainder;
	    TInt actualLength = data.Length();
	    data.SetLength(CSmsAddress::KSmsAddressMaxAddressLength);

	    for (TUint8 j = actualLength; j < CSmsAddress::KSmsAddressMaxAddressLength; j++)
	        {
	        data[j] = 0;
	        }

	    TGsmuLex8 encodedAddress(data);
	    decodedAddress->DecodeL(encodedAddress);
	    // Should be the last piece of data to be decoded
	    // The next line is included for completeness.
	    aVoiceMailInfo.Inc(encodedAddress.Offset());
	    }
	else
	    {
	    decodedAddress->DecodeL(aVoiceMailInfo);
	    }

	TInt length=(decodedAddress->Address()).Length();
	NewBufferL(length);
	iCallingLineIdentity->Des().Copy((decodedAddress->Address().Ptr()),length);

	iTypeOfAddress = decodedAddress->TypeOfAddress();

	CleanupStack::PopAndDestroy(decodedAddress);

	if (iMessageExtensionIndicator)
	    {
	    TUint8 extensionLength = aVoiceMailInfo.GetL();
	    aVoiceMailInfo.Inc(extensionLength);
	    }
	} // CVoiceMailNotification::DecodeL


/**
 *  @internalComponent
 *  
 *  Prevent clients from using the copy constructor by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
CVoiceMailNotification::CVoiceMailNotification(const CVoiceMailNotification&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILNOTIFICATION_CTOR_1, "CVoiceMailNotification::CVoiceMailNotification");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Prevent clients from using the equality operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
TBool CVoiceMailNotification::operator==(const CVoiceMailNotification&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILNOTIFICATION_OPERATOR_1, "CVoiceMailNotification::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    return EFalse;
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
void CVoiceMailNotification::operator=(const CVoiceMailNotification&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILNOTIFICATION_OPERATOR1_1, "CVoiceMailNotification::operator=");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    BULLSEYE_RESTORE
    }

CVoiceMailNotification::CVoiceMailNotification()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILNOTIFICATION_CTOR1_1, "CVoiceMailNotification::CVoiceMailNotification()");

	iMessageId                 = 0;
	iMessageLength             = 0;
	iRetentionDays             = 0;
	iOctetN8Bit1               = EFalse;
	iPriorityIndication        = EFalse;
	iMessageExtensionIndicator = EFalse;
	iTypeOfAddress             = 0;
	} // CVoiceMailNotification::CVoiceMailNotification


/**
 *  @publishedAll
 *  
 *  Class Destructor
 *  
 *  @capability None
 */
EXPORT_C CVoiceMailNotification::~CVoiceMailNotification()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_DTOR_1, "CVoiceMailNotification::~CVoiceMailNotification");
	delete iCallingLineIdentity;
	delete iExtension;
	} // CVoiceMailNotification::CVoiceMailNotification


void CVoiceMailNotification::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILNOTIFICATION_CONSTRUCTL_1, "CVoiceMailNotification::ConstructL()");

	NewBufferL(0);
	NewExtensionL(0);
	} // CVoiceMailNotification::ConstructL


/**
 *  @publishedAll
 *  
 *  Class construction method.
 *  
 *  @capability None
 */
EXPORT_C CVoiceMailNotification* CVoiceMailNotification::NewL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILNOTIFICATION_NEWL_1, "CVoiceMailNotification::NewL()");

	CVoiceMailNotification* aCVoiceMailNotification=new(ELeave) CVoiceMailNotification();
	CleanupStack::PushL(aCVoiceMailNotification);
	aCVoiceMailNotification->ConstructL();
	CleanupStack::Pop(aCVoiceMailNotification);
	return aCVoiceMailNotification;
	} // CVoiceMailNotification::NewL


void CVoiceMailNotification::DoSetParsedAddressL(const TDesC& aAddress)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILNOTIFICATION_DOSETPARSEDADDRESSL_1, "CVoiceMailNotification::DoSetParsedAddressL()");

	TInt length=aAddress.Length();
	if ((iTypeOfAddress.TON()==EGsmSmsTONInternationalNumber) &&
	    (length && (aAddress[0]!='+')))
		{
		NewBufferL(length+1);
		iCallingLineIdentity->Des()[0]='+';
		TPtr ptr((TText*) (iCallingLineIdentity->Des().Ptr()+1),length,length);
		ptr.Copy(aAddress);
		}
	else
		{
		NewBufferL(length);
		iCallingLineIdentity->Des().Copy(aAddress);
		}
	} // CVoiceMailNotification::DoSetParsedAddressL


/**
 *  @publishedAll
 *  
 *  Retrieves the number of voice message notifications contained in
 *  this information. The range is 0 to 15.
 *  
 *  @return
 *  The number of voice message notifications in this information element.
 *  
 *  @capability None
 */
EXPORT_C TUint8 CEnhancedVoiceMailNotification::NumberOfVoiceMails()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILNOTIFICATION_NUMBEROFVOICEMAILS_1, "CEnhancedVoiceMailNotification::NumberOfVoiceMails()");

	return (TUint8) iNotifications->Count();
	} // CEnhancedVoiceMailNotification::NumberOfVoiceMails


/**
 *  @publishedAll
 *  
 *  Provides a reference to the collection that is used to contain the Voice Mail Notifications.
 *  A maximum of 15 voice mail confirmations is supported. If more that 15 voice mails notifications are added
 *  the CEnhancedVoiceMailNotification will not be added to the CSmsMessage by the CSmsEnhancedVoiceMailOperations
 *  class.
 *  
 *  @return
 *  A reference to the collection that is used to contain the Voice Mail Notifications.
 *  
 *  @capability None
 */
EXPORT_C RPointerArray<CVoiceMailNotification>& CEnhancedVoiceMailNotification::GetVoiceMailNotifications()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILNOTIFICATION_GETVOICEMAILNOTIFICATIONS_1, "CEnhancedVoiceMailNotification::GetVoiceMailNotifications()");

	return *iNotifications;
	} // CEnhancedVoiceMailNotification::GetVoiceMailNotifications


void CEnhancedVoiceMailNotification::NewExtensionL(TInt aLength)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILNOTIFICATION_NEWEXTENSIONL_1, "CEnhancedVoiceMailNotification::NewExtensionL()");

	HBufC* buffer=HBufC::NewL(aLength);
	delete iExtension;
	iExtension=buffer;
	iExtension->Des().SetLength(aLength);
	iExtension->Des().FillZ();
	} // CEnhancedVoiceMailNotification::NewExtensionL


/**
 *  @publishedAll
 *  
 *  Class construction method.
 *  
 *  @capability None
 */
EXPORT_C  CEnhancedVoiceMailNotification* CEnhancedVoiceMailNotification::NewL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILNOTIFICATION_NEWL_1, "CEnhancedVoiceMailNotification::NewL()");

	CEnhancedVoiceMailNotification* aCEnhancedVoiceMailNotification=new(ELeave) CEnhancedVoiceMailNotification();
	CleanupStack::PushL(aCEnhancedVoiceMailNotification);
	aCEnhancedVoiceMailNotification->CEnhancedVoiceMailBoxInformation::ConstructL();
	aCEnhancedVoiceMailNotification->ConstructL();
	CleanupStack::Pop(aCEnhancedVoiceMailNotification);
	return aCEnhancedVoiceMailNotification;
	} // CEnhancedVoiceMailNotification::NewL


CEnhancedVoiceMailNotification::CEnhancedVoiceMailNotification() : CEnhancedVoiceMailBoxInformation(EGsmSmsVoiceMailNotification)
	{
	} // CEnhancedVoiceMailNotification::CEnhancedVoiceMailNotification


/**
 *  @internalComponent
 *  
 *  Prevent clients from using the copy constructor by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
CEnhancedVoiceMailNotification::CEnhancedVoiceMailNotification(const CEnhancedVoiceMailNotification&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILNOTIFICATION_CTOR1_1, "CEnhancedVoiceMailNotification::CEnhancedVoiceMailNotification");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Prevent clients from using the equality operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
TBool CEnhancedVoiceMailNotification::operator==(const CEnhancedVoiceMailNotification&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILNOTIFICATION_OPERATOR_1, "CEnhancedVoiceMailNotification::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    return EFalse;
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
void CEnhancedVoiceMailNotification::operator=(const CEnhancedVoiceMailNotification&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILNOTIFICATION_OPERATOR1_1, "CEnhancedVoiceMailNotification::operator=");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    BULLSEYE_RESTORE
    }

/**
 *  @publishedAll
 *  
 *  Class destructor.
 *  
 *  @capability None
 */
EXPORT_C  CEnhancedVoiceMailNotification::~CEnhancedVoiceMailNotification()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILNOTIFICATION_DTOR_1, "CEnhancedVoiceMailNotification::~CEnhancedVoiceMailNotification");
	delete iExtension;
	iNotifications->ResetAndDestroy();
	iNotifications->Close();
	delete iNotifications;
	} // CEnhancedVoiceMailNotification::operator


void CEnhancedVoiceMailNotification::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILNOTIFICATION_CONSTRUCTL_1, "CEnhancedVoiceMailNotification::ConstructL()");

	NewExtensionL(0);
	iNotifications = new (ELeave) RPointerArray<CVoiceMailNotification>(KMaxNumberOfNotifications);
	} // CEnhancedVoiceMailNotification::ConstructL


TUint8* CEnhancedVoiceMailNotification::EncodeL(TUint8* aCurrentPtr, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILNOTIFICATION_ENCODEL_1, "CEnhancedVoiceMailNotification::EncodeL");

	TUint8* startPtr   = aCurrentPtr;

	aCurrentPtr = CEnhancedVoiceMailBoxInformation::EncodeL(aCurrentPtr, aCharacterSetConverter, aFs);

	TUint8 count = (TUint8)  iNotifications->Count();
	if (count > KMaxNumberOfNotifications)
	    {
	    User::Leave(KErrArgument);
	    }

	*aCurrentPtr = (count & KSmsNotificationBitMask);
	aCurrentPtr++;

	TInt16 spaceAlreadyAllocated = 0; // handle architectures whose address space increments and whose address space decrements.
	(aCurrentPtr > startPtr) ? (spaceAlreadyAllocated = aCurrentPtr - startPtr) : (spaceAlreadyAllocated = startPtr - aCurrentPtr);

	TInt16 remainingSize = (TInt16)(CEnhancedVoiceMailNotification::KSmsMaxEnhancedVoiceMailSize - spaceAlreadyAllocated);
	for (TUint i = 0; i < count; i++)
	    {
	    remainingSize -= (TInt16)(*iNotifications)[i]->SizeL(aCharacterSetConverter,aFs);
	    if (remainingSize < 0)
	        {
	        User::Leave(KErrArgument);
	        }

	    aCurrentPtr = (*iNotifications)[i]->EncodeL(aCurrentPtr, aCharacterSetConverter, aFs);
	    }

	return aCurrentPtr;
	} // CEnhancedVoiceMailNotification::EncodeL


void CEnhancedVoiceMailNotification::DecodeL(TGsmuLex8& aVoiceMailInfo, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILNOTIFICATION_DECODEL_1, "CEnhancedVoiceMailNotification::DecodeL");

	CEnhancedVoiceMailBoxInformation::DecodeL(aVoiceMailInfo, aCharacterSetConverter, aFs);

	TUint8 numberOfNotifications = (aVoiceMailInfo.GetL() & KSmsNotificationBitMask);

	if (iExtensionIndicator)
	    {
	    TUint8 extensionLength = aVoiceMailInfo.GetL();
	    aVoiceMailInfo.Inc(extensionLength);
	    }

	for (TUint8 i = 0; i < numberOfNotifications; i++)
	    {
	    CVoiceMailNotification* voiceMailNotification = CVoiceMailNotification::NewL();
	    CleanupStack::PushL(voiceMailNotification);
	    voiceMailNotification->DecodeL(aVoiceMailInfo, aCharacterSetConverter, aFs);
	    iNotifications->AppendL(voiceMailNotification);
	    CleanupStack::Pop(voiceMailNotification);
	    }
	} // CEnhancedVoiceMailNotification::DecodeL


/**
 *  @publishedAll
 *  
 *  Sets the message ID of the specific Voice Mail message
 *  whose deletion is being confirmed.
 *  
 *  @param aMessageId
 *  The message ID of the specific voice mail message whose deletion
 *  is being confirmed.
 *  
 *  @capability None
 */
EXPORT_C void CVoiceMailDeletion::SetMessageId(TUint16 aMessageId)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILDELETION_SETMESSAGEID_1, "CVoiceMailDeletion::SetMessageId()");

	iMessageId=aMessageId;
	} // CVoiceMailDeletion::SetMessageId


/**
 *  @publishedAll
 *  
 *  Retrieves the message ID of the Voice Mail message
 *  whose deletion is being confirmed, range 0 to 65535.
 *  
 *  @return
 *  The message ID of the Voice Mail message whose deletion
 *  is being confirmed, range 0 to 65535.
 *  
 *  @capability None
 */
EXPORT_C TUint16 CVoiceMailDeletion::MessageId() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILDELETION_MESSAGEID_1, "CVoiceMailDeletion::MessageId()");

	return iMessageId;
	} // CVoiceMailDeletion::MessageId


/**
 *  @publishedAll
 *  
 *  Indicates whether the voice mail deletion contains extension bytes.
 *  
 *  @return
 *  True if the voice mail deletion contains extension bytes.
 *  False otherwise.
 *  
 *  @capability None
 */
EXPORT_C TBool CVoiceMailDeletion::MessageExtensionIndication() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILDELETION_MESSAGEEXTENSIONINDICATION_1, "CVoiceMailDeletion::MessageExtensionIndication()");

	return iExtensionIndicator;
	} // CVoiceMailDeletion::MessageExtensionIndication


TUint8 CVoiceMailDeletion::SizeL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILDELETION_SIZEL_1, "CVoiceMailDeletion::SizeL()");

	const TUint8 KSizeOfVoiceMailDeletion = 3;
	return KSizeOfVoiceMailDeletion;
	} // CVoiceMailDeletion::SizeL


TUint8* CVoiceMailDeletion::EncodeL(TUint8* aPtr) const
	{
	// When changes are made which affect the
	// number of bytes encoded, this should be
	// reflected in VoiceMailDeletion::SizeL()
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILDELETION_ENCODEL_1, "CVoiceMailDeletion::EncodeL");

	*aPtr =  (TUint8) (iMessageId >> 8);
	aPtr++;
	*aPtr =  (TUint8) iMessageId;
	aPtr++;
	*aPtr = ((TUint8) iExtensionIndicator) << 7;
	aPtr++;
	return aPtr;
	} // CVoiceMailDeletion::EncodeL


void CVoiceMailDeletion::DecodeL(TGsmuLex8& aVoiceMailInfo)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILDELETION_DECODEL_1, "CVoiceMailDeletion::DecodeL");

	iMessageId = (((TUint16) aVoiceMailInfo.GetL()) << 8) +
	              ((TUint16) aVoiceMailInfo.GetL());
	iExtensionIndicator =    (aVoiceMailInfo.GetL() >> 7);

	if (iExtensionIndicator)
	    {
	    TUint8 extensionLength = aVoiceMailInfo.GetL();
	    aVoiceMailInfo.Inc(extensionLength);
	    }
	} // CVoiceMailDeletion::DecodeL


CVoiceMailDeletion::CVoiceMailDeletion()
	{
	iMessageId = 0;
	iExtensionIndicator = EFalse;
	} // CVoiceMailDeletion::CVoiceMailDeletion


/**
 *  @internalComponent
 *  
 *  Prevent clients from using the copy constructor by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
CVoiceMailDeletion::CVoiceMailDeletion(const CVoiceMailDeletion&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILDELETION_CTOR1_1, "CVoiceMailDeletion::CVoiceMailDeletion");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Prevent clients from using the equality operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
TBool CVoiceMailDeletion::operator==(const CVoiceMailDeletion&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILDELETION_OPERATOR_1, "CVoiceMailDeletion::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    return EFalse;
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
void CVoiceMailDeletion::operator=(const CVoiceMailDeletion&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILDELETION_OPERATOR1_1, "CVoiceMailDeletion::operator=");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    BULLSEYE_RESTORE
    }

/**
 *  @publishedAll
 *  
 *  Class destructor.
 *  
 *  @capability None
 */
EXPORT_C CVoiceMailDeletion::~CVoiceMailDeletion()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILDELETION_DTOR_1, "CVoiceMailDeletion::~CVoiceMailDeletion");

	delete iExtension;
	} // CVoiceMailDeletion::operator


void CVoiceMailDeletion::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILDELETION_CONSTRUCTL_1, "CVoiceMailDeletion::ConstructL()");

	NewBufferL(0);
	} // CVoiceMailDeletion::ConstructL


void CVoiceMailDeletion::NewBufferL(TInt aLength)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CVOICEMAILDELETION_NEWBUFFERL_1, "CVoiceMailDeletion::NewBufferL()");

	HBufC* buffer=HBufC::NewL(aLength);
	delete iExtension;
	iExtension=buffer;
	iExtension->Des().SetLength(aLength);
	iExtension->Des().FillZ();
	} // CVoiceMailDeletion::NewBufferL


/**
 *  @publishedAll
 *  
 *  Class constructor
 *  
 *  @capability None
 */
EXPORT_C CVoiceMailDeletion* CVoiceMailDeletion::NewL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CVOICEMAILDELETION_NEWL_1, "CVoiceMailDeletion::NewL()");

	CVoiceMailDeletion* voiceMailDeletion=new(ELeave) CVoiceMailDeletion();
	CleanupStack::PushL(voiceMailDeletion);
	voiceMailDeletion->ConstructL();
	CleanupStack::Pop(voiceMailDeletion);
	return voiceMailDeletion;
	} // CVoiceMailDeletion::NewL


void CEnhancedVoiceMailDeleteConfirmations::NewExtensionL(TInt aLength)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILDELETECONFIRMATIONS_NEWEXTENSIONL_1, "CEnhancedVoiceMailDeleteConfirmations::NewExtensionL()");

	HBufC* buffer=HBufC::NewL(aLength);
	delete iExtension;
	iExtension=buffer;
	iExtension->Des().SetLength(aLength);
	iExtension->Des().FillZ();
	} // CEnhancedVoiceMailDeleteConfirmations::NewExtensionL


CEnhancedVoiceMailDeleteConfirmations::CEnhancedVoiceMailDeleteConfirmations() : CEnhancedVoiceMailBoxInformation(EGsmSmsVoiceMailDeleteConfirmation)
	{
	//NOP
	} // CEnhancedVoiceMailDeleteConfirmations::CEnhancedVoiceMailDeleteConfirmations


/**
 *  @publishedAll
 *  
 *  Class destructor
 *  
 *  @capability None
 */
EXPORT_C  CEnhancedVoiceMailDeleteConfirmations::~CEnhancedVoiceMailDeleteConfirmations()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILDELETECONFIRMATIONS_DTOR_1, "CEnhancedVoiceMailDeleteConfirmations::~CEnhancedVoiceMailDeleteConfirmations");

	delete iExtension;
	iVoiceMailDeletions->ResetAndDestroy();
	iVoiceMailDeletions->Close();
	delete iVoiceMailDeletions;
	} // CEnhancedVoiceMailDeleteConfirmations::CEnhancedVoiceMailDeleteConfirmations


/**
 *  @internalComponent
 *  
 *  Prevent clients from using the copy constructor by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
CEnhancedVoiceMailDeleteConfirmations::CEnhancedVoiceMailDeleteConfirmations(const CEnhancedVoiceMailDeleteConfirmations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILDELETECONFIRMATIONS_CTOR1_1, "CEnhancedVoiceMailDeleteConfirmations::CEnhancedVoiceMailDeleteConfirmations");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Prevent clients from using the equality operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
TBool CEnhancedVoiceMailDeleteConfirmations::operator==(const CEnhancedVoiceMailDeleteConfirmations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILDELETECONFIRMATIONS_OPERATOR_1, "CEnhancedVoiceMailDeleteConfirmations::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    return EFalse;
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
void CEnhancedVoiceMailDeleteConfirmations::operator=(const CEnhancedVoiceMailDeleteConfirmations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILDELETECONFIRMATIONS_OPERATOR1_1, "CEnhancedVoiceMailDeleteConfirmations::operator=");
    Panic(KGsmuPanicMethodBodyNotImplemented);
    BULLSEYE_RESTORE
    }

void CEnhancedVoiceMailDeleteConfirmations::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILDELETECONFIRMATIONS_CONSTRUCTL_1, "CEnhancedVoiceMailDeleteConfirmations::ConstructL()");

	NewExtensionL(0);

	iVoiceMailDeletions = new (ELeave) RPointerArray<CVoiceMailDeletion>(15);
	} // CEnhancedVoiceMailDeleteConfirmations::ConstructL


/**
 *  @publishedAll
 *  
 *  Class constructor
 *  
 *  @capability None
 */
EXPORT_C  CEnhancedVoiceMailDeleteConfirmations* CEnhancedVoiceMailDeleteConfirmations::NewL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILDELETECONFIRMATIONS_NEWL_1, "CEnhancedVoiceMailDeleteConfirmations::NewL()");

	CEnhancedVoiceMailDeleteConfirmations* aCEnhancedVoiceMailDeleteConfirmations=new(ELeave) CEnhancedVoiceMailDeleteConfirmations();
	CleanupStack::PushL(aCEnhancedVoiceMailDeleteConfirmations);
	aCEnhancedVoiceMailDeleteConfirmations->CEnhancedVoiceMailBoxInformation::ConstructL();
	aCEnhancedVoiceMailDeleteConfirmations->ConstructL();
	CleanupStack::Pop(aCEnhancedVoiceMailDeleteConfirmations);
	return aCEnhancedVoiceMailDeleteConfirmations;
	} // CEnhancedVoiceMailDeleteConfirmations::NewL


/**
 *  @publishedAll
 *  
 *  Indicates the number of message IDs that follow in this IE.
 *  
 *  @return
 *  The number of message IDs that follow.
 *  
 *  @capability None
 */
EXPORT_C TUint8  CEnhancedVoiceMailDeleteConfirmations::NumberOfDeletes()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILDELETECONFIRMATIONS_NUMBEROFDELETES_1, "CEnhancedVoiceMailDeleteConfirmations::NumberOfDeletes()");

	return iVoiceMailDeletions->Count();
	} // CEnhancedVoiceMailDeleteConfirmations::NumberOfDeletes


/**
 *  @publishedAll
 *  
 *  Provides a reference to the collection that is used to contain the Voice Mail Deletion
 *  notifications. Up to 31 instances of CVoiceMailDeletion can be stored. If more than
 *  31 instances are added, the CEnhancedVoiceMailDeleteConfirmations will not be added to
 *  the CSmsMessage.
 *  
 *  @return
 *  A reference to the collection that is used to contain the Voice Mail Deletion
 *  notifications.
 *  
 *  @capability None
 */
EXPORT_C RPointerArray<CVoiceMailDeletion>& CEnhancedVoiceMailDeleteConfirmations::GetVoiceMailDeletions()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CENHANCEDVOICEMAILDELETECONFIRMATIONS_GETVOICEMAILDELETIONS_1, "CEnhancedVoiceMailDeleteConfirmations::GetVoiceMailDeletions()");

	return *iVoiceMailDeletions;
	} // CEnhancedVoiceMailDeleteConfirmations::GetVoiceMailDeletions


TUint8* CEnhancedVoiceMailDeleteConfirmations::EncodeL(TUint8* aCurrentPtr, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILDELETECONFIRMATIONS_ENCODEL_1, "CEnhancedVoiceMailDeleteConfirmations::EncodeL");

	TUint8* startPtr   = aCurrentPtr;

	aCurrentPtr = CEnhancedVoiceMailBoxInformation::EncodeL(aCurrentPtr, aCharacterSetConverter, aFs);

	TUint8 count = ((TUint8)iVoiceMailDeletions->Count());

	if (count > KMaxNumberOfNotifications)
	    {
	    User::Leave(KErrArgument);
	    }

	*aCurrentPtr= (count & KSmsNotificationBitMask);
	aCurrentPtr++;

	TInt16 spaceAlreadyAllocated; // handle architectures whose address space increments and whose address space decrements.
	(aCurrentPtr > startPtr) ? (spaceAlreadyAllocated = aCurrentPtr - startPtr) : (spaceAlreadyAllocated = startPtr - aCurrentPtr);

	// allow space for id and length bytes
	TInt16 remainingSize = (TInt16)(CEnhancedVoiceMailBoxInformation::KSmsMaxEnhancedVoiceMailSize - spaceAlreadyAllocated);

	for (TUint i = 0; i < count; i++)
	    {
	    remainingSize -= (TInt16)(*iVoiceMailDeletions)[i]->SizeL();
	    if (remainingSize < 0)
	        {
	        User::Leave(KErrArgument);
	        }

	    aCurrentPtr = (*iVoiceMailDeletions)[i]->EncodeL(aCurrentPtr);
	    }

	return aCurrentPtr;
	} // CEnhancedVoiceMailDeleteConfirmations::EncodeL


void CEnhancedVoiceMailDeleteConfirmations::DecodeL(TGsmuLex8& aVoiceMailInfo, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CENHANCEDVOICEMAILDELETECONFIRMATIONS_DECODEL_1, "CEnhancedVoiceMailDeleteConfirmations::DecodeL");

	CEnhancedVoiceMailBoxInformation::DecodeL(aVoiceMailInfo, aCharacterSetConverter, aFs);

	TUint numberOfVMDeletions = (aVoiceMailInfo.GetL() & KSmsNotificationBitMask);

	if (iExtensionIndicator)
		{
		TUint8 extensionLength = aVoiceMailInfo.GetL();
		aVoiceMailInfo.Inc(extensionLength);
		}

	for (TUint8 i = 0; i < numberOfVMDeletions; i++)
		{
		// Create a Voice Mail Deletion
		CVoiceMailDeletion* voiceMailDeletion = CVoiceMailDeletion::NewL();
		CleanupStack::PushL(voiceMailDeletion);
		voiceMailDeletion->DecodeL(aVoiceMailInfo);
		CleanupStack::Pop(voiceMailDeletion);
		iVoiceMailDeletions->Append(voiceMailDeletion);
		}
	} // CEnhancedVoiceMailDeleteConfirmations::DecodeL
