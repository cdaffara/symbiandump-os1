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
// This file defines the container class CSmsIEOperation and its specialisations.
// The classes provide interfaces that allow the client to operate on a number
// of control information elements.
// 
//

/**
 @file
*/

#ifndef __GSMUIEOPERATIONS_
#define __GSMUIEOPERATIONS_


#include <e32base.h>
#include <gsmumsg.h>
#include <gsmuelem.h>


/**
 *  SMS Stack clients use specialisations of this class to configure a SMS Message with Control Information Elements
 *  which have been introduced in 23.040 v6.5.0.
 *  
 *  The 23.040 v6.5.0 standard defines individual requirements for how each of these control information elements
 *  is to be encoded and decoded. These requirements mean that these control information elements need to be
 *  configured in the CSmsMessage using derivations of this interface, rather than CSmsUserData::AddInformationElementL().
 *  
 *  CSmsUserData::AddInformationElement specifies those information elements it supports and those that are supported
 *  by CSmsIEOperation and its derivatives.
 *  
 *  Each control information element that uses this interface is supported by a class derived from CSmsIEOperation.
 *  The derived class operates on the CSmsMessage, allowing the client to add, remove and access control information
 *  elements inside the CSmsMessage.
 *  
 *  The client gets access to an operations class using the following interface:
 *  
 *  CSmsIEOperation& CSmsMessage::GetOperationsForIEL(CSmsInformationElement::TSmsInformationElementIdentifier aId);
 *  
 *  When the CSmsMessage is deleted, all its associated CSmsIEOperations classes are also deleted and references to
 *  them become stale. Each instance of the CSmsIEOperation Class is an attributes of the CSmsMessage itself.
 *  
 *  @publishedAll
 *  @released
 */
class CSmsIEOperation : public CBase
{
public:
	static CSmsIEOperation* NewL(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs);
	IMPORT_C CSmsInformationElement::TSmsInformationElementIdentifier Id() const;
	virtual ~CSmsIEOperation(){};
protected:
	virtual TBool MessageTypeSupported() const;
	virtual void ValidateOperationL() const {};
private:
    void ConstructL();
private:
    CSmsInformationElement::TSmsInformationElementIdentifier iId;
protected:
    CSmsMessage& iMessage;
    CSmsIEOperation(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage);
    void operator=(const CSmsIEOperation&);
    TBool operator==(const CSmsIEOperation&);
};

class CSmsCtrlOperation : public CSmsIEOperation
{
protected:
    CSmsCtrlOperation(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage);
    ~CSmsCtrlOperation(){};
    void operator=(const CSmsCtrlOperation&);
    TBool operator==(const CSmsCtrlOperation&);
};


/**
 *  Clients use this class to configure a CSmsMessage with hyperlinks per 23.040 v6.5.0 section 9.2.3.24.12.
 *  
 *  @publishedAll
 *  @released
 */
class CSmsHyperLinkOperations : public CSmsCtrlOperation
{
public:
    IMPORT_C void  AddHyperLinkL(TUint aPosition, TUint8 aTitleLength,  TUint8  aURLLength) const;
    IMPORT_C TUint NumberOfHyperLinksL() const;
    IMPORT_C void  CopyHyperLinkAtIndexL(TUint index, TUint& aPosition, TUint8& aTitleLength,  TUint8&  aURLLength) const;
    IMPORT_C void  RemoveAllHyperLinksL() const;
    IMPORT_C void  RemoveHyperLinkL(TUint aIndex) const;
    CSmsHyperLinkOperations(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage);
    ~CSmsHyperLinkOperations(){};
protected:
    void ValidateOperationL() const;
    void operator=(const CSmsHyperLinkOperations&);
    TBool operator==(const CSmsHyperLinkOperations&);
};


/**
 *  Clients use this class to configure a CSmsMessage with a reply address per 23.040 v6.5.0 section 9.2.3.24.10.1.17.
 *  
 *  @publishedAll
 *  @released
 */
class CSmsReplyAddressOperations : public CSmsCtrlOperation
{
public:
    static CSmsReplyAddressOperations* NewL(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage,
                                             CCnvCharacterSetConverter& iCharacterSetConverter, RFs& iFs);

    IMPORT_C void  AddReplyAddressL(const TDesC& aAddress) const;
    IMPORT_C void  AddParsedReplyAddressL(const TGsmSmsTelNumber& aParsedAddress) const;
    IMPORT_C TBool ContainsReplyAddressIEL() const;
    IMPORT_C HBufC* GetReplyAddressL() const;
    IMPORT_C TInt  GetParsedReplyAddressL(TGsmSmsTelNumber& aParsedAddress) const;
    IMPORT_C void  RemoveReplyAddressL() const;
    CSmsReplyAddressOperations(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage,
                           CCnvCharacterSetConverter& iCharacterSetConverter, RFs& iFs);
    ~CSmsReplyAddressOperations() {};
protected:
    void ValidateOperationL() const;
    void operator=(const CSmsReplyAddressOperations&);
    TBool operator==(const CSmsReplyAddressOperations&);
protected:
    CCnvCharacterSetConverter& iCharacterSetConverter;
	RFs& iFs;
};


/**
 *  Clients use this class to configure a CSmsMessage with a Special SMS Message Indication Information Element per 23.040 v6.5.0
 *  section 9.2.3.24.2.
 *  
 *  @publishedAll
 *  @released
 */
class CSmsSpecialSMSMessageOperations : public CSmsCtrlOperation
{
public:
    IMPORT_C void AddSpecialMessageIndicationL(TBool aStore, TSmsMessageIndicationType aMessageIndicationType,
                                               TExtendedSmsIndicationType aExtendedType, TSmsMessageProfileType aProfile,
                                               TUint8 aMessageCount) const;
    IMPORT_C TUint GetCountOfSpecialMessageIndicationsL() const;
    IMPORT_C void GetMessageIndicationIEL(TUint aIndex, TBool& aStore, TSmsMessageIndicationType& aMessageIndicationType,
                                          TExtendedSmsIndicationType& aExtendedType, TSmsMessageProfileType&  aProfile,
                                          TUint8& aMessageCount) const;
    IMPORT_C void RemoveSpecialMessageIndicationL(TSmsMessageIndicationType aMessageIndicationType, TExtendedSmsIndicationType aExtendedType) const;
    IMPORT_C void RemoveAllSpecialMessageIndicationsL() const;
    CSmsSpecialSMSMessageOperations(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage);
    ~CSmsSpecialSMSMessageOperations() {};
protected:
    void ValidateOperationL() const;
    void operator=(const CSmsSpecialSMSMessageOperations&);
    TBool operator==(const CSmsSpecialSMSMessageOperations&);
};


/**
 *  Clients use this class to configure a CSmsMessage with either an Enhanced Voice Mail Notification or a
 *  Enhanced Voice Mail Delete Confirmation  per 23.040 v6.5.0 section 9.2.3.24.13.
 *  
 *  Clients should be aware that 23.040 v6.5.0 specifies that this information element must fit into the
 *  user data field of a single PDU. The amount of space available in the user data field depends on both
 *  the number and size of the mandatory information elements that must also be present.
 *  Intuitively the largest Enhanced Voice Mail information element can be added when no mandatory information
 *  elements need to be encoded in the PDU. To achieve this, the CSmsMessage must be configured with
 *  only the Enhanced Voice Mail Information Element and no other information elements or text.
 *  
 *  @publishedAll
 *  @released
 */
class CSmsEnhancedVoiceMailOperations : public CSmsCtrlOperation
{
public:
    IMPORT_C void AddEnhancedVoiceMailIEL(const CEnhancedVoiceMailBoxInformation& aEVMI) const;
    IMPORT_C CEnhancedVoiceMailBoxInformation* RemoveEnhancedVoiceMailIEL() const;
    IMPORT_C CEnhancedVoiceMailBoxInformation* CopyEnhancedVoiceMailIEL() const;
    IMPORT_C TBool ContainsEnhancedVoiceMailIEL() const;
    CSmsEnhancedVoiceMailOperations(CSmsInformationElement::TSmsInformationElementIdentifier aId,  CSmsMessage& aMessage,
                                CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs);
    ~CSmsEnhancedVoiceMailOperations() {};
protected:
    CEnhancedVoiceMailBoxInformation* GetEnhancedVoiceMailIEL(TBool aRemove) const;
    void ValidateOperationL() const;
    void operator=(const CSmsEnhancedVoiceMailOperations&);
    TBool operator==(const CSmsEnhancedVoiceMailOperations&);
protected:
    CCnvCharacterSetConverter& iCharacterSetConverter;
	RFs& iFs;
};


class CSmsSMSCCtrlParameterOperations : public CSmsCtrlOperation
	{
public:
	CSmsSMSCCtrlParameterOperations(CSmsInformationElement::TSmsInformationElementIdentifier aId, CSmsMessage& aMessage);
    ~CSmsSMSCCtrlParameterOperations() {};
    IMPORT_C TInt GetStatusReport(TUint aSegmentSequenceNum, TUint8& aSelectiveStatus) const;
    IMPORT_C TInt SetStatusReportL(TUint aSegmentSequenceNum, TUint8 aSelectiveStatus);
    IMPORT_C void SetSchemeL();
    IMPORT_C TSmsStatusReportScheme GetScheme() const;
   	IMPORT_C void ResetSchemeL();
   	IMPORT_C void SetDefaultL(TUint8 aDefaultSelectiveStatus);
protected:
    void ValidateOperationL() const;
    TBool ValidateControlParametersL(TUint8& aSelectiveStatus) const;
    void operator=(const CSmsSMSCCtrlParameterOperations&);
    TBool operator==(const CSmsSMSCCtrlParameterOperations&);
	};

#endif // __GSMUIEOPERATIONS_
