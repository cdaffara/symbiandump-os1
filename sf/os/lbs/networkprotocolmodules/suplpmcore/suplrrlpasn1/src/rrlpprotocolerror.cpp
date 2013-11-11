// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "RRLP-Messages.h"
#include "rrlpprotocolerror.h"
#include "supldevloggermacros.h" 

/**
Static Factory Constructor
*/
EXPORT_C CRrlpProtocolError* CRrlpProtocolError::NewL(TBool aIsOutgoing)
	{
	SUPLLOG(ELogP1, "CRrlpProtocolError::NewL() Begin\n");
	CRrlpProtocolError* self = new (ELeave) CRrlpProtocolError(aIsOutgoing);
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CRrlpProtocolError::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}


/**
Private default constructor
*/
CRrlpProtocolError::CRrlpProtocolError(TBool aIsOutgoing)
 : CRrlpMessageBase(ERrlpProtocolError, aIsOutgoing)
	{
	}


/**
Second stage constructor
*/
void CRrlpProtocolError::ConstructL()
	{
	// call base class ConstructL to create data structures.
	CRrlpMessageBase::ConstructL();

	// construct messaage specific data 
	// containers if this is an outgoing message
	if (iIsOutgoingMessage)
		{
		// local reference to context object
		OSCTXT* pctxt = iControl->getCtxtPtr();

		iData->component.t = T_RRLP_Component_protocolError;
		iData->component.u.protocolError = (ASN1T_ProtocolError*)rtxMemAllocZ(pctxt, sizeof(ASN1T_ProtocolError));
	   	LeaveIfAllocErrorL();
		}
	}


/**
Destructor
*/
CRrlpProtocolError::~CRrlpProtocolError()
	{
	SUPLLOG(ELogP1, "CRrlpProtocolError::~CRrlpProtocolError() Begin\n");
	SUPLLOG(ELogP1, "CRrlpProtocolError::~CRrlpProtocolError() End\n");
	}


/** 
SetProtocolError()

@param  aErrorCode - the outgoing RRLP Protocol error code
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CRrlpProtocolError::SetProtocolError(const TRrlpErrorCode& aErrorCode)
	{
	SUPLLOG(ELogP8, "CRrlpProtocolError::SetProtocolError() Begin\n");
	SUPLLOG2(ELogP8, "    - TRrlpErrorCode aLocError = %d", aErrorCode);
	
	__ASSERT_DEBUG(iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->component.u.protocolError != NULL, User::Invariant());
	
	iData->component.u.protocolError->errorCause = aErrorCode;

	SUPLLOG(ELogP1, "CRrlpProtocolError::SetProtocolError() End\n");
	return KErrNone;
	}


/** 
ProtocolError()

@param  aErrorCode on return, populated with the received RRLP Error Code
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CRrlpProtocolError::GetProtocolError(TRrlpErrorCode& aErrorCode)
	{
	SUPLLOG(ELogP8, "CRrlpProtocolError::ProtocolError() Begin\n");
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->component.u.protocolError != NULL, User::Invariant());
	
	aErrorCode = (TRrlpErrorCode)iData->component.u.protocolError->errorCause;
	
	SUPLLOG2(ELogP8, "    - TRrlpErrorCode aLocError = %d", aErrorCode);
	SUPLLOG(ELogP1, "CRrlpProtocolError::ProtocolError() End\n");
	return KErrNone;
	}
	
	
/**
GetExtendedReference()

Populates aRrlpRef if Rel-5 Extended Reference is present in the received message

@param  aRrlpRef local copy of the session reference details
@return KErrNotFound if the extended reference parameters are not present,
        KErrNone otherwise
*/
TInt CRrlpProtocolError::GetExtendedReference(TRrlpReference& aRrlpRef)
	{
	__ASSERT_DEBUG(iData->component.u.protocolError != NULL, User::Invariant());
	
	if (iData->component.u.protocolError->m.rel_5_ProtocolError_ExtensionPresent !=0)
		{
		if (iData->component.u.protocolError->rel_5_ProtocolError_Extension.m.extended_referencePresent !=0)
			{
			aRrlpRef.aRel5EntendedRefPresent = ETrue;
			aRrlpRef.aRel5SmlcCode = iData->component.u.protocolError->rel_5_ProtocolError_Extension.extended_reference.smlc_code;
			aRrlpRef.aRel5TransactionId = iData->component.u.protocolError->rel_5_ProtocolError_Extension.extended_reference.transaction_ID;
			}
		else
			{
			return KErrNotFound;
			}
		}
	else
		{
		return KErrNotFound;
		}
	return KErrNone;
	}


/**
SetExtendedReference()

Sets the extended reference parameters in the outgoing message, if they are set
in the passed container.

@param aRrlpRef on return, populated with the session reference details
@return KErrNotFound if no extended reference data is present, 
		KErrNone otherwise
*/	
TInt CRrlpProtocolError::SetExtendedReference(const TRrlpReference& aRrlpRef)
	{
	__ASSERT_DEBUG(iData->component.u.protocolError != NULL, User::Invariant());
	
	// if present, populate the optional Rel-5 extended reference
	if (aRrlpRef.aRel5EntendedRefPresent)
		{
		// mark the optional component present
		iData->component.u.protocolError->m.rel_5_ProtocolError_ExtensionPresent = 1;
		iData->component.u.protocolError->rel_5_ProtocolError_Extension.m.extended_referencePresent = 1;
		ASN1T_Extended_reference* extendedRef = &iData->component.u.protocolError->rel_5_ProtocolError_Extension.extended_reference;
		extendedRef->smlc_code = aRrlpRef.aRel5SmlcCode;
		extendedRef->transaction_ID = aRrlpRef.aRel5TransactionId;
		}
	else
		{
		return KErrNotFound;
		}
	
	return KErrNone;
	}
	

