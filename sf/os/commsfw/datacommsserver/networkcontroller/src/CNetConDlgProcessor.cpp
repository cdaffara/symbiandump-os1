// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CNetConDlgProcessor.h"
#include <comms-infras/dialogprocessor.h>
#include "CNetworkController.h"
#include "MNetConDialogProcAccess.h"
#include "NetConPanic.h"


/*
Template adapter class:
Implement MNetConDialogProcAccess by forwarding all calls to 'Implementor' class
Also can optionally 'own' the instance of the implementor (i.e. deletes it on destruction)
*/
template<typename Implementor>
class CMDialogProcessorImpl : public CBase, public MNetConDialogProcAccess
	{
protected:
	typedef Implementor TImplementation; //< Type of the implementor

public:
	/**
	   Construct an MDialogProcessorImpl object.

	   @param aPimpl  Points to implementation object
	   @param aOwnImpl If ETrue, then the aPimpl will be destroyed when *this is destroyed.
	   @return Instance of CMDialogProcessorImpl
	*/
	static CMDialogProcessorImpl* NewL( TImplementation* aPimpl, TBool aOwnImpl )
		{
		CMDialogProcessorImpl* self = new (ELeave) CMDialogProcessorImpl( aPimpl, aOwnImpl );
		return self;
		}

	/**
	   Forward the CancelEverything request to the implementation
	*/
	virtual void CancelEverything()
		{
		GetImpl()->CancelEverything();
		}
	/**
	   Forward the SelectConnection request to the implementation
	*/
	virtual void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs)
		{
		GetImpl()->SelectConnection( aObserver, aPrefs );
		}
	/**
	   Forward the SelectConnection request to the implementation
	*/
	virtual void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, TInt aLastError )
		{
		GetImpl()->SelectConnection( aObserver, aPrefs, aLastError );
		}
	/**
	   Forward the SelectModemAndLocation request to the implementation
	*/
	virtual void SelectModemAndLocation(MDialogProcessorObserver& aObserver)
		{
		GetImpl()->SelectModemAndLocation( aObserver );
		}
	/**
	   Forward the WarnNewConnection request to the implementation
	*/
	virtual void WarnNewConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, const TDesC* aNewIapName, const TIspConnectionNames* aNewConnectionNames, TInt aLastError )
		{
		GetImpl()->WarnNewConnection( aObserver, aPrefs, aNewIapName, aNewConnectionNames, aLastError );
		}
	/**
	   Forward the Reconnect request to the implementation
	*/
	virtual void Reconnect(MDialogProcessorObserver& aObserver )
		{
		GetImpl()->Reconnect( aObserver );
		}

	/**
	   Destroy this object.
	*/
	virtual ~CMDialogProcessorImpl()
		{
		if( iOwnImpl )
			{
			delete iPimpl;		// We own it, so get rid of it
			}
		iPimpl = 0;
		}
protected:
	/**
	   Constructor. Unexciting in the extreme.
	   @param aPimpl pointer to implementation
	   @param aOwnImpl If ETrue, transfers ownership of the implementation to this wrapper
	*/
	CMDialogProcessorImpl( TImplementation *aPimpl, TBool aOwnImpl )
		:iPimpl(aPimpl), iOwnImpl(aOwnImpl)
		{
		__ASSERT_DEBUG( iPimpl, NetConPanic(NetworkController::ETelBearerBadState));
		}

private:
	/**
	   Return the implementation pointer
	*/
	inline TImplementation* GetImpl() const
		{
		__ASSERT_DEBUG( iPimpl, NetConPanic(NetworkController::ETelBearerBadState));
		return iPimpl;
		}
private:
	TImplementation* iPimpl;	//< Pointer to class that implements the interface in a non-virtual fashion
	TBool iOwnImpl;				//< ETrue if the destructor should delete iPimpl
	};



/**
   Implement MNetConDialogProcAccess destructor
*/
MNetConDialogProcAccess::~MNetConDialogProcAccess()
	{
	}

/**
  Construct a CNetConDlgProcessor using the default CDialogProcessor implementation

  @return a pointer to the newly created CNetConDlgProcessor
*/
CNetConDlgProcessor* CNetConDlgProcessor::NewL()
	{
	// To do this, we have to wrap a CDialogProcessor up so it looks like a MNetConDialogProcAccess.
	// so use the CMDialogProcessorImpl wrapper class

	// First, construct the CDialogProcessor itself...
	CDialogProcessor* processor = CDialogProcessor::NewL();
	CleanupStack::PushL( processor );

	// Now costruct the MNetConDialogProcAccess wrapper for it
	CMDialogProcessorImpl<CDialogProcessor>* impl = CMDialogProcessorImpl<CDialogProcessor>::NewL( processor, ETrue );

	// impl now owns aProcess, so get it back from the cleanup stack
	CleanupStack::Pop( processor );

	// and save impl...
	CleanupStack::PushL( impl );

	// Construct the CNetConDlgProcessor itself
	CNetConDlgProcessor* self = new (ELeave) CNetConDlgProcessor( impl );

	// and cleanup the cleanup
	CleanupStack::Pop( impl );
	return self;
	}

/**
  Construct a CNetConDlgProcessor using a supplied MNetConDialogProcAccess implementation

  @param aImpl The supplied implementation
  @param aOwnImpl True if the CNetConDlgProcessor takes ownership of aImpl

  @return a pointer to the newly created CNetConDlgProcessor
*/
CNetConDlgProcessor* CNetConDlgProcessor::NewL( MNetConDialogProcAccess* aImpl, TBool aOwnImpl )
	{
	// Note: We can probably get away without the CMDialogProcessorImpl wrapper in the case where aOwnImpl is true
	// Construct a wrapper for the implementation
	CMDialogProcessorImpl<MNetConDialogProcAccess>* impl = CMDialogProcessorImpl<MNetConDialogProcAccess>::NewL( aImpl, aOwnImpl );
	CleanupStack::PushL( impl );
	// Transfer ownership of the wrapper to the dialog processor itself
	CNetConDlgProcessor* self = new (ELeave) CNetConDlgProcessor( impl );
	CleanupStack::Pop( impl );
	return self;
	}

CNetConDlgProcessor::~CNetConDlgProcessor()
	{
	delete iDialogProc;
	}

/**
   Constructor

   @param aImpl  Pointer to dialog processor implementation
*/
CNetConDlgProcessor::CNetConDlgProcessor( MNetConDialogProcAccess* aImpl )
	:iDialogProc( aImpl )
	{
	ASSERT( aImpl );
	}

/**
   Retrieve the implementation pointer
*/
MNetConDialogProcAccess* CNetConDlgProcessor::GetImpl() const
	{
	__ASSERT_DEBUG( iDialogProc, NetConPanic(NetworkController::ETelBearerBadState));
	return iDialogProc;
	}


/**
   Cancel all outstanding dialog processor requests
*/
void CNetConDlgProcessor::CancelEverything()
	{
	GetImpl()->CancelEverything();
	}

/**
   Display a dialog to the user asking them to select an IAP to connect to

   @param aObserver the object to receive notification when the dialog box completes
   @param aPrefs the connection preferences used to select this IAP
*/
void CNetConDlgProcessor::SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs)
	{
	GetImpl()->SelectConnection( aObserver, aPrefs );
	}

/**
   Display a dialog to the user asking them to select an IAP to connect to

   @param aObserver the object to receive notification when the dialog box completes
   @param aPrefs the connection preferences used to select this IAP
   @param aLastError if this is not the 1st connection attempt this is the error of the last attempt
*/
void CNetConDlgProcessor::SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, TInt aLastError )
	{
	GetImpl()->SelectConnection( aObserver, aPrefs, aLastError );
	}

/**
   Display a dialog to the user asking the user to select a modem and location entry from CommDb

   @param aObserver the object to receive notification when the dialog box completes
*/
void CNetConDlgProcessor::SelectModemAndLocation(MDialogProcessorObserver& aObserver)
	{
	GetImpl()->SelectModemAndLocation( aObserver );
	}

/**
   Display a dialog to the user warning that a new connection is about to be attempted to the specified IAP

   @param aObserver the object to receive notification when the dialog box completes
   @param aPrefs the connection preferences used to select this IAP
   @param aNewIapName the name of the IAP used for the connection.
   @param aNewConnectionNames this parameter is no-longer used.
   @param aLastError if this is not the 1st connection attempt this is the error of the last attempt
*/
void CNetConDlgProcessor::WarnNewConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, const TDesC* aNewIapName, const TIspConnectionNames* aNewConnectionNames, TInt aLastError )
	{
	GetImpl()->WarnNewConnection( aObserver, aPrefs, aNewIapName, aNewConnectionNames, aLastError );
	}

/**
   Display a dialog to the user asking whether to reconnect a failed connection

   @param aObserver the object to receive notification when the dialog box completes
*/
void CNetConDlgProcessor::Reconnect(MDialogProcessorObserver& aObserver )
	{
	GetImpl()->Reconnect( aObserver );
	}

