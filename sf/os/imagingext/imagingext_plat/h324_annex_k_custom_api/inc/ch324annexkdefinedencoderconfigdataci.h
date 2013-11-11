/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  user defined encoder init data custom interface
*
*/


/**
* @Example of the CI usage:
* 1. Create CI. 
* MH324AnnexKDefinedEncoderConfigDataCI* userConfigCI = NULL;
* userConfigCI = (MH324AnnexKDefinedEncoderConfigDataCI*)iDevvrInstance->CustomInterface( hwdevUid, H324AnnexKDefinedEncoderConfigDataCIUid );
* 
* CDEVVRVideoRecord iDevvrInstance    - The inctance of the DeviceVideoRecord;
* TUid hwdevUid                       - Encoder HwDevice Uid;
* H324AnnexKDefinedEncoderConfigDataCIUid  - Custom Interface Uid;
* 
*
* 2. TInt status = userConfigCI->H324AnnexKDefinedEncoderConfigDataOn();
*
*    if ( status != KErrNone )
*        {
*        // handle error
*        }
*
* 3. This CI API H324AnnexKDefinedEncoderConfigDataOn() can only be called before the initializing phase (Before Initialize() method is called by DevVideo). 
*
*/

#ifndef __CH324ANNEXKDEFINEDENCODERCONFIGDATACI_H
#define __CH324ANNEXKDEFINEDENCODERCONFIGDATACI_H


// CONSTANTS
const TUid H324AnnexKDefinedEncoderConfigDataCIUid =   {0x10204C0B};  // Custom Interface UId


// CLASS DECLARATION
/**
*  User defined encoder confguration data custom interface
*
*/
class MH324AnnexKDefinedEncoderConfigDataCI
    {
    public:
        
        /**
        * Enables inserting H324 pre-defind config data (VOL / SPS PPS / etc. )
        * @param none
        * @return KErrNone - Success, otherwise KErrGeneral - Error, unable to process operation
        */
        virtual TInt H324AnnexKDefinedEncoderConfigDataOn() = 0;

    };


#endif  // __CH324ANNEXKDEFINEDENCODERCONFIGDATACI_H
