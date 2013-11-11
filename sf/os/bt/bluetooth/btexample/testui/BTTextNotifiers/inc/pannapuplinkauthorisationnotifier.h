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
//

#ifndef NAPUPLINKAUTHORISATIONNOTIFIER_H
#define NAPUPLINKAUTHORISATIONNOTIFIER_H

#include <e32base.h>
#include <e32cmn.h>
#include <twintnotifier.h>
#include <btextnotifiers.h>


class CPanNapUplinkAuthorisationNotifierEngine;

NONSHARABLE_CLASS(MPanNapUplinkAuthorisationNotifierNotifier)
	{
public:
	virtual void CompleteNotifier(TInt aError, TNapConnectionResult aResult) = 0;
	};




NONSHARABLE_CLASS(CPanNapUplinkAuthorisationNotifier) 
	: public CBase
	, public MNotifierBase2
	, public MPanNapUplinkAuthorisationNotifierNotifier
	{
public:
	static CPanNapUplinkAuthorisationNotifier* NewLC();
	~CPanNapUplinkAuthorisationNotifier();

	// From MNotifierBase2
	 
	/**
	Frees all resources owned by this notifier.

    This function is called by the notifier framework when all resources
    allocated by notifiers should be freed. As a minimum, the implementation
    should delete this object (i.e. delete this;).

    Note that it is important to implement this function correctly
    to avoid memory leaks.
	*/
	virtual void Release();


		
    /**
    Performs any initialisation that this notifier may require.

    The function is called when the notifier is first loaded,
    i.e. when the plug-in DLL is loaded. It is called only once.

    As a minimum, the implementation should return a TNotifierInfo instance
    describing the notifier parameters. A good implementation would be to set
    up a TNotifierInfo as a data member, and then to return it. This is because
    the same information is returned by Info().

    The function is safe to leave from, so it is possible,
    although rarely necessary, to allocate objects as you would normally do
    in a ConstructL() function as part of two-phase construction.

	@return The parameters of the notifier.

	@see MNotifierBase2::Info
    */	 
	virtual TNotifierInfo RegisterL();



	/**
	Gets the notifier parameters.

    This is the same information as returned by RegisterL(), although
    the returned values may be varied at run-time.
	*/
	virtual TNotifierInfo Info() const;



	/**
	Starts the notifier.

    This is called as a result of a client-side call
    to RNotifier::StartNotifier(), which the client uses to start a notifier
    from which it does not expect a response.

    The function is synchronous, but it should be implemented so that
    it completes as soon as possible, allowing the notifier framework
    to enforce its priority mechanism.

    It is not possible to wait for a notifier to complete before returning
    from this function unless the notifier is likely to finish implementing
    its functionality immediately.

	@param	aBuffer Data that can be passed from the client-side.
	        The format and meaning of any data is implementation dependent.
	        
	@return A pointer descriptor representing data for the initial response

	@see RNotifier::StartNotifier
	*/ 
	virtual TPtrC8 StartL(const TDesC8& aBuffer);



	/**
	Starts the notifier.

    This is called as a result of a client-side call to
    the asynchronous function RNotifier::StartNotifierAndGetResponse().
    This means that the client is waiting, asynchronously, for the notifier
    to tell the client that it has finished its work.

    It is important to return from this function as soon as possible,
    
    The implementation of a derived class must make sure that Complete() is
    called on the RMessage2 object when the notifier is deactivated.

    This function may be called multiple times if more than one client starts
    the notifier.

    @param aBuffer    Data that can be passed from the client-side. The format
                      and meaning of any data is implementation dependent.
    @param aReplySlot The offset within the message arguments for the reply.
                      This message argument will refer to a modifiable
                      descriptor, a TDes8 type, into which data
                      can be returned. The format and meaning
                      of any returned data is implementation dependent.
    @param aMessage   Encapsulates a client request.
    
    @see RNotifier::StartNotifierAndGetResponse
    */
	virtual void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	
	
	
	/**
	Cancels an active notifier.

    This is called as a result of a client-side call
    to RNotifier::CancelNotifier().

    An implementation should free any relevant resources, and complete
    any outstanding messages, if relevant.
    
    @see RNotifier::CancelNotifier
	*/ 
	virtual void Cancel();



	/**
	Updates a currently active notifier with new data.

	This is called as a result of a client-side call
	to RNotifier::UpdateNotifier().
	
	@param aBuffer Data that can be passed from the client-side.
	               The format and meaning of any data is
	               implementation dependent.
	
	@return A pointer descriptor representing data that may be returned.
	        The format and meaning of any data is implementation dependent.

	@see RNotifier::UpdateNotifier
	*/ 
	virtual TPtrC8 UpdateL(const TDesC8& aBuffer);


	// From MPanNapUplinkAuthorisationNotifierNotifier
	virtual void CompleteNotifier(TInt aError, TNapConnectionResult aResult);
	
private:
	CPanNapUplinkAuthorisationNotifier();
	void ConstructL();

private:
	const static TInt KScreenOutputChannelNumber = 0x00000666;

private:
	TNotifierInfo iNotifierInfo;
	RMessage2 iMessage;				//< Client message to signal when device selection is complete
	TBool iMessageOutstanding;		//< Have we got a message outstanding that needs to be completed?
	TInt iReplySlot;				//< The slot in the message that contains the reply buffer
	
	TNapConnectionResultPckg iResultPckg;
	
	CPanNapUplinkAuthorisationNotifierEngine* iSelectorEngine;
	};


NONSHARABLE_CLASS(CPanNapUplinkAuthorisationNotifierEngine) : public CActive
	{
public:
	static CPanNapUplinkAuthorisationNotifierEngine* NewL(MPanNapUplinkAuthorisationNotifierNotifier& aNotifier);
	~CPanNapUplinkAuthorisationNotifierEngine();

	void GetLocalServiceL(const TDesC8& aBuffer);
	
private:
	CPanNapUplinkAuthorisationNotifierEngine(MPanNapUplinkAuthorisationNotifierNotifier& aNotifier);
	void ConstructL();

	void RunL();
	void DoCancel();

	void DrawExistingConnections();
	void DrawInstructionsAndWaitForInput();
	
	void CloseConsoleWindow();
	
private:
	MPanNapUplinkAuthorisationNotifierNotifier& iNotifier;
	TPanConnectionListPckg iConnectionListPckg;
	CConsoleBase* iConsole;
	};


#endif // NAPUPLINKAUTHORISATIONNOTIFIER_H
