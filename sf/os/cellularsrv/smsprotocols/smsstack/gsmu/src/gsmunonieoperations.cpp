// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "gsmunonieoperationsTraces.h"
#endif

#include "Gsmumain.h" 
#include "gsmunonieoperations.h" 
#include "Gsmumsg.h"
#include "gsmumsgadditionalattributes.h"
#include "Gsmuelem.h"
#include "smsstacklog.h"


/** 
 *  @internalComponent 
 *      
 *  This is used by the CSmsMessage to create a CSmsNonIEOperation.
 *  Each instance is owned by the CSmsMessage which created it.
 *  Therefore only the CSmsMessage (GSMU component) is allowed to
 *  create and delete CSmsNonIEOperations.
 *  
 *  @param aId  The ID for the element for which an operation class needs to be created.
 *  @param aMessage  The CSmsMessage that this operations class belongs to.
 */
CSmsNonIEOperation*  CSmsNonIEOperation::NewL(TSmsNonIEIdentifier aId, CSmsMessage& aMessage)
	{
	CSmsNonIEOperation* nonIEOperation = NULL;

	if(aId == ESmsTPSRRParameter)
		{
		nonIEOperation = new (ELeave)  CSmsTPSRROperations(aId, aMessage);	
		}
	else if (aId == ESmsIncompleteClass0MessageParameter)
		{
		nonIEOperation = new (ELeave)  CIncompleteClass0MessageInfo(aId, aMessage);
		}
	else
		{
		User::Leave(KErrArgument);	
		}
	CleanupStack::PushL(nonIEOperation);
	nonIEOperation->ConstructL();
	CleanupStack::Pop();
	return nonIEOperation;
	}


/** 
 *  @publishedAll
 *      
 *  Identifies the element which is supported by this operations class.
 *  
 *  @return The element ID supported by this operations class.
 *
 *  @capability None
 */
EXPORT_C TSmsNonIEIdentifier CSmsNonIEOperation::Id() const
	{
	return iId;    
	}


/** 
 *  @internalComponent
 *      
 *  Identifies whether the message type supports is supported
 *  
 *  @return True if the message type is supported. False otherwise.
 */
TBool CSmsNonIEOperation::MessageTypeSupported() const
	{
	CSmsPDU::TSmsPDUType type = iMessage.Type();

	    return ((type == CSmsPDU::ESmsDeliver) ||
	            (type == CSmsPDU::ESmsSubmit));
	}

CSmsNonIEOperation::CSmsNonIEOperation(TSmsNonIEIdentifier aId, CSmsMessage& aMessage) : iId(aId), iMessage(aMessage)
	{
	}

/**
 *  @internalComponent  
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *   
 *  @capability None
 */
void CSmsNonIEOperation::operator=(const CSmsNonIEOperation&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSNONIEOPERATION_OPERATOR_1, "CSmsNonIEOperation::operator=");
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
TBool CSmsNonIEOperation::operator==(const CSmsNonIEOperation&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSNONIEOPERATION_OPERATOR1_1, "CSmsNonIEOperation::operator==");
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
void CSmsCtrlNonIEOperation::operator=(const CSmsCtrlNonIEOperation&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCTRLNONIEOPERATION_OPERATOR_1, "CSmsCtrlNonIEOperation::operator=");
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
TBool CSmsCtrlNonIEOperation::operator==(const CSmsCtrlNonIEOperation&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCTRLNONIEOPERATION_OPERATOR1_1, "CSmsCtrlNonIEOperation::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
    return EFalse;
    BULLSEYE_RESTORE
    }

CSmsCtrlNonIEOperation::CSmsCtrlNonIEOperation(TSmsNonIEIdentifier aId, CSmsMessage& aMessage) : CSmsNonIEOperation(aId, aMessage)
	{
	}

void CSmsNonIEOperation::ConstructL()
	{    
	}


/**
 *  Status Reporting.
 *  
 *  @note This should be the last thing that should be configured in CSmsMessage.
 *  If anything is changed after this then the number of PDUs might change which
 *  will affect status reporting.
 */	
CSmsTPSRROperations::CSmsTPSRROperations(TSmsNonIEIdentifier aId, CSmsMessage& aMessage)
: CSmsCtrlNonIEOperation(aId, aMessage)
	{
	
	}

	
/**
 *  @internalComponent  
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *   
 *  @capability None
 */
void CSmsTPSRROperations::operator=(const CSmsTPSRROperations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSTPSRROPERATIONS_OPERATOR_1, "CSmsTPSRROperations::operator=");
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
TBool CSmsTPSRROperations::operator==(const CSmsTPSRROperations&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSTPSRROPERATIONS_OPERATOR1_1, "CSmsTPSRROperations::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
    return EFalse;
    BULLSEYE_RESTORE
    }

/** 
 *  @internalComponent
 *      
 *  Identifies whether the message type or version supports this operation
 *  
 *  @leave If the message type or version does not support this operation.
 */
void CSmsTPSRROperations::ValidateOperationL() const
	{
	if (iMessage.Version() < CSmsMessage::ESmsMessageV2)
	    {
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSTPSRROPERATIONS_VALIDATEOPERATIONL_1, "CSmsTPSRROperations Operation not supported, Msg Version %d", iMessage.Version());
	    User::Leave(KErrNotSupported); 
	    }

	if (!MessageTypeSupported())
	    {
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSTPSRROPERATIONS_VALIDATEOPERATIONL_2, "CSmsTPSRROperations Operation not supported by this PDU type, type = %d", iMessage.Type());
	    User::Leave(KErrNotSupported);
	    }
	}


/** 
 *  @publishedAll
 *      
 *  Sets the TPSRR bit for the last PDU depending on the value of aEnable.
 *  First the scheme is obtained by calling the GetStatusReportScheme. 
 *  Then the array iTPSRRStatusReport is traversed to find the last segement. 
 *  Once that is done the TPSRR bit for it is updated. If the last segment is
 *  not present in the array then it is appended to it with its TPSRR status.
 *  
 *  @param aEnable
 *  ETrue to set the TPSRR bit to TSmsFirstOctet::ESmsStatusReportRequested
 *  or EFalse to set it to TSmsFirstOctet::ESmsStatusReportNotRequested.
 *  
 *  @leave	KErrNotFound If the scheme is not valid.
 *  
 *  @capability None
 */		
EXPORT_C void CSmsTPSRROperations::SetLastSegmentStatusReportL(TBool aEnable)
	{
	ValidateOperationL();
	
	CSmsMessageAdditionalAttributes& additionalAttributes = *((CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo());
	CSmsMessageAdditionalAttributes::CSmsStatusReportScheme& scheme = additionalAttributes.GetStatusReportScheme();
	if (scheme.Id() == ETPSRRScheme)
		{
		CSmsMessageAdditionalAttributes::CTPSRRScheme& tpsrrScheme = (CSmsMessageAdditionalAttributes::CTPSRRScheme&)scheme;	
		
		TBool found(EFalse);
		TInt count = (tpsrrScheme.iTPSRRStatusReport).Count();
	
		for (TInt ii=0; !found && ii<count; ii++)
			{
			if ((tpsrrScheme.iTPSRRStatusReport[ii]).iSegmentSequenceNum == tpsrrScheme.iNumOfPDUs-1)
				{
				if(aEnable)
					{
					(tpsrrScheme.iTPSRRStatusReport[ii]).iTPSRRStatus = TSmsFirstOctet::ESmsStatusReportRequested;
					}
				else
					{
					(tpsrrScheme.iTPSRRStatusReport[ii]).iTPSRRStatus = TSmsFirstOctet::ESmsStatusReportNotRequested;
					}
				found = ETrue;
				break;
				}
			}
			
		if (!found)
			{
			CSmsMessageAdditionalAttributes::CTPSRRScheme::TSmsTPSRRStatus tpsrrStatus;
			tpsrrStatus.iSegmentSequenceNum = tpsrrScheme.iNumOfPDUs-1;
			if(aEnable)
				{
				tpsrrStatus.iTPSRRStatus = TSmsFirstOctet::ESmsStatusReportRequested;	
				}
			else
				{
				tpsrrStatus.iTPSRRStatus = TSmsFirstOctet::ESmsStatusReportNotRequested;	
				}
			(tpsrrScheme.iTPSRRStatusReport).AppendL(tpsrrStatus);
			}
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}


/** 
 *  @publishedAll
 *      
 *  Sets the TPSRR bit for any PDU depending on the value of aEnable.
 *  First the scheme is obtained by calling the GetStatusReportScheme. 
 *  Then the array iTPSRRStatusReport is traversed to find the segment
 *  which has the reference number = aSegmentSequenceNum. Once that is done
 *  the TPSRR bit for it is updated.
 *  
 *  If the refernce number is not found then it is added to the array.
 *  
 *  @param aSegmentSequenceNum
 *  Segment Sequence number of the PDU that is to be updated.
 *  @param aEnable
 *  ETrue to set the TPSRR bit to TSmsFirstOctet::ESmsStatusReportRequested
 *  or EFalse to set it to TSmsFirstOctet::ESmsStatusReportNotRequested.
 *
 *  @return KErrNone if the operation is successful.
 *  KErrNotFound if the Segment Sequence Number is out of range or 
 *  if the scheme is not valid.
 *
 *  @leave If ValidateOperationL or AppendL leaves.
 *  
 *  @capability None
 */	
EXPORT_C TInt CSmsTPSRROperations::SetSegmentStatusReportL(TUint aSegmentSequenceNum, TBool aEnable)
	{
	ValidateOperationL();
		
	CSmsMessageAdditionalAttributes& additionalAttributes = *((CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo());
	CSmsMessageAdditionalAttributes::CSmsStatusReportScheme& scheme = additionalAttributes.GetStatusReportScheme();
	if (scheme.Id() == ETPSRRScheme)
		{
		CSmsMessageAdditionalAttributes::CTPSRRScheme& tpsrrScheme = (CSmsMessageAdditionalAttributes::CTPSRRScheme&)scheme;	
		
		if( aSegmentSequenceNum>=tpsrrScheme.iNumOfPDUs)
			{
			return KErrNotFound;	//	aSegmentSequenceNum out of range.
			}
	
		TBool found(EFalse);
		
		TInt count = (tpsrrScheme.iTPSRRStatusReport).Count();

		for (TInt ii=0; !found && ii<count; ii++)
			{
			if ((tpsrrScheme.iTPSRRStatusReport[ii]).iSegmentSequenceNum == aSegmentSequenceNum)
				{
				if(aEnable)
					{
					(tpsrrScheme.iTPSRRStatusReport[ii]).iTPSRRStatus = TSmsFirstOctet::ESmsStatusReportRequested;	
					}
				else
					{
					(tpsrrScheme.iTPSRRStatusReport[ii]).iTPSRRStatus = TSmsFirstOctet::ESmsStatusReportNotRequested;	
					}
				found = ETrue;
				break;
				}
			}
			
		if (!found)
			{
			CSmsMessageAdditionalAttributes::CTPSRRScheme::TSmsTPSRRStatus tpsrrStatus;
			tpsrrStatus.iSegmentSequenceNum = aSegmentSequenceNum;
			if(aEnable)
				{
				tpsrrStatus.iTPSRRStatus = TSmsFirstOctet::ESmsStatusReportRequested;	
				}
			else
				{
				tpsrrStatus.iTPSRRStatus = TSmsFirstOctet::ESmsStatusReportNotRequested;	
				}
			(tpsrrScheme.iTPSRRStatusReport).AppendL(tpsrrStatus);
			}
			
		return KErrNone;
		}
	else
		{
		return KErrNotFound;	//	Invalid Scheme	
		}
	}

	
/** 
 *  @publishedAll
 *      
 *  Gets the selective status for a PDU if the scheme is set to the TPSRR Scheme.
 *  
 *  @param aSegmentSequenceNum  The segment sequence number of the PDU whose status report
 *                              is required.
 *
 *  @return The TPSRR bit for the PDU.
 *
 *  @capability None
 */	
EXPORT_C TInt CSmsTPSRROperations::GetStatusReport(TUint aSegmentSequenceNum) const
	{
	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
	CSmsMessageAdditionalAttributes::CSmsStatusReportScheme& scheme = additionalAttributes->GetStatusReportScheme();
	if (scheme.Id() == ETPSRRScheme)
		{
		CSmsMessageAdditionalAttributes::CTPSRRScheme& tpsrrScheme = (CSmsMessageAdditionalAttributes::CTPSRRScheme&)scheme;
		
		if(aSegmentSequenceNum>=tpsrrScheme.iNumOfPDUs)
			{
			return KErrNotFound;	//	aSegmentSequenceNum out of range.
			}
		
		TInt count = (tpsrrScheme.iTPSRRStatusReport).Count();
		
		for (TInt ii=0; ii<count; ii++)
			{
			if ((tpsrrScheme.iTPSRRStatusReport[ii]).iSegmentSequenceNum == aSegmentSequenceNum)
				{
				return 	(tpsrrScheme.iTPSRRStatusReport[ii]).iTPSRRStatus;
				}
			}
			
		return tpsrrScheme.iDefaultStatusReport;
		}
	else
		{
		return KErrNotFound;
		}	
	}

	
/** 
 *  @publishedAll
 *      
 *  Sets the default value of the status report to requested or not requested as per aEnable.
 *  
 *  @param aEnable  To set the default to Requested or Not Requested.
 *  
 *  @capability None
 */		
EXPORT_C void CSmsTPSRROperations::SetDefaultL(TBool aEnable)
	{
	CSmsMessageAdditionalAttributes& additionalAttributes = *((CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo());
	CSmsMessageAdditionalAttributes::CSmsStatusReportScheme& scheme = additionalAttributes.GetStatusReportScheme();
	if (scheme.Id() == ETPSRRScheme)
		{
		CSmsMessageAdditionalAttributes::CTPSRRScheme& tpsrrScheme = (CSmsMessageAdditionalAttributes::CTPSRRScheme&)scheme;
		if(aEnable)
			{
			tpsrrScheme.iDefaultStatusReport = TSmsFirstOctet::ESmsStatusReportRequested;	
			}
		else
			{
			tpsrrScheme.iDefaultStatusReport = TSmsFirstOctet::ESmsStatusReportNotRequested;	
			}
		}
	else
		{
		User::Leave(KErrNotFound);	
		}
	}


/** 
 *  @publishedAll
 *      
 *  This method is called to set the scheme to TPSRR Scheme.
 *  First iStatusReportScheme, which is obtained by calling GetStatusReportScheme,
 *  is deleted and the set to NULL. Then a new scheme is created and a default value is set.
 *  The last segment is set to have TPSRR bit as Requested. This should be the last method
 *  to be called in a message sending process as all the operations in this interface depend
 *  on the number of PDUs being set.
 *  
 *  @leave If ValidateOperationL or NumMessagePDUsL leaves.
 *
 *  @capability None
 */		
EXPORT_C void CSmsTPSRROperations::SetSchemeL()
	{
	ValidateOperationL();
	
	//check if UserData has SMSC Information Element, if so delete it
	CSmsUserData& userData = iMessage.SmsPDU().UserData();
	TInt index=0;
	TBool hasSMSC=userData.InformationElementIndex(CSmsInformationElement::ESmsIEISMSCControlParameters,index);
	if(hasSMSC)
	{
		userData.RemoveInformationElement(index);	
	}
		
	CSmsMessageAdditionalAttributes& additionalAttributes = *((CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo());
	additionalAttributes.SetStatusReportSchemeL(ETPSRRScheme);
	
	CSmsMessageAdditionalAttributes::CSmsStatusReportScheme& scheme = additionalAttributes.GetStatusReportScheme();
	CSmsMessageAdditionalAttributes::CTPSRRScheme& tpsrrScheme = (CSmsMessageAdditionalAttributes::CTPSRRScheme&)scheme;
	
	tpsrrScheme.iNumOfPDUs = iMessage.NumMessagePDUsL();

	SetDefaultL(EFalse);
	}


/** 
 *  @publishedAll
 *      
 *  Gets the current scheme being used.
 *  
 *  @return ETPSRRScheme is returned when TPSRR Scheme is being used.
 *
 *  @capability None
 */			
EXPORT_C TSmsStatusReportScheme CSmsTPSRROperations::GetScheme() const
	{
	CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo();
	return (additionalAttributes->GetStatusReportScheme()).Id();
	}


/** 
 *  @publishedAll
 *      
 *  If the current scheme is set to TPSRR Scheme then
 *  this method sets the scheme to the Default scheme.
 *  
 *  @capability None
 */		
EXPORT_C void CSmsTPSRROperations::ResetSchemeL()
	{
	CSmsMessageAdditionalAttributes& additionalAttributes = *((CSmsMessageAdditionalAttributes*) iMessage.AdditionalInfo());
		    
	additionalAttributes.SetStatusReportSchemeL(EDefaultScheme);
	}

/** 
 * constructor
 *      
 */
CIncompleteClass0MessageInfo::CIncompleteClass0MessageInfo(TSmsNonIEIdentifier aId, CSmsMessage& aMessage)
	: CSmsNonIEOperation(aId, aMessage)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINCOMPLETECLASS0MESSAGEINFO_CTOR_1, "CIncompleteClass0MessageInfo constructor");
	}

/** 
 *  @publishedAll
 *      
 *  Gets the incomplete message info.
 *  This information will indicate the partial complete message consists of which PDUs.
 *  
 *  @param aStartPos Starting PDU position of decoded message.
 *  @param aEndPos End PDU position of decoded message.
 *	@param aLastPartialCompleteMsg boolean value indicating this is the last
 *			incomplete message for a particular concatenated message.
 *  @capability None
 */			
EXPORT_C void CIncompleteClass0MessageInfo::GetIncompleteMessageInfoL(TInt& aStartPos, TInt& aEndPos, TBool& aIsLastIncompleteMessage)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CINCOMPLETECLASS0MESSAGEINFO_GETINCOMPLETEMESSAGEINFOL_1, "CIncompleteClass0MessageInfo::GetIncompleteMessageInfoL");
	ValidateOperationL();
	aStartPos = iStartPos;
	aEndPos = iEndPos;
	aIsLastIncompleteMessage = iIsLastIncompleteMessage;	
	}

/** 
 *  @publishedAll
 *      
 *  Sets the incomplete message info.
 *  This information will indicate the partial complete message consists of which PDUs.
 *  
 *  @param aStartPos Starting PDU position of decoded message.
 *  @param aEndPos End PDU position of decoded message.
 *	@param aLastPartialCompleteMsg boolean value indicating this is the last
 *			incomplete message for a particular concatenated message.
 *  @capability None
 */
EXPORT_C void CIncompleteClass0MessageInfo::SetIncompleteMessageInfoL(TInt aStartPos, TInt aEndPos, TBool aIsLastIncompleteMessage)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CINCOMPLETECLASS0MESSAGEINFO_SETINCOMPLETEMESSAGEINFOL_1, "CIncompleteClass0MessageInfo::SetIncompleteMessageInfoL");
	ValidateOperationL();
	iStartPos = aStartPos;
	iEndPos = aEndPos;
	iIsLastIncompleteMessage = aIsLastIncompleteMessage;
	}

/**
 *  @internalComponent  
 *  
 *  Prevent clients from using the assignment operator by including it in the class definition
 *  but making it protected and not exporting it.
 *   
 *  @capability None
 */
void CIncompleteClass0MessageInfo::operator=(const CIncompleteClass0MessageInfo&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINCOMPLETECLASS0MESSAGEINFO_OPERATOR_1, "CIncompleteClass0MessageInfo::operator=");
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
TBool CIncompleteClass0MessageInfo::operator==(const CIncompleteClass0MessageInfo&)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINCOMPLETECLASS0MESSAGEINFO_OPERATOR1_1, "CIncompleteClass0MessageInfo::operator==");
    Panic(KGsmuPanicMethodBodyNotImplemented1);
    return EFalse;
    BULLSEYE_RESTORE
    }

/** 
 *  @internalComponent
 *  
 *  Identifies which message type is supported
 *  
 *  @return True if the message type is supported. False otherwise.
 */
TBool CIncompleteClass0MessageInfo::MessageTypeSupported() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINCOMPLETECLASS0MESSAGEINFO_MESSAGETYPESUPPORTED_1, "CIncompleteClass0MessageInfo::MessageTypeSupported");
	CSmsPDU::TSmsPDUType type = iMessage.Type();
	return (type == CSmsPDU::ESmsDeliver);
	}

/** 
 *  @internalComponent
 *      
 *  Identifies whether the message type or version supports this operation
 *  
 *  @leave If the message type or version does not support this operation.
 */
void CIncompleteClass0MessageInfo::ValidateOperationL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINCOMPLETECLASS0MESSAGEINFO_VALIDATEOPERATIONL_1, "CIncompleteClass0MessageInfo::ValidateOperationL");

	if (iMessage.Version() != CSmsMessage::ESmsIncompleteClass0MessageV)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINCOMPLETECLASS0MESSAGEINFO_VALIDATEOPERATIONL_2, "CIncompleteClass0MessageInfo Operation not supported, Msg Version %d", iMessage.Version());
		User::Leave(KErrNotSupported); 
		}

	if (!MessageTypeSupported())
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINCOMPLETECLASS0MESSAGEINFO_VALIDATEOPERATIONL_3, "CIncompleteClass0MessageInfo Operation not supported by this PDU type, type = %d", iMessage.Type());
		User::Leave(KErrNotSupported);
		}
	}

/**
 *  Returns the incomplete class 0 message version number.
 *  @capability
 */
TInt CIncompleteClass0MessageInfo::Version()
    {
    // Ignore in code coverage - as there is only one version of this
    // class, no need to check the version. Expected to be used if
    // a new version of the class is added
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINCOMPLETECLASS0MESSAGEINFO_VERSION_1, "CIncompleteClass0MessageInfo::Version()");
    return iVersion;
    BULLSEYE_RESTORE
    }

/**
 *  @internalComponent
 *  Validates and sets the message version number.
 *  
 *  @param aVersion version number to set.
 *  @capability None
 */
void CIncompleteClass0MessageInfo::SetVersion(TInt aVersion)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINCOMPLETECLASS0MESSAGEINFO_SETVERSION_1, "CIncompleteClass0MessageInfo::SetVersion() version:%d", aVersion);

	__ASSERT_DEBUG((aVersion<EMaxSmsIncompleteClass0MessageV),Panic(KGsmuPanicVersionNotSupported));

	iVersion=aVersion;
	} // CSmsMessage::SetVersion

/**
 *  Internalises all object data.
 *  
 *  @param aStream Stream to read from
 *  @capability None
 */
void CIncompleteClass0MessageInfo::InternalizeL(RReadStream& aStream)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINCOMPLETECLASS0MESSAGEINFO_INTERNALIZEL_1, "CIncompleteClass0MessageInfo::InternalizeL()");
	iVersion = aStream.ReadInt32L();
	iStartPos = aStream.ReadInt32L();
	iEndPos = aStream.ReadInt32L();
	iIsLastIncompleteMessage = aStream.ReadInt32L();
	}

/**
 *  Externalises all object data.
 *  
 *  @param aStream Stream to write to
 *  @capability None
 */
void CIncompleteClass0MessageInfo::ExternalizeL(RWriteStream& aStream) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINCOMPLETECLASS0MESSAGEINFO_EXTERNALIZEL_1, "CIncompleteClass0MessageInfo::ExternalizeL()");
	aStream.WriteInt32L(iVersion);
	aStream.WriteInt32L(iStartPos);
	aStream.WriteInt32L(iEndPos);
	aStream.WriteInt32L(iIsLastIncompleteMessage);
	}
