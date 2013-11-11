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

#ifndef __SSMSTARTUPPROPERTIES_H__
#define __SSMSTARTUPPROPERTIES_H__

#include <e32base.h>
#include <ssm/ssmcmd.hrh>

class CResourceFile;
class RReadStream;
class RWriteStream;
class RResourceReader;

/**
Structure to store monitor information.
@publishedPartner
@released
*/
struct TSsmMonitorInfo
	{
	TSsmRecoveryMethod iRestartPolicy;
	TInt iRestartMode;
	TInt32 iTimeout;
	TInt iRetries;
	};

/** A parameter class containing application/process starter information that is shared among client 
applications, system components and client/server processes. This class is used to pass multiple 
different types of data to API's in only one parameter while ensuring proper data initialization, memory 
allocation and serializing over IPC when required. 

When an instance of this class is created it contains default values for all properties. The usage of this 
class for different scenarios only requires calling setter functions to overrride parameters which has 
non-default values. 

@publishedPartner
@released
*/
NONSHARABLE_CLASS (CSsmStartupProperties) : public CBase
	{
public:
    IMPORT_C static CSsmStartupProperties* NewL();
    IMPORT_C static CSsmStartupProperties* NewLC();

    IMPORT_C static CSsmStartupProperties* NewL(const TDesC& aFileName, const TDesC& aArgs);
    IMPORT_C static CSsmStartupProperties* NewLC(const TDesC& aFileName, const TDesC& aArgs);

    IMPORT_C static CSsmStartupProperties* NewL(RResourceReader& aReader, CResourceFile* aResourceFile, TSsmCommandType aCommandType);
    IMPORT_C static CSsmStartupProperties* NewLC(RResourceReader& aReader, CResourceFile* aResourceFile, TSsmCommandType aCommandType);
    IMPORT_C static CSsmStartupProperties* NewL(const CSsmStartupProperties& aStartupProperties);
	IMPORT_C ~CSsmStartupProperties();

	//Setter functions for member variables
    IMPORT_C void SetFileParamsL(const TDesC& aFileName, const TDesC& aArgs);
    IMPORT_C void SetCommandTypeL(TSsmCommandType aCommandType);
    IMPORT_C void SetExecutionBehaviour(TSsmExecutionBehaviour aExecutionBehaviour);
    IMPORT_C void SetRetries(TInt aRetries);
    IMPORT_C void SetTimeout(TInt32 aTimeout);
    IMPORT_C void SetViewless(TBool aViewless);
    IMPORT_C void SetStartInBackground(TBool aStartInBackground);

	IMPORT_C void SetMonitorInfoL(TSsmMonitorInfo aMonitorInfo);

    // getter functions
    IMPORT_C TPtrC FileName() const;
    IMPORT_C TPtrC Args() const;
    IMPORT_C TSsmCommandType CommandType() const;
    IMPORT_C TSsmExecutionBehaviour ExecutionBehaviour() const;
    IMPORT_C TInt Retries() const;
    IMPORT_C TInt32 Timeout() const;
	IMPORT_C TBool IsMonitoringRequired() const;
	IMPORT_C TBool Viewless() const;
	IMPORT_C TBool StartInBackground() const;
	IMPORT_C TSsmMonitorInfo MonitorInfo() const;

    IMPORT_C void ExternalizeL(RWriteStream& aWriteStream) const;
    IMPORT_C void InternalizeL(RReadStream& aReadStream);

    IMPORT_C void Reset();
private:
	CSsmStartupProperties();
	CSsmStartupProperties(const CSsmStartupProperties& aStartupProperties);
	void ConstructL(const CSsmStartupProperties& aStartupProperties);
	void ConstructFromResourceL(RResourceReader& aReader, CResourceFile* aResourceFile, TSsmCommandType aCommandType);

private:
	TSsmCommandType iCommandType;
	HBufC* iFileName;
	HBufC* iArgs;
	TSsmExecutionBehaviour iExecutionBehaviour;
	TInt32 iTimeout;
	TInt iRetries;
	TBool iViewless;
	TBool iStartInBackground;
	TSsmMonitorInfo iMonitorInfo;
	TBool iMonitoringRequired;

	// for future use
	TInt iReserved1;
	TInt iReserved2;
	TInt iReserved3;
	};

#endif	// __SSMSTARTUPPROPERTIES_H__

