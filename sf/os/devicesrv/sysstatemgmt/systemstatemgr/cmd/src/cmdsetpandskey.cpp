// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32strm.h>
#include <e32property.h>
#include <barsread2.h>

#include "ssmcommandparameters.h"
#include "cmdsetpandskey.h"
#include "ssmdebug.h"
#include "ssmpanic.h"

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Used to create an instance of CCmdSetPAndSKey class, from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity The severity of the command
@param aCategory The UID that identifies the property category.
@param aKey The property sub-key, i.e. the key that identifies the specific property within the category.
@param aValue The new property value.
@param aPriority The priority of the command in the list
@return	A pointer to an object of type CCmdSetPAndSKey.
*/
CCmdSetPAndSKey* CCmdSetPAndSKey::NewL(TCmdErrorSeverity aSeverity, const TUid& aCategory, TUint aKey, TInt aValue, const TUint16 aPriority)
	{
	CCmdSetPAndSKey* self = new (ELeave) CCmdSetPAndSKey(aSeverity, aCategory, aKey, aValue, aPriority);
	return self;
	}
#endif

/**
Used to create an instance of CCmdSetPAndSKey class, from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity The severity of the command
@param aCategory The UID that identifies the property category.
@param aKey The property sub-key, i.e. the key that identifies the specific property within the category.
@param aValue The new property value.
@return	A pointer to an object of type CCmdSetPAndSKey.
*/
CCmdSetPAndSKey* CCmdSetPAndSKey::NewL(TCmdErrorSeverity aSeverity, const TUid& aCategory, TUint aKey, TInt aValue)
	{
	CCmdSetPAndSKey* self = new (ELeave) CCmdSetPAndSKey(aSeverity, aCategory, aKey, aValue);
	return self;
	}

/**
Used to create an instance of CCmdSetPAndSKey class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

@param aReadStream Read stream containing data through which object can be created
@return	A pointer to an object of type CCmdSetPAndSKey.
*/
CCmdSetPAndSKey* CCmdSetPAndSKey::NewL(RReadStream& aReadStream)
	{
	CCmdSetPAndSKey* self = new (ELeave) CCmdSetPAndSKey();
	CleanupStack::PushL(self);
	self->ConstructL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdSetPAndSKey class from resource.

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdSetPAndSKey.
*/
CCmdSetPAndSKey* CCmdSetPAndSKey::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdSetPAndSKey* self = new (ELeave) CCmdSetPAndSKey();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdSetPAndSKey class from CCmdSetPAndSKey object
Must be used only by CLE
@param aCmdSetPAndSKey CCmdSetPAndSKey reference 
@return A pointer to an object of type CCmdSetPAndSKey.
*/
CCmdSetPAndSKey* CCmdSetPAndSKey::NewLC(const CCmdSetPAndSKey& aCmdSetPAndSKey)
    {
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
    CCmdSetPAndSKey* self = new (ELeave) CCmdSetPAndSKey(aCmdSetPAndSKey.Severity(),aCmdSetPAndSKey.Category(),
                                                         aCmdSetPAndSKey.Key(),aCmdSetPAndSKey.Value(),aCmdSetPAndSKey.Priority());
#else
    CCmdSetPAndSKey* self = new (ELeave) CCmdSetPAndSKey(aCmdSetPAndSKey.Severity(),aCmdSetPAndSKey.Category(),
                                                             aCmdSetPAndSKey.Key(),aCmdSetPAndSKey.Value());
#endif
    CleanupStack::PushL(self);
    self->ConstructL(aCmdSetPAndSKey);
    return self;
    }

void CCmdSetPAndSKey::ConstructL(const CCmdSetPAndSKey& aCmdSetPAndSKey)
    {
    iConditionalResourceId = aCmdSetPAndSKey.ConditionalInformation();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdSetPAndSKey.GetCommandResourceFileName());
        } 
#endif 
     }   

/**
Returns the version of the BIC
@return		The maximum supported version of the BIC
*/
TInt CCmdSetPAndSKey::MaxSupportedVersion()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	return (static_cast<TInt>(ECmdSetPAndSKeyVersionWithPriority));
#else
	return (static_cast<TInt>(ECmdSetPAndSKeyInitialVersion));
#endif	
	}

/**
Destructor
*/
CCmdSetPAndSKey::~CCmdSetPAndSKey()
	{
	Cancel();
	}

/**
Execute this command
@param aStatus 	The TRequestStatus which is completed when value is set either 
				with KErrNone or with error value.
*/
void CCmdSetPAndSKey::Execute(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;

	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

/**
Releases resources associated with this BIC
*/
void CCmdSetPAndSKey::Release()
	{
	delete this;
	}

/**
Cancels the BIC
*/
void CCmdSetPAndSKey::ExecuteCancel()
	{
	//cancel the outstanding request
	Cancel();
	//and complete the request with KErrCancel
	CompleteExecuteRequest(KErrCancel);
	}

/**
Returns the type of the BIC
@return Type of BIC
*/
TSsmCommandType CCmdSetPAndSKey::Type() const
	{
	return (ESsmCmdSetPAndSKey);
	}

/**
Externalises the configuration of the BIC

@param aWriteStream A write stream to write BIC data to
*/
 void CCmdSetPAndSKey::ExternalizeL(RWriteStream& aWriteStream) const
 	{
 	aWriteStream.WriteInt16L(iSeverity);
 	aWriteStream.WriteUint32L(iCategory.iUid);
 	aWriteStream.WriteUint32L(iKey);
 	aWriteStream.WriteInt16L(iValue);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	aWriteStream.WriteUint16L(iPriority);
#endif
 	}
 
void CCmdSetPAndSKey::RunL()
	{
	TInt err = RProperty::Set(iCategory, iKey, iValue);
	DEBUGPRINT3A("Value %d is set with err : %d", iValue, err);
	if (KErrNotFound == err)
		{
		DEBUGPRINT1A("key not defined");
		//try to define
		_LIT_SECURITY_POLICY_PASS(KReadPolicy);
		_LIT_SECURITY_POLICY_S0(KWritePolicy, RProcess().SecureId());
		DEBUGPRINT1A("Defining key");
 		err = RProperty::Define(iCategory, iKey, RProperty::EInt, KReadPolicy, KWritePolicy);
 		DEBUGPRINT2A("Key defined with err : %d", err);
	 	if (KErrNone == err)
	 		{
	 		DEBUGPRINT1A("Property defined, setting it again");
	 		// and set it again
	 		err = RProperty::Set(iCategory, iKey, iValue);
	 		DEBUGPRINT2A("Value set with err : %d", err);
	 		}		
		}
	//complete the request
	CompleteExecuteRequest(err);
	}

/**
Cancellation of an outstanding request.
*/
void CCmdSetPAndSKey::DoCancel()
	{
	// This command need not to cancel as iStatus is always completed before it's SetActive()
	}

/**
Complete the command request with aError. It gets called when RunL leaves.

@param aError Error code with which RunL has left.
@return KErrNone, as error is handled in this function.
*/
TInt CCmdSetPAndSKey::RunError(TInt aError)
	{
	DEBUGPRINT2A("CCmdSetPAndSKey::RunError called with reason %d", aError);
	CompleteExecuteRequest(aError);
	return KErrNone;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Overloaded constructor

@param aSeverity 	The severity of the command
@param aCategory 	The UID that identifies the property category.
@param aKey 		The property sub-key, i.e. the key that identifies the specific property within the 
					category.
@param aValue 		The new property value.
@param aPriority 	The priority of the command in the list
*/	
CCmdSetPAndSKey::CCmdSetPAndSKey(TCmdErrorSeverity aSeverity, const TUid& aCategory, TUint aKey, TInt aValue, const TUint16 aPriority)
	: CSsmCommandBase(aSeverity, aPriority), iCategory(aCategory), iKey(aKey), iValue(aValue)
	{
	}
#endif

/**
Overloaded constructor

@param aSeverity 	The severity of the command
@param aCategory 	The UID that identifies the property category.
@param aKey 		The property sub-key, i.e. the key that identifies the specific property within the 
					category.
@param aValue 		The new property value.
*/	
CCmdSetPAndSKey::CCmdSetPAndSKey(TCmdErrorSeverity aSeverity, const TUid& aCategory, TUint aKey, TInt aValue)
	: CSsmCommandBase(aSeverity), iCategory(aCategory), iKey(aKey), iValue(aValue)
	{
	}

/**
 Default constructor.
 */
CCmdSetPAndSKey::CCmdSetPAndSKey()
	{
	}

/**
Constructs the object through read stream.
@param aReadStream
*/
void CCmdSetPAndSKey::ConstructL(RReadStream& aReadStream)
	{
	InternalizeL(aReadStream);
	}

/**
Constructs an object from resource file.

@param aCommandParameters Object data from a resource file
*/
void CCmdSetPAndSKey::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdSetPAndSKey::MaxSupportedVersion()), KErrNotSupported);
	iSeverity = static_cast<TCmdErrorSeverity>(reader.ReadInt16L());
	iCategory = TUid::Uid(reader.ReadUint32L());
	iKey = reader.ReadUint32L();
	iValue = reader.ReadInt16L();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = (version > ECmdSetPAndSKeyInitialVersion) ? reader.ReadUint16L() : KDefaultCommandPriority;
#endif
	}

/**
Configures the BIC using data contained in a ReadStream
 
@param aReadStream 	A read stream containing BIC data
*/
void CCmdSetPAndSKey::InternalizeL(RReadStream& aReadStream)
	{
	iSeverity = static_cast<TCmdErrorSeverity>(aReadStream.ReadInt16L());
	iCategory.iUid = aReadStream.ReadUint32L();
	iKey = aReadStream.ReadUint32L();
	iValue = aReadStream.ReadInt16L();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = aReadStream.ReadUint16L();
#endif	
	}

