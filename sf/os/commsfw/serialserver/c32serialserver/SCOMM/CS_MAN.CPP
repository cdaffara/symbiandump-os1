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


/** @file
 *
 * Implements CPortManager and various small functions
 */

#include "CS_STD.H"
#include <f32file.h>
#include "C32LOG.H"

LOCAL_C void CloseCObject(TAny* aCObject)
/**
 * Utility func for cleanup stack - clean up a CObject
 *
 * @param aObj pointer to object to be closed
 */
	{
	((CObject*)aCObject)->Close();
	}


//
// implementation of CPortManager
//

CPort* CPortManager::GetPortL(const TDesC& aName, TUint aPort, CSerial* aSerial,
							  TUint aMode, TUint aRole, CCommSession* aSession)
/**
 * Return a port matching a given name for a given mode.
 * If the port already exists, increments the count on this CObject.
 *
 * @param aName    name of the port 
 * @param aPort    port number
 * @param aSerial  pointer to the CSerial object
 * @param aMode    mode; Exclusive, Shared or Preemptable
 * @param aRole    role; DTE or DCE
 * @param aSession handle to the clients session
 * @return CPort*  pointer to the CPort object
 *
 * @leave Leave This function may leave
 */
	{
	CPort* p = NULL;
	TInt handle = 0;
	TFullName dummy;
	TInt res = iPorts->FindByFullName(handle, aName, dummy);
	switch (res)
		{
	 case KErrNotFound:
			{
			(void)aSerial->Open();	// CObject->Open() always returns KErrNone so its safe to ignore return value
			TCleanupItem serialClose(CloseCObject, aSerial);
			CleanupStack::PushL(serialClose);
			// And create a port from it.
			p = aSerial->NewPortL(aPort);
			TCleanupItem portClose(CloseCObject, p);
			CleanupStack::PushL(portClose);
			p->iExtra = new(ELeave) CPort::CExtra();
			p->DoOpenL(aSession,(TInternalCommAccess)aMode,(TCommRole)aRole,ETrue);
			p->iPortManager = this;
			iPorts->AddL(p);
			p->SetOwner(aSerial);
			C32LOG2(KC32Player, _L8("CPortManager::GetPortL() New Port - AccessCount: %d"), p->AccessCount());
			CleanupStack::Pop(2); // portClose, serialClose
			break;
			}
	case KErrNone:
		// find the port, attempt to open it as shared or take possession, and increment
		// object usage count.
		p = (CPort*)iPorts->At(handle);
		p->DoOpenL(aSession, (TInternalCommAccess)aMode,(TCommRole)aRole,EFalse);
		(void)p->Open();	// CObject->Open() always returns KErrNone so its safe to ignore return value
		C32LOG2(KC32Player, _L8("CPortManager::GetPortL() Existing Port - AccessCount: %d"), p->AccessCount());
		break;
	default:
		User::Leave(res);
		}
	return p;
	}


CPortManager* CPortManager::NewL()
/**
 * Locate and initialise all loadable port modules
 *
 * @return an newly created CPortManager object
 * @leave Leave This function may leave
 */
	{
	CPortManager* p = new (ELeave) CPortManager();
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();
	return p;
	}

CPortManager::~CPortManager()
	{
	iContainer->Remove(iProviders);		
	iContainer->Remove(iPorts);
	delete iContainer;
	}

void CPortManager::ConstructL()
/**
 * construct and init the children
 */
	{
    iContainer = CObjectConIx::NewL();
	iPorts = iContainer->CreateL();
	iProviders = iContainer->CreateL();
	}


LOCAL_C void CloseLibrary(TAny* aLib)
/**
 * Close a library from the cleanup stack
 *
 * @param aLib pointer to the library to close
 */
	{
	((RLibrary*)aLib)->Close();
	}



CSerial* CPortManager::GetSerialL(const TDesC& aName)
/**
 * Find a previously loaded CSY by name and return its factory
 *
 * @param aName name of the port
 * @return CSerial* pointer to the serial object
 * @leave Leave This function may leave
 */
	{
	TInt handle = 0;
	TName matching;
	(void)User::LeaveIfError(iProviders->FindByName(handle, aName, matching));
	return (CSerial*)iProviders->At(handle);
	}


CSerial* CPortManager::LoadCommModuleL(const TDesC& aFileName)
/**
 * Load a comms module on user request or increments the ref count for the module.
 * This function loads the CSY
 * specified in aFileName, and if found calls the first ordinal on it.
 * The CSY instantiates itself and returns the pointer to the CSerial
 * object implemented in the CSY. If everything goes OK then this 
 * pointer will be returned to the callee which then takes ownedship of it.
 *
 * @param aFileName name of the new CSY module to load (i.e. ECUART.CSY)
 * @return CSerial* pointer to the new CSerial object
 * @leave Leave This function may leave
 */	
	{
	C32LOG2(KC32Player,_L("CPortManager::LoadCommModuleL(), Comms Module Name : %S"), &aFileName);
	RLibrary lib;
	TInt r=lib.Load(aFileName);
	if (r!=KErrNone)
		User::Leave(r);

	TCleanupItem libClose(CloseLibrary,&lib);
	CleanupStack::PushL(libClose);

	// Check the Uid2 [unicode]
	if(lib.Type()[1]!=TUid::Uid(KUidUnicodeCommServerModuleV02))
		User::Leave(KErrBadLibraryEntryPoint);

	TSerialNewL libEntry=(TSerialNewL)lib.Lookup(1);
	if (libEntry==NULL)
		User::Leave(KErrBadLibraryEntryPoint);

	CSerial* s = NULL;
	s = (*libEntry)();	// libEntry may leave.  
    User::LeaveIfNull(s); // libEntry may also return null

	CleanupStack::PushL(TCleanupItem(CloseCObject,s));
	s->ConstructL(lib); // Once lib is transferred it's handle is nulified because
	                    // deleting s will cause the reference to be closed

	TRAPD(ret, iProviders->AddL(s));

	switch (ret)
		{
	case KErrAlreadyExists:
		{
		// The CSY is already loaded since we already have a handle to
		// its CSerial factory, so just increase the refcount on the factory.
		TInt fHand = 0;
		TName dummy;
		iProviders->FindByName(fHand, s->Name(), dummy); // Can't be an error
		CSerial* s2 = (CSerial*)iProviders->At(fHand);
		s->Close(); // Should delete s and the library reference will be closed
		s2->Open();
		s = s2;
		}
		break;
	case KErrNone:
		// success!
		break;
	default:
		User::Leave(ret);
		break;
		}

	CleanupStack::Pop(2); // s and library reference are now in safe hands

	return s;
	}


TInt CPortManager::PortInfo(const TPortName& aName, TSerialInfo& aSerial)
/**
 * get info on a specified port.
 *
 * @param aName name of the port
 * @param aSerial serial info will be written here
 * @return TInt error code
 */
	{
	C32LOG2(KC32Player,_L("CPortManager::PortInfo() by Port Name : %S"), &aName);
	TInt handle = 0;
	TName matching;
	TInt res = iProviders->FindByName(handle,aName,matching);
	if(res == KErrNotFound) // INC052021
		{
		TFileName fname;
		TInt index = iProviders->Count();
		while(index>0)
			{
			index--;
			CSerial* actual = (CSerial*)iProviders->operator[](index);
			actual->ModuleName(fname);
			if(fname == aName)
				{
				actual->Info(aSerial);
				return KErrNone;
				}
			}
		}
	if (res != KErrNone)
		return res;
	CSerial* s = (CSerial*)(iProviders->At(handle));
	s->Info(aSerial);
	return KErrNone;
	}


// EOF - CS_MAN.CPP
