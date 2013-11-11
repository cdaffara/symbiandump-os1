// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Sms AT Command Base Class Header
// 
//

/**
 @file
 @internalAll 
*/


#ifndef __SMSBASE_H__
#define	__SMSBASE_H__

#include "ATBASE.H"
#include "smsutil.h"		// for KMaxAsciiPduLength

// Constants 
const TUint KATWriteTimeout=20000;		// Is in milli seconds, 
const TInt  KATResponseTimeout=40;		// Is in seconds, set to 40 seconds as +CMGS response
                                      // can take upto 15 seconds when the GSM network is busy
const TUint KCapsCnmiMode1 = 0x02;
const TUint KCapsCnmiMode2 = 0x04;
const TUint KCapsCnmiMode3 = 0x08;

class CMobileSmsStore;

class CATSmsCommands : public CATCommands
	{
public:
	CATSmsCommands(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aGsmStatus);
	~CATSmsCommands();

protected:
	// from CATBase
	virtual void EventSignal(TEventSource aEventSource);

	// Request common AT commands be carried out
	enum TRequest {ENone, 
	               EGetSCAFromPhone, ESetSCAInPhone, 
				   ESetPhoneToCMTMode,ESetPhoneToCMTIMode};
	TInt RequestATCommand(TRequest aReq);
	void RequestATCommandCancel();
	TRequest RequestATCompleted();		// Sets iRequestCcompleted to EFalse
	TBool RequestATActive() const;

	// Utility
	void HandleWriteCompletion(TEventSource aSource);
	TInt HandleResponseCompletion(TEventSource aSource,TBool aValidateExpectString=ETrue);
	TInt ParseRxResultsForCSCAResponse(RMobilePhone::TMobileAddress& aTelNumber);
	TInt DoParseRxResultsForCSCAResponseL(RMobilePhone::TMobileAddress& aTelNumber);
	TInt CMSErrorValue();
	TInt ConvertCMSErrorToKErr(const TInt& aCmsError) const;
	TBool ComparePrefMem(const TStorageType& aName) const;
	void SetCurrentPrefMem(const TStorageType& aStorageName);

private:
	// Request functions
	TInt GetSCAFromPhone(const TEventSource& aEventSource);
	TInt SetSCAInPhone(const TEventSource& aEventSource);
	TInt SetPhoneToCMTMode(const TEventSource& aEventSource);
	TInt SetPhoneToCMTIMode(const TEventSource& aEventSource);

	// Utility 
	void CompleteRequest(TInt aError=KErrNone);
	void InitializeRequestData(TRequest aReq);
	void ProcessCapsElementL(TDblQueIter<CATParamListEntry>& aIter,TInt32& aCapsMask);
	void ParseCNMIFirstCapabilityL(TInt& aHighestValue);

protected:
	TInt  iRequestError;		//KErr... value set when request completes
	RMobilePhone::TMobileAddress iRequestSCA; // Used by EGetSCAFromPhone & ESetSCAInPhone

private:
	enum { EStage0=0, EStage1, EStage2, EStage3, EStage4, EStage5, EStage6, EStage7 };

	enum 
		{
		ECnmiMode0,
		ECnmiMode1,
		ECnmiMode2,
		ECnmiMode3
		};

	TBool iRequestCompleted;	//ETrue if, and only if, a request has completed
	TRequest iRequest;
	TInt iRequestStage;
	TBool iRequestCancel;
	};

#endif
