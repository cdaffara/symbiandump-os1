// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains method definitions for CMmPtrHolder and all of the classes that derive from it
// 
//

/**
 @file
*/

// From core API
#include <etelext.h>

// Multimode header files
#include <etelmm.h>
#include "mm_hold.h"


//
// CMmPtrHolder - class owned by RMobile... objects to contain the TPtr8's and other member
// data required by asynchronous functions so that any additional functions will not require 
// breaking BC.
//

/*
CMmPtrHolder* CMmPtrHolder::NewL(TInt aSizeOfPtrArray, TInt aSizeOfPtrCArray)
	{
	CMmPtrHolder* p = new (ELeave) CMmPtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}
*/
CMmPtrHolder::CMmPtrHolder()
/** Standard C++ constructor */
	{}

CMmPtrHolder::~CMmPtrHolder()
/** Standard destructor */
	{
	iPtrArray.Close();
	iPtrCArray.Close();
	}

void CMmPtrHolder::ConstructL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method constructs the two arrays: one to hold TPtr8's and the other TPtrC8's
 */
	{
	TPtr8 ptr(NULL,0);

	TInt i;
	for (i = 0; i < aSizeOfPtrArray; i++)
		{
		User::LeaveIfError(iPtrArray.Append(ptr));
		}

	TPtrC8 ptrC(NULL,0);
	for (i = 0; i < aSizeOfPtrCArray; i++)
		{
		User::LeaveIfError(iPtrCArray.Append(ptrC));
		}
	}

TPtr8& CMmPtrHolder::Ptr(TInt aIndex)
/**
 * This method returns a reference to the TPtr8 at aIndex within the array
 *
 * @param aIndex The array location of the desired TPtr8
 * @return TPtr8&
 */
	{
	return iPtrArray[aIndex];
	}

TPtrC8& CMmPtrHolder::PtrC(TInt aIndex)
/**
 * This method returns a reference to the TPtrC8 at aIndex within the array
 *
 * @param aIndex The array location of the desired TPtrC8
 * @return TPtrC8&
 */
	{
	return iPtrCArray[aIndex];
	}

//
// CMobilePhonePtrHolder
//

CMobilePhonePtrHolder* CMobilePhonePtrHolder::NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method creates the RMobilePhone ptr holder object 
 * Array sizes depend on number of RMobilePhone requests that require ptr holder slots
 *
 * @param aSizeOfPtrArray Number of TPtr8 slots required
 * @param aSizeOfPtrCArray Number of TPtrC8 slots required
 * @return CMobilePhonePtrHolder* Pointer to newly created ptr holder object
 */
	{
	CMobilePhonePtrHolder* p = new (ELeave) CMobilePhonePtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}
	
CMobilePhonePtrHolder::CMobilePhonePtrHolder() :
	iStoreNetworkPtr(NULL,0,0),iStoreWlanSIDPtr(NULL,0,0)
/**
Constructor
*/
	{}
	
	
CMobilePhonePtrHolder::~CMobilePhonePtrHolder()
/**
Destructor
*/
	{
	delete iStoreNetworkBuf;
	delete iStoreWlanSIDBuf;
	}

//
// CMobileLinePtrHolder
//

CMobileLinePtrHolder* CMobileLinePtrHolder::NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method creates the RMobileLine ptr holder object 
 * Array sizes depend on number of RMobileLine requests that require ptr holder slots
 *
 * @param aSizeOfPtrArray Number of TPtr8 slots required
 * @param aSizeOfPtrCArray Number of TPtrC8 slots required
 * @return CMobilePhonePtrHolder* Pointer to newly created ptr holder object
 */
	{
	CMobileLinePtrHolder* p = new (ELeave) CMobileLinePtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

//
// CMobileCallPtrHolder
//

CMobileCallPtrHolder* CMobileCallPtrHolder::NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method creates the RMobileCall ptr holder object 
 * Array sizes depend on number of RMobileCall requests that require ptr holder slots
 *
 * @param aSizeOfPtrArray Number of TPtr8 slots required
 * @param aSizeOfPtrCArray Number of TPtrC8 slots required
 * @return CMobilePhonePtrHolder* Pointer to newly created ptr holder object
 */
	{
	CMobileCallPtrHolder* p = new (ELeave) CMobileCallPtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}
	
//
// CSmsMessagingPtrHolder
//

CSmsMessagingPtrHolder* CSmsMessagingPtrHolder::NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method creates the RMobileSmsMessaging ptr holder object 
 * Array sizes depend on number of RMobileSmsMessaging requests that require ptr holder slots
 *
 * @param aSizeOfPtrArray Number of TPtr8 slots required
 * @param aSizeOfPtrCArray Number of TPtrC8 slots required
 * @return CMobilePhonePtrHolder* Pointer to newly created ptr holder object
 */
	{
	CSmsMessagingPtrHolder* p = new (ELeave) CSmsMessagingPtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

CSmsMessagingPtrHolder::CSmsMessagingPtrHolder() : 
	iSmspPtr(NULL,0,0)
	{
	}

CSmsMessagingPtrHolder::~CSmsMessagingPtrHolder()
	{
	delete iSmspBuf;
	}

//
// CCbsMessagingPtrHolder
//

CCbsMessagingPtrHolder* CCbsMessagingPtrHolder::NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method creates the RMobileBroadcastMessaging ptr holder object 
 * Array sizes depend on number of RMobileBroadcastMessaging requests that require ptr holder slots
 *
 * @param aSizeOfPtrArray Number of TPtr8 slots required
 * @param aSizeOfPtrCArray Number of TPtrC8 slots required
 * @return CMobilePhonePtrHolder* Pointer to newly created ptr holder object
 */
	{
	CCbsMessagingPtrHolder* p = new (ELeave) CCbsMessagingPtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

CCbsMessagingPtrHolder::CCbsMessagingPtrHolder() : 
	iBroadcastIdPtr(NULL,0,0)
	{
	}

CCbsMessagingPtrHolder::~CCbsMessagingPtrHolder()
	{
	delete iBroadcastIdBuf;
	}

//
// CMobilePhoneStorePtrHolder
//

CMobilePhoneStorePtrHolder* CMobilePhoneStorePtrHolder::NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method creates the RMobilePhoneStore ptr holder object 
 * Array sizes depend on number of RMobilePhoneStore requests that require ptr holder slots
 *
 * @param aSizeOfPtrArray Number of TPtr8 slots required
 * @param aSizeOfPtrCArray Number of TPtrC8 slots required
 * @return CMobilePhonePtrHolder* Pointer to newly created ptr holder object
 */
	{
	CMobilePhoneStorePtrHolder* p = new (ELeave) CMobilePhoneStorePtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

//
// CSmsStorePtrHolder
//

CSmsStorePtrHolder* CSmsStorePtrHolder::NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method creates the RMobileSmsStore ptr holder object 
 * Array sizes depend on number of RMobileSmsStore requests that require ptr holder slots
 *
 * @param aSizeOfPtrArray Number of TPtr8 slots required
 * @param aSizeOfPtrCArray Number of TPtrC8 slots required
 * @return CMobilePhonePtrHolder* Pointer to newly created ptr holder object
 */
	{
	CSmsStorePtrHolder* p = new (ELeave) CSmsStorePtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

//
// CNamStorePtrHolder
//

CNamStorePtrHolder* CNamStorePtrHolder::NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method creates the RMobileNamStore ptr holder object 
 * Array sizes depend on number of RMobileNamStore requests that require ptr holder slots
 *
 * @param aSizeOfPtrArray Number of TPtr8 slots required
 * @param aSizeOfPtrCArray Number of TPtrC8 slots required
 * @return CMobilePhonePtrHolder* Pointer to newly created ptr holder object
 */
	{
	CNamStorePtrHolder* p = new (ELeave) CNamStorePtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

CNamStorePtrHolder::CNamStorePtrHolder() :
	iNamPtr(NULL,0),iNamPtrV4(NULL,0)
	{
	}

CNamStorePtrHolder::~CNamStorePtrHolder()
	{
	delete iNamBuf;
	delete iNamBufV4;
	}

//
// CONStorePtrHolder
//

CONStorePtrHolder* CONStorePtrHolder::NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method creates the RMobileONStore ptr holder object 
 * Array sizes depend on number of RMobileONStore requests that require ptr holder slots
 *
 * @param aSizeOfPtrArray Number of TPtr8 slots required
 * @param aSizeOfPtrCArray Number of TPtrC8 slots required
 * @return CMobilePhonePtrHolder* Pointer to newly created ptr holder object
 */
	{
	CONStorePtrHolder* p = new (ELeave) CONStorePtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

CONStorePtrHolder::CONStorePtrHolder() : 
	iONPtr(NULL,0)
	{
	}

CONStorePtrHolder::~CONStorePtrHolder()
	{
	delete iONBuf;
	}

//
// CPhoneBookStorePtrHolder
//

CPhoneBookStorePtrHolder* CPhoneBookStorePtrHolder::NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method creates the RMobilePhoneBookStore ptr holder object 
 * Array sizes depend on number of RMobilePhoneBookStore requests that require ptr holder slots
 *
 * @param aSizeOfPtrArray Number of TPtr8 slots required
 * @param aSizeOfPtrCArray Number of TPtrC8 slots required
 * @return CMobilePhonePtrHolder* Pointer to newly created ptr holder object
 */
	{
	CPhoneBookStorePtrHolder* p = new (ELeave) CPhoneBookStorePtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

//
// CMobileConferenceCallPtrHolder
//

CMobileConferenceCallPtrHolder* CMobileConferenceCallPtrHolder::NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method creates the RMobileConferenceCall ptr holder object 
 * Array sizes depend on number of RMobileConferenceCall requests that require ptr holder slots
 *
 * @param aSizeOfPtrArray Number of TPtr8 slots required
 * @param aSizeOfPtrCArray Number of TPtrC8 slots required
 * @return CMobilePhonePtrHolder* Pointer to newly created ptr holder  object
 */
	{
	CMobileConferenceCallPtrHolder* p = new (ELeave) CMobileConferenceCallPtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}


//
// CMobileSmartCardEapPtrHolder
//

/**
This method creates the RMobileSmartCardEap ptr holder object.
Array sizes depend on number of RMobileSmartCardEap requests that
require ptr holder slots.

@param aSizeOfPtrArray Number of TPtr8 slots required
@param aSizeOfPtrCArray Number of TPtrC8 slots required
@return Pointer to a new CMobileSmartCardEapPtrHolder object
*/
CMobileSmartCardEapPtrHolder* CMobileSmartCardEapPtrHolder::NewL(TInt aSizeOfPtrArray, TInt aSizeOfPtrCArray)
	{
	CMobileSmartCardEapPtrHolder* self = new (ELeave) CMobileSmartCardEapPtrHolder();
	CleanupStack::PushL(self);
	self->ConstructL(aSizeOfPtrArray, aSizeOfPtrCArray);
	CleanupStack::Pop();
	return self;
	}

/**
Default constructor.
*/
CMobileSmartCardEapPtrHolder::CMobileSmartCardEapPtrHolder()
: iReqAuthData(NULL,0)
	{
	// NOP
	}
	
/**
Default destructor.
*/
CMobileSmartCardEapPtrHolder::~CMobileSmartCardEapPtrHolder()
	{
	// NOP
	}


//
// CMobileLocationServicesPtrHolder
//

CMobileLocationServicesPtrHolder* CMobileLocationServicesPtrHolder::NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method creates the RMobileLocationServices ptr holder object 
 * Array sizes depend on number of RMobileLocationServices requests that require ptr holder slots
 *
 * @param aSizeOfPtrArray Number of TPtr8 slots required
 * @param aSizeOfPtrCArray Number of TPtrC8 slots required
 * @return CMobilePhonePtrHolder* Pointer to newly created ptr holder  object
 */
	{
	CMobileLocationServicesPtrHolder* p = new (ELeave) CMobileLocationServicesPtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}
