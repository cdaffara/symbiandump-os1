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
#include <barsread2.h>
#include <f32file.h>
#include <e32property.h>
#include <ssm/ssmstatemanager.h>

#include "cmdcreateswp.h"
#include "ssmdebug.h"
#include "ssmpanic.h"
#include "ssmcommandparameters.h"

/**
Used to create an instance of CCmdCreateSwp class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

@param aReadStream Read stream containing data through which object can be created
@return	A pointer to an object of type CCmdCreateSwp.
*/
CCmdCreateSwp* CCmdCreateSwp::NewL(RReadStream& aReadStream)
	{
	CCmdCreateSwp* self = new (ELeave) CCmdCreateSwp();
	CleanupStack::PushL(self);
	self->ConstructL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Used to create an instance of CCmdCreateSwp class from given parameters.
This method is used by SsmCommandFactory to create a command.
 
@param aSeverity The severity of the command
@param aSwpInfo	The system wide property info
@param aFilename The policy file to associate it to
@param aPriority The priority of the command in the list
@return	A pointer to an object of type CCmdCreateSwp.
*/
CCmdCreateSwp* CCmdCreateSwp::NewL(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwpInfo, const TDesC& aFilename, const TUint16 aPriority)
	{
	CCmdCreateSwp* self = new (ELeave) CCmdCreateSwp(aSeverity, aSwpInfo, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aFilename);
	CleanupStack::Pop(self);
	return self;
	}
#endif

/**
Used to create an instance of CCmdCreateSwp class from given parameters.
This method is used by SsmCommandFactory to create a command.
 
@param aSeverity The severity of the command
@param aSwpInfo	The system wide property info
@param aFilename The policy file to associate it to
@return	A pointer to an object of type CCmdCreateSwp.
*/
CCmdCreateSwp* CCmdCreateSwp::NewL(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwpInfo, const TDesC& aFilename)
	{
	CCmdCreateSwp* self = new (ELeave) CCmdCreateSwp(aSeverity, aSwpInfo);
	CleanupStack::PushL(self);
	self->ConstructL(aFilename);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdCreateSwp class from resource.

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdCreateSwp.
*/
CCmdCreateSwp* CCmdCreateSwp::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdCreateSwp* self = new (ELeave) CCmdCreateSwp();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdCreateSwp class from CCmdCreateSwp object
Must be used only by CLE
@param aCmdCreateSwp CCmdCreateSwp reference 
@return A pointer to an object of type CCmdCreateSwp.
*/
CCmdCreateSwp* CCmdCreateSwp::NewLC(const CCmdCreateSwp& aCmdCreateSwp)
    {
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
    CCmdCreateSwp* self = new (ELeave) CCmdCreateSwp(aCmdCreateSwp.Severity(),aCmdCreateSwp.SsmSwpInfo(),
                                                     aCmdCreateSwp.Priority());
#else
    CCmdCreateSwp* self = new (ELeave) CCmdCreateSwp(aCmdCreateSwp.Severity(),aCmdCreateSwp.SsmSwpInfo());
#endif
    CleanupStack::PushL(self);
    self->ConstructL(aCmdCreateSwp);
    return self;
    }

void CCmdCreateSwp::ConstructL(const CCmdCreateSwp& aCmdCreateSwp)
    {
    iConditionalResourceId = aCmdCreateSwp.ConditionalInformation();
    iFileName.CreateL(aCmdCreateSwp.FileName());
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdCreateSwp.GetCommandResourceFileName());
        }
#endif    
    }

/**
Destructor
*/	
CCmdCreateSwp::~CCmdCreateSwp()
	{
	Cancel();
	iSsm.Close();
	iFileName.Close();
	}

/**
Initialises the BIC's active object and initiates the command
 
@param aStatus The TRequestStatus of the active object calling this BIC
*/	
void CCmdCreateSwp::Execute(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;

	CompleteRequest(iStatus, iSsm.Connect());
	SetActive();
	}

/**
Releases resources associated with this BIC
*/	
void CCmdCreateSwp::Release()
	{
	delete this;
	}

/**
Initiates a Cancel on the object. 
*/	
void CCmdCreateSwp::ExecuteCancel()
	{
	//cancel the outstanding request
	Cancel();
	//and complete the request with KErrCancel
	CompleteExecuteRequest(KErrCancel);
	}

/**
Returns the type of the BIC
  
@return	The type of the BIC
*/	
TSsmCommandType CCmdCreateSwp::Type() const
	{
	return (ESsmCmdCreateSwp);
	}

/**
Returns the Version of the BIC

@return	The maximum supported version of the BIC
*/	
TInt CCmdCreateSwp::MaxSupportedVersion()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	return (static_cast<TInt>(ECmdCreateSwpVersionWithPriority));
#else
	return (static_cast<TInt>(ECmdCreateSwpInitialVersion));
#endif
	}

/**
Configures the BIC using data contained in a ReadStream

@param aReadStream 	A read stream containing BIC data
*/
void CCmdCreateSwp::InternalizeL(RReadStream& aReadStream)
	{
	iSeverity = static_cast<TCmdErrorSeverity>(aReadStream.ReadInt16L());
	const TUint key = aReadStream.ReadUint32L();
	const TInt value = aReadStream.ReadInt32L();	
	TSsmSwp swpInfo(key, value);
	iSwpInfo = swpInfo;
	HBufC* filename = HBufC::NewL(aReadStream, KMaxFileName);
	CleanupStack::PushL(filename);
	ConstructL(*filename);
	CleanupStack::PopAndDestroy(filename);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = aReadStream.ReadUint16L();
#endif	
	}

/**
Externalises the configuration of the BIC
  
@param aWriteStream A write stream to write BIC data to
*/
void CCmdCreateSwp::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt16L(iSeverity);
	aWriteStream.WriteUint32L(iSwpInfo.Key());	
	aWriteStream.WriteInt32L(iSwpInfo.Value());
	aWriteStream << iFileName;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	aWriteStream.WriteUint16L(iPriority);
#endif
	}

/**
Completes the tasks of the BIC
*/
void CCmdCreateSwp::RunL()
	{
	SSMLOGLEAVEIFERROR(iStatus.Int());

	TUid category = RProcess().SecureId();
	_LIT_SECURITY_POLICY_PASS(KReadPolicy);
	_LIT_SECURITY_POLICY_S0(KWritePolicy, RProcess().SecureId());
	TInt err = RProperty::Define(category, iSwpInfo.Key(), RProperty::EInt, KReadPolicy, KWritePolicy);
	if (KErrNone != err && KErrAlreadyExists != err)
		{
		SSMLOGLEAVE(err);
		}
	SSMLOGLEAVEIFERROR(RProperty::Set(category, iSwpInfo.Key(), iSwpInfo.Value()));
	SSMLOGLEAVEIFERROR(iSsm.RegisterSwpMapping(iSwpInfo.Key(), iFileName));
	CompleteExecuteRequest(KErrNone);	//control reaching here implies there are no errors
	}

/**
Called to handle any cleanup if RunL leaves

@param aError The error to finish with
@return	KErrNone
*/
TInt CCmdCreateSwp::RunError(TInt aError)
	{
	CompleteExecuteRequest(aError);
	return KErrNone;
	}

/**
Called during cancellation of the active BIC
*/
void CCmdCreateSwp::DoCancel()
	{
	}

/**
Default constructor.
*/
CCmdCreateSwp::CCmdCreateSwp()
	: iSwpInfo(0, 0)
	{
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Overloaded constructor

@param aSeverity The severity of the command
@param aSwpInfo The system wide property info
@param aPriority The priority of the command in the list
*/
CCmdCreateSwp::CCmdCreateSwp(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwpInfo, const TUint16 aPriority)
	: CSsmCommandBase(aSeverity, aPriority), iSwpInfo(aSwpInfo)
	{
	}
#endif

/**
Overloaded constructor

@param aSeverity The severity of the command
@param aSwpInfo The system wide property info
*/
CCmdCreateSwp::CCmdCreateSwp(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwpInfo)
	: CSsmCommandBase(aSeverity), iSwpInfo(aSwpInfo)
	{
	}

/**
Constructs the object through read stream.

@param aFileName Name of the dll implementing the MSsmSwpPolicy interface.
*/
void CCmdCreateSwp::ConstructL(const TDesC& aFileName)
	{
	iFileName.CreateL(aFileName.Length());
	iFileName.Append(aFileName);

	ValidateL();
	}

/**
Constructs the object through read stream and validates the data.

@param aReadStream A read stream containing BIC data
*/
void CCmdCreateSwp::ConstructL(RReadStream& aReadStream)
	{
	InternalizeL(aReadStream);
	ValidateL();
	}

/**
Validate the commands data
@leave KErrArgument If file name is not valid
*/
void CCmdCreateSwp::ValidateL()
	{
	RFs fs;
	SSMLOGLEAVEIFERROR(fs.Connect());
	TBool isValidFileName = fs.IsValidName(iFileName);
	fs.Close();
	if (!isValidFileName)
		{
		SSMLOGLEAVE(KErrArgument);
		}
	}

/**
Constructs an object from resource file and validates the data..

@param aCommandParameters Object data from a resource file
*/
void CCmdCreateSwp::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();

	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdCreateSwp::MaxSupportedVersion()), KErrNotSupported);
	iSeverity = static_cast<TCmdErrorSeverity>(reader.ReadInt16L());
	const TUint key = {reader.ReadUint32L()};
	const TInt32 value = reader.ReadInt32L();
	const TSsmSwp swpInfo(key, value);
	iSwpInfo = swpInfo;
	const TPtrC fileName = reader.ReadTPtrCL();
	ConstructL(fileName);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = (version > ECmdCreateSwpInitialVersion) ? reader.ReadUint16L() : KDefaultCommandPriority;	
#endif
	}

