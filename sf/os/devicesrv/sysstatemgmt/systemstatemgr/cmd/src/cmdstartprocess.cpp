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

#include "ssmcommandparameters.h"
#include "cmdstartprocess.h"
#include "ssmdebug.h"
#include "ssmpanic.h"

/**
Used to create an instance of CCmdStartProcess class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

@param aReadStream 	Read stream containing data through which object can be created
@return				A pointer to an object of type CCmdStartProcess.
*/
CCmdStartProcess* CCmdStartProcess::NewL(RReadStream& aReadStream)
	{
	CCmdStartProcess* self = new (ELeave) CCmdStartProcess();
	CleanupStack::PushL(self);
	self->ConstructL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Used to create an instance of CCmdStartProcess class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity The severity of the command
@param aAppInfo Contains information which can be used to start the process
@param aPriority The priority of the command in the list
@panic ECmdNullPtr if the information used to create command is null
@return	A pointer to an object of type CCmdStartProcess.
*/
CCmdStartProcess* CCmdStartProcess::NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo, const TUint16 aPriority)
	{
	__ASSERT_ALWAYS(aAppInfo != NULL, PanicNow(KPanicCmdStartProcess, ECmdNullPtr));
	CCmdStartProcess* self = new (ELeave) CCmdStartProcess(aSeverity, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aAppInfo);
	CleanupStack::Pop(self);
	return self;
	}
#endif

/**
Used to create an instance of CCmdStartProcess class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity The severity of the command
@param aAppInfo Contains information which can be used to start the process
@panic ECmdNullPtr if the information used to create command is null
@return	A pointer to an object of type CCmdStartProcess.
*/
CCmdStartProcess* CCmdStartProcess::NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo)
	{
	__ASSERT_ALWAYS(aAppInfo != NULL, PanicNow(KPanicCmdStartProcess, ECmdNullPtr));
	CCmdStartProcess* self = new (ELeave) CCmdStartProcess(aSeverity);
	CleanupStack::PushL(self);
	self->ConstructL(aAppInfo);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdStartProcess class from CCmdStartProcess object
Must be used only by CLE
@param aCmdStartProcess CCmdStartProcess reference 
@param aUtilProvider CSsmCommandUtilProvider reference 
@return A pointer to an object of type CCmdStartProcess.
*/
CCmdStartProcess* CCmdStartProcess::NewLC(const CCmdStartProcess& aCmdStartProcess, CSsmCommandUtilProvider* aUtilProvider)
    {
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
    CCmdStartProcess* self = new (ELeave) CCmdStartProcess(aCmdStartProcess.Severity(),aCmdStartProcess.Priority());
#else
    CCmdStartProcess* self = new (ELeave) CCmdStartProcess(aCmdStartProcess.Severity());
#endif
    CleanupStack::PushL(self);
    self->ConstructL(aCmdStartProcess, aUtilProvider);
    return self;
    }

void CCmdStartProcess::ConstructL(const CCmdStartProcess& aCmdStartProcess, CSsmCommandUtilProvider* aUtilProvider)
    {
    iConditionalResourceId = aCmdStartProcess.ConditionalInformation();
	SetUtilProvider(*aUtilProvider);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdStartProcess.GetCommandResourceFileName());
        }
#endif
    CCmdStarterBase::ConstructL(aCmdStartProcess.AppInfo(),FALSE);
    }

/**
Used to create an instance of CCmdStartProcess class from resource.

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdStartProcess.
*/
CCmdStartProcess* CCmdStartProcess::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdStartProcess* self = new (ELeave) CCmdStartProcess();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}

/**
Returns the version of the BIC
@return		The maximum supported version of the BIC
*/
TInt CCmdStartProcess::MaxSupportedVersion()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	return (static_cast<TInt>(ECmdStartProcessVersionWithPriority));
#else
	return (static_cast<TInt>(ECmdStartProcessInitialVersion));
#endif	
	}

/**
Destructor
*/
CCmdStartProcess::~CCmdStartProcess()
	{
	}

/**
Overloaded constructor
*/
CCmdStartProcess::CCmdStartProcess()
	: CCmdStarterBase(ESsmCmdStartProcess)
	{
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Overloaded constructor
@param aSeverity Severity of command
@param aPriority The priority of the command in the list
*/
CCmdStartProcess::CCmdStartProcess(TCmdErrorSeverity aSeverity, const TUint16 aPriority)
	: CCmdStarterBase(ESsmCmdStartProcess, aSeverity, aPriority)
	{
	}
#endif

/**
Overloaded constructor
@param aSeverity Severity of command
*/
CCmdStartProcess::CCmdStartProcess(TCmdErrorSeverity aSeverity)
	: CCmdStarterBase(ESsmCmdStartProcess, aSeverity)
	{
	}

/**
Constructs an object from resource file.
@param aCommandParameters Object data from a resource file
*/
void CCmdStartProcess::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdStartProcess::MaxSupportedVersion()), KErrNotSupported);
	CCmdStarterBase::ConstructL(aCommandParameters);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = (version > ECmdStartProcessInitialVersion)? reader.ReadUint16L() : KDefaultCommandPriority ;
#endif
	}

/**
Configures the BIC using data contained in a ReadStream
@param aReadStream 	A read stream containing BIC data
*/
void CCmdStartProcess::ConstructL(RReadStream& aReadStream)
	{
	CCmdStarterBase::ConstructL(aReadStream);
	}

/**
Constructs an object from aAppInfo.
@param aAppInfo Startup properties through which command will be constructed.
*/
void CCmdStartProcess::ConstructL(const CSsmStartupProperties *aAppInfo)
	{
	CCmdStarterBase::ConstructL(aAppInfo);
	}
