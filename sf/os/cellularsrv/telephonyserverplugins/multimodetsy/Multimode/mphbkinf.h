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
// Multimode TSY Phone book Header file.
// Describes the GSM Phone Book AT Commands - Read, Write, Delete and Get Info.
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __MPHBKINF_H__
#define __MPHBKINF_H__

#include "ATBASE.H"
#include "mPHBOOK.H"
#include "mpbutil.h"

/**
 * \class CATPhoneBookInfo MPHBKINF.H "...Multimode/MPHBKINF.H"
 * \brief Determines the phone's supported phonebooks and the associated information.
 *
 * CATPhoneBookInfo inherits from CATCommands class. It's intended for CPhoneMobile usage.
 * This class transmits the following AT Commands: "AT+CPBS=?", "AT+CPBS?", "AT+CPBR=?"
 * and "AT+CPBS=xx" (where xx is the relevant phonebook memory to be set) to determine
 * information about the supported phone books.
 */
class CATPhoneBookInfo :public CATCommands
	{
public:
	static CATPhoneBookInfo* NewL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	CATPhoneBookInfo(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATPhoneBookInfo();

	void SetStorageType(TStorageType aStorageType);
	void SetIndexByStorageType();
	void Stop(TTsyReqHandle aTsyReqHandle);

	inline void MapClientIndexToPhoneIndex(TInt& aIndex) const;
	inline void MapPhoneIndexToClientIndex(TInt& aIndex) const;
	inline TBool IsValidPhoneIndex(TInt aIndex) const;
	inline TInt UsedEntries() const;
	inline TInt TotalEntries() const;
	inline TBool Completed() const;

protected:
	inline CMobilePhonebookStore& PhoneBook() const;

private:
//
// CATCommmands pure virtual
//
	virtual void Start(TTsyReqHandle aTsyReqHandle,TAny* aParams);
//
// CATBase pure virtuals
// 
	virtual void EventSignal(TEventSource aEventSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
//

	void ConstructL();
//
	void StartSelect();
	void StartGetInfo();
	void StartGetMaxLength();
//
	void ParseListResponseL();
	void ParseGetInfoResponseL();
	void ParseGetMaxLengthResponseL();

private:
	enum TPhoneBookInfoState {
		EIdle,
		EListWaitForWriteComplete,
		EListWaitForReadComplete,
		ESelectWaitForWriteComplete,
		ESelectWaitForReadComplete,
		EGetInfoWaitForWriteComplete,
		EGetInfoWaitForReadComplete,
		EGetMaxLengthWaitForWriteComplete,
		EGetMaxLengthWaitForReadComplete
		};

private:
	void WriteComplete(TEventSource aSource,TPhoneBookInfoState aState);
	virtual void Complete(TInt aError,TEventSource aSource);

private:
	TPhoneBookInfoState iState;
	TBool iCompleted;
	TInt iIndex;
	TInt iIndexOffset;		// Offset of index values for phone book store, will be 0 with most phones
	TStorageType iStorageType;
	
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 iPhbkInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg* iInfoPckg;

	CArrayFixFlat<TStorageType>* iStorageTypeArray;
	};

#endif
