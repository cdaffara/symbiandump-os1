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

/**
 @file
 @internalComponent
*/

#ifndef __RPSOWNNUMBER_H__
#define __RPSOWNNUMBER_H__


#include <etelmm.h>
#include <mmretrieve.h>


class TRPSOwnNumbers
	{
public:
	IMPORT_C TRPSOwnNumbers();
	IMPORT_C TInt RetrieveOwnNumbersL(RMobilePhone& aPhone);	
	
	inline TPtrC OwnVoiceNumber() const;
	inline TPtrC OwnDataNumber() const;
	inline TPtrC OwnFaxNumber() const;
	
private:
	TInt ExtractANumber(const RMobileONStore::TMobileONEntryV1& aEntry);
	
private:
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> iOwnVoiceNumber;   
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> iOwnDataNumber;   
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> iOwnFaxNumber;
	
	TBool iFoundVoiceNum;
	TBool iFoundFaxNum;
	TBool iFoundDataNum;
	};
	
inline TPtrC TRPSOwnNumbers::OwnVoiceNumber() const
	{
	return iOwnVoiceNumber.Ptr();
	}
inline TPtrC TRPSOwnNumbers::OwnDataNumber() const
	{
	return iOwnDataNumber.Ptr();
	}
inline TPtrC TRPSOwnNumbers::OwnFaxNumber() const
	{
	return iOwnFaxNumber.Ptr();
	}


//
// Helper object to retrieve SIM's Own Number List
//
NONSHARABLE_CLASS(CActiveONRetrieverRps) : public CActive
	{
public:
	CActiveONRetrieverRps(RMobileONStore& aONStore, CMobilePhoneONList*& aONList, TInt& aError, CActiveScheduler*& aScheduler );
	~CActiveONRetrieverRps();
	virtual void StartL();
	virtual void RunL();
	virtual void DoCancel();

private:
	RMobileONStore& iONStore;
	CMobilePhoneONList*& iONList;
	TInt& iError;
	CRetrieveMobilePhoneONList* iRetriever;
	CActiveScheduler*& iScheduler;	// Not owned
	};


#endif  // __RPSOWNNUMBER_H__
