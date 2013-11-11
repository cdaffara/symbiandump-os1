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
//

/**
 @file
 @internalComponent
 @test
*/


#ifndef BTUINOTIFIERS_H
#define BTUINOTIFIERS_H

#include <e32std.h>
#include <e32base.h>
#include <e32property.h>
#include <eiknotapi.h>
#include <btdevice.h>
#include <btmanclient.h>

/**
UID of Publish & Subscribe variable category to control auto notifiers

UID of the automatic authorisation RNotifier plugin - unused
@publishedPartner
@released
*/
const TUid KUidAutoNotifierCat = { 0x1020DBC4}; // unused

/**
UID range for Publish & Subscribe variables to control auto notifiers

@internalTechnology
*/
const TUint KUidBluetoothTestPubSubKeyBase = 0x1027235E; //	Range of 10 values registered
const TUint KUidBluetoothTestPubSubKeyBase2 = 0x10286508; // Range of 50 values registered
/**
UID for auto notifiers - Pin Code

@internalTechnology
*/
const TUint KUidAutoPropertyPinCode = (KUidBluetoothTestPubSubKeyBase +0); 

/**
UID for auto notifiers - Authorise Accept

@internalTechnology
*/
const TUint KUidAutoPropertyAuthoriseAccept = (KUidBluetoothTestPubSubKeyBase +1);

/**
UID for Pbap auto notifiers - Password

@internalTechnology
*/
const TUint KUidPbapAutoNotifierPassword = (KUidBluetoothTestPubSubKeyBase + 2); 

/**
UID for Pbap auto notifiers - Executed Status

@internalTechnology
*/
const TUint KUidPbapAutoNotifierExecuted = (KUidBluetoothTestPubSubKeyBase + 3);

/**
UID for PAN NAP Auth auto notifiers - Result (i.e., accept as NAP, GN or disallow)

@internalTechnology
*/
const TUint KUidPanNapAutoNotifierResult = (KUidBluetoothTestPubSubKeyBase + 4); 

/**
UID for PAN NAP Auth auto notifiers - Executed Status

@internalTechnology
*/
const TUint KUidPanNapAutoNotifierExecuted = (KUidBluetoothTestPubSubKeyBase + 5);

/**
Pin input completed - Status

@internalTechnology
*/
const TUint KUidAuthenticationExecuted = (KUidBluetoothTestPubSubKeyBase + 6);

/**
Authorisation completed - Status

*/
const TUint KUidAuthorisationExecuted = (KUidBluetoothTestPubSubKeyBase + 7);


/**
Numeric value from the controller

*/
const TUint KUidSspNumericValue = (KUidBluetoothTestPubSubKeyBase + 8);

/**
Passkey value
*/
const TUint KUidSspPassKey = (KUidBluetoothTestPubSubKeyBase + 9);

/**
 * P&S key for identifying the type of notifier that has run.
 */
const TUint KUidLastNotifierTypeRun = (KUidBluetoothTestPubSubKeyBase2 + 1);

/** 
 * P&S key to hold the information whether the autonotifier should pass the authorization or not
 * By default it will pass. 
 */
const TUint KUidSspAuthorisation = (KUidBluetoothTestPubSubKeyBase2 + 2);

/** 
 * P&S key to hold the error which should be issued from the notifiers. Currently it is mainly used
 * for canceling the Notifiers.
 * Offset 3 and 4 are used by PLM and LSTO tests so I am using offset 5
 */
const TUint KUidErrorNotifiers = (KUidBluetoothTestPubSubKeyBase2 + 5);

/** 
 * P&S key to hold the information about who initiated the authetication. 
 */
const TUint KUidLocallyInitiated = (KUidBluetoothTestPubSubKeyBase2 + 6);



_LIT(KPanicCat, "BTAutoNotifiers");
enum TPanicCode
	{
	ESynchronousStartWithNumericComparison,
	ESynchronousStartWithPasskeyDisplay,
	ESynchronousStartWithPinCodeEntry,
	};


enum TAutoNotifierType
	{
	ENone,
	ESspPassKeyNotifier,
	ESspNumericComparisonNotifier,
	ELegacyPinCodeNotifier,
	};

class CBTUIAutoPasskeyNotifier:  public MEikSrvNotifierBase2
	{
public:
	/*
	 * Contructs a CBTUIPasskeyNotifier object.
	 * @return CBTUIPasskeyNotifier which is the actual object.
	 */
	static CBTUIAutoPasskeyNotifier* NewL();
	/*
	 * Destructor
	 */
	virtual ~CBTUIAutoPasskeyNotifier();

	//from MEikSrvNotifierBase2
	/**
	 * Called when all resources allocated by notifiers shoudl be freed.
	 * @see MEikSrvNotifierBase2::Release()
	 */
	void Release();
	/**
	 * Called when a notifier is first loaded to allow any initial construction that is required.
	 * @see MEikSrvNotifierBase2::RegisterL()
	 */
	TNotifierInfo RegisterL();
	/**
	 * Return the priority a notifier takes and the channels it acts on.  The return value may be varied 
	 * at run-time.
	 * @see MEikSrvNotifierBase2::Info()
	 */
	TNotifierInfo Info() const;
	/**
	 * Start the notifier with data aBuffer and return an initial response.
	 * @see MEikSrvNotifierBase2::StartL(const TDesC8& aBuffer) 
	 */
	TPtrC8 StartL( const TDesC8& aBuffer );
	/**
	 * Start the notifier with data aBuffer.  aMessage should be completed when the notifier is deactivated.
	 * May be called multiple times if more than one client starts the notifier.  The notifier is immediately
	 * responsible for completing aMessage.
	 * @see MEikSrvNotifierBase2::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	 */
	void StartL( const TDesC8& aBuffer, TInt aReplySlot,  const RMessagePtr2& aMessage );
	/**
	 * Update a currently active notifier with data aBuffer.
	 * This is called as a result of a client-side call to the asynchronous 
	 * function RNotifier::UpdateNotifierAndGetResponse(). 
	 * @see MEikSrvNotifierBase2::UpdateL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	 */
	void UpdateL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	/**
	 * Update a currently active notifier with data aBuffer.
	 * This is called as a result of a client-side call to the synchronous function RNotifier::UpdateNotifier().
	 * @see MEikSrvNotifierBase2::UpdateL(const TDesC8& aBuffer)
	 */
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	/**
	 * The notifier has been deactivated so resources can be freed and outstanding messages completed.
	 * @see MEikSrvNotifierBase2::Cancel()
	 */
	void Cancel();	//implemented by CActive!!

private:
	/*
	 * Constructor
	 */
	CBTUIAutoPasskeyNotifier();
	/*
	 * Reads and loads the resource file.
	 */
	void ConstructL();
	/*
	 * Calls SetActive() and starts the active object.
	 */ 

private:
	
	/**
	* RMessagePtr2
	*/
	RMessagePtr2			iMessage;
	
	/**
	* Return value from iMessage
	*/
	TInt iReplySlot;

	/**
	* Notifier info
	*/
	TNotifierInfo		iInfo;

	void PinInputComplete(const TBTPinCode& aPin, TInt aReason);
	
	};


class CBTUIAutoAuthNotifier:  public MEikSrvNotifierBase2
	{
public:
	/*
	 * Contructs a CBTUIPasskeyNotifier object.
	 * @return CBTUIPasskeyNotifier which is the actual object.
	 */
	static CBTUIAutoAuthNotifier* NewL();
	/*
	 * Destructor
	 */
	virtual ~CBTUIAutoAuthNotifier();

	//from MEikSrvNotifierBase2
	/**
	 * Called when all resources allocated by notifiers shoudl be freed.
	 * @see MEikSrvNotifierBase2::Release()
	 */
	void Release();
	/**
	 * Called when a notifier is first loaded to allow any initial construction that is required.
	 * @see MEikSrvNotifierBase2::RegisterL()
	 */
	TNotifierInfo RegisterL();
	/**
	 * Return the priority a notifier takes and the channels it acts on.  The return value may be varied 
	 * at run-time.
	 * @see MEikSrvNotifierBase2::Info()
	 */
	TNotifierInfo Info() const;
	/**
	 * Start the notifier with data aBuffer and return an initial response.
	 * @see MEikSrvNotifierBase2::StartL(const TDesC8& aBuffer) 
	 */
	TPtrC8 StartL( const TDesC8& aBuffer );
	/**
	 * Start the notifier with data aBuffer.  aMessage should be completed when the notifier is deactivated.
	 * May be called multiple times if more than one client starts the notifier.  The notifier is immediately
	 * responsible for completing aMessage.
	 * @see MEikSrvNotifierBase2::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	 */
	void StartL( const TDesC8& aBuffer, TInt aReplySlot,  const RMessagePtr2& aMessage );
	/**
	 * Update a currently active notifier with data aBuffer.
	 * This is called as a result of a client-side call to the asynchronous 
	 * function RNotifier::UpdateNotifierAndGetResponse(). 
	 * @see MEikSrvNotifierBase2::UpdateL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	 */
	void UpdateL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	/**
	 * Update a currently active notifier with data aBuffer.
	 * This is called as a result of a client-side call to the synchronous function RNotifier::UpdateNotifier().
	 * @see MEikSrvNotifierBase2::UpdateL(const TDesC8& aBuffer)
	 */
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	/**
	 * The notifier has been deactivated so resources can be freed and outstanding messages completed.
	 * @see MEikSrvNotifierBase2::Cancel()
	 */
	void Cancel();	//implemented by CActive!!

private:
	/*
	 * Constructor
	 */
	CBTUIAutoAuthNotifier();
	/*
	 * Reads and loads the resource file.
	 */
	void ConstructL();
	/*
	 * Calls SetActive() and starts the active object.
	 */ 

private:
	
	/**
	* RMessagePtr2
	*/
	RMessagePtr2			iMessage;
	
	/**
	* Return value from iMessage
	*/
	TInt iReplySlot;

	/**
	* Notifier info
	*/
	TNotifierInfo		iInfo;

	void AuthorisationComplete(TInt aAuthorised);
	
	};


class CPbapUIAutoAuthNotifier:  public MEikSrvNotifierBase2
	{
public:
	/*
	 * Contructs a CPbapUIAutoAuthNotifier object.
	 * @return CPbapUIAutoAuthNotifier which is the actual object.
	 */
	static CPbapUIAutoAuthNotifier* NewL();
	/*
	 * Destructor
	 */
	virtual ~CPbapUIAutoAuthNotifier();

	//from MEikSrvNotifierBase2
	/**
	 * Called when all resources allocated by notifiers shoudl be freed.
	 * @see MEikSrvNotifierBase2::Release()
	 */
	void Release();
	/**
	 * Called when a notifier is first loaded to allow any initial construction that is required.
	 * @see MEikSrvNotifierBase2::RegisterL()
	 */
	TNotifierInfo RegisterL();
	/**
	 * Return the priority a notifier takes and the channels it acts on.  The return value may be varied 
	 * at run-time.
	 * @see MEikSrvNotifierBase2::Info()
	 */
	TNotifierInfo Info() const;
	/**
	 * Start the notifier with data aBuffer and return an initial response.
	 * @see MEikSrvNotifierBase2::StartL(const TDesC8& aBuffer) 
	 */
	TPtrC8 StartL( const TDesC8& aBuffer );
	/**
	 * Start the notifier with data aBuffer.  aMessage should be completed when the notifier is deactivated.
	 * May be called multiple times if more than one client starts the notifier.  The notifier is immediately
	 * responsible for completing aMessage.
	 * @see MEikSrvNotifierBase2::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	 */
	void StartL( const TDesC8& aBuffer, TInt aReplySlot,  const RMessagePtr2& aMessage );
	/**
	 * Update a currently active notifier with data aBuffer.
	 * This is called as a result of a client-side call to the asynchronous 
	 * function RNotifier::UpdateNotifierAndGetResponse(). 
	 * @see MEikSrvNotifierBase2::UpdateL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	 */
	void UpdateL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	/**
	 * Update a currently active notifier with data aBuffer.
	 * This is called as a result of a client-side call to the synchronous function RNotifier::UpdateNotifier().
	 * @see MEikSrvNotifierBase2::UpdateL(const TDesC8& aBuffer)
	 */
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	/**
	 * The notifier has been deactivated so resources can be freed and outstanding messages completed.
	 * @see MEikSrvNotifierBase2::Cancel()
	 */
	void Cancel();	//implemented by CActive!!

private:
	/*
	 * Constructor
	 */
	CPbapUIAutoAuthNotifier();

private:	
	/**
	* RMessagePtr2
	*/
	RMessagePtr2			iMessage;
	
	/**
	* Return value from iMessage
	*/
	TInt iReplySlot;

	/**
	* Notifier info
	*/
	TNotifierInfo iInfo;

	RProperty iProperty;
	
	void PasswordInputComplete(const TDesC& aPassword, TInt aReason);	
	};


class CPanNapAuthAutoNotifier:  public MEikSrvNotifierBase2
	{
public:
	/*
	 * Contructs a CPanNapAuthAutoNotifier object.
	 * @return CPanNapAuthAutoNotifier which is the actual object.
	 */
	static CPanNapAuthAutoNotifier* NewL();
	/*
	 * Destructor
	 */
	virtual ~CPanNapAuthAutoNotifier();

	//from MEikSrvNotifierBase2
	/**
	 * Called when all resources allocated by notifiers shoudl be freed.
	 * @see MEikSrvNotifierBase2::Release()
	 */
	void Release();
	/**
	 * Called when a notifier is first loaded to allow any initial construction that is required.
	 * @see MEikSrvNotifierBase2::RegisterL()
	 */
	TNotifierInfo RegisterL();
	/**
	 * Return the priority a notifier takes and the channels it acts on.  The return value may be varied 
	 * at run-time.
	 * @see MEikSrvNotifierBase2::Info()
	 */
	TNotifierInfo Info() const;
	/**
	 * Start the notifier with data aBuffer and return an initial response.
	 * @see MEikSrvNotifierBase2::StartL(const TDesC8& aBuffer) 
	 */
	TPtrC8 StartL( const TDesC8& aBuffer );
	/**
	 * Start the notifier with data aBuffer.  aMessage should be completed when the notifier is deactivated.
	 * May be called multiple times if more than one client starts the notifier.  The notifier is immediately
	 * responsible for completing aMessage.
	 * @see MEikSrvNotifierBase2::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	 */
	void StartL( const TDesC8& aBuffer, TInt aReplySlot,  const RMessagePtr2& aMessage );
	/**
	 * Update a currently active notifier with data aBuffer.
	 * @see MEikSrvNotifierBase2::UpdateL(const TDesC8& aBuffer)
	 */
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	/**
	 * The notifier has been deactivated so resources can be freed and outstanding messages completed.
	 * @see MEikSrvNotifierBase2::Cancel()
	 */
	void Cancel();	//implemented by CActive!!

private:
	/*
	 * Constructor
	 */
	CPanNapAuthAutoNotifier();

private:	
	/**
	* RMessagePtr2
	*/
	RMessagePtr2			iMessage;
	
	/**
	* Return value from iMessage
	*/
	TInt iReplySlot;

	/**
	* Notifier info
	*/
	TNotifierInfo iInfo;

	RProperty iProperty;
	
	void PanNapNotifierComplete(TNapConnectionResult aResult, TInt aReason);	
	};

	
	

// ========================================
// |                                      |
// | Secure Simple Pairing Auto Notifiers |
// v                                      v
//	
	
NONSHARABLE_CLASS(CSSPNumericComparisionAutoNotifier)
	: public CBase
	, public MEikSrvNotifierBase2
	{
public:
	static CSSPNumericComparisionAutoNotifier* NewL();

private: // from MEikSrvNotifierBase2
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	TPtrC8 StartL( const TDesC8& aBuffer );
	void StartL( const TDesC8& aBuffer, TInt aReplySlot,  const RMessagePtr2& aMessage );
	/**
	 * Update a currently active notifier with data aBuffer.
	 * This is called as a result of a client-side call to the asynchronous 
	 * function RNotifier::UpdateNotifierAndGetResponse(). 
	 * @see MEikSrvNotifierBase2::UpdateL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	 */
	void UpdateL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	/**
	 * Update a currently active notifier with data aBuffer.
	 * This is called as a result of a client-side call to the synchronous function RNotifier::UpdateNotifier().
	 * @see MEikSrvNotifierBase2::UpdateL(const TDesC8& aBuffer)
	 */
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	
	void Cancel();

private:
	CSSPNumericComparisionAutoNotifier();
	~CSSPNumericComparisionAutoNotifier();
	
	void SSPNumericComparisonComplete(TInt aReason);	

private:	
	RMessagePtr2	iMessage;
	TInt			iReplySlot;
	TNotifierInfo	iInfo;
	};
	
	

NONSHARABLE_CLASS(CSSPPassKeyAutoNotifier)
	: public CBase
	, public MEikSrvNotifierBase2
	{
public:
	static CSSPPassKeyAutoNotifier* NewL();

private: // from MEikSrvNotifierBase2
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	TPtrC8 StartL( const TDesC8& aBuffer );
	void StartL( const TDesC8& aBuffer, TInt aReplySlot,  const RMessagePtr2& aMessage );	
	/**
	 * Update a currently active notifier with data aBuffer.
	 * This is called as a result of a client-side call to the asynchronous 
	 * function RNotifier::UpdateNotifierAndGetResponse(). 
	 * @see MEikSrvNotifierBase2::UpdateL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	 */
	void UpdateL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	/**
	 * Update a currently active notifier with data aBuffer.
	 * This is called as a result of a client-side call to the synchronous function RNotifier::UpdateNotifier().
	 * @see MEikSrvNotifierBase2::UpdateL(const TDesC8& aBuffer)
	 */
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	void Cancel();

private:
	CSSPPassKeyAutoNotifier();
	~CSSPPassKeyAutoNotifier();
	
private:	
	RMessagePtr2	iMessage;
	TInt			iReplySlot;
	TNotifierInfo	iInfo;
	TBool iNeedToCompleteMessage;
	};



NONSHARABLE_CLASS(CBTUIAutoPinCodeNotifier)
	: public CBase
	, public MEikSrvNotifierBase2
	{
public:
	static CBTUIAutoPinCodeNotifier* NewL();

private: //from MEikSrvNotifierBase2
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	TPtrC8 StartL( const TDesC8& aBuffer );
	void StartL( const TDesC8& aBuffer, TInt aReplySlot,  const RMessagePtr2& aMessage );
	void UpdateL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	void Cancel();

private:
	CBTUIAutoPinCodeNotifier();
	~CBTUIAutoPinCodeNotifier();
	
	void PinInputComplete(const TBTPinCode& aPin, TInt aReason);

private:
	RMessagePtr2	iMessage;
	TInt			iReplySlot;
	TNotifierInfo	iInfo;
	};
	
#endif //BTUINOTIFIERS_H

