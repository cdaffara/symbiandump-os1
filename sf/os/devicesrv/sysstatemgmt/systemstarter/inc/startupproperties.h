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

#ifndef __STARTUPPROPERTIES_H__
#define __STARTUPPROPERTIES_H__


#include <e32base.h>
#include <startup.hrh>

class TResourceReader;

/** A parameter class containing application/process starter information that is shared among client 
applications, system components and client/server processes. This class is used to pass multiple 
different types of data to API's in only one parameter while ensuring proper data initialization, memory 
allocation and serializing over IPC when required. 

When an instance of this class is created it contains default values for all properties. The usage of this 
class for different scenarios only requires calling setter functions to overrride parameters which has 
non-default values. 

@publishedAll
@deprecated Instead of this use CSsmStartupProperties
*/
NONSHARABLE_CLASS (CStartupProperties) : public CBase
	{
public:
    IMPORT_C static CStartupProperties* NewL();
    IMPORT_C static CStartupProperties* NewLC();
    IMPORT_C static CStartupProperties* NewL(const TDesC& aFileName, const TDesC& aArgs);
    IMPORT_C static CStartupProperties* NewLC(const TDesC& aFileName, const TDesC& aArgs);
    IMPORT_C static CStartupProperties* NewL(TResourceReader& aSource);
    IMPORT_C static CStartupProperties* NewLC(TResourceReader& aSource);
	IMPORT_C ~CStartupProperties();    

    IMPORT_C void SetFileParamsL(const TDesC& aFileName, const TDesC& aArgs);
    IMPORT_C void SetStartupType(TStartupType aType);
    IMPORT_C void SetStartMethod(TStartMethod aStartMethod);
    IMPORT_C void SetNoOfRetries(TInt aNumRetries);
    IMPORT_C void SetTimeout(TInt aTimeout);
    IMPORT_C void SetActionOnCommandFailure(TActionOnCommandFailure aActionOnCommandFailure);
    IMPORT_C void SetRecoveryParams(TRecoveryMethod aRecoveryMethod, TInt aRestartMode);    
    IMPORT_C void SetMonitored(TBool aMonitored);
    IMPORT_C void SetViewless(TBool aViewless);
    IMPORT_C void SetStartInBackground(TBool aStartInBackground);

    IMPORT_C TInt Version() const;
    IMPORT_C TPtrC FileName() const;
    IMPORT_C TPtrC Args() const;
    IMPORT_C TStartupType StartupType() const;
    IMPORT_C TStartMethod StartMethod() const;
    IMPORT_C TInt NoOfRetries() const;
    IMPORT_C TInt Timeout() const;
	IMPORT_C TBool Monitored() const;    
	IMPORT_C TActionOnCommandFailure ActionOnCommandFailure() const;
    IMPORT_C TRecoveryMethod RecoveryMethod() const;
	IMPORT_C TInt RestartMode() const;
	IMPORT_C TBool Viewless() const;
	IMPORT_C TBool StartInBackground() const;

    IMPORT_C void ExternalizeL(CBufBase& aBufPtr) const;
    IMPORT_C void InternalizeL(const TPtrC8& aBufPtr);
    IMPORT_C TInt Size() const;

    IMPORT_C void Reset();
private:
	CStartupProperties();
	void ConstructL();
	void ConstructFromResourceL(TResourceReader& aSource);
	void DoSanityCheck(TStartupCommandType aCommandType) const;
	
private:	
	/** Internal datatype to store the fail_on_error/retry_failure_recovery_method
	members from version 1 and version 2 of the resource structs. */
	union TRecoveryPolicy
		{
		TActionOnCommandFailure iActionOnCommandFailure;
		TRecoveryMethod iRecoveryMethod;
		};
	/** Internal version for the CStartupProperties object */
	TInt iVersion;
	/** Path to the application	*/
	HBufC* iFileName; 	
	/** Application arguments */
	HBufC* iArgs; 
	/** What to start, application or process. */
	TStartupType iStartupType;		
	/** Whether system starter should wait for application to signal completion
	or use 'fire and forget' mode.
	*/
	TStartMethod iStartMethod;			
	/** No of retries if the server/app/process fails to start */
	TInt iNoOfRetries;
	/** Whether to kill a process that's taking too long.
	a value of zero means "do not time this process out".
	*/
	TInt iTimeout;
	/** How to recover monitored application */
	TRecoveryPolicy iRecoveryMethod;
	/** Which startup mode to restart the OS if recovery mode is RestartOSWithMode */
	TInt iRestartMode;
	/** Whether the application should be monitored	*/
	TBool iMonitored;
	/** Whether the application should be started viewless.	*/
	TBool iViewless;
	/** Whether the application should be started in the background. */
	TBool iStartInBackground;
	};

#endif
