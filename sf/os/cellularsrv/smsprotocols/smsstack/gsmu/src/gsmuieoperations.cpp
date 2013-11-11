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
//


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "gsmuieoperationsTraces.h"
#endif

#include "Gsmumain.h"
#include "gsmuieoperations.h"
#include "Gsmumsg.h"
#include "gsmumsgadditionalattributes.h"
#include "Gsmuelem.h"
#include "smsstacklog.h"


/**
 *  This is used by the CSmsMessage to create a CSmsIEOperation.
 *  Each instance is owned by the CSmsMessage which created it.
 *  Therefore only the CSmsMessage (GSMU component) is allowed to
 *  create and delete CSmsIEOperations.
 *  
 *  @param aId
 *  The ID for the CSmsInformationElement for whom an operation class needs to be created.
 *  @param aMessage
 *  The CSmsMessage that this operations class belongs to.
 *  @param aCharacterSetConverter
 *  A reference to a character set converter, needed by some derived operation classes.
 *  @param aFs
 *  A reference to the file server, needed by some derived operations classes.
 *  
 *  @internalComponent
 */
CSmsIEOperation*  CSmsIEOperation::NewL(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSIEOPERATION_NEWL_1, "CSmsIEOperation::NewL()");

	CSmsIEOperation* iEOperation = NULL;

	switch (aId)
	    {
	    case CSmsInformationElement::ESmsHyperLinkFormat:
	        iEOperation = new (ELeave)  CSmsHyperLinkOperations(aId,aMessage);
	        break;
	    case CSmsInformationElement::ESmsReplyAddressFormat:
	        iEOperation = new (ELeave) CSmsReplyAddressOperations(aId, aMessage, aCharacterSetConverter, aFs);
	        break;
	    case CSmsInformationElement::ESmsEnhanceVoiceMailInformation:
	        iEOperation = new (ELeave) CSmsEnhancedVoiceMailOperations(aId, aMessage, aCharacterSetConverter, aFs);
	        break;
	    case CSmsInformationElement::ESmsIEISpecialSMSMessageIndication:
	        iEOperation = new (ELeave)  CSmsSpecialSMSMessageOperations(aId,aMessage);
	        break;
	    case CSmsInformationElement::ESmsIEISMSCControlParameters:
	        iEOperation = new (ELeave)  CSmsSMSCCtrlParameterOperations(aId,aMessage); 
	        break;
	    default:
	        User::Leave(KErrArgument);
	        break;
	    }

	iEOperation->ConstructL();

	return iEOperation;
	} // CSmsIEOperation::NewL


/**
 *  @publishedAll
 *  
 *  Identifies the information element which is supported by this operations class.
 *  
 *  @return The information element ID supported by this operations class.
 *  @capability None
 */
EXPORT_C CSmsInformationElement::TSmsInformationElementIdentifier CSmsIEOperation::Id() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSIEOPERATION_ID_1, "CSmsIEOperation::Id()");

	return iId;
	} // CSmsInformationElement::TSmsInformationElementIdentifier


/**
 *  @internalComponent
 *  
 *  Identifies whether the message type supports is supported
 *  
 *  @return True if the message type is supported. False otherwise.
 */
TBool CSmsIEOperation::MessageTypeSupported() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSIEOPERATION_MESSAGETYPESUPPORTED_1, "CSmsIEOperation::MessageTypeSupported()");

	CSmsPDU::TSmsPDUType type = iMessage.Type();

    return ((type == CSmsPDU::ESmsDeliver) ||
            (type == CSmsPDU::ESmsSubmit));
	} // CSmsIEOperation::MessageTypeSupported


CSmsIEOperation::CSmsIEOperation(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage) : iId(aId), iMessage(aMessage)
	{
	//NOP
	} // CSmsIEOperation::CSmsIEOperation


/**
 *  @internalComponent
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
void CSmsIEOperation::operator=(const CSmsIEOperation&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSIEOPERATION_OPERATOR_1, "CSmsIEOperation::operator=");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
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
TBool CSmsIEOperation::operator==(const CSmsIEOperation&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSIEOPERATION_OPERATOR1_1, "CSmsIEOperation::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
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
void CSmsCtrlOperation::operator=(const CSmsCtrlOperation&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCTRLOPERATION_OPERATOR_1, "CSmsCtrlOperation::operator=");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
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
TBool CSmsCtrlOperation::operator==(const CSmsCtrlOperation&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCTRLOPERATION_OPERATOR1_1, "CSmsCtrlOperation::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
    return EFalse;
    BULLSEYE_RESTORE
    }

CSmsCtrlOperation::CSmsCtrlOperation(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage) : CSmsIEOperation(aId, aMessage)
	{
	//NOP
	} // CSmsCtrlOperation::CSmsCtrlOperation


CSmsHyperLinkOperations::CSmsHyperLinkOperations(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage) : CSmsCtrlOperation(aId, aMessage)
	{
	//NOP
	} // CSmsHyperLinkOperations::CSmsHyperLinkOperations


void CSmsIEOperation::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSIEOPERATION_CONSTRUCTL_1, "CSmsIEOperation::ConstructL()");
	} // CSmsIEOperation::ConstructL


/**
 *  @internalComponent
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
void CSmsHyperLinkOperations::operator=(const CSmsHyperLinkOperations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSHYPERLINKOPERATIONS_OPERATOR_1, "CSmsHyperLinkOperations::operator=");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
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
TBool CSmsHyperLinkOperations::operator==(const CSmsHyperLinkOperations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSHYPERLINKOPERATIONS_OPERATOR1_1, "CSmsHyperLinkOperations::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
    return EFalse;
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Identifies whether the message type or version supports this operation
 *  
 *  @leave
 *  If the message type or version does not support this operation.
 */
void CSmsHyperLinkOperations::ValidateOperationL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSHYPERLINKOPERATIONS_VALIDATEOPERATIONL_1, "CSmsHyperLinkOperations::ValidateOperationL()");

	if (iMessage.Version() < CSmsMessage::ESmsMessageV1)
	    {
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSHYPERLINKOPERATIONS_VALIDATEOPERATIONL_2, "CSmsHyperLinkOperations::AddHyperLinkL, Operation not supported, Msg Version %d", iMessage.Version());
	    User::Leave(KErrNotSupported);
	    }

	if (!MessageTypeSupported())
	    {
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSHYPERLINKOPERATIONS_VALIDATEOPERATIONL_3, "CSmsHyperLinkOperations::AddHyperLinkL, Operation not supported by this PDU type, type = %d", iMessage.Type());
	    User::Leave(KErrNotSupported);
	    }
	} // CSmsHyperLinkOperations::ValidateOperationL


/**
 *  @publishedAll
 *  
 *  This method adds a HyperLink to the CSmsMessage.
 *  
 *  @param aPosition
 *  The absolute character position within the message text.
 *  @param aTitle
 *  An integer representation of the number of characters in the hyperlink title.
 *  @param URL Length
 *  An integer representation of the number of characters in the URL.
 *  @leave KErrNotSupported
 *  If the message version or type does not support hyperlinks.
 *  @capability None
 */
EXPORT_C void CSmsHyperLinkOperations::AddHyperLinkL(TUint aPosition, TUint8 aTitleLength,  TUint8  aURLLength) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSHYPERLINKOPERATIONS_ADDHYPERLINKL_1, "CSmsHyperLinkOperations::AddHyperLinkL()");

	ValidateOperationL();

	CSmsMessageAdditionalAttributes& additionalAttributes = *((CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo());

	TBuf8 <4> buffer;
	buffer.SetLength(4);
	buffer[0] = (TUint8) (aPosition >> 8); // lowest octet contains the
	buffer[1] = (TUint8)  aPosition;       // most significant bits
	buffer[2] = aTitleLength;
	buffer[3] = aURLLength;

	CSmsInformationElement* iE = CSmsInformationElement::NewL(CSmsInformationElement::ESmsHyperLinkFormat, buffer);
	CleanupStack::PushL(iE);
	additionalAttributes.AddControlInformationElementL(iE);
	CleanupStack::Pop(iE);
	} // CSmsHyperLinkOperations::AddHyperLinkL


/**
 *  @publishedAll
 *  
 *  Returns the number of Hyper Link IEs contained in this message.
 *  @leave KErrNotSupported
 *  If this functionality is not supported by this version or type of message.
 *  @return
 *  The number of hyper links contained in this message.
 *  @capability None
 */
EXPORT_C TUint CSmsHyperLinkOperations::NumberOfHyperLinksL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSHYPERLINKOPERATIONS_NUMBEROFHYPERLINKSL_1, "CSmsHyperLinkOperations::NumberOfHyperLinks");

	ValidateOperationL();

	TUint numberOfHyperLinks = 0;

	TSmsInformationElementCategories::TInformationElementCategory category;

	if (TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsHyperLinkFormat, category) == EFalse)
			{
		User::Leave(KErrArgument);
		}

	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
	TUint numberOfElements = additionalAttributes->NumberOfControlInformationElements(category);

	for (TUint i = 0; i < numberOfElements; i++)
	    {
	    CSmsInformationElement::TSmsInformationElementIdentifier id = CSmsInformationElement::ESmsIEMaximum;
	    
	    // If function leaves, allow the error to propagate upwards
	    id = additionalAttributes->GetControlInformationElementL(category, i).Identifier();

	    if (id == CSmsInformationElement::ESmsHyperLinkFormat)
	        {
	        numberOfHyperLinks++;
	        }
	    }

	return numberOfHyperLinks;
	} // CSmsHyperLinkOperations::NumberOfHyperLinksL


/**
 *  @publishedAll
 *  
 *  Copies the attributes of the specified hyperlink.
 *  aIndex has the range [0..[n-1]] where n is the
 *  value returned by NumberOfHyperLinks()
 *  
 *  @param aIndex
 *  Identifies the hyperlink to extract. Has the range [0..[n-1]]
 *  where n is the value returned by NumberOfHyperLinks().
 *  @param &aPosition
 *  The absolute character position within the message text.
 *  @param &aTitleLength
 *  An integer representation of the number of characters in the hyperlink title.
 *  @param &aURLLength
 *  An integer representation of the number of characters in the URL.
 *  @leave KErrNotSupported
 *  If the message version or type does not support hyperlinks.
 *  @leave KErrArgument
 *  If aIndex is out of range.
 *  @capability None
 */
EXPORT_C void  CSmsHyperLinkOperations::CopyHyperLinkAtIndexL(TUint aIndex, TUint& aPosition, TUint8& aTitleLength,  TUint8&  aURLLength) const
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSHYPERLINKOPERATIONS_COPYHYPERLINKATINDEXL_1, "CSmsHyperLinkOperations::CopyHyperLinkAtIndexL(): aIndex=%d", aIndex);

	ValidateOperationL();

	TUint numberOfHyperLinks = 0;

	TSmsInformationElementCategories::TInformationElementCategory category;
	if (TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsHyperLinkFormat, category) == EFalse)
		{
		User::Leave(KErrArgument);
		}

	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
	TUint numberOfElements = additionalAttributes->NumberOfControlInformationElements(category);

	if (aIndex >= numberOfElements)
	    {
	    User::Leave(KErrArgument);
	    }

	for (TUint i = 0; i < numberOfElements; i++)
	    {
	    CSmsInformationElement::TSmsInformationElementIdentifier id = CSmsInformationElement::ESmsIEMaximum;
	    
	    // If function leaves, allow the error to propagate upwards
	    id = additionalAttributes->GetControlInformationElementL(category, i).Identifier();

	    if ((id == CSmsInformationElement::ESmsHyperLinkFormat) &&
	        (numberOfHyperLinks++ == aIndex))
	        {
	        aPosition     = (additionalAttributes->GetControlInformationElementL(category, i).Data()[0] << 8);
	        aPosition    +=  additionalAttributes->GetControlInformationElementL(category, i).Data()[1];

	        aTitleLength = additionalAttributes->GetControlInformationElementL(category, i).Data()[2];
	        aURLLength   = additionalAttributes->GetControlInformationElementL(category, i).Data()[3];
	        break;
	        }
	    }
	} // CSmsHyperLinkOperations::CopyHyperLinkAtIndexL


/**
 *  @publishedAll
 *  
 *  Removes all hyper Link IEs contained in this message.
 *  
 *  @leave KErrNotSupported
 *  If the message version or type does not support hyperlinks.
 *  @capability None
 */
EXPORT_C void  CSmsHyperLinkOperations::RemoveAllHyperLinksL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSHYPERLINKOPERATIONS_REMOVEALLHYPERLINKSL_1, "CSmsHyperLinkOperations::RemoveAllHyperLinks");

	ValidateOperationL();

	TSmsInformationElementCategories::TInformationElementCategory category;

	if (TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsHyperLinkFormat, category) == EFalse)
		{
		User::Leave(KErrArgument);
		}

	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
	TUint i = additionalAttributes->NumberOfControlInformationElements(category);

	while (i-- != 0)
	    {
	    CSmsInformationElement::TSmsInformationElementIdentifier id = CSmsInformationElement::ESmsIEMaximum;
	    
	    // If function leaves, allow the error to propagate upwards
	    id = additionalAttributes->GetControlInformationElementL(category, i).Identifier();

	    if (id == CSmsInformationElement::ESmsHyperLinkFormat)
	        {
	        additionalAttributes->DeleteControlInformationElement(category, i);
	        }
	    }
	} // CSmsHyperLinkOperations::RemoveAllHyperLinksL


/**
 *  @publishedAll
 *  
 *  Removes the indexed hyperlink from this message.
 *  
 *  @param aIndex
 *  aIndex has the range [0..[n-1]] where n is the
 *  value returned by NumberOfHyperLinks()
 *  @leave KErrNotSupported
 *  If the message version or type does not support hyperlinks.
 *  @leave KErrArgument
 *  If the index is out of range.
 *  @capability None
 */
EXPORT_C void  CSmsHyperLinkOperations::RemoveHyperLinkL(TUint aIndex) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSHYPERLINKOPERATIONS_REMOVEHYPERLINKL_1, "CSmsHyperLinkOperations::RemoveHyperLink");

	ValidateOperationL();

	TUint numberOfHyperLinks = 0;

	TSmsInformationElementCategories::TInformationElementCategory category;

	if (TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsHyperLinkFormat, category) == EFalse)
		{
		User::Leave(KErrArgument);
		}

	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
	TUint numberOfElements = additionalAttributes->NumberOfControlInformationElements(category);

	if (aIndex >= numberOfElements)
	    {
	    User::Leave(KErrArgument);
	    }

	for (TUint i = 0; i < numberOfElements; i++)
	    {
	    CSmsInformationElement::TSmsInformationElementIdentifier id = CSmsInformationElement::ESmsIEMaximum;

	    // If function leaves, allow the error to propagate upwards
	    id = additionalAttributes->GetControlInformationElementL(category, i).Identifier();

	    if ((id == CSmsInformationElement::ESmsHyperLinkFormat) &&
	        (numberOfHyperLinks++ == aIndex))
	        {
	        additionalAttributes->DeleteControlInformationElement(category, i);
	        break;
	        }
	    }
	} // CSmsHyperLinkOperations::RemoveHyperLinkL


/**
 *  @internalComponent
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
void CSmsReplyAddressOperations::operator=(const CSmsReplyAddressOperations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREPLYADDRESSOPERATIONS_OPERATOR_1, "CSmsReplyAddressOperations::operator=");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
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
TBool CSmsReplyAddressOperations::operator==(const CSmsReplyAddressOperations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREPLYADDRESSOPERATIONS_OPERATOR1_1, "CSmsReplyAddressOperations::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
    return EFalse;
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Identifies whether the message type or version supports this operation
 *  
 *  @leave
 *  If the message type or version does not support this operation.
 */
void CSmsReplyAddressOperations::ValidateOperationL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREPLYADDRESSOPERATIONS_VALIDATEOPERATIONL_1, "CSmsReplyAddressOperations::ValidateOperationL()");

	if (iMessage.Version() < CSmsMessage::ESmsMessageV1)
	    {
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREPLYADDRESSOPERATIONS_VALIDATEOPERATIONL_2, "CSmsReplyAddressOperations::AddReplyAddressL, Operation not supported, Msg Version %d", iMessage.Version());
	    User::Leave(KErrNotSupported);
	    }

	if (!MessageTypeSupported())
	    {
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREPLYADDRESSOPERATIONS_VALIDATEOPERATIONL_3, "CSmsReplyAddressOperations::AddReplyAddressL, Operation not supported by this PDU type, type = %d", iMessage.Type());
	    User::Leave(KErrNotSupported);
	    }
	} // CSmsReplyAddressOperations::ValidateOperationL


/**
 *  @publishedAll
 *  
 *  Adds a reply address information element.
 *  
 *  @param aAddress
 *  The international number to be used in the reply address.
 *  @leave KErrNotSupported
 *  If the message version or type does not support a reply addresses.
 *  @leave KErrAlreadyExists
 *  If the message already contains a reply address.
 *  @capability None
 */
EXPORT_C void  CSmsReplyAddressOperations::AddReplyAddressL(const TDesC& aAddress) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSREPLYADDRESSOPERATIONS_ADDREPLYADDRESSL_1, "CSmsReplyAddressOperations::AddReplyAddressL()");

	ValidateOperationL();

	if (ContainsReplyAddressIEL())
	    {
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSREPLYADDRESSOPERATIONS_ADDREPLYADDRESSL_2, "CSmsReplyAddressOperations::AddReplyAddressL, Already Exists");
	    User::Leave(KErrAlreadyExists);
	    }

	CSmsMessageAdditionalAttributes& additionalAttributes = *((CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo());

	CSmsAddress* address = CSmsAddress::NewL(iCharacterSetConverter,iFs); // needed for encoding the address into the IE.
	CleanupStack::PushL(address);
	address->SetAddressL(aAddress);
	TBuf8<CSmsAddress::KSmsAddressMaxAddressLength> addressBuffer;
	addressBuffer.SetLength(CSmsAddress::KSmsAddressMaxAddressLength);
	TUint8* startPtr = &addressBuffer[0];
	TUint8* endPtr = address->EncodeL(startPtr);
	TInt length = 0;
	(endPtr > startPtr) ? (length = endPtr - startPtr) : (length = startPtr - endPtr);
	addressBuffer.SetLength(length);
	CleanupStack::PopAndDestroy(address);

	CSmsInformationElement* iE = CSmsInformationElement::NewL(CSmsInformationElement::ESmsReplyAddressFormat, addressBuffer);
	CleanupStack::PushL(iE);
	additionalAttributes.AddControlInformationElementL(iE);
	CleanupStack::Pop(iE);
	} // CSmsReplyAddressOperations::AddReplyAddressL


/**
 *  @publishedAll
 *  
 *  Adds a reply address information element.
 *  
 *  @param aAddress
 *  Sets To and From addresses in ETSI format.
 *  @leave KErrNotSupported
 *  If the message version or type does not support a reply addresses.
 *  @leave KErrAlreadyExists
 *  If the message already contains a reply address.
 *  @capability None
 */
EXPORT_C void  CSmsReplyAddressOperations::AddParsedReplyAddressL(const TGsmSmsTelNumber& aParsedAddress) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSREPLYADDRESSOPERATIONS_ADDPARSEDREPLYADDRESSL_1, "CSmsReplyAddressOperations::AddParsedReplyAddressL");

	ValidateOperationL();

	if (ContainsReplyAddressIEL())
	    {
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSREPLYADDRESSOPERATIONS_ADDPARSEDREPLYADDRESSL_2, "CSmsReplyAddressOperations::AddParsedReplyAddressL, Already Exists");
	    User::Leave(KErrAlreadyExists);
	    }

	CSmsMessageAdditionalAttributes& additionalAttributes = *((CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo());

	CSmsAddress* address = CSmsAddress::NewL(iCharacterSetConverter,iFs);
	CleanupStack::PushL(address);
	address->SetParsedAddressL(aParsedAddress);
	TBuf8<CSmsAddress::KSmsAddressMaxAddressLength> addressBuffer;
	addressBuffer.SetLength(CSmsAddress::KSmsAddressMaxAddressLength);
	TUint8* startPtr = &addressBuffer[0];
	TUint8* endPtr = address->EncodeL(startPtr);
	TInt length = 0;
	(endPtr > startPtr) ? (length = endPtr - startPtr) : (length = startPtr - endPtr);
	addressBuffer.SetLength(length);
	CleanupStack::PopAndDestroy(address);

	CSmsInformationElement* iE = CSmsInformationElement::NewL(CSmsInformationElement::ESmsReplyAddressFormat, addressBuffer);
	CleanupStack::PushL(iE);
	additionalAttributes.AddControlInformationElementL(iE);
	CleanupStack::Pop(iE);
	} // CSmsReplyAddressOperations::AddParsedReplyAddressL


/**
 *  @publishedAll
 *  
 *  Identifies whether a reply address has already been added to this
 *  CSmsMessage using the CSmsReplyAddressOperations interface.
 *  
 *  @return
 *  True if the reply address has already been added using this interface.
 *  @leave KErrNotSupported
 *  If the message version or type does not support this query.
 *  @capability None
 */
EXPORT_C TBool CSmsReplyAddressOperations::ContainsReplyAddressIEL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSREPLYADDRESSOPERATIONS_CONTAINSREPLYADDRESSIEL_1, "CSmsReplyAddressOperations::ContainsReplyAddressIEL()");

	TBool rc = EFalse;

	ValidateOperationL();

	TSmsInformationElementCategories::TInformationElementCategory category;

	if (TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsReplyAddressFormat, category) == EFalse)
		{
		User::Leave(KErrArgument);
		}

	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
	TUint numberOfElements = additionalAttributes->NumberOfControlInformationElements(category);

	for (TUint i = 0; i < numberOfElements; i++)
	    {
	    CSmsInformationElement::TSmsInformationElementIdentifier id = CSmsInformationElement::ESmsIEMaximum;
	    
	    // If function leaves, allow the error to propagate upwards
	    id = additionalAttributes->GetControlInformationElementL(category, i).Identifier();

	    if (id == CSmsInformationElement::ESmsReplyAddressFormat)
	        {
	        rc = ETrue;
	        break;
	        }
	    }

	return rc;
	} // CSmsReplyAddressOperations::ContainsReplyAddressIEL


/**
 *  @publishedAll
 *  
 *  Returns the reply address as an international number
 *  
 *  @return
 *  The reply address as an international number.
 *  @leave KErrNotSupported
 *  If the message version or type does not support this query.
 *  @leave KErrNotFound
 *  If the message does not contain a reply address information element.
 *  @leave KErrCorrupt
 *  If the reply address is corrupt
 *  @capability None
 */
EXPORT_C HBufC* CSmsReplyAddressOperations::GetReplyAddressL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSREPLYADDRESSOPERATIONS_GETREPLYADDRESSL_1, "CSmsReplyAddressOperations::GetReplyAddressL");

	ValidateOperationL();

	TBool found = EFalse;

	CSmsAddress* address = CSmsAddress::NewL(iCharacterSetConverter,iFs);
	CleanupStack::PushL(address);

	TSmsInformationElementCategories::TInformationElementCategory category;

	if (TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsReplyAddressFormat, category) == EFalse)
		{
		User::Leave(KErrArgument);
		}

	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
	TUint numberOfElements = additionalAttributes->NumberOfControlInformationElements(category);

	for (TUint i = 0; i < numberOfElements; i++)
	    {
	    CSmsInformationElement::TSmsInformationElementIdentifier id = CSmsInformationElement::ESmsIEMaximum;
	    
	    // If function leaves, allow the error to propagate upwards
	    id = additionalAttributes->GetControlInformationElementL(category, i).Identifier();

	    if (id == CSmsInformationElement::ESmsReplyAddressFormat)
	        {
	        TGsmuLex8 encodedAddress(additionalAttributes->GetControlInformationElementL(category, i).Data());
	        TRAPD(err,address->DecodeL(encodedAddress));

	        if (err == KErrNone)
	            {
	            found = ETrue;
	            break;
	            }
	        else
	            {
	            User::Leave(KErrCorrupt);
	            }
	        }
	    }

	HBufC* buf = NULL;
	if (found)
	    {
	    buf = HBufC::NewL(TGsmSmsTelNumberMaxLen); // 20 characters preceeded by '+'
	    *buf = address->Address();
	    }
	else
	    {
	    User::Leave(KErrNotFound);
	    }

	CleanupStack::PopAndDestroy(address);

	return buf;
	} // CSmsReplyAddressOperations::GetReplyAddressL


/**
 *  @publishedAll
 *  
 *  Returns the reply address in ETSI format.
 *  
 *  @return
 *  True, if the CSmsMessage contains a reply address.
 *  False, otherwise
 *  @leave KErrNotSupported
 *  If the message version or type does not support this query.
 *  @leave KErrCorrupt
 *  If the reply address is corrupt
 *  @capability None
 */
EXPORT_C TInt  CSmsReplyAddressOperations::GetParsedReplyAddressL(TGsmSmsTelNumber& aParsedAddress) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSREPLYADDRESSOPERATIONS_GETPARSEDREPLYADDRESSL_1, "CSmsReplyAddressOperations::GetParsedReplyAddressL");

	ValidateOperationL();

	TBool rc = EFalse;

	CSmsAddress* address = CSmsAddress::NewL(iCharacterSetConverter,iFs);
	CleanupStack::PushL(address);

	TSmsInformationElementCategories::TInformationElementCategory category;

	if (TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsReplyAddressFormat, category) == EFalse)
		{
		User::Leave(KErrArgument);
		}

	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
	TUint numberOfElements = additionalAttributes->NumberOfControlInformationElements(category);

	for (TUint i = 0; i < numberOfElements; i++)
	    {
	    CSmsInformationElement::TSmsInformationElementIdentifier id = CSmsInformationElement::ESmsIEMaximum;
	    
	    // If function leaves, allow the error to propagate upwards
	    id = additionalAttributes->GetControlInformationElementL(category, i).Identifier();

	    // CSmsAddress is used here for encoding the reply address into an IE.
	    // It also used to encode the source / destination address in the CSmsMessage.
	    // CSmsAddress::DecodeL expects internally that alphanumeric addresses will be size
	    // CSmsAddress::KSmsAddressMaxAddressValueLength=10.
	    // Need to add padding bytes to bring the address field up to the maximum size.
	    if (id == CSmsInformationElement::ESmsReplyAddressFormat)
	        {
	        TBuf8<CSmsAddress::KSmsAddressMaxAddressLength> data;
	        data = additionalAttributes->GetControlInformationElementL(category, i).Data();
	        TUint8 actualDataLength = data.Length();
	        data.SetLength(CSmsAddress::KSmsAddressMaxAddressLength);
	        for (TUint8 j = actualDataLength; j < CSmsAddress::KSmsAddressMaxAddressLength; j++)
	            {
	            data[j] = 0;
	            }

	        TGsmuLex8 encodedAddress(data);
	        TRAPD(err,address->DecodeL(encodedAddress));
	        if (err==KErrNone)
	            {
	            address->ParsedAddress(aParsedAddress);
	            rc = ETrue;
	            break;
	            }
	        else
	            {
	            User::Leave(KErrCorrupt);
	            }
	        }
	    }

	CleanupStack::PopAndDestroy(address);

	return rc;
	} // CSmsReplyAddressOperations::GetParsedReplyAddressL


/**
 *  @publishedAll
 *  
 *  Removes the reply address information element.
 *  
 *  @leave KErrNotSupported
 *  If the message version or type does not support this query.
 *  @capability None
 */
EXPORT_C void  CSmsReplyAddressOperations::RemoveReplyAddressL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSREPLYADDRESSOPERATIONS_REMOVEREPLYADDRESSL_1, "CSmsReplyAddressOperations::RemoveReplyAddress");

	ValidateOperationL();

	TSmsInformationElementCategories::TInformationElementCategory category;

	if (TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsReplyAddressFormat, category) == EFalse)
		{
		User::Leave(KErrArgument);
		}

	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
	TUint numberOfElements = additionalAttributes->NumberOfControlInformationElements(category);

	for (TInt i = 0; i < numberOfElements; i++)
	    {
	    CSmsInformationElement::TSmsInformationElementIdentifier id = CSmsInformationElement::ESmsIEMaximum;
	    
	    // If function leaves, allow the error to propagate upwards
	    id = additionalAttributes->GetControlInformationElementL(category, i).Identifier();

	    if (id == CSmsInformationElement::ESmsReplyAddressFormat)
	        {
	        additionalAttributes->DeleteControlInformationElement(category, i);
	        break;
	        }
	    }
	} // CSmsReplyAddressOperations::RemoveReplyAddressL


CSmsReplyAddressOperations::CSmsReplyAddressOperations(CSmsInformationElement::TSmsInformationElementIdentifier aId,  CSmsMessage& aMessage,
                                                       CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs) : CSmsCtrlOperation(aId, aMessage),
                                                       iCharacterSetConverter(aCharacterSetConverter), iFs(aFs)
	{
	//NOP
	} // CSmsReplyAddressOperations::CSmsReplyAddressOperations


/**
 *  @internalComponent
 *  
 *  Identifies whether the message type or version supports this operation
 *  
 *  @leave
 *  If the message type or version does not support this operation.
 */
void CSmsSpecialSMSMessageOperations::ValidateOperationL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSPECIALSMSMESSAGEOPERATIONS_VALIDATEOPERATIONL_1, "CSmsSpecialSMSMessageOperations::ValidateOperationL()");

	if (iMessage.Version() < CSmsMessage::ESmsMessageV1)
	    {
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSPECIALSMSMESSAGEOPERATIONS_VALIDATEOPERATIONL_2, "CSmsSpecialSMSMessageOperations::AddSpecialMessageIndicationL, Operation not supported, version %d", iMessage.Version());
	    User::Leave(KErrNotSupported);
	    }

	if (!MessageTypeSupported())
	    {
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSPECIALSMSMESSAGEOPERATIONS_VALIDATEOPERATIONL_3, "CSmsSpecialSMSMessageOperations::AddSpecialMessageIndicationL, Operation not supported by this PDU type, type = %d", iMessage.Type());
	    User::Leave(KErrNotSupported);
	    }
	} // CSmsSpecialSMSMessageOperations::ValidateOperationL


/**
 *  @internalComponent
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
void CSmsSpecialSMSMessageOperations::operator=(const CSmsSpecialSMSMessageOperations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSPECIALSMSMESSAGEOPERATIONS_OPERATOR_1, "CSmsSpecialSMSMessageOperations::operator=");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
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
TBool CSmsSpecialSMSMessageOperations::operator==(const CSmsSpecialSMSMessageOperations&)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSPECIALSMSMESSAGEOPERATIONS_OPERATOR1_1, "CSmsSpecialSMSMessageOperations::operator==");
	Panic(KGsmuPanicMethodBodyNotImplemented1);
	return EFalse;
	} // CSmsSpecialSMSMessageOperations::operator


/**
 *  @publishedAll
 *  
 *  Either adds a new or updates an existing special message indication information element.
 *  
 *  @param aStore
 *  Indicates whether or not the message shall be stored.
 *  @param aMessageIndicationType
 *  Indicates the basic message type
 *  @param aExtendedType
 *  Indicates the extended message type
 *  @param aProfile
 *  Indicates the profile ID of the Multiple Subscriber Profile
 *  @param aMessageCount
 *  Indicates the number of messages of the type specified in Octet 1 that are waiting
 *  @leave KErrNotSupported
 *  If the message version or type does not support this query.
 *  @capability None
 */
EXPORT_C void CSmsSpecialSMSMessageOperations::AddSpecialMessageIndicationL(TBool aStore,
                                                                            TSmsMessageIndicationType aMessageIndicationType,
                                                                            TExtendedSmsIndicationType aExtendedType,
                                                                            TSmsMessageProfileType aProfile,
                                                                            TUint8 aMessageCount) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSPECIALSMSMESSAGEOPERATIONS_ADDSPECIALMESSAGEINDICATIONL_1, "CSmsSpecialSMSMessageOperations::AddSpecialMessageIndicationL");

	ValidateOperationL();

	CSmsUserData& userData = iMessage.SmsPDU().UserData();

	TBuf8<2> data;
	data.SetLength(2);

	data[0] = ( ((TUint8) aMessageIndicationType) +
	            ((TUint8) (aExtendedType << 2))   +
	            ((TUint8) (aProfile      << 5))   +
	            ((TUint8) (aStore        << 7)) );

	data[1] = aMessageCount;

	CArrayFixFlat<TInt>* indices = new(ELeave) CArrayFixFlat<TInt>(4);
	CleanupStack::PushL(indices);
	userData.InformationElementIndicesL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication, *indices);

	TBool found = EFalse;
	TInt count = indices->Count();
	for (TInt i=0; ((i<count) && (found==EFalse)); i++)
	    {
		TUint index = indices->operator[](i);
		CSmsInformationElement& ieAlreadyInWorkingPDU = userData.InformationElement(index);

	    if (ieAlreadyInWorkingPDU.Data()[0] == data[0])
	        {
	        ieAlreadyInWorkingPDU.Data()[1] = data[1];
	        found = ETrue;
	        break;
	        }
	    }
	CleanupStack::PopAndDestroy(indices);

	if (found == EFalse)
	    {
	    userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,data);
	    }
	} // CSmsSpecialSMSMessageOperations::AddSpecialMessageIndicationL


/**
 *  @publishedAll
 *  
 *  Gets a count of the number of special message indication information elements which
 *  are stored inside the user data.
 *  
 *  @return The number of special message indication information elements which
 *  are stored inside the user data.
 *  @leave KErrNotSupported
 *  If the message version or type does not support this query.
 *  @capability None
 */
EXPORT_C TUint CSmsSpecialSMSMessageOperations::GetCountOfSpecialMessageIndicationsL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSPECIALSMSMESSAGEOPERATIONS_GETCOUNTOFSPECIALMESSAGEINDICATIONSL_1, "CSmsSpecialSMSMessageOperations::GetCountOfSpecialMessageIndicationsL()");

	ValidateOperationL();

	CSmsUserData& userData = iMessage.SmsPDU().UserData();

	CArrayFixFlat<TInt>* indices = new (ELeave) CArrayFixFlat<TInt>(8);
	CleanupStack::PushL(indices);
	userData.InformationElementIndicesL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,*indices);
	TUint  count = indices->Count();
	CleanupStack::PopAndDestroy(indices);

	return count;
	} // CSmsSpecialSMSMessageOperations::GetCountOfSpecialMessageIndicationsL


/**
 *  @publishedAll
 *  
 *  Gets the attributes of the Special Message Indication specified by aIndex.
 *  
 *  @param aIndex
 *  aIndex is a value less than GetCountOfSpecialMessageIndications().
 *  @param aStore
 *  Indicates whether or not the message shall be stored.
 *  @param aMessageIndicationType
 *  Indicates the basic message type
 *  @param aExtendedType
 *  Indicates the extended message type
 *  @param aProfile
 *  Indicates the profile ID of the Multiple Subscriber Profile
 *  @param aMessageCount
 *  Indicates the number of messages of the type specified in Octet 1 that are waiting
 *  @leave KErrNotSupported
 *  If the message version or type does not support this query.
 *  @leave KErrArgument
 *  If aIndex is >= GetCountOfSpecialMessageIndications()
 *  @capability None
 */
EXPORT_C void CSmsSpecialSMSMessageOperations::GetMessageIndicationIEL(TUint aIndex,
                                                                       TBool& aStore,
                                                                       TSmsMessageIndicationType& aMessageIndicationType,
                                                                       TExtendedSmsIndicationType& aExtendedType,
                                                                       TSmsMessageProfileType&  aProfile,
                                                                       TUint8& aMessageCount) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSPECIALSMSMESSAGEOPERATIONS_GETMESSAGEINDICATIONIEL_1, "CSmsSpecialSMSMessageOperations::GetMessageIndicationIEL()");

	ValidateOperationL();

	CSmsUserData& userData = iMessage.SmsPDU().UserData();

	CArrayFixFlat<TInt>* indices = new (ELeave) CArrayFixFlat<TInt>(8);
	CleanupStack::PushL(indices);
	userData.InformationElementIndicesL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,*indices);

	if (aIndex < indices->Count())
	    {
	    CSmsInformationElement& informationElement = userData.InformationElement((*indices)[aIndex]);

	    aMessageIndicationType =  (TSmsMessageIndicationType)   (informationElement.Data()[0]       & 0x03);
	    aExtendedType          =  (TExtendedSmsIndicationType) ((informationElement.Data()[0] >> 2) & 0x07);
	    aProfile               =  (TSmsMessageProfileType)     ((informationElement.Data()[0] >> 5) & 0x03);
	    aStore                 =  (TBool)                      ((informationElement.Data()[0] >> 7) & 0x01);
	    aMessageCount          =  (TUint8)                      (informationElement.Data()[1]);
	    }
	else
	    {
	    User::Leave(KErrArgument);
	    }

	CleanupStack::PopAndDestroy(indices);
	} // CSmsSpecialSMSMessageOperations::GetMessageIndicationIEL


/**
 *  @publishedAll
 *  
 *  Removes the Special Message Indication specified by the input attributes, provided it exists.
 *  
 *  @param aMessageIndicationType
 *  Indicates the basic message type
 *  @param aExtendedType
 *  Indicates the extended message type
 *  @leave KErrNotSupported
 *  If the message version or type does not support this query.
 *  @capability None
 */
EXPORT_C void CSmsSpecialSMSMessageOperations::RemoveSpecialMessageIndicationL(TSmsMessageIndicationType aMessageIndicationType, TExtendedSmsIndicationType aExtendedType) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSPECIALSMSMESSAGEOPERATIONS_REMOVESPECIALMESSAGEINDICATIONL_1, "CSmsSpecialSMSMessageOperations::RemoveAllSpecialMessageIndications");

	ValidateOperationL();

	CSmsUserData& userData = iMessage.SmsPDU().UserData();

	CArrayFixFlat<TInt>* indices = new (ELeave) CArrayFixFlat<TInt>(8);
	CleanupStack::PushL(indices);
	userData.InformationElementIndicesL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,*indices);

	TUint i = indices->Count();

	while (i-- != 0)
	    {
	    CSmsInformationElement& informationElement = userData.InformationElement((*indices)[i]);

	    TSmsMessageIndicationType          type = ((TSmsMessageIndicationType)   (informationElement.Data()[0]       & 0x03));
	    TExtendedSmsIndicationType extendedType =  (TExtendedSmsIndicationType) ((informationElement.Data()[0] >> 2) & 0x07);

	    if ( (aMessageIndicationType == type) &&
	         (aExtendedType == extendedType) )
	        {
	        userData.RemoveInformationElement((*indices)[i]);
	        break;
	        }
	    }

	CleanupStack::PopAndDestroy(indices);
	} // CSmsSpecialSMSMessageOperations::RemoveSpecialMessageIndicationL


/**
 *  @publishedAll
 *  
 *  Removes all Special Message Indications Information Elements contained in the message.
 *  
 *  @leave KErrNotSupported
 *  If the message version or type does not support this query.
 *  @capability None
 */
EXPORT_C void CSmsSpecialSMSMessageOperations::RemoveAllSpecialMessageIndicationsL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSPECIALSMSMESSAGEOPERATIONS_REMOVEALLSPECIALMESSAGEINDICATIONSL_1, "CSmsSpecialSMSMessageOperations::RemoveAllSpecialMessageIndicationsL");

	ValidateOperationL();

	CSmsUserData& userData = iMessage.SmsPDU().UserData();

	CArrayFixFlat<TInt>* indices = new (ELeave) CArrayFixFlat<TInt>(8);
	CleanupStack::PushL(indices);
	userData.InformationElementIndicesL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,*indices);

	TUint i = indices->Count();

	while (i-- != 0)
	   {
	   userData.RemoveInformationElement((*indices)[i]);
	   }

	CleanupStack::PopAndDestroy(indices);
	} // CSmsSpecialSMSMessageOperations::RemoveAllSpecialMessageIndicationsL


CSmsSpecialSMSMessageOperations::CSmsSpecialSMSMessageOperations(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage) : CSmsCtrlOperation(aId, aMessage)
	{
	//NOP
	} // CSmsSpecialSMSMessageOperations::CSmsSpecialSMSMessageOperations


/**
 *  @internalComponent
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
void CSmsEnhancedVoiceMailOperations::operator=(const CSmsEnhancedVoiceMailOperations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSENHANCEDVOICEMAILOPERATIONS_OPERATOR_1, "CSmsEnhancedVoiceMailOperations::operator=");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
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
TBool CSmsEnhancedVoiceMailOperations::operator==(const CSmsEnhancedVoiceMailOperations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSENHANCEDVOICEMAILOPERATIONS_OPERATOR1_1, "CSmsEnhancedVoiceMailOperations::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
    return EFalse;
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Identifies whether the message type or version supports this operation
 *  
 *  @leave
 *  If the message type or version does not support this operation.
 */
void CSmsEnhancedVoiceMailOperations::ValidateOperationL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSENHANCEDVOICEMAILOPERATIONS_VALIDATEOPERATIONL_1, "CSmsEnhancedVoiceMailOperations::ValidateOperationL()");

	if (iMessage.Version() < CSmsMessage::ESmsMessageV1)
	    {
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSENHANCEDVOICEMAILOPERATIONS_VALIDATEOPERATIONL_2, "CSmsEnhancedVoiceMailInformation::AddEnhancedVoiceMailIEL, Operation not supported, Msg Version %d", iMessage.Version());
	    User::Leave(KErrNotSupported);
	    }

	if (!MessageTypeSupported())
	    {
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSENHANCEDVOICEMAILOPERATIONS_VALIDATEOPERATIONL_3, "CSmsSpecialSMSMessageOperations::AddEnhancedVoiceMailIEL, Operation not supported by this PDU type, type = %d", iMessage.Type());
	    User::Leave(KErrNotSupported);
	    }
	} // CSmsEnhancedVoiceMailOperations::ValidateOperationL


/**
 *  @publishedAll
 *  
 *  Adds an Enhanced Voice Mail Information Element to the CSmsMessage.
 *  The information element can be either an Enhanced Voice Mail Notification or
 *  an Enhanced Voice Mail Delete Confirmation. Enhanced Voice Mail Notifications
 *  and Enhanced Voice Mail Delete Confirmations are abstracted in classes derived
 *  from CEnhancedVoiceMailBoxInformation.
 *  @param aEVMI
 *  A pointer to the Enhanced Voice Mail Information Element.
 *  @leave KErrNotSupported
 *  If the message version or type does not support this query.
 *  @leave KErrAlreadyExists
 *  If an Enhanced Voice Mail Information Element has already been installed.
 *  @leave KErrArgument
 *  If the input parameter contains invalid parameters.
 *  @capability None
 */
EXPORT_C void CSmsEnhancedVoiceMailOperations::AddEnhancedVoiceMailIEL(const CEnhancedVoiceMailBoxInformation& aEVMI) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSENHANCEDVOICEMAILOPERATIONS_ADDENHANCEDVOICEMAILIEL_1, "CSmsEnhancedVoiceMailInformation::AddEnhancedVoiceMailIEL");

	ValidateOperationL();

	if (ContainsEnhancedVoiceMailIEL())
	    {
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSENHANCEDVOICEMAILOPERATIONS_ADDENHANCEDVOICEMAILIEL_2, "CSmsEnhancedVoiceMailInformation::AddEnhancedVoiceMailIEL, Already Exists");
	    User::Leave(KErrAlreadyExists);
	    }

	CSmsMessageAdditionalAttributes& additionalAttributes = *((CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo());

	// The Enhanced Voice Mail Information Element must fit into a single PDU.
	// The maximum size for its data field is therefore
	// the maximum size of the user data minus user data header length, the ie identifier byte and
	// ie length byte.
	TBuf8<CEnhancedVoiceMailBoxInformation::KSmsMaxEnhancedVoiceMailSize> voiceMailInfo;
	voiceMailInfo.SetLength(CEnhancedVoiceMailNotification::KSmsMaxEnhancedVoiceMailSize);
	TUint8* startPtr = &voiceMailInfo[0];
	TUint8* endPtr = aEVMI.EncodeL(startPtr, iCharacterSetConverter, iFs);
	TInt length = (TInt) (endPtr - startPtr);
	voiceMailInfo.SetLength(length);

	CSmsInformationElement* iE = CSmsInformationElement::NewL(CSmsInformationElement::CSmsInformationElement::ESmsEnhanceVoiceMailInformation, voiceMailInfo);
	CleanupStack::PushL(iE);

	additionalAttributes.AddControlInformationElementL(iE);
	CleanupStack::Pop();
	} // CSmsEnhancedVoiceMailOperations::AddEnhancedVoiceMailIEL


/**
 *  @publishedAll
 *  
 *  Removes the Enhanced Voice Mail Information Element from the CSmsMessage,
 *  provided it exists.
 *  @leave KErrNotSupported
 *  If the message version or type does not support this query.
 *  @leave KErrNotFound
 *  If the CSmsMessage does not contain an Enhanced Voice Mail Information Element
 *  @capability None
 */
EXPORT_C CEnhancedVoiceMailBoxInformation* CSmsEnhancedVoiceMailOperations::RemoveEnhancedVoiceMailIEL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSENHANCEDVOICEMAILOPERATIONS_REMOVEENHANCEDVOICEMAILIEL_1, "CSmsEnhancedVoiceMailInformation::RemoveEnhancedVoiceMailIE");

	ValidateOperationL();

	CEnhancedVoiceMailBoxInformation* voiceMailBoxInfo = NULL;
	TBool deleteInformationElement = ETrue;
	
	// If function leaves, allow the error to propagate upwards
	voiceMailBoxInfo = GetEnhancedVoiceMailIEL(deleteInformationElement);

	return voiceMailBoxInfo;
	} // CSmsEnhancedVoiceMailOperations::RemoveEnhancedVoiceMailIEL


/**
 *  @publishedAll
 *  
 *  Copies the Enhanced Voice Mail Information Element contained in the CSmsMessage,
 *  provided one exists.
 *  @leave KErrNotSupported
 *  If the message version or type does not support this query.
 *  @leave KErrNotFound
 *  If the CSmsMessage does not contain an Enhanced Voice Mail Information Element to copy.
 *  @capability None
 */
EXPORT_C CEnhancedVoiceMailBoxInformation* CSmsEnhancedVoiceMailOperations::CopyEnhancedVoiceMailIEL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSENHANCEDVOICEMAILOPERATIONS_COPYENHANCEDVOICEMAILIEL_1, "CSmsEnhancedVoiceMailInformation::CopyEnhancedVoiceMailIEL");

	ValidateOperationL();

	CEnhancedVoiceMailBoxInformation* voiceMailBoxInfo = NULL;
	TBool deleteInformationElement = EFalse;
	
	// If function leaves, allow the error to propagate upwards
	voiceMailBoxInfo = GetEnhancedVoiceMailIEL(deleteInformationElement);

	return voiceMailBoxInfo;
	} // CSmsEnhancedVoiceMailOperations::CopyEnhancedVoiceMailIEL


/**
 *  @internalComponent
 *  
 *  Returns a copy of the Enhanced Voice Mail Information Element contained in the CSmsMessage,
 *  provided one exists. Optionally deletes the information element.
 *  @param aRemove
 *  Indicates that the information element should be deleted once its contents have been copied.
 *  @leave KErrNotFound
 *  If the CSmsMessage does not contain an Enhanced Voice Mail Information Element to copy.
 */
CEnhancedVoiceMailBoxInformation* CSmsEnhancedVoiceMailOperations::GetEnhancedVoiceMailIEL(TBool aRemove) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSENHANCEDVOICEMAILOPERATIONS_GETENHANCEDVOICEMAILIEL_1, "CSmsEnhancedVoiceMailOperations::GetEnhancedVoiceMailIEL()");

	TBool found = EFalse;

	CEnhancedVoiceMailBoxInformation* voiceMailBoxInfo = NULL;

	TSmsInformationElementCategories::TInformationElementCategory category;

	if (TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhanceVoiceMailInformation, category) == EFalse)
		{
		User::Leave(KErrArgument);
		}

	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
	TUint numberOfElements = additionalAttributes->NumberOfControlInformationElements(category);

	for (TUint i = 0; i < numberOfElements; i++)
	    {
	    CSmsInformationElement::TSmsInformationElementIdentifier id = CSmsInformationElement::ESmsIEMaximum;
	    id = additionalAttributes->GetControlInformationElementL(category, i).Identifier();

	    if (id == CSmsInformationElement::CSmsInformationElement::ESmsEnhanceVoiceMailInformation)
	        {
	        if (additionalAttributes->GetControlInformationElementL(category, i).Data()[0] & 0x01)
	            {
	            voiceMailBoxInfo = CEnhancedVoiceMailDeleteConfirmations::NewL();
	            }
	        else
	            {
	            voiceMailBoxInfo = CEnhancedVoiceMailNotification::NewL();
	            }

	        CleanupStack::PushL(voiceMailBoxInfo);
	        TGsmuLex8 encodedVoiceMailBox(additionalAttributes->GetControlInformationElementL(category, i).Data());
	        voiceMailBoxInfo->DecodeL(encodedVoiceMailBox, iCharacterSetConverter, iFs);
	        CleanupStack::Pop(voiceMailBoxInfo);

	        if (aRemove)
	            {
	            additionalAttributes->DeleteControlInformationElement(category, i);
	            }

	        found = ETrue;

	        break;
	        }
	    }

	if (found == EFalse)
	    {
	    User::Leave(KErrNotFound);
	    }

	return voiceMailBoxInfo;
	} // CSmsEnhancedVoiceMailOperations::GetEnhancedVoiceMailIEL


/**
 *  @publishedAll
 *  
 *  Determines whether the CSmsMessage contains an Enhanced Voice Mail Information Element.
 *  @leave KErrNotSupported
 *  If the message version or type does not support this query.
 *  @return
 *  True if the CSmsMessage contains an Enhanced Voice Mail Information Element.
 *  @capability None
 */
EXPORT_C TBool CSmsEnhancedVoiceMailOperations::ContainsEnhancedVoiceMailIEL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSENHANCEDVOICEMAILOPERATIONS_CONTAINSENHANCEDVOICEMAILIEL_1, "CSmsEnhancedVoiceMailInformation::ContainsEnhancedVoiceMailIEL");

	ValidateOperationL();

	TBool rc = EFalse;

	TSmsInformationElementCategories::TInformationElementCategory category;

	if (TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhanceVoiceMailInformation, category) == EFalse)
		{
		User::Leave(KErrArgument);
		}

	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
	TUint numberOfElements = additionalAttributes->NumberOfControlInformationElements(category);

	for (TUint i = 0; i < numberOfElements; i++)
	    {
	    CSmsInformationElement::TSmsInformationElementIdentifier id = CSmsInformationElement::ESmsIEMaximum;
	    
	    // If function leaves, allow the error to propagate upwards
	    id = additionalAttributes->GetControlInformationElementL(category, i).Identifier();

	    if (id == CSmsInformationElement::ESmsEnhanceVoiceMailInformation)
	        {
	        rc = ETrue;
	        break;
	        }
	    }

	return rc;
	} // CSmsEnhancedVoiceMailOperations::ContainsEnhancedVoiceMailIEL


/**
 *  Status Reporting.
 *  
 *  @note This should be the last thing that should be configured in CSmsMessage.
 *  If anything is changed after this then the number of PDUs might change which
 *  will affect status reporting.
 */
CSmsEnhancedVoiceMailOperations::CSmsEnhancedVoiceMailOperations(CSmsInformationElement::TSmsInformationElementIdentifier aId,  CSmsMessage& aMessage,
                                                                 CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs) : CSmsCtrlOperation(aId, aMessage),
                                                                 iCharacterSetConverter(aCharacterSetConverter), iFs(aFs)
	{
	// NOP
	} // CSmsEnhancedVoiceMailOperations::CSmsEnhancedVoiceMailOperations


CSmsSMSCCtrlParameterOperations::CSmsSMSCCtrlParameterOperations(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage)
: CSmsCtrlOperation(aId, aMessage)
	{
	
	} // CSmsSMSCCtrlParameterOperations::CSmsSMSCCtrlParameterOperations
	

/**
 *  @internalComponent  
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *  
 *  @capability None
 */
void CSmsSMSCCtrlParameterOperations::operator=(const CSmsSMSCCtrlParameterOperations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSMSCCTRLPARAMETEROPERATIONS_OPERATOR_1, "CSmsSMSCCtrlParameterOperations::operator=");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
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
TBool CSmsSMSCCtrlParameterOperations::operator==(const CSmsSMSCCtrlParameterOperations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSMSCCTRLPARAMETEROPERATIONS_OPERATOR1_1, "CSmsSMSCCtrlParameterOperations::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
    return EFalse;
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  
 *  Identifies whether the message type or version supports this operation
 *  
 *  @leave
 *  If the message type or version does not support this operation.
 */
void CSmsSMSCCtrlParameterOperations::ValidateOperationL() const
	{
	if (iMessage.Version() < CSmsMessage::ESmsMessageV2)
	    {
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSMSCCTRLPARAMETEROPERATIONS_VALIDATEOPERATIONL_1, "CSmsSMSCCtrlParameterOperations Operation not supported, Msg Version %d", iMessage.Version());
	    User::Leave(KErrNotSupported); 
	    }

	if (!MessageTypeSupported())
	    {
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSMSCCTRLPARAMETEROPERATIONS_VALIDATEOPERATIONL_2, "CSmsSMSCCtrlParameterOperations Operation not supported by this PDU type, type = %d", iMessage.Type());
	    User::Leave(KErrNotSupported);
	    }
	} // CSmsSMSCCtrlParameterOperations::ValidateOperationL()


/**
 *  @internalComponent
 *  
 *  Checks that the octet passed to it has the valid bits set in it. If the bits which 
 *  are not supported yet are set then reset it. 
 *  This method is called by SetStatusReportL.
 *  
 *  @param aSelectiveStatus
 *  The octet which needs to be set as the Selective Status for the report.
 *  @return
 *  ETrue if the selective status is valid.
 *  @leave KErrNotSupported
 *  If the last 4 bits are set as they are not supported.
 */
TBool CSmsSMSCCtrlParameterOperations::ValidateControlParametersL(TUint8& aSelectiveStatus) const
	{
	if (aSelectiveStatus & ESmsStatusReportForFutureUse1)
		{
		User::Leave(KErrNotSupported);
		}
		
	if (aSelectiveStatus & ESmsStatusReportForFutureUse2)
		{
		User::Leave(KErrNotSupported);	
		}
		
	if (aSelectiveStatus & ESmsStatusReportCancelRestSRR)
		{
		User::Leave(KErrNotSupported);	
		}
		
	if (aSelectiveStatus & ESmsStatusReportIncludeOriginalUDH)
		{
		User::Leave(KErrNotSupported);
		}
	
	return ETrue;	
	} // CSmsSMSCCtrlParameterOperations::ValidateControlParametersL


/**
 *  @publishedAll
 *  
 *  Sets the status report for a PDU. First the scheme is obtained by calling the 
 *  GetStatusReportScheme and checked if it is the valid scheme. Then aSelectiveStatus is 
 *  checked to see if it is the default value. If it is then nothing is done. But if 
 *  aSelectiveStatus is non-default then it is appended to the array 
 *  iControlParametersStatusReport along with the aSegmentSequenceNum 
 *  
 *  @param aSegmentSequenceNum
 *  The segment sequence number of the PDU which needs to be updated.
 *  @param aSelectiveStatus
 *  The octet which needs to be set as the Selective Status for the report.
 *  @return KErrNone if the operation is successful.
 *  KErrNotFound if the Segment Sequence Number is out of range or the scheme is inavlid.
 *  KErrNotSupported if aSelectiveStatus is not valid.
 *  @leave
 *  If ValidateOperationL or AppendL leaves.
 *  @capability None
 */
EXPORT_C TInt CSmsSMSCCtrlParameterOperations::SetStatusReportL(TUint aSegmentSequenceNum, TUint8 aSelectiveStatus)
	{
	ValidateOperationL();
	TBool validateStatus = ValidateControlParametersL(aSelectiveStatus);
	
	if(validateStatus)
		{
		CSmsMessageAdditionalAttributes& additionalAttributes = *((CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo());
		CSmsMessageAdditionalAttributes::CSmsStatusReportScheme& scheme = additionalAttributes.GetStatusReportScheme();
		if (scheme.Id() == EControlParametersScheme)
			{
			CSmsMessageAdditionalAttributes::CControlParametersScheme& ctrlParamsScheme = (CSmsMessageAdditionalAttributes::CControlParametersScheme&)scheme;
			if(aSegmentSequenceNum>=ctrlParamsScheme.iNumOfPDUs)
				{
				return KErrNotFound;	//	aSegmentSequenceNum out of range.
				}
			if (aSelectiveStatus == ctrlParamsScheme.iDefaultStatusReport)
				{
				return KErrNone;	// It's the same as default so need for any updation.
				}
			else
				{
				TInt count = (ctrlParamsScheme.iControlParametersStatusReport).Count();
				TBool found(EFalse);
				for (TInt ii=0; !found && ii<count; ii++)
					{
					if ((ctrlParamsScheme.iControlParametersStatusReport[ii]).iSegmentSequenceNum == aSegmentSequenceNum)
						{
						(ctrlParamsScheme.iControlParametersStatusReport[ii]).iSelectiveStatus = aSelectiveStatus;
						found = ETrue;
						break;
						}
					}
					
				if (!found)
					{
					CSmsMessageAdditionalAttributes::CControlParametersScheme::TSmsSMSCCtrlParameterStatus smscCtrlParameterStatus;
					smscCtrlParameterStatus.iSegmentSequenceNum = aSegmentSequenceNum;
					smscCtrlParameterStatus.iSelectiveStatus = aSelectiveStatus;
					(ctrlParamsScheme.iControlParametersStatusReport).AppendL(smscCtrlParameterStatus);
					}
			
				return KErrNone;	
				}
			}
		else
			{
			return KErrNotFound;	//	Scheme not valid.
			}
		}
	else
		{
		return KErrNotSupported;	//	Invalid aSlectiveStatus.
		}
	} // CSmsSMSCCtrlParameterOperations::SetStatusReportL


/**
 *  @publishedAll
 *  
 *  Gets the selective status for a PDU if the scheme is set to the Control
 *  Parameters Scheme.
 *  
 *  @param aSegmentSequenceNum
 *  The segment sequence number of the PDU whose status report is required.
 *  @param aSelectiveStatus
 *  Returns the selective status octet for that PDU.
 *  
 *  @return KErrNotFound if the segment sequence number or the scheme is invalid.
 *  
 *  @capability None
 */
EXPORT_C TInt CSmsSMSCCtrlParameterOperations::GetStatusReport(TUint aSegmentSequenceNum,
															   TUint8& aSelectiveStatus) const
	{
	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
	CSmsMessageAdditionalAttributes::CSmsStatusReportScheme& scheme = additionalAttributes->GetStatusReportScheme();
	if (scheme.Id() == EControlParametersScheme)
		{
		CSmsMessageAdditionalAttributes::CControlParametersScheme& ctrlParamsScheme = (CSmsMessageAdditionalAttributes::CControlParametersScheme&)scheme;
		
		if(aSegmentSequenceNum>=ctrlParamsScheme.iNumOfPDUs)
			{
			return KErrNotFound;	//	aSegmentSequenceNum out of range.
			}

		TInt count = (ctrlParamsScheme.iControlParametersStatusReport).Count();
		
		for (TInt ii=0; ii<count; ii++)
			{
			if (ctrlParamsScheme.iControlParametersStatusReport[ii].iSegmentSequenceNum == aSegmentSequenceNum)
				{
				aSelectiveStatus = ctrlParamsScheme.iControlParametersStatusReport[ii].iSelectiveStatus;
				return KErrNone;
				}
			}
			
		aSelectiveStatus = ctrlParamsScheme.iDefaultStatusReport;
		return KErrNone;
		}

	return KErrNotFound;
	} // CSmsSMSCCtrlParameterOperations::GetStatusReport


/**
 *  @publishedAll
 *  
 *  Sets the default value of the status report to aDefaultSelectiveStatus.
 *  
 *  @param aDefaultSelectiveStatus
 *  The selective status to be used as default.
 *  @leave KErrNotFound
 *  If the scheme is invalid.
 *  @capability None
 */
EXPORT_C void CSmsSMSCCtrlParameterOperations::SetDefaultL(TUint8 aDefaultSelectiveStatus)
	{
	CSmsMessageAdditionalAttributes& additionalAttributes = *((CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo());
	CSmsMessageAdditionalAttributes::CSmsStatusReportScheme& scheme = additionalAttributes.GetStatusReportScheme();
	if (scheme.Id() == EControlParametersScheme)
		{
		CSmsMessageAdditionalAttributes::CControlParametersScheme& ctrlParamsScheme = (CSmsMessageAdditionalAttributes::CControlParametersScheme&)scheme;
		if(aDefaultSelectiveStatus & ESmsSMSCControlParametersMask)
			{
			ctrlParamsScheme.iDefaultStatusReport = aDefaultSelectiveStatus;
			}
		else
			{
			ctrlParamsScheme.iDefaultStatusReport = 0x00;	
			}
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	} // CSmsSMSCCtrlParameterOperations::SetDefaultL


/**
 *  @publishedAll
 *  
 *  This method is called to set the scheme to Control Parameters Scheme.
 *  First iStatusReportScheme, which is obtained by calling GetStatusReportScheme,
 *  is deleted and set to NULL. Then a new scheme is created and a default value is set.
 *  This should be the last method to be called in a message sending process as all the 
 *  operations in this interface depend on the number of PDUs being set.
 *  
 *  @leave
 *  If ValidateOperationL or NumMessagePDUsL leaves.
 *  @capability None
 */
EXPORT_C void CSmsSMSCCtrlParameterOperations::SetSchemeL()
	{
	ValidateOperationL();
	
	CSmsMessageAdditionalAttributes& additionalAttributes = *((CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo());
	additionalAttributes.SetStatusReportSchemeL(EControlParametersScheme);
	
	TBuf8 <1> buffer;
	buffer.SetLength(1);
	buffer[0] = 0;
	
	CSmsUserData& userData = iMessage.SmsPDU().UserData();

	userData.AddInformationElementL(CSmsInformationElement::ESmsIEISMSCControlParameters,buffer);
	
	CSmsMessageAdditionalAttributes::CSmsStatusReportScheme& scheme = additionalAttributes.GetStatusReportScheme();
	CSmsMessageAdditionalAttributes::CControlParametersScheme& ctrlParamsScheme = (CSmsMessageAdditionalAttributes::CControlParametersScheme&)scheme;
	ctrlParamsScheme.iNumOfPDUs = iMessage.NumMessagePDUsL();

	SetDefaultL(0);
	} // CSmsSMSCCtrlParameterOperations::SetSchemeL


/**
 *  @publishedAll
 *  
 *  Gets the current scheme being used.
 *  
 *  @return
 *  EControlParametrsScheme is returned when Control Parameters Scheme is being used.
 *  @capability None
 */
EXPORT_C TSmsStatusReportScheme CSmsSMSCCtrlParameterOperations::GetScheme() const
	{
	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
		
	return (additionalAttributes->GetStatusReportScheme()).Id();
	} // CSmsSMSCCtrlParameterOperations::GetScheme


/**
 *  @publishedAll
 *  
 *  This method re-sets the scheme to the Default Scheme.
 *  
 *  @capability None
 */
EXPORT_C void CSmsSMSCCtrlParameterOperations::ResetSchemeL()
	{
	CSmsMessageAdditionalAttributes& additionalAttributes = *((CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo());
	
	CSmsUserData& userData = iMessage.SmsPDU().UserData();

	CArrayFixFlat<TInt>* indices = new (ELeave) CArrayFixFlat<TInt>(8);
	CleanupStack::PushL(indices);
	userData.InformationElementIndicesL(CSmsInformationElement::ESmsIEISMSCControlParameters,*indices);

	TUint i = indices->Count();

	while (i-- != 0)
	    {
	    userData.RemoveInformationElement((*indices)[i]);
	    }
    
    CleanupStack::PopAndDestroy(indices);
    additionalAttributes.SetStatusReportSchemeL(EDefaultScheme);
	} // CSmsSMSCCtrlParameterOperations::ResetSchemeL

