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

#include <barsread2.h>
#include <s32strm.h>
#include <ssm/ssmstartupproperties.h>
#include "ssmdebug.h"
#include "ssmpanic.h"

/**
 * Constructs and returns a blank CSsmStartupProperties.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C CSsmStartupProperties* CSsmStartupProperties::NewL()
	{
  	CSsmStartupProperties* self = CSsmStartupProperties::NewLC();
	CleanupStack::Pop(self);
    return self;
	}

/**
 * Constructs and returns a blank CSsmStartupProperties.
 * 
 * Pushes the returned object onto the cleanup stack.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C CSsmStartupProperties* CSsmStartupProperties::NewLC()
	{
	CSsmStartupProperties* self = new(ELeave) CSsmStartupProperties();
	CleanupStack::PushL(self);
	return self;
	}

/**
 * Constructs and returns a CSsmStartupProperties.
 * 
 * @param aFileName The filename to use for the returned object.
 * @param aArgs The arguments to use for the returned object.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C CSsmStartupProperties* CSsmStartupProperties::NewL(const TDesC& aFileName, const TDesC& aArgs)
	{
	CSsmStartupProperties* self = CSsmStartupProperties::NewLC(aFileName, aArgs);
	CleanupStack::Pop(self);
    return self;	
	}

/**
 * Constructs and returns a CSsmStartupProperties.
 * 
 * Pushes the returned object onto the cleanup stack.
 * 
 * @param aFileName The filename to use for the returned object.
 * @param aArgs The arguments to use for the returned object.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C CSsmStartupProperties* CSsmStartupProperties::NewLC(const TDesC& aFileName, const TDesC& aArgs)
	{
	CSsmStartupProperties* self = CSsmStartupProperties::NewLC();
	self->SetFileParamsL(aFileName, aArgs);
	return self;
	}

/**
 * Constructs and returns a CSsmStartupProperties from a resource file.
 * 
 * @param aReader A reader at the location of the startup properties to be read.
 * @param aResourceFile The resource file containing the information to be read. 
 * @param aCommandType The command type for the properties being constructed.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C CSsmStartupProperties* CSsmStartupProperties::NewL(RResourceReader& aReader, CResourceFile* aResourceFile, TSsmCommandType aCommandType)
	{
	CSsmStartupProperties* self = CSsmStartupProperties::NewLC(aReader, aResourceFile, aCommandType);
	CleanupStack::Pop(self);
	return self;
	}

/**
 * Constructs and returns a CSsmStartupProperties from a resource file.
 * 
 * Pushes the returned object onto the cleanup stack.
 * 
 * @param aReader A reader at the location of the startup properties to be read.
 * @param aResourceFile The resource file containing the information to be read. 
 * @param aCommandType The command type for the properties being constructed.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C CSsmStartupProperties* CSsmStartupProperties::NewLC(RResourceReader& aReader, CResourceFile* aResourceFile, TSsmCommandType aCommandType)
	{
	CSsmStartupProperties* self = CSsmStartupProperties::NewLC();
	self->ConstructFromResourceL(aReader, aResourceFile, aCommandType);
	return self;
	}

/**
 * Constructs and returns a copied CSsmStartupProperties.
 * 
 * @param aStartupProperties The CSsmStartupProperties object to copy.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C CSsmStartupProperties* CSsmStartupProperties::NewL(const CSsmStartupProperties& aStartupProperties)
	{
	CSsmStartupProperties* self = CSsmStartupProperties::NewLC();
	self->ConstructL(aStartupProperties);
	CleanupStack::Pop(self);
	return self;
	}

/**
 * @internalTechnology
 */
void CSsmStartupProperties::ConstructFromResourceL(RResourceReader& aReader, CResourceFile* aResourceFile, TSsmCommandType aCommandType)
	{
	TPtrC fileName = aReader.ReadTPtrCL();			//LTEXT name
	TPtrC arguments = aReader.ReadTPtrCL();			//LTEXT args
	SetFileParamsL(fileName, arguments);
	iCommandType = aCommandType;
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(aReader.ReadUint8L()); //BYTE execution_behaviour
	iTimeout = aReader.ReadInt32L();				//LONG timeout
	iRetries = aReader.ReadInt16L();				//WORD retries
	if (ESsmCmdStartApp == aCommandType)
		{
		iViewless = aReader.ReadUint8L();				//BYTE viewless
		iStartInBackground = aReader.ReadUint8L();		//BYTE startInBackground
		}

	const TInt monitorResourceId = aReader.ReadUint32L();			//LLINK monitor_info
	if (monitorResourceId < 0)
		{
		SSMLOGLEAVE(KErrArgument);
		}
	else if (monitorResourceId > 0)
		{
		// open a new reader
		RResourceReader tempReader;
		// move reader to monitorResourceId
		tempReader.OpenL(aResourceFile, monitorResourceId);
		CleanupClosePushL(tempReader);
		const TSsmCommandType type = static_cast<TSsmCommandType>(tempReader.ReadInt16L());
		SSMLOGLEAVEIFFALSE(type == ESsmMonitorInfo, KErrNotSupported);
		const TInt version = tempReader.ReadInt16L();
		SSMLOGLEAVEIFFALSE((version <= ESsmMonitorInfoInitialVersion) && (version > 0), KErrNotSupported);
		iMonitoringRequired = ETrue;
		iMonitorInfo.iRestartPolicy = static_cast<TSsmRecoveryMethod>(tempReader.ReadUint8L());	//BYTE restart_policy
		iMonitorInfo.iRestartMode = tempReader.ReadUint8L();	//BYTE restart_mode
		iMonitorInfo.iTimeout = tempReader.ReadInt32L();		//LONG timeout
		iMonitorInfo.iRetries = tempReader.ReadInt16L();		//WORD retries
		CleanupStack::PopAndDestroy(&tempReader); 
		}
	}

/**
 * Frees any resources allocated to this object.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C CSsmStartupProperties::~CSsmStartupProperties()
	{
	delete iArgs;
	delete iFileName;
	}

/**
Sets file parameters that is file name and arguments
@param aFileName A file name containing full path to be used to start up a component.
@param aArgs A string that is used as an argument to the component when starting a process or application
*/
EXPORT_C void CSsmStartupProperties::SetFileParamsL(const TDesC& aFileName, const TDesC& aArgs)
	{
	HBufC* fname = aFileName.AllocL();
	delete iFileName;
	iFileName = fname;
	TPtr writeableFilename = iFileName->Des();
	writeableFilename.TrimAll();	// remove leading and trailing whitespace characters from filename

	HBufC* args = aArgs.AllocL();
	delete iArgs; 
	iArgs = args;	
	TPtr writeableArgs = iArgs->Des();
	writeableArgs.TrimAll();	// remove leading and trailing whitespace characters from arguments
	}

/**
Sets the command type for startup properties. It can be ESsmCmdStartProcess or ESsmCmdStartApp.
@param aType startup type of TSsmCommandType
@leave KErrArgument If aCommandType is not ESsmCmdStartProcess or ESsmCmdStartApp.
@see TSsmCommandType
*/
EXPORT_C void CSsmStartupProperties::SetCommandTypeL(TSsmCommandType aCommandType)
	{
	SSMLOGLEAVEIFFALSE(((aCommandType == ESsmCmdStartProcess) || (aCommandType == ESsmCmdStartApp)), KErrArgument);
	iCommandType = aCommandType;
	}

/**
Sets the execution behaviour.
@param aExecutionBehaviour Execution behaviour of type TSsmExecutionBehaviour.
@see TSsmExecutionBehaviour
*/
EXPORT_C void CSsmStartupProperties::SetExecutionBehaviour(TSsmExecutionBehaviour aExecutionBehaviour)
	{
	iExecutionBehaviour = aExecutionBehaviour;
	}

/**
Sets number of retries. These retries are used to launch application or process.
@param aRetries Number of retries to set.
*/
EXPORT_C void CSsmStartupProperties::SetRetries(TInt aRetries)
	{
	iRetries = aRetries;
	}

/**
Sets the value of timeout
@param aTimeout Timeout to set
*/
EXPORT_C void CSsmStartupProperties::SetTimeout(TInt32 aTimeout)
	{
	iTimeout = aTimeout;
	}

/**
Sets the monitor information. This monitoring information is used by RSysMonSession::MonitorL()
@param aMonitorInfo 
@see RSysMonSession::MonitorL
*/
EXPORT_C void CSsmStartupProperties::SetMonitorInfoL(TSsmMonitorInfo aMonitorInfo)
	{
	iMonitoringRequired = ETrue;
	iMonitorInfo.iRestartPolicy = aMonitorInfo.iRestartPolicy;
	iMonitorInfo.iRestartMode = aMonitorInfo.iRestartMode;
	iMonitorInfo.iTimeout = aMonitorInfo.iTimeout;
	iMonitorInfo.iRetries = aMonitorInfo.iRetries;
	}

/**
Set the value of member Viewless
@param aViewless Boolean value to set.
*/
EXPORT_C void CSsmStartupProperties::SetViewless(TBool aViewless)
	{
	iViewless = aViewless;
	}

/**
Sets an extra startup in background (used only for applications)
@param aStartInBackground ETrue if the application should start in the background.
*/
EXPORT_C void CSsmStartupProperties::SetStartInBackground(TBool aStartInBackground)
	{
	iStartInBackground = aStartInBackground;
	}

/**
Returns file name specified in startup properties.
@return Return file name specified, or KNullDesC empty file name is given.
*/
EXPORT_C TPtrC CSsmStartupProperties::FileName() const
	{
	return (iFileName) ? *iFileName : KNullDesC();
	}

/**
Returns argument spcified in startup properties.
@return Return argument specified, or KNullDesC empty argument is given.
*/
EXPORT_C TPtrC CSsmStartupProperties::Args() const
	{
	return (iArgs) ? *iArgs : KNullDesC();
	}

/**
Returns command type i.e. ESsmCmdStartApp/ESsmCmdStartProcess
@return Command type.
*/
EXPORT_C TSsmCommandType CSsmStartupProperties::CommandType() const
	{
	return iCommandType;
	}

/**
Returns execution behaviour of process/application.
@return Execution behaviour of type TSsmExecutionBehaviour.
@see TSsmExecutionBehaviour
*/
EXPORT_C TSsmExecutionBehaviour CSsmStartupProperties::ExecutionBehaviour() const
	{
	return iExecutionBehaviour;
	}

/**
Returns number of retries.
@return Number of retries.
*/
EXPORT_C TInt CSsmStartupProperties::Retries() const
	{
	return iRetries;
	}

/**
Returns the timeout value.
@return Timeout value
*/
EXPORT_C TInt32 CSsmStartupProperties::Timeout() const
	{
	return iTimeout;
	}

/**
Function returns monitored property.
@return ETrue if monitor info is set, EFalse otherwise.
*/
EXPORT_C TBool CSsmStartupProperties::IsMonitoringRequired() const
	{
	return iMonitoringRequired;
	}

/**
Function returns viewless property.
@return Viewless property.
*/
EXPORT_C TBool CSsmStartupProperties::Viewless() const
	{
	return iViewless;
	}

/**
Function returns start in background property.
@return Start in background property.
*/
EXPORT_C TBool CSsmStartupProperties::StartInBackground() const
	{
	return iStartInBackground;
	}

/**
Externalises the values of startup properties to write stream.

@param aWriteStream A write stream to write startup properties.
*/
EXPORT_C void CSsmStartupProperties::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint16L(iCommandType);
	aWriteStream << FileName();
	aWriteStream << Args();
	aWriteStream.WriteUint8L(iExecutionBehaviour);
	aWriteStream.WriteInt32L(iTimeout);
	aWriteStream.WriteInt16L(iRetries);
	aWriteStream.WriteUint8L(iViewless);
	aWriteStream.WriteUint8L(iStartInBackground);

	aWriteStream.WriteUint8L(iMonitoringRequired ? ETrue : EFalse);
	if (iMonitoringRequired)
		{
		aWriteStream.WriteUint8L(iMonitorInfo.iRestartPolicy);
		aWriteStream.WriteUint8L(iMonitorInfo.iRestartMode);
		aWriteStream.WriteInt32L(iMonitorInfo.iTimeout);
		aWriteStream.WriteInt16L(iMonitorInfo.iRetries);
		}
	}

/**
Configures startup property using read stream.
@leave EInvalidCmdType If command type is not ESsmCmdStartProcess / ESsmCmdStartApp
@param aReadStream 	A read stream containing startup property.
*/
EXPORT_C void CSsmStartupProperties::InternalizeL(RReadStream& aReadStream)
	{
	iCommandType = static_cast<TSsmCommandType>(aReadStream.ReadUint16L());
	
	__ASSERT_DEBUG(((iCommandType == ESsmCmdStartApp) || (iCommandType == ESsmCmdStartProcess)), PanicNow(KPanicStartupProperties, EInvalidCmdType));
	iFileName = HBufC::NewL(aReadStream, KMaxFileName);
	iArgs = HBufC::NewL(aReadStream, KMaxFileName);
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(aReadStream.ReadUint8L());
	iTimeout = aReadStream.ReadInt32L();
	iRetries = aReadStream.ReadInt16L();
	iViewless = aReadStream.ReadUint8L();
	iStartInBackground = aReadStream.ReadUint8L();

	iMonitoringRequired = aReadStream.ReadUint8L();
	if (iMonitoringRequired)
		{
		iMonitorInfo.iRestartPolicy = static_cast<TSsmRecoveryMethod>(aReadStream.ReadUint8L());
		iMonitorInfo.iRestartMode = aReadStream.ReadUint8L();
		iMonitorInfo.iTimeout = aReadStream.ReadInt32L();
		iMonitorInfo.iRetries = aReadStream.ReadInt16L();
		}
	}
/**
Reset all the values to default.
*/
EXPORT_C void CSsmStartupProperties::Reset()
	{
	iCommandType = ESsmCmdStartProcess;	
	delete iFileName;
	iFileName = NULL;
	delete iArgs;
	iArgs = NULL;
	iExecutionBehaviour = ESsmFireAndForget;
	iTimeout = 0;
	iRetries = 0;
	iViewless = EFalse;
	iStartInBackground = EFalse;
	iMonitoringRequired = EFalse;
	}

/**
Default CTOR
*/
CSsmStartupProperties::CSsmStartupProperties()
	: iCommandType(ESsmCmdStartProcess), iExecutionBehaviour(ESsmFireAndForget), iMonitoringRequired(EFalse)
	{
	}

/**
Construct startup properties from the given properties
*/
void CSsmStartupProperties::ConstructL(const CSsmStartupProperties& aStartupProperties)
	{
	iCommandType = aStartupProperties.CommandType();
	SetFileParamsL(aStartupProperties.FileName(), aStartupProperties.Args());
	iExecutionBehaviour = aStartupProperties.ExecutionBehaviour();
	iTimeout = aStartupProperties.Timeout();
	iRetries = aStartupProperties.Retries();
	iViewless = aStartupProperties.Viewless();	//this value is used only for applications
	iStartInBackground = aStartupProperties.StartInBackground();//this value is used only for applications

	if (aStartupProperties.IsMonitoringRequired())
		{
		iMonitoringRequired = ETrue;
		iMonitorInfo.iRestartPolicy = aStartupProperties.iMonitorInfo.iRestartPolicy;
		iMonitorInfo.iRestartMode = aStartupProperties.iMonitorInfo.iRestartMode;
		iMonitorInfo.iTimeout = aStartupProperties.iMonitorInfo.iTimeout;
		iMonitorInfo.iRetries = aStartupProperties.iMonitorInfo.iRetries;
		}
	else
		{
		iMonitoringRequired = EFalse;
		}
	}

/**
Gets the monitor information.
@return Pointer to monitor info object if Monitoring is set to True otherwise NULL.
*/
EXPORT_C TSsmMonitorInfo CSsmStartupProperties::MonitorInfo() const
	{
	return iMonitorInfo;
	}

