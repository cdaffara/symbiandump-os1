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

#include "cmdstartapp.h"
#include "ssmdebug.h"
#include "ssmpanic.h"

/**
Used to create an instance of CCmdStartApp class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

@param aReadStream 	Read stream containing data through which object can be created
@return	A pointer to an object of type CCmdStartApp.
*/
CCmdStartApp* CCmdStartApp::NewL(RReadStream& aReadStream)
	{
	CCmdStartApp* self = new (ELeave) CCmdStartApp();
	CleanupStack::PushL(self);
	self->ConstructL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Used to create an instance of CCmdStartApp class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity The severity of the command
@param aAppInfo Contains information which can be used to start the process
@param aPriority The priority of the command in the list
@panic ECmdNullPtr if the information used to create command is null
@return	A pointer to an object of type CCmdStartApp.
*/
CCmdStartApp* CCmdStartApp::NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo, const TUint16 aPriority)
	{
	__ASSERT_ALWAYS(aAppInfo != NULL, PanicNow(KPanicCmdStartApp, ECmdNullPtr));
	CCmdStartApp* self = new (ELeave) CCmdStartApp(aSeverity, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aAppInfo);
	CleanupStack::Pop(self);
	return self;
	}
#endif

/**
Used to create an instance of CCmdStartApp class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity The severity of the command
@param aAppInfo Contains information which can be used to start the process
@panic ECmdNullPtr if the information used to create command is null
@return	A pointer to an object of type CCmdStartApp.
*/
CCmdStartApp* CCmdStartApp::NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo)
	{
	__ASSERT_ALWAYS(aAppInfo != NULL, PanicNow(KPanicCmdStartApp, ECmdNullPtr));
	CCmdStartApp* self = new (ELeave) CCmdStartApp(aSeverity);
	CleanupStack::PushL(self);
	self->ConstructL(aAppInfo);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdStartApp class from resource.

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdStartApp.
*/
CCmdStartApp* CCmdStartApp::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdStartApp* self = new (ELeave) CCmdStartApp();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self); 
	return self;
	}

/**
Used to create an instance of CCmdStartApp class from CCmdStartApp object
Must be used only by CLE
@param aCmdStartApp CCmdStartApp reference 
@param aUtilProvider CSsmCommandUtilProvider reference 
@return A pointer to an object of type CCmdStartApp.
*/
CCmdStartApp* CCmdStartApp::NewLC(const CCmdStartApp& aCmdStartApp, CSsmCommandUtilProvider* aUtilProvider)
    {
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
    CCmdStartApp* self = new (ELeave) CCmdStartApp(aCmdStartApp.Severity(),aCmdStartApp.Priority());
#else
    CCmdStartApp* self = new (ELeave) CCmdStartApp(aCmdStartApp.Severity());
#endif
    CleanupStack::PushL(self);
    self->ConstructL(aCmdStartApp, aUtilProvider);
    return self;
    }

void CCmdStartApp::ConstructL(const CCmdStartApp& aCmdStartApp, CSsmCommandUtilProvider* aUtilProvider)
    {
    iConditionalResourceId = aCmdStartApp.ConditionalInformation();
	SetUtilProvider(*aUtilProvider);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdStartApp.GetCommandResourceFileName());
        }
#endif
    CCmdStarterBase::ConstructL(aCmdStartApp.AppInfo(),FALSE);
    }

/**
Returns the version of the BIC
@return		The maximum supported version of the BIC
*/
TInt CCmdStartApp::MaxSupportedVersion()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	return (static_cast<TInt>(ECmdStartAppVersionWithPriority));
#else
	return (static_cast<TInt>(ECmdStartAppInitialVersion));
#endif
	}

/**
Destructor
*/
CCmdStartApp::~CCmdStartApp()
	{
	}

/**
Overloaded constructor.
*/
CCmdStartApp::CCmdStartApp()
	: CCmdStarterBase(ESsmCmdStartApp)
	{
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Overloaded constructor
@param aSeverity Severity of command
@param aPriority The priority of the command in the list
*/
CCmdStartApp::CCmdStartApp(TCmdErrorSeverity aSeverity, const TUint16 aPriority)
	: CCmdStarterBase(ESsmCmdStartApp, aSeverity, aPriority)
	{
	}
#endif


/**
Overloaded constructor
@param aSeverity Severity of command
*/
CCmdStartApp::CCmdStartApp(TCmdErrorSeverity aSeverity)
	: CCmdStarterBase(ESsmCmdStartApp, aSeverity)
	{
	}

/**
Constructs an object from resource file.
@param aCommandParameters Object data from a resource file
*/
void CCmdStartApp::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdStartApp::MaxSupportedVersion()), KErrNotSupported);
	CCmdStarterBase::ConstructL(aCommandParameters);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = (version > ECmdStartAppInitialVersion)? reader.ReadUint16L() : KDefaultCommandPriority ;
#endif
	}

/**
Configures the BIC using data contained in a ReadStream
@param aReadStream 	A read stream containing BIC data
*/
void CCmdStartApp::ConstructL(RReadStream& aReadStream)
	{
	CCmdStarterBase::ConstructL(aReadStream);
	}

/**
Constructs an object from aAppInfo.
@param aAppInfo Startup properties through which command will be constructed.
*/
void CCmdStartApp::ConstructL(const CSsmStartupProperties *aAppInfo)
	{
	CCmdStarterBase::ConstructL(aAppInfo);
	}

