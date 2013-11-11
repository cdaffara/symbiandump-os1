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
//

#if !defined(__SATPTR_H__)
#define __SATPTR_H__

/** @file
CSatPtrHolder header file
@internalTechnology
*/

#if !defined(__ETELSAT_H__)
#include "etelsat.h"
#endif


// The ptr holder slot numbers used by RSat asynchronous requests
enum TSatPtrHolderSlots
	{
	ESlotPCmd=0,
	ESlot1GetIcon,
	ESlot2GetIcon,
	ESlotMsgRef,
	ESlot1GetClut,
	ESlot1NotifyTsyStateUpdated,
	EMaxNumberSatPtrSlots
	};

enum TSatCPtrHolderSlots
	{
	ESlot1GetProvisioningRefFile,
	ESlotSingleEvent,
	EMaxNumberSatCPtrSlots
	};

/** 
 * CSatPtrHolder inherits from CBase.
 *
 * The two main purposes of CSatPtrHolder are as follows:
 *
 *     - non-descriptor parameters in API methods will be copied 
 *	     into a descriptor and held in the CSatPtrHolder descriptor array, iPtrArray. 
 *
 *     - CSatPtrHolder public data members ( e.g. iPcmd) store copies of API parameters
 *       that are passed in by value rather than by reference. 
 *       This keeps the R-classes size constant as the API evolves and keeps BC.
 */

class CSatPtrHolder : public CBase
	{
public:
	static CSatPtrHolder* NewL(TInt aSizeOfPtrArray, TInt aSizeOfPtrCArray);
	~CSatPtrHolder();

	template<typename T> TPtr8& Set(TInt aSlot,T& aObject)
		{
		TPtr8& ptr=Ptr(aSlot);
		ptr.Set(reinterpret_cast<TText8*>(&aObject),sizeof(T),sizeof(T));
		return ptr;
		};

	template <typename T> inline TPtrC8& SetC(TInt aSlot, const T& aObject)
		{
		TPtrC8& ptr=PtrC(aSlot);
		ptr.Set(REINTERPRET_CAST(const TText8*,(&aObject)),sizeof(T));
		return ptr;
		};


protected:
	virtual void ConstructL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);	
	CSatPtrHolder();

private:
	inline TPtr8& Ptr(TInt aIndex);
	inline TPtrC8& PtrC(TInt aIndex);
public:
	RSat::TPCmd iPCmd;									//< Holds the PCmd name
	TUint8 iGetIcon;									//< Holds the icon record number
	TUint16 iMsgRef;									//< Holds the SMS message reference
	RSat::TInstanceNumberAndOffset iGetClut;			//< Holds the instance number and offset associated with the CLUT.
	RSat::TEventList iSingleEvent;						//< Holds the single event
	RSat::TProvisioningFileRef iGetProvisioningRefFile; //< Holds the Provisioning reference file

protected:
	RArray<TPtr8> iPtrArray;
	RArray<TPtrC8> iPtrCArray;
	};
//
// Inline Functions
//

inline TPtr8& CSatPtrHolder::Ptr(TInt aIndex)
	{
	__ASSERT_ALWAYS(aIndex<iPtrArray.Count(),PanicClient(EEtelPanicIndexOutOfRange));
	return iPtrArray[aIndex];
	}

inline TPtrC8& CSatPtrHolder::PtrC(TInt aIndex)
	{
	__ASSERT_ALWAYS(aIndex<iPtrCArray.Count(),PanicClient(EEtelPanicIndexOutOfRange));
	return iPtrCArray[aIndex];
	}

#endif


