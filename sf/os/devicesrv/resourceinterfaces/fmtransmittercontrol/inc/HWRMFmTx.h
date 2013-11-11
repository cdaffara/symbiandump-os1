/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef HWRMFMTX_H
#define HWRMFMTX_H

// INCLUDES

#include <e32base.h> 
#include <hwrm/hwrmfmtxdata.h>

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MHWRMFmTxObserver;

// CLASS DECLARATIONS

/**
* Base class of FM Tx Client API of HWRM
*
* The HWRM FM Tx API provides the ability to control the device's FM radio transmitter
*
*  @publishedPartner
*  @deprecated
*/
class CHWRMFmTx : public CBase
    {
    public:  // Constructors
        
        /**
        * Two-phased constructor for implementation class.
        * Use this method for creating a FM Tx client with callbacks.
        *
        * @param aCallback Pointer to callback instance (NULL if callbacks are 
        *        not required).
        * @return New CHWRMFmTx implementing instance.
        * @exception KErrNotSupported Device doesn't support FM Tx feature.
        *            KErrNoMemory if there is a memory allocation failure. 
        */
        IMPORT_C static CHWRMFmTx* NewL(MHWRMFmTxObserver* aCallback = NULL);
        
        /**
        * Two-phased constructor for implementation class. 
        * Use this method for creating a FM Tx client with callbacks.
        * Leaves instance on cleanup stack.
        *
        * @param aCallback Pointer to callback instance (NULL if callbacks are 
        *        not required).
        * @return New CHWRMFmTx implementing instance.
        * @exception KErrNotSupported Device doesn't support FM Tx feature.
        *            KErrNoMemory if there is a memory allocation failure. 
        */
        IMPORT_C static CHWRMFmTx* NewLC(MHWRMFmTxObserver* aCallback = NULL);

    public: // New functions

        /**
        * Reserves FM Tx resource exclusively for this client.
        * A higher priority client may cause lower priority client reservation to be 
        * temporarily suspended.  If FM Tx is already reserved by a higher or equal 
        * priority application, reserving will still succeed, but reservation is 
        * immediately suspended.
        * The suspended client will not get any notification about suspension.  Any 
        * ongoing commands will be cancelled, when a client’s reservation is suspended.  
        * Only reservation and status query commands may be issued, in the suspended 
        * state.
        *		
        * @param aForceNoCCoeEnv If EFalse, then reservation requires that this
        *        client is on the foreground at the time of reservation and FM Tx
        *        will be automatically released and re-reserved based on
        *        background/foreground status of the this client. This also implies
        *        that CCoeEnv::Static() != NULL is required.
        *        If ETrue, the client will not require CCoeEnv to be present nor
        *        does it automatically reserve/release FM Tx.
        *        Only trusted clients are allowed to set this flag to ETrue.
        *
        * @exception KErrAccessDenied if aForceNoCCoeEnv is ETrue and client is not
    	*            trusted.
        *            KErrBadHandle if aForceNoCCoeEnv is EFalse and no CCoeEnv present.
        *            KErrNotReady if trying to reserve while on background and 
        *            aForceNoCCoeEnv is EFalse.
        *            KErrNoMemory if there is a memory allocation failure. 
    	*/
        virtual void ReserveL(TBool aForceNoCCoeEnv)=0;    	

        /**
        * Releases FM Tx resource, if it was previously reserved for this client.
        * If this client has not reserved FM Tx, then this method does nothing.
        */    	
    	virtual void Release() = 0;
    	
        /**
        * This method retrieves the current FM Tx state. 
        *
        * @return TFmTxState The current FM Tx state. 
        * @see MHWRMFmTxObserver
        */
        virtual TFmTxState Status() const = 0;
    	
        /** 
        * The EnableL method switches the FM transmitter on.  This is a synchronous 
        * method that will block the client application.
        *
        * @param aFrequency The initial transmitter frequency. Defaults to the previous 
        *        frequency setting in Central Repository.
    	* @exception KErrInUse if reserved by another client.
    	*            KErrArgument if frequency is out of range
    	*            KErrNotReady if there is a wired accessory conflict. Some  
    	*            accessories are mutually exclusive with FM Tx. 
        *            KErrNoMemory if there is a memory allocation failure. 
        *            KErrGeneral if there is a hardware error.
		*/
		virtual void EnableL(TInt aFrequency=0) = 0;

        /** 
        * The DisableL method switches the FM transmitter off.  This is a synchronous 
        * method that will block the client application.
        *
    	* @exception KErrInUse if reserved by another client.
    	*            KErrGeneral if there is a hardware error.
		*/
		virtual void DisableL() = 0;

        /** 
        * The Frequency method queries the current FM Tx frequency
        *
        * @return TInt The current FM Tx frequency (kHz).
		*/
        virtual TInt Frequency() const = 0;

		/** 
        * The GetFrequencyRangeL method queries the valid FM frequency range
        *
        * @param aFrequencyRange On return, min. and max. frequencies and step size
        * of the FM range (kHz)
        * @exception KErrInUse if reserved by another client
        *            KErrGeneral if there is a hardware error
		*/
		virtual void GetFrequencyRangeL(TFmTxFrequencyRange& aFrequencyRange) const = 0;

        /**
        * The SetFrequencyL method sets the FM transmission frequency. FM Tx
        * does not need to be enabled. This is a synchronous method 
        * that will block the client application.
        *
        * @param TInt aFrequency The new Tx frequency (Hz)
        * @exception KErrInUse if reserved by another client
        *            KErrArgument if frequency parameter is out of range
        *            KErrGeneral if there is a hardware error
        */
        virtual void SetFrequencyL(TInt aFrequency) = 0;

        /**
        * The GetNextClearFrequency method requests a new clear frequency 
        * suggestion.  This results in a RSSI scan at the hardware level, 
        * starting from the frequency increment after the previous suggested 
        * clear frequency.  This is a synchronous method that will block
        * the client application.
        *
        * @param aClearFrequency  On return, this is set to a new (suggested) 
        * clear frequency.
        * @exception KErrInUse if reserved by another client
        *            KErrNotSupported if RSSI scanning is unsupported
        *            KErrGeneral if there is a hardware error
        *            KErrNotReady if FM Tx is not enabled
        */ 
        virtual void GetNextClearFrequencyL(TInt& aClearFrequency) = 0;

        /**
        * The GetNextClearFrequency method requests a new clear frequency 
        * suggestion.  This results in a RSSI scan at the hardware level, 
        * starting from the frequency increment after the previous suggested 
        * clear frequency.  This is a synchronous method that will block
        * the client application.
        *
        * @param aClearFrequencyArray  On return, this contains 0-KClearFrequencyArrayMax
        * clear frequencies.
        * @param aCount  The number of clear frequencies requested (1-KClearFrequencyArrayMax)
        * On return, this is set to the actual number of clear frequencies found.
        * @exception KErrInUse if reserved by another client
        *            KErrArgument  If aCount is out of range
        *            KErrNotSupported if RSSI scanning is unsupported
        *            KErrGeneral if there is a hardware error
        *            KErrNotReady if FM Tx is not enabled
        */ 
        virtual void GetNextClearFrequencyL(TClearFrequencyArray& aClearFrequencyArray, TUint& aCount) = 0;

        /**
        * The GetNextClearFrequency method requests a new clear frequency 
        * suggestion.  This results in a RSSI scan at the hardware level, 
        * starting from the frequency increment after the previous suggested 
        * clear frequency.  This is an asynchronous method.
        *
        * @param aStatus This will be completed when a clear frequency is found
        * @param aClearFrequency The clear frequency is returned in this parameter,
        * which is only valid when aStatus == KErrNone.
        */ 
        virtual void GetNextClearFrequency(TRequestStatus& aStatus, TInt& aClearFrequency) = 0;

        /**
        * The GetNextClearFrequency method requests a new clear frequency 
        * suggestion.  This results in a RSSI scan at the hardware level, 
        * starting from the frequency increment after the previous suggested 
        * clear frequency.  This is an asynchronous method.
        *
        * @param aStatus This will be completed when a clear frequency is found        
        * @param aClearFrequencyArray  On return, this contains 0-KClearFrequencyArrayMax 
        * clear frequencies.
        * @param aCount  The number of clear frequencies requested (1-KClearFrequencyArrayMax).  
        * On completion, this is set to the actual number of clear frequencies found.
        * @exception KErrArgument if aCount is out of range
        */ 
        virtual void GetNextClearFrequencyL(TRequestStatus& aStatus, TClearFrequencyArray& aClearFrequencyArray, TUint& aCount) = 0;

        /**
        * The CancelGetNextClearFrequency method cancels any outstanding
        * GetNextClearFrequency requests.  If no requests have been made,
        * then this has no effect.  Note that the request may complete,
        * before it can be cancelled.
        */
        virtual void CancelGetNextClearFrequency() = 0;

        /**
        * The SetNextClearFrequency method sets the FM Tx frequency to a
        * new clear frequency suggested.  This is equivalent to calling
        * SetFrequencyL, with a result returned from GetNextClearFrequency.
        * This is a synchronous method that will block the client application.
        *
        * @param aClearFrequency On return, this is set to the new Tx frequency.
        * @exception KErrInUse if reserved by another client.
        *            KErrNotSupported if RSSI scanning is unsupported
        *            KErrGeneral if there is a hardware error.
        *            KErrNotReady if FM Tx is not enabled
        */
        virtual void SetNextClearFrequencyL(TInt& aClearFrequency) = 0;
   
        /**
        * The SetNextClearFrequency method sets the FM Tx frequency to a
        * new clear frequency suggested.  This is equivalent to calling
        * SetFrequencyL, with a result returned from GetNextClearFrequency.
        * This is an asynchronous method.
        *
        * @param aStatus This will be completed when a clear frequency is found
        * @param aClearFrequency The clear frequency is returned in this parameter,
        * which is only valid when aStatus == KErrNone. 
        */
        virtual void SetNextClearFrequency(TRequestStatus& aStatus, TInt& aClearFrequency) = 0;

        /**
        * The CancelSetNextClearFrequency method cancels any outstanding
        * SetNextClearFrequency requests.  If no requests have been made,
        * then this has no effect.  Note that the request may complete,
        * before it can be cancelled.
        */
        virtual void CancelSetNextClearFrequency() = 0;
             
        /**
        * The IsAutoFrequencyEnabled method queries whether auto frequency 
        * mode is enabled.
        *
        * @return TBool ETrue if auto frequency is enabled.
        */
        virtual TBool IsAutoFrequencyEnabled() = 0;
        
        /**
        * The EnableAutoFrequency method enables the FM Tx automatic frequency 
        * tuning mode.  In this mode, the transmitter will continuously scan
        * for interference from other transmitters.  When interference is detected,
        * the FM Tx frequency will change automatically, resulting in a
        * MHWRMFmTxObserver::FmTxFrequencyChanged indication.
        *
        * @exception KErrInUse if reserved by another client.
        *            KErrNotSupported if auto frequency is unsupported
        *            KErrGeneral if there is a hardware error.
        *            KErrNotReady if FM Tx is not enabled
        */
        virtual void EnableAutoFrequencyL() = 0;
        
        /**
        * The DisableAutoFrequency method disables the FM Tx automatic frequency
        * tuning mode.
        *
        * @exception KErrInUse if reserved by another client.
        *            KErrGeneral if there is a hardware error.
        */
        virtual void DisableAutoFrequencyL() = 0;

        /**
        * The GetRdsData method Queries the current FM Tx RDS data.
        * If FM Tx is enabled, then this data is being transmitted.
        *
        * @param aData The current FM Tx RDS data.
        */        
        virtual void GetRdsData(TFmTxRdsData& aData) = 0;
        
        /**
        * The SetRdsPTY method sets the RDS Programme Type to be transmitted.
        *
        * @param aPty The new Programme Type
        *
        * @exception KErrInUse if reserved by another client.
        *            KErrNotSupported if RDS is unsupported
        *            KErrArgument if aPty out of range
        *            KErrGeneral if there is a hardware error.
        *            KErrNotReady if FM Tx is not enabled
        */
        virtual void SetRdsPtyL(TRdsProgrammeType aPty) = 0;
        
        /**
        * The SetRdsPTYN method sets the RDS Programme Type Name to be transmitted.
        *
        * @param aPtyn The new Programme Type Name
        *
        * @exception KErrInUse if reserved by another client.
        *            KErrNotSupported if RDS is unsupported
        *            KErrArgument if non-latin characters are supplied or
        *            programme type name too long
        *            KErrGeneral if there is a hardware error.
        *            KErrNotReady if FM Tx is not enabled
        */
        virtual void SetRdsPtynL(TRdsProgrammeTypeName& aPtyn) = 0;
        
        /**
        * The SetRdsMS method sets the RDS Music/Speech flag
        *
        * @param aMusic ETrue if transmitting music.  EFalse if transmitting speech.
        *
        * @exception KErrInUse if reserved by another client.
        *            KErrNotSupported if RDS is unsupported
        *            KErrGeneral if there is a hardware error.
        *            KErrNotReady if FM Tx is not enabled
        */
        virtual void SetRdsMsL(TBool aMusic) = 0;
        
        /**
        * The SetRdsLanguageId method sets the RDS language ID.  Only relevant if MS flag
        * is set to Speech.
        *
        * @param aLanguageId The new language ID.
        *
        * @exception KErrInUse if reserved by another client.
        *            KErrNotSupported if RDS is unsupported
        *            KErrArgument if aLanguageId out of range
        *            KErrGeneral if there is a hardware error.
        *            KErrNotReady if FM Tx is not enabled
        */
        virtual void SetRdsLanguageIdL(TRdsLanguageIdType aLanguageId) = 0;

        /**
        * The ClearRdsData method clears any RDS data previously set by clients.
        * This does not include Radiotext or Radiotext+ data.
        */
        virtual void ClearRdsData() = 0;
        
        /**
        * The SetRadiotext method sets the Radiotext to be transmitted.
        *
        * @param aText The new Radiotext data.
        *
        * @exception KErrInUse if reserved by another client.
        *            KErrNotSupported if Radiotext is unsupported
        *            KErrArgument if non-latin characters are supplied
        *            KErrGeneral if there is a hardware error.
        *            KErrNotReady if FM Tx is not enabled
        */
        virtual void SetRadiotextL(TDes& aText) = 0;
        
        /**
        * The SetRadiotextPlus method sets the Radiotext+ to be transmitted.
        *
        * @param aText The new Radiotext+ data.
        * @param aTag The Radiotext+ content type.
        *
        * @exception KErrInUse if reserved by another client.
        *            KErrNotSupported if Radiotext+ is unsupported
        *            KErrArgument if non-latin characters are supplied or 
        * 				if content type not valid.
        *            KErrGeneral if there is a hardware error.
        *            KErrNotReady if FM Tx is not enabled
        */
        virtual void SetRadiotextPlusL(TDes& aText, TRtPlusContentType aContentType) = 0;
        
        /**
        * The SetRadiotextPlus method sets the Radiotext+ to be transmitted.
        *
        * @param aText The new Radiotext+ data.
        * @param aTag A Radiotext+ tag.
        *
        * @exception KErrInUse if reserved by another client.
        *            KErrNotSupported if Radiotext+ is unsupported
        *            KErrArgument if non-latin characters are supplied or 
        * 				 if RT-plus tag content not valid.
        *            KErrGeneral if there is a hardware error.
        *            KErrNotReady if FM Tx is not enabled
        */
        virtual void SetRadiotextPlusL(TDes& aText, TRtPlusTag aTag) = 0;
        
        /**
        * The SetRadiotextPlus method sets the Radiotext+ to be transmitted.
        *
        * @param aText The new Radiotext+ data.
        * @param aTag A Radiotext+ tag.
        * @param aTag A second Radiotext+ tag.
        *
        * @exception KErrInUse if reserved by another client.
        *            KErrNotSupported if Radiotext+ is unsupported
        *            KErrArgument if non-latin characters are supplied or 
        * 				 if RT-plus tag content not valid.
        *            KErrGeneral if there is a hardware error.
        *            KErrNotReady if FM Tx is not enabled
        */
        virtual void SetRadiotextPlusL(TDes& aText, TRtPlusTag aTag1, TRtPlusTag aTag2) = 0;

        /**
        * The ClearRdsData method clears any Radiotext or Radiotext+ data previously 
		* set by clients.
        */
        virtual void ClearRadiotextData() = 0;
    };

/**
*  MHWRMFmTxObserver
*  A client should derive from this abstract class, to receive FM Tx status notifications.
*
* @publishedPartner
* @released
*/
class MHWRMFmTxObserver
    {    
    public:
        
        /** 
        * Called when the device FM Tx status changes.
        *
        * @param aState Indicates current FM Tx state.
        *
	 	* @see TFmTxState
		*/
        virtual void FmTxStatusChanged(TFmTxState aState) = 0;
        
        /** 
        * Called when the device FM Tx frequency changes.
        *
        * @param aFrequency Indicates current FM Tx frequency (Hz).
		*/        
        virtual void FmTxFrequencyChanged(TInt aFrequency) = 0;
	};


#endif // HWRMFMTX_H   
            
// End of File
