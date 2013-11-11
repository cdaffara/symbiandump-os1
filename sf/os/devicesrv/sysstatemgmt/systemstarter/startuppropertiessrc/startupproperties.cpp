// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "startupproperties.h"
#include <barsread.h>
#include <e32debug.h>
#include <s32mem.h>
#include <e32hal.h>
#include <e32modes.h>

#include "SysStartDebug.h"
#include "sysstartpanic.h"

/** Internal version for the CStartupProperties object created by the NewL factory 
@internalTechnology
*/
const TInt KParameterObjectVersion = 1; 

/**
Used to create an instance of CStartupProperties class.
@return An instance of CStartupProperties
@publishedAll
@deprecated
*/
EXPORT_C CStartupProperties* CStartupProperties::NewL()
	{
    CStartupProperties* properties = CStartupProperties::NewLC();
	CleanupStack::Pop(properties);
    return properties;
	}

/**
Used to create an instance of CStartupProperties class.
The new instance is put on the CleanupStack.
@return An instance of CStartupProperties
@publishedAll
@deprecated
*/
EXPORT_C CStartupProperties* CStartupProperties::NewLC()
	{
	CStartupProperties* properties = new(ELeave) CStartupProperties();
	CleanupStack::PushL(properties);
	properties->ConstructL();
	return properties;
	}

/**
Used to create an instance of CStartupProperties class with the specified process 
filename and arguments.
@param aFileName A file name containing full path to be used to start up a component.
@param aArgs A string that will be used as an argument to the component when starting.
@return An instance of CStartupProperties
@publishedAll
@deprecated
*/
EXPORT_C CStartupProperties* CStartupProperties::NewL(const TDesC& aFileName, const TDesC& aArgs)
	{
	CStartupProperties* properties = CStartupProperties::NewLC(aFileName, aArgs);
	CleanupStack::Pop(properties);
    return properties;
	}	
	
/**
Used to create an instance of CStartupProperties class with the specified process 
filename and arguments. The new instance is pushed onto the CleanupStack.
@see CStartupProperties::NewL(const TDesC& aFileName, const TDesC& aArgs)
@publishedAll
@deprecated
*/
EXPORT_C CStartupProperties* CStartupProperties::NewLC(const TDesC& aFileName, const TDesC& aArgs)
	{
	CStartupProperties* properties = CStartupProperties::NewLC();
	properties->SetFileParamsL(aFileName, aArgs);
	return properties;
	}	

/**
Used to create an instance of CStartupProperties class from a resource definition.
Supported resource structs are @c START_PROCESS_INFO, @c START_PROCESS_INFO2, 
@c START_APP_INFO and @c START_APP_INFO2. 
@return An instance of CStartupProperties
@param aSource The resource reader with which to access the resource values
@publishedAll
@deprecated
@panic if struct is not supported.
*/
EXPORT_C CStartupProperties* CStartupProperties::NewL(TResourceReader& aSource)
	{
	CStartupProperties* properties = CStartupProperties::NewLC(aSource);
	CleanupStack::Pop(properties);
	return properties;
	}

/**
Used to create an instance of CStartupProperties class from a resource definition.
The new instance is put on the CleanupStack.
@see CStartupProperties::NewL(TResourceReader& aResourceReader)
@publishedAll
@deprecated
@panic if struct is not supported.
*/
EXPORT_C CStartupProperties* CStartupProperties::NewLC(TResourceReader& aSource)
	{
	CStartupProperties* properties = CStartupProperties::NewLC();
	properties->ConstructFromResourceL(aSource);
	return properties;
	}

CStartupProperties::CStartupProperties() : 
	iVersion(KParameterObjectVersion),
	iStartupType(EStartProcess), 	
	iStartMethod(EFireAndForget),
	iRestartMode(EStartupModeUndefined)
	{
	iRecoveryMethod.iRecoveryMethod = EIgnoreOnFailure;
	}
	
void CStartupProperties::ConstructL()
	{
	}

/**
Reset all variables in this instance back to constructor defaults.
@publishedAll
@deprecated
*/
EXPORT_C void CStartupProperties::Reset()
	{
	//from constructor
	iVersion = KParameterObjectVersion;
	iStartupType = EStartProcess;	
	iStartMethod = EFireAndForget;
	iRestartMode = EStartupModeUndefined;
	iRecoveryMethod.iRecoveryMethod = EIgnoreOnFailure;
	//from CBase
	delete iArgs;
	delete iFileName;
	iNoOfRetries = 0;
	iTimeout = 0;
	iMonitored = EFalse;
	iViewless = EFalse;
	iStartInBackground = EFalse;
	}

/**
Destructor for CStartupProperties class
@publishedAll
@deprecated
*/
EXPORT_C CStartupProperties::~CStartupProperties()
	{
	delete iArgs;
	delete iFileName;
	}

/** Accessor function returning internal version of the parameter object
@publishedAll
@deprecated
@return Internal version of the parameter object
*/
EXPORT_C TInt CStartupProperties::Version() const
	{ 
	return iVersion; 
	}

/** Sets file parameters
@publishedAll
@deprecated
@param aFileName A file name containing full path to be used to start up a component.
@param aArgs A string that will be used as an argument to the component when starting.
*/
EXPORT_C void CStartupProperties::SetFileParamsL(const TDesC& aFileName, const TDesC& aArgs)
	{
	HBufC* fname = aFileName.AllocL();
	delete iFileName;
	iFileName = fname;
	TPtr writeableFilename = iFileName->Des();
	writeableFilename.TrimAll();
	
	HBufC* args = aArgs.AllocL();
	delete iArgs; 
	iArgs = args;	
	TPtr writeableArgs = iArgs->Des();
	writeableArgs.TrimAll();
	}

/** Sets startup type parameter
@publishedAll
@deprecated
@param aStartupType EStartProcess for starting processes, EStartApp for starting applications
*/
EXPORT_C void CStartupProperties::SetStartupType(TStartupType aStartupType) 
	{ 
	iStartupType = aStartupType; 
	}

/** Sets startup method parameter.
@publishedAll
@deprecated
@param aStartMethod The method to be used to start up the component. 
*/
EXPORT_C void CStartupProperties::SetStartMethod(TStartMethod aStartMethod)
	{ 
	iStartMethod = aStartMethod; 
	}

/** Sets failure parameter @c NoOfRetries. Only valid when @c TStartMethod = @c EWaitForStart.
@publishedAll
@deprecated
@param aNoOfRetries Number of retries to attempt if the component fails to start for the first 
					time during system startup. Also determines if a monitored component should be
					restarted when it fails. Only valid when @c TStartMethod = @c EWaitForStart.
*/
EXPORT_C void CStartupProperties::SetNoOfRetries(TInt aNoOfRetries)
	{ 
	iNoOfRetries = aNoOfRetries;
	}

/** Sets failure parameter Timeout. Used together with start-method @c EWaitStart to determine 
how long to wait for the component being started to rendevouz before aborting an attempt. 
@publishedAll
@deprecated
@param aTimeout The time in milliseconds to wait before terminating a component 
				that's taking longer to start (rendezvouz) than this specified timeout value. 
				A value of zero means "do not time this component out". Only valid when 
				@c TStartMethod = @c EWaitForStart.
*/
EXPORT_C void CStartupProperties::SetTimeout(TInt aTimeout)
	{ 
	iTimeout = aTimeout;
	}

/** Sets the monitoring parameter. Monitoring a component means the System Monitor server
observes the component and takes action when it exits. When a monitored component
exits, the System Monitor server will try to restart it the number of times specified in 
@c NoOfRetries. If the number of retries is 0, or the number of retries have been exhausted, 
the @c TRecoveryMethod is executed.
@publishedAll
@deprecated
@param aMonitored ETrue if the component should be monitored for failure
*/
EXPORT_C void CStartupProperties::SetMonitored(TBool aMonitored)
	{ 
	iMonitored = aMonitored;
	}

/** Sets recovery parameters. When a component being monitored exits and the system fails 
to restart the component within the specified  number of retries, the @c TRecoveryMethod 
is executed. 
Used to set the retry_failure_recovery_method within START_PROCESS_INFO2 and 
START_APP_INFO2 structures.

@publishedAll
@deprecated
@param aRecoveryMethod The method to use to recover when a monitored component fails.
@param aRestartMode The startup mode to restart the OS if recovery method is @c ERestartOSWithMode.
					If aRecoveryMethod==ERestartOS this parameter should be zero.
*/
EXPORT_C void CStartupProperties::SetRecoveryParams(TRecoveryMethod aRecoveryMethod, TInt aRestartMode)
	{ 
	iRecoveryMethod.iRecoveryMethod = aRecoveryMethod;
	iRestartMode = aRestartMode;
	}

/** Action to be taken on command failure. Provides backward compatibility with earlier SSC format.
Used to set the fail_on_error fields within START_PROCESS_INFO, START_APP_INFO, START_DLL_INFO, 
and MULTIPLE_WAIT structures.

@publishedAll
@deprecated
@param aActionOnCommandFailure Action to be taken if the system starter command fails.
*/
EXPORT_C void CStartupProperties::SetActionOnCommandFailure(TActionOnCommandFailure aActionOnCommandFailure)
	{ 
	iRecoveryMethod.iActionOnCommandFailure = aActionOnCommandFailure;
	}

/** Sets extra startup viewless parameter only needed if starting an application
@publishedAll
@deprecated
@param aViewless ETrue if the application should be viewless
*/
EXPORT_C void CStartupProperties::SetViewless(TBool aViewless)
	{ 
	iViewless = aViewless; 
	}
	
/** Sets extra startup start in background parameter only needed if starting an application
@publishedAll
@deprecated
@param aStartInBackground ETrue if the application should start in the background
*/
EXPORT_C void CStartupProperties::SetStartInBackground(TBool aStartInBackground)
	{ 
	iStartInBackground = aStartInBackground; 	
	}

/** Accessor function returning filename property
@publishedAll
@deprecated
@return Filename property
*/
EXPORT_C TPtrC CStartupProperties::FileName() const
	{
	return (iFileName) ? *iFileName : KNullDesC();
	}

/** Accessor function returning arguments property
@publishedAll
@deprecated
@return Arguments property
*/
EXPORT_C TPtrC CStartupProperties::Args() const
	{
	return (iArgs) ? *iArgs : KNullDesC();
	}

/** Accessor function returning startup type property
@publishedAll
@deprecated
@return Startup type property
*/
EXPORT_C TStartupType CStartupProperties::StartupType() const
	{ 
	return iStartupType; 
	}

/** Accessor function returning start method property
@publishedAll
@deprecated
@return Start method property
*/
EXPORT_C TStartMethod CStartupProperties::StartMethod() const
	{ 
	return iStartMethod; 
	}

/** Accessor function returning number of retries property. 
Only valid when @c TStartMethod = @c EWaitForStart.
@publishedAll
@deprecated
@return Number of retries property
*/
EXPORT_C TInt CStartupProperties::NoOfRetries() const
	{ 
	return iNoOfRetries; 
	}

/** Accessor function returning timeout property.
Only valid when @c TStartMethod = @c EWaitForStart.
@publishedAll
@deprecated
@return Timeout property
*/
EXPORT_C TInt CStartupProperties::Timeout() const
	{ 
	return iTimeout; 
	}

/** Accessor function returning the legacy fail_on_error property
used by the old resource structs START_APP_INFO and START_PROCESS_INFO.
@publishedAll
@deprecated
@return Recovery method property
*/
EXPORT_C TActionOnCommandFailure CStartupProperties::ActionOnCommandFailure() const
	{ 
	return iRecoveryMethod.iActionOnCommandFailure;
	}

/** Accessor function returning recovery method property read from
the retry_failure_recovery_method property in the resource structs 
START_PROCESS_INFO2 and START_APP_INFO2.
@publishedAll
@deprecated
@return Recovery method property
*/
EXPORT_C TRecoveryMethod CStartupProperties::RecoveryMethod() const
	{ 
	return iRecoveryMethod.iRecoveryMethod;
	}

/** Accessor function returning restart mode property
@publishedAll
@deprecated
@return Restart mode property
*/
EXPORT_C TInt CStartupProperties::RestartMode() const
	{ 
	return iRestartMode; 
	}

/** Accessor function returning monitored property
@publishedAll
@deprecated
@return Monitored property
*/
EXPORT_C TBool CStartupProperties::Monitored() const
	{ 
	return iMonitored; 
	}

/** Accessor function returning viewless property
@publishedAll
@deprecated
@return Viewless property
*/
EXPORT_C TBool CStartupProperties::Viewless() const
	{ 
	return iViewless; 
	}

/** Accessor function returning start in background property
@publishedAll
@deprecated
@return Start in background property
*/
EXPORT_C TBool CStartupProperties::StartInBackground() const
	{ 
	return iStartInBackground; 
	}

/** Externalize the class into a buffer.
This function has to be updated every time a member has been added to the class to maintain binary 
compatibility. 
@internalTechnology
@deprecated
@param aBufPtr A buffer of sufficient size to contain the externalized class data on function return
*/
EXPORT_C void CStartupProperties::ExternalizeL(CBufBase& aBufPtr) const
	{
    RBufWriteStream writeStream(aBufPtr);

    writeStream.WriteInt32L(iVersion);
    writeStream << FileName();	
    writeStream << Args();	
    writeStream.WriteInt8L(iStartupType); //enumeration has few values so 8bit should be enough
    writeStream.WriteInt8L(iStartMethod); //enumeration has few values so 8bit should be enough
    writeStream.WriteInt32L(iNoOfRetries); 
    writeStream.WriteInt32L(iTimeout); 
    writeStream.WriteInt8L(iRecoveryMethod.iRecoveryMethod); //enumeration has few values so 8bit should be enough       
    writeStream.WriteInt32L(iRestartMode); 
    writeStream.WriteInt8L(iMonitored); //8bit should be enough for bool also
    writeStream.WriteInt8L(iViewless); //8bit should be enough for bool also
    writeStream.WriteInt8L(iStartInBackground); //8bit should be enough for bool also
    
    // Add any new data member externalization code here, before CommitL
    // e.g. writeStream.WriteInt32L(iSomethingElse);        
    
    writeStream.CommitL();
	writeStream.Release();
    
	}

/** Internalize the class from a buffer.
This function has to be updated every time a member has been added to the class to maintain binary 
compatibility. 
@internalTechnology
@deprecated
@param aBufPtr A buffer containing the externalized class data
*/
EXPORT_C void CStartupProperties::InternalizeL(const TPtrC8& aBufPtr)
	{
    RDesReadStream readStream;
    readStream.Open(aBufPtr);
    CleanupClosePushL(readStream);

	iVersion = readStream.ReadInt32L();

	delete iFileName;
	iFileName=NULL;
	iFileName = HBufC::NewL(readStream,KMaxFileName);
	delete iArgs;
	iArgs=NULL;	
	iArgs = HBufC::NewL(readStream,KMaxFileName);

    iStartupType = static_cast<TStartupType>(readStream.ReadInt8L());
    iStartMethod = static_cast<TStartMethod>(readStream.ReadInt8L());
	iNoOfRetries = readStream.ReadInt32L();
	iTimeout = readStream.ReadInt32L();	
    iRecoveryMethod.iRecoveryMethod = static_cast<TRecoveryMethod>(readStream.ReadInt8L());	
	iRestartMode = readStream.ReadInt32L();	    
    iMonitored = static_cast<TBool>(readStream.ReadInt8L());
    iViewless = static_cast<TBool>(readStream.ReadInt8L());
    iStartInBackground = static_cast<TBool>(readStream.ReadInt8L());        
	
    // Add any new data member internalization code here
    // e.g. iSomethingElse = readStream.ReadInt32L();       
        
	CleanupStack::PopAndDestroy(&readStream);
	}

/** 
@return The size in bytes for this particular object.
*/
EXPORT_C TInt CStartupProperties::Size() const
	{
	const TInt nameSize = iFileName ? iFileName->Size() : 0;
	const TInt argsSize = iArgs ? iArgs->Size() : 0;
	return sizeof(CStartupProperties) + nameSize + argsSize; 
	}

void CStartupProperties::ConstructFromResourceL(TResourceReader& aSource)
	{
	const TStartupCommandType commandType = static_cast<TStartupCommandType>(aSource.ReadUint16());	
	iStartupType = ((commandType==EStartupApp)||(commandType==EStartupApp2)) ? EStartApp : EStartProcess;
	
	iFileName = aSource.ReadHBufCL();
	if(iFileName)
		{
		TPtr writeableFilename = iFileName->Des();
		writeableFilename.TrimAll();		
		}
	
	iArgs = aSource.ReadHBufCL(); 
	if(iArgs)
		{
		TPtr writeableArgs = iArgs->Des();
		writeableArgs.TrimAll();
		}
	
	iStartMethod = static_cast<TStartMethod>(aSource.ReadUint16());	
	
	switch(commandType)
		{
		case EStartupApp:
			delete iArgs; // deleted for backwards compatibility
			iArgs = NULL;
			iViewless = static_cast<TUint16>(aSource.ReadUint16());
			iStartInBackground = static_cast<TUint16>(aSource.ReadUint16());
			iTimeout = aSource.ReadInt32();
			iRecoveryMethod.iActionOnCommandFailure = static_cast<TActionOnCommandFailure> (aSource.ReadUint16());
			iNoOfRetries = static_cast<TUint16> (aSource.ReadUint16());
			break;
			
		case EStartupApp2:
			iViewless = static_cast<TUint16>(aSource.ReadUint16());
			iStartInBackground = static_cast<TUint16>(aSource.ReadUint16());
			iTimeout = aSource.ReadInt32();
			iRecoveryMethod.iRecoveryMethod = static_cast<TRecoveryMethod> (aSource.ReadUint16());
			iNoOfRetries = static_cast<TUint16> (aSource.ReadUint16());
			iMonitored = static_cast<TUint16>(aSource.ReadUint16());
			iRestartMode = aSource.ReadInt16();
			break;
			
		case EStartupProcess:
			iTimeout = aSource.ReadInt32();
			iRecoveryMethod.iActionOnCommandFailure = static_cast<TActionOnCommandFailure>(aSource.ReadUint16());
			iNoOfRetries = static_cast<TUint16> (aSource.ReadUint16());	
			break;
			
		case EStartupProcess2:
			iTimeout = aSource.ReadInt32();
			iRecoveryMethod.iRecoveryMethod = static_cast<TRecoveryMethod>(aSource.ReadUint16());
			iNoOfRetries = static_cast<TUint16> (aSource.ReadUint16());	
			iMonitored = static_cast<TUint16>(aSource.ReadUint16());	
			iRestartMode = aSource.ReadInt16();
			break;
			
		default:
			PanicNow(KPanicStartupProperties, EInvalidCommandType);
			break;
		}
	
	DoSanityCheck(commandType);
	}

void CStartupProperties::DoSanityCheck(TStartupCommandType aCommandType) const
	{
	if ((iStartMethod < EFireAndForget) || (iStartMethod > EDeferredWaitForStart))
		{	
		DEBUGPRINT2(_L("Invalid start_method value of %d"), iStartMethod);
		PanicNow(KPanicStartupProperties, EInvalidStartMethod);
		} 
	
	// The timeout and no_of_retries_on_failure fields can only be used
 	// with EWaitForStart. Check that no values other than the defaults have been 
 	// provisioned
 	if (iStartMethod != EWaitForStart)
 		{
 		if (iTimeout)   
			{	
			DEBUGPRINT2(_L("Timeout should be 0, invalid timeout value of %d"), iTimeout);
			PanicNow(KPanicStartupProperties, EInvalidTimeout);
			} 
		if (iNoOfRetries)
			{
			DEBUGPRINT2(_L("no_of_retries_on_failure should be 0, invalid value of %d"), iNoOfRetries);
			PanicNow(KPanicStartupProperties, EInvalidNoOfRetries);
			}
 		}
 	
	if ((aCommandType == EStartupApp) || (aCommandType == EStartupProcess))
		{
		// Check that the fail_on_error value is valid for version 1 struct.
		if ((iRecoveryMethod.iActionOnCommandFailure != EIgnoreCommandFailure) && (iRecoveryMethod.iActionOnCommandFailure != EPanicOnCommandFailure))
			{	
			DEBUGPRINT2(_L("Invalid fail_on_error: %d"), iRecoveryMethod.iActionOnCommandFailure);
			PanicNow(KPanicStartupProperties, EInvalidRecoveryMethod);
			} 
		}
	else if ((aCommandType == EStartupApp2) || (aCommandType == EStartupProcess2))
		{
		// Check that the fail_on_error value is valid for version 2 struct.	
		if ((iRecoveryMethod.iRecoveryMethod > ERestartOSWithMode) || (iRecoveryMethod.iRecoveryMethod < EIgnoreOnFailure))
			{	
			DEBUGPRINT2(_L("Invalid retry_failure_recovery_method: %d"), iRecoveryMethod.iRecoveryMethod);
			PanicNow(KPanicStartupProperties, EInvalidRecoveryMethod);
			} 
		}
	}
