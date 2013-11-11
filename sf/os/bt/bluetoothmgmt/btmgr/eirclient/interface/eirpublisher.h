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
// EIR Publisher client header.
// Implements the C-class used by clients to publish data to the local EIR
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef _EIRPUBLISHER_H
#define _EIRPUBLISHER_H

#include <e32std.h>
#include <e32base.h>
#include <bluetooth/eirmanshared.h>
#include <bluetooth/eirclient.h>


class MEirPublisherNotifier
	{
public:
	virtual void MepnSpaceAvailable(TUint aBytesAvailable) = 0;
	virtual void MepnSetDataError(TInt aResult) = 0;
	};

class CEirPublisher;

/**
@internalComponent
*/
NONSHARABLE_CLASS(CEirPublisherNotifier) : public CActive
	{
public:
	CEirPublisherNotifier(CEirPublisher& aPublisher);
	~CEirPublisherNotifier();
	
	void Start();
	
private:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
private:
	CEirPublisher&	iPublisher;
	TUint			iSpaceAvailable;
	};

NONSHARABLE_CLASS(CEirPublisher) : public CActive
/**
The CEirPublisher class encapsulates a data publisher to the local EIR
*/
	{
public:
	// Factory
	IMPORT_C static CEirPublisher* NewL(TEirTag aEirTag, MEirPublisherNotifier& aNotifier);
	~CEirPublisher();

	// Public methods
	IMPORT_C void SetData(const TDesC8& aData, TEirDataMode aDataMode);
	IMPORT_C void PublishData(TInt aRequiredLength);
	
public: // used by CEirPublisherNotifier
	void SpaceAvailable(TUint aSpace);
	void NotifierError(TInt aError);
	REirMan& EirMan();
	
private:
	CEirPublisher(TEirTag aEirTag, MEirPublisherNotifier& aNotifier);
	void ConstructL();

private:
	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	
private:
	enum TState
		{
		EInvalid,
		EConnecting,
		ENewData,
		ERegisterTag,
		EDataCallback,
		ESettingData,
		EIdle,
		ENoConnection,
		};

private:
	TEirTag 				iEirTag;
	MEirPublisherNotifier& 	iNotifier;
	CEirPublisherNotifier*	iNotifierAo;
	REirMan					iEirMan;
	TUint 					iBytesAvailable;
	TBool 					iSetDataPending;
	TState 					iState;
	TBool					iPublishOutstanding;
	TInt					iRequiredBytes;
	TBool					iCallbackOutstanding;
	TUint					iAvailableBytes;
	};
	
enum TBTEirPublisherPanic
	{
	EUnexpectedPublisherState,
	EInvalidEirManHandle,
	};
	
_LIT(KBTEirPublisherPanic, "BT Eir Publisher");
	
#endif // _EIRPUBLISHER_H
