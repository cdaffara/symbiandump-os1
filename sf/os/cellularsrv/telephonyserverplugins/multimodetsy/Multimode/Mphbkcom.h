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

#ifndef __MPHBKCOM_H__
#define __MPHBKCOM_H__

#include "ATBASE.H"
#include <mpbutil.h>
#include "mPHBOOK.H"
#include "mpbutil.h"
#include "mphbkinf.h"
#include "Matstd.h"			// for AT command strings


GLREF_C void ConvertStorageTypeToName(const TStorageType& aStorageType,TDes& aName);

/** 
 * \class CATPhoneBookCommands MPHBKCOM.H "...MULTIMODE/MPHBKCOM.H"
 * \brief This is an abstract class from which the CATPhoneBookInit, CATPhoneBookWrite,
 * CATPhoneBookRead and CATPhoneBookDelete classes inherit from.
 *
 * CATPhoneBookCommands inherits from the CATCommands class. It contains common 
 * functionality for the Write, Read, Delete and Initialization operations. This class is 
 * intended for CPhoneBook usage.
 */
class CATPhoneBookCommands : public CATCommands
	{
public:
	CATPhoneBookCommands(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATPhoneBookCommands();
//
// CATCommmands pure virtuals
//
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams) =0;
//
// CATBase pure virtuals
// 
	virtual void EventSignal(TEventSource aEventSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
//
// new pure virtual
//
	virtual void StartThisCommand() =0;
//
	virtual void Stop(TTsyReqHandle aTsyReqHandle);

protected:
//
// new pure virtual
//
	virtual void ParseResponseL() =0;
	void StartStorageSelect();
	virtual void Complete(TInt aError,TEventSource aSource);

protected:
	enum {
		EATPhoneBookCommandIdle,
		EATPhoneBookStorageSelectWaitForWriteComplete,
		EATPhoneBookStorageSelectWaitForReadComplete,
		EATPhoneBookCommandWaitForWriteComplete,
		EATPhoneBookCommandWaitForReadComplete,
		EATPhoneBookCommandWaitForAlternativeWriteComplete,
		EATPhoneBookCommandWaitForAlternativeReadComplete,
		EATPhoneBookCommandExtendedRead
		} iState;

	TBool iCancelFlag;
	};

/** 
 * \class CATPhoneBookRead MPHBKCOM.H "...MULTIMODE/MPHBKCOM.H"
 * \brief Reads one or more entry(s) from the specified phonebook memory.
 *
 * CATPhoneBookRead inherits from the CATPhoneBookCommands class. It reads one or more 
 * entry(s) from the selected phone book memory.
 */

class CATPhoneBookRead : public CATPhoneBookCommands
	{
public:
	static CATPhoneBookRead* NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals,CATPhoneBookInfo *aInfo);
	~CATPhoneBookRead();

	inline void SetPBData(TDes8* aPBData);

private:
	CATPhoneBookRead(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals,CATPhoneBookInfo *aInfo);
	void ConstructL();
//
// CATBas pure virtual
//
	virtual void EventSignal(TEventSource aEventSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
//
// CATCommmands pure virtual
//
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
//
// CATPhoneBookCommands pure virtual
// 
	virtual void ParseResponseL();
	void StartThisCommand();
	

private:
	CATPhoneBookInfo* iInfo;		// pointer to clients CATPhoneBookInfo object
	CCommChatString* iCPBRResponse;
	CPhoneBookBuffer* iPbBuffer; // Pointer to converter utility and memory provided by ETel to store the phonebook data
	RMobilePhoneBookStore::TPBIndexAndNumEntries* iRead; // Basic phonebook data
	TInt iNumReadEntries;			// < The number of phonebook entries successfully read during a read request
	};

/** 
 * \class CATPhoneBookWrite MPHBKCOM.H "...MULTIMODE/MPHBKCOM.H"
 * \brief Enables an entry to be written to the specified phonebook memory.
 *
 * CATPhoneBookWrite inherits from the CATPhoneBookCommands class. It writes an 
 * entry to the selected phone book memory.
 */
class CATPhoneBookWrite : public CATPhoneBookCommands
	{
public:
	static CATPhoneBookWrite* NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATPhoneBookWrite();
	inline TInt Index() const;
	inline void SetPBData(TDes8* aPBData);

private:
	CATPhoneBookWrite(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	void ConstructL();
//
// CATCommmands pure virtual
//
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
//
// CATPhoneBookCommands pure virtual
// 
	virtual void StartThisCommand();
	virtual void ParseResponseL();

//
// Class local utility functions
//
	TUint NumberTypefromMMToGSM(TUint aTypeOfNumberMM);

private:
	TInt  iIndex;  // Copy of index value which client wants this class to write to phone book store
	CPhoneBookBuffer* iPbBuffer; // Pointer to converter utility and memory provided by ETel to store the phonebook data
	};

/** 
 * \class CATPhoneBookDelete MPHBKCOM.H "...MULTIMODE/MPHBKCOM.H"
 * \brief Enables an entry to be deleted from the specified phonebook memory.
 *
 * CATPhoneBookDelete inherits from the CATPhoneBookCommands class. It deletes an entry
 * from the selected phone book memory.
 */
class CATPhoneBookDelete : public CATPhoneBookCommands
	{
public:
	static CATPhoneBookDelete* NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATPhoneBookDelete();
	//
	inline TInt Index() const;
private:
	CATPhoneBookDelete(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
//
// CATBase pure virtual
// 
	virtual void EventSignal(TEventSource aEventSource);
//
// CATCommmands pure virtual
//
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
//
// CATPhoneBookCommands pure virtual
// 
	virtual void StartThisCommand();
	virtual void ParseResponseL();
//
	void StartAlternativeDelete();
private:
	TInt iIndex;
	};

#include "mphbkcom.inl"

#endif
