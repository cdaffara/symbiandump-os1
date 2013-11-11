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



#ifndef CMMCALLEXTINTERFACE_H
#define CMMCALLEXTINTERFACE_H

//  INCLUDES
#include <et_tsy.h>

#include "MmTsy_Csd_Defs.h"

#include "ctsysystemstatepluginhandler.h"

// FORWARD DECLARATIONS
class CMmDataPackage;

// CLASS DECLARATION

/**
 *  CMmCallExtInterface is used to provide an extension interface for system 
 *  specific call object extensions.
 */
class CMmCallExtInterface : public CBase
    {
    public: // Constructors and destructor
       
        //none

    public: // New functions
        
        /**
         * Returns extension specific data call caps.
         *         
         *
         * @param aCallDataCapsV1: a reference to the parameter object to be filled
         */
        virtual void GetMobileDataCallCapsV1( 
            RMobileCall::TMobileCallDataCapsV1& /*aCallDataCapsV1*/ ) = 0;

        /**
         * Received extension specific data call capabilities.
         *         
         *
         * @param aCaps New data call capabilities
         */
        virtual void ReceivedMobileDataCallCaps(
            RMobileCall::TMobileCallDataCapsV1* /*aCaps*/ )= 0;

        /**
         * Returns call parameters
         *         
         *
         * @param aParams 
         */
        virtual void GetCallParams( TDes8* /*aParams*/ ) = 0;

        /**
         * Extension specific bearer service info.
         *         
         *
         * @param RCall::TBearerService* aBearerService
         * @return KErrNotSupported
         */
        virtual TInt GetBearerServiceInfo( 
            RCall::TBearerService* /*aBearerService*/ ) = 0;

        /**
         * Extension specific dynamic HSCSD parameters.
         *         
         *
         * @param RMobileCall::TMobileCallAiur* aAiur
         * @param TInt* aRxTimeslots
         * @return KErrNotSupported
         */
        virtual TInt SetDynamicHscsdParamsL( 
            RMobileCall::TMobileCallAiur* /*aAiur*/, 
            TInt* /*aRxTimeslots*/ ) = 0;


        /**
         * Extension specific HSCSD info.
         *         
         *
         * @param TDes8* aHSCSDInfo
         * @return KErrNotSupported
         */
        virtual TInt GetCurrentHscsdInfo( TDes8* /*aHSCSDInfo*/ )= 0;

        /**
         * New MO call is created using Dial method.
         *         
         *
         * @param TUint8 aCallMode
         * @param const TDesC8* aCallParams
         * @param const TDesC* aNumber
         * @param const TInt aExtensionId
         * @return KErrNotSupported
         */
        virtual TInt DialL( RMobilePhone::TMobileService /*aCallMode*/, 
            const TDesC8* /*aCallParams*/, 
            const TDesC* /*aNumber*/,
            TInt /*aExtensionId*/ )= 0;

        /**
         * Extension specific dial for data calls.
         *         
         *
         * @param RMobilePhone::TMobileService aCallMode
         * @param const TDesC8* aCallParams
         * @param TDesC* aTelNumber
         * @return KErrNotSupported
         */
        virtual TInt DialDataCallL( 
            RMobilePhone::TMobileService /*aCallMode*/,
            const TDesC8* /*aCallParams*/,
            const TDesC* /*aTelNumber*/ ) = 0;

        /**
         * Emergency call is created using this method
         *         
         *
         * @param aCallbackData
         * @return success/failure value
         */
        virtual TInt DialEmergencyCall(TCtsySsmCallbackData& /*aCallbackData*/ ) = 0;
        /**
         * Extension specific answer incoming call.
         *         
         *
         * @param TInt aCallId
         * @param TInt aExtensionId
         * @return KErrNotSupported
         */
	    virtual TInt AnswerIncomingCallL(
            TInt /*aCallId*/, TInt aExtensionId ) = 0;

        /**
         * Extension specific answer for incoming data calls.
         *         
         *
         * @param TInt aCallId
         * @return KErrNotSupported
         */
        virtual TInt AnswerIncomingDataCallL( TInt /*aCallId*/ ) = 0;

        /**
         * Sets the call on hold
         *         
         *
         * @param TInt: call id
         * @return KErrNotSupported
         */
        virtual TInt HoldL( TInt /*aCallId*/ ) = 0;

        /**
         * This method is used to resume a held call
         *
         * @param TInt: call id
         * @return TInt: KErrNotSupported
         */
        virtual TInt ResumeL( TInt /*aCallId*/ ) = 0;

        /**
         * This method is used to swap between active and held call
         *         
         *
         * @param TInt: call id
         * @return KErrNotSupported
         */
        virtual TInt SwapL( TInt /*aCallId*/ ) = 0;

        /**
         * Deflects the call to the given number
         *         
         *
         * @param TMobileCallDeflect*: deflect type
         * @param TMobileAddress*: given number
         * @return KErrNotSupported
         */
        virtual TInt DeflectL(  const TDataPackage& /*aData*/ ) = 0;

        /**
         * Inform extension dll about the current status.
         *         
         *
         * @param TMobileCallStatus: new call status
         * @return KErrNotSupported
         */
        virtual TInt CompleteNotifyStatusChange( 
             RMobileCall::TMobileCallStatus /*aCallStatus*/ ) = 0;

        /**
         * Default implementation of the GetMobileCallCaps, returns KErrNotFound
         * if method has not been defined in the extension
         *         
         *
         * @param aCallCapsV1: a reference to the parameter object to be filled
         */
         virtual void GetMobileCallCapsV1( 
            RMobileCall::TMobileCallCapsV1& /*aCallCapsV1*/ ) = 0;


        /**
         * Default implementation of the Transfer-method
         *         
         *
         * @return KErrNotSupported
         */
        virtual TInt TransferL() = 0;

        /**
         * Default implementation of the GoOneToOneL-method
         *         
         *
         * @param TInt aCallId: Call ID
         * @return KErrNotSupported
         */
        virtual TInt GoOneToOneL( TInt /*aCallId*/ ) = 0;

        /**
         * Extension specific reset internal attributes.
         *         
         *
         */
        virtual void ResetInternalAttributes() = 0;

        /**
         * Extension specific received call params.
         *         
         *
         * @param const CMmDataPackage* aDataPackage: call parameters
         */
        virtual void ReceivedCallParams(
            const CMmDataPackage* /*aDataPackage*/ ) = 0;

        /**
         * Extension specific received bearer service info.
         *         
         *
         * @param RCall::TBearerService aBearerService
         */
        virtual void ReceivedBearerServiceInfo( 
            RCall::TBearerService /*aBearerService*/ ) = 0;

        /**
         * Extension specific connect.
         *         
         *
         * @param const TDesC8* aCallParams
         */
        virtual void Connect( const TDesC8* /*aCallParams*/ ) = 0;

        /**
         * Default implementation of the FillMobileCallInfo,
         * returns KErrNotFound if method has not been defined in the extension
         *         
         *
         * @param RMobileCall::TMobileCallInfoV1* aInfoV1: mobile call info
         * @return KErrNotSupported
         */
        virtual TInt FillMobileCallInfo( 
            RMobileCall::TMobileCallInfoV1* /*aInfoV1*/ ) = 0;

        /**
         * Default implementation of the FillMobileCallInfoV3,
         * returns KErrNotFound if method has not been defined in the extension
         *         
         *
         * @param RMobileCall::TMobileCallInfoV3* aInfoV3: mobile call info
         * @return KErrNotSupported
         */         
         virtual TInt FillMobileCallInfoV3(  RMobileCall::TMobileCallInfoV3* /*aInfoV3*/ ) = 0;
        
         /**
         * Default implementation of the FillMobileCallInfoV7,
         * returns KErrNotFound if method has not been defined in the extension
         *         
         *
         * @param RMobileCall::TMobileCallInfoV7* aInfoV7: mobile call info
         * @return KErrNotSupported
         */
         virtual TInt FillMobileCallInfoV7(  RMobileCall::TMobileCallInfoV7* /*aInfoV7*/ ) = 0;

         /**
          * Default implementation of the FillMobileCallInfoV8,
          * returns KErrNotFound if method has not been defined in the extension
          *         
          *
          * @param RMobileCall::TMobileCallInfoV8* aInfoV8: mobile call info
          * @return KErrNotSupported
          */
        virtual TInt FillMobileCallInfoV8( RMobileCall::TMobileCallInfoV8* /*aInfoV8*/ ) = 0;
      
        /**
         * Default implementation of the GetRemotePartyInfo,
         * returns KErrNotFound if method has not been defined in the extension
         *         
         *
         * @param RMobileCall::TMobileCallRemotePartyInfoV1Pckg* aInfoV1Pckg: 
         * remote party information
         * @return KErrNotSupported
         */
        virtual TInt GetRemotePartyInfo( 
            RMobileCall::TMobileCallRemotePartyInfoV1Pckg* /*aInfoV1Pckg*/ ) = 0;

        /**
         * Extension specific set mobile call data
         *         
         *
         * @param aDataPackage: New mobile call data
         * @param aCallDirection New call direction
         */
        virtual void SetMobileCallData( CMmDataPackage* /*aDataPackage*/,
            RMobileCall::TMobileCallDirection /*aCallDirection*/ ) = 0;

        /**
         * Extension specific received HSCSD info.
         *         
         *
         * @param CMmDataPackage* aDataPackage
         */
        virtual void ReceivedHscsdInfo( 
            CMmDataPackage* /*aDataPackage*/ ) = 0;

        /**
         * Received extension specific data call capabilities.
         *         
         *
         * @param CMmDataPackage* aDataPackage
         */
        virtual void ReceivedMobileDataCallCaps(
            CMmDataPackage* /*aDataPackage*/ ) = 0;

        /**
         * Adds requested caps to GSM specific mobile 
         * call capabilities.
         *         
         *
         * @param aCaps Call capabilities to be added
         */
        virtual void AddGSMCallCaps( TUint32 /*aCaps*/ ) = 0;

        /**
         * Removes requested caps from GSM specific 
         * mobile call capabilities.
         *         
         *
         * @param aCaps Call capabilities to be removed
         */
        virtual void RemoveGSMCallCaps( TUint32 /*aCaps*/ ) = 0;

    };

#endif // CMMCALLEXTINTERFACE_H   
            
// End of File
