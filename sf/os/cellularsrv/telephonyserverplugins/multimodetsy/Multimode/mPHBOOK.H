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
// GSM Phone book access Header file.
// 
//

/**
 @file
 @internalAll 
*/


#ifndef __MPHBOOK_H__
#define __MPHBOOK_H__

#include <et_phone.h>
#include "ATSTD.H"

enum TPhoneBookStorageType {
	KPhoneBookNoStorage =0x0,
	KPhoneBookFDStorage =0x1,
	KPhoneBookLDStorage =0x2,
	KPhoneBookMEStorage =0x4,
	KPhoneBookMTStorage =0x8,
	KPhoneBookSMStorage =0x10,  
	KPhoneBookTAStorage =0x20,
	//
	KPhoneBookLastDialledStorage =0x40,
	KPhoneBookEmergencyStorage =0x80,  
	KPhoneBookBarredStorage =0x100,
	KPhoneBookMissedCallsStorage =0x200,
	KPhoneBookOwnNumbersStorage =0x400,   
	KPhoneBookReceivedCallsStorage =0x800,
	KPhoneBookServiceNumberStorage =0x1000,
	};

class CATIO;
class CATInit;
class CPhoneGlobals;
class CATPhoneBookInfo;
class CATPhoneBookRead;
class CATPhoneBookWrite;
class CATPhoneBookDelete;

/**
 * \class CMobilePhoneBookStore MPHBOOK.H "MMTSY/MULTIMODE/MPHBOOK.H"
 * \brief The CMobilePhoneBookStore class is one of the Multi mode TSY's sub-session extensions responsible
 * for handling Client-side calls from the Multi mode API.
 *
 * CMobilePhoneBookStore inherits from the CSubSessionExtBase class defined in et_phone.h (which in turn
 * inherits from various mixin classes (MSubSessionExtBaseTSY, MTelObjectTSY). This sub-session
 * extension class implements all the pure virtuals (functions) from the inherited classes
 * including ExtFunc(), CancelService(), ReqModeL() etc.
 */
class CMobilePhonebookStore : public CSubSessionExtBase
	{
public:
	static CMobilePhonebookStore* NewL(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aGsmStatus,const TDesC8& aStorageType);
	~CMobilePhonebookStore();
//
// CTelObject, MTelObject pure virtuals
//
	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual void Init();
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aName);
//
// CSubSessionExtBase pure virtuals
//
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle, const TInt aIpc, const TDataPackage& aPackage);

	const TStorageType& StorageType() const;
	TInt GetWrittenIndex() const;
	TInt GetDeletedIndex() const;
	TInt GetNumberOfUsedEntries();
	TInt GetTotalNumberOfEntries();
	void ProcessPostponedCommand(TInt aError);

	TInt HandleExtFunc(TTsyReqHandle aTsyReqHandle, TInt aIpc, const TDataPackage& aPackage);
	void InitFailed();

private:
	void ConstructL();

	CMobilePhonebookStore(CATIO* aATIO, CATInit* aInit, CPhoneGlobals* aGsmStatus, const TDesC8& aStorageType);

	TInt GetInfo(TTsyReqHandle aTsyReqHandle, TDes8* aInfoPckg);
	TInt Read(TTsyReqHandle aTsyReqHandle, RMobilePhoneBookStore::TPBIndexAndNumEntries* aIndexAndEntries, TDes8* aPBData);
	TInt Write(TTsyReqHandle aTsyReqHandle, TDes8* aPBData, TInt* aIndex);
	TInt Delete(TTsyReqHandle aTsyReqHandle, TInt* aIndex);

	TInt PhoneBookCapabilities(TDes& aName);

private:
	enum TState {EInit,EOK,EFail};	// phonebook initialisation status

	CATIO* iIo;
	CATInit* iInit;
	CPhoneGlobals* iPhoneGlobals;
	CATPhoneBookInfo* iInfo;		// gets phonebook store information
	CATPhoneBookRead* iRead;        // enables an entry to be read from phonebook memory
	CATPhoneBookWrite* iWrite;		// enables an entry to be written to phonebook memory
	CATPhoneBookDelete* iDelete;	// enables an entry to be deleted from phonebook memory

	TInt iIndex;
	RMobilePhoneBookStore::TPBIndexAndNumEntries iIndexAndEntries;

	TStorageType iStorageType;
	TUint iReadWriteAccess;
	TState iState;
	};

#include "mphbook.inl"

#endif

