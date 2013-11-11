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
// This file defines the container class CSmsMessageAdditionalAttributes.
// 
//

/**
 @file
*/


#ifndef __GSMUMSGADDITIONALATTRIBUTES_
#define __GSMUMSGADDITIONALATTRIBUTES_

#include <e32base.h>
#include <e32std.h>
#include "Gsmuelem.h"
#include "gsmuieoperations.h"
#include "gsmunonieoperations.h"

/**
 *  @internalComponent
 *  
 *  CSmsMessageAdditionalAttributes
 *  
 *  This class contains additional attributes that would normally be
 *  stored in CSmsMessage.
 *  
 *  The class contains a collections of control information elements.
 *  These elements will either be copied into the working PDU for encoding
 *  or have been copied from the working pdu for decoding.
 */
class CSmsMessageAdditionalAttributes : public CBase
{
public:
    typedef TSmsInformationElementCategories::TInformationElementCategory TCategory;
    typedef CSmsInformationElement::TSmsInformationElementIdentifier TInformationElementId;
    typedef CSmsInformationElement* CSmsInformationElementPtr;

public:
	static CSmsMessageAdditionalAttributes* NewL();

    // client side accessors (intended to be used by CSmsMessage)
	void  AddControlInformationElementL(CSmsInformationElement* aIE);
	TBool Find1stInstanceOfControlInformationElement(TInformationElementId aId, TUint& index) const;
    TBool FindNextInstanceOfControlInformationElement(TInformationElementId aId, TUint aStartIndex, TUint& aIndex) const;
    CSmsInformationElement& GetControlInformationElementL(TInformationElementId aId, TUint aIndex) const;
    TBool RemoveControlInformationElement(TInformationElementId aId, TUint aIndex, CSmsInformationElementPtr& aIE);

    // stack side interface / encoding / decoding
	void AddControlInformationElementL(TCategory aCategory, CSmsInformationElement* aIE);
    CSmsInformationElement& GetControlInformationElementL(TCategory aCategory, TUint aIndex) const;
    TBool RemoveNextControlInformationElement(TCategory aCategory, CSmsInformationElementPtr& aIE);
    void DeleteControlInformationElement(TCategory aCategory, TUint8 index);
    TUint NumberOfControlInformationElements(TCategory aCategory) const;

    CSmsIEOperation& GetIEOperationL(TInformationElementId aId) const;
    CSmsNonIEOperation& GetNonIEOperationL(TSmsNonIEIdentifier aId) const;
    void SetIEOperationL(CSmsIEOperation* aOperation);
    void SetNonIEOperationL(CSmsNonIEOperation* aOperation);

 	void InternalizeL(RReadStream& aStream, TInt aVersion);
	void ExternalizeL(RWriteStream& aStream, TInt aVersion) const;

    void ResetAttributesL();

    ~CSmsMessageAdditionalAttributes();
    
    class CSmsStatusReportScheme
    	{
    	public:
    		virtual TSmsStatusReportScheme Id() = 0;
    		virtual ~CSmsStatusReportScheme() {};
       	};
    
    class CControlParametersScheme : public CSmsStatusReportScheme
    	{
    	public:
    		CControlParametersScheme();
    		~CControlParametersScheme();
    		TSmsStatusReportScheme Id();
    	public:
			struct TSmsSMSCCtrlParameterStatus
				{
			/** Message reference. */
				TUint iSegmentSequenceNum;
			/** Selective Status Report */
				TUint8 iSelectiveStatus;
				};
			RArray<TSmsSMSCCtrlParameterStatus> iControlParametersStatusReport;
			TUint8 iDefaultStatusReport;
			TInt iNumOfPDUs;
		};
    	
    class CTPSRRScheme : public CSmsStatusReportScheme
		{
		public:
			CTPSRRScheme();
			~CTPSRRScheme();
    		TSmsStatusReportScheme Id();
		public:
		    struct TSmsTPSRRStatus
				{
			/** Message reference. */
				TUint iSegmentSequenceNum;
			/** TP-SRR Bit. */
				TSmsFirstOctet::TSmsStatusReportRequest iTPSRRStatus;
				};
			RArray<TSmsTPSRRStatus> iTPSRRStatusReport;	
			TSmsFirstOctet::TSmsStatusReportRequest iDefaultStatusReport;
			TInt iNumOfPDUs;
		};
		
	class CDefaultScheme : public CSmsStatusReportScheme
		{
		public:
			CDefaultScheme();
			~CDefaultScheme();
    		TSmsStatusReportScheme Id();
		};
		
	CSmsStatusReportScheme& GetStatusReportScheme() const;
	void SetStatusReportSchemeL(TSmsStatusReportScheme aId);

	RPointerArray<CSmsPDU>&  SmsPDUArray() { return iSmsPDUArray; };

	TSmsEncoding Alternative7bitEncoding() const;
	void SetAlternative7bitEncoding(TSmsEncoding aEncoding);

private:
    void ConstructL();
    CSmsMessageAdditionalAttributes();

private:
    typedef CArrayPtrFlat<CSmsInformationElement> CArrayOfPointersToControlInformationElements;
    // comment, not intending to store all categories in additional attributes, some categories will be stored in the working PDU
    // The following categories will be stored here:
    // ECtrlMandatoryIn1stPDUOnly,    e.g  Reply Address
    // ECtrlSingleInstanceOnly,       e.g. Enhanced Voice Mail
    // ECtrlMultipleInstancesAllowed, e.g. Hyperlink format
    
    CArrayOfPointersToControlInformationElements* iCollectionOfPointersToIEArrays[TSmsInformationElementCategories::ENumberOfCategories];

    CSmsHyperLinkOperations*         iHyperLinkOperations;
    CSmsReplyAddressOperations*      iReplyAddressOperations;
    CSmsSpecialSMSMessageOperations* iSpecialSMSMessageOperations;
    CSmsEnhancedVoiceMailOperations* iEnhancedVoiceMailOperations;
    CSmsSMSCCtrlParameterOperations* iSMSCCtrlParameterOperations;
    CSmsTPSRROperations*			 iTPSRROperations;
    CSmsStatusReportScheme*			 iStatusReportScheme;
	CIncompleteClass0MessageInfo*	 iIncompleteClass0MessageInfo;
	RPointerArray<CSmsPDU>           iSmsPDUArray;
	TSmsEncoding                     iAlternative7bitEncoding;
};

#endif // __GSMUMSGADDITIONALATTRIBUTES_
