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
* Name        : CBerTlv.h
* Part of     : Series 60 / utility
* Based on 3GPP TS 11.14 V8.8.0 
* Version     : 1.0
*
*/



//  INCLUDES
#ifndef CBERTLV_H
#define CBERTLV_H

#include <cstktsy/CTlv.h>				    // For class CTlvBase
#include <cstktsy/bertlv_defs.h>			// BerTlv constants

// DESCRIPTION
// CBerTlv contains methods to read BER-TLV data

class CBerTlv  : public CTlvBase
    {

    public: // Constructor

        /**
        * Default constructor
        */
        IMPORT_C CBerTlv();

    public: // Methods

        /**
        * Find TLV by tag value
        * @param aTlv: 
        * @param aTlvTag: TlvTag
        * @param aItemNbr: number of requested TLV, default is 0
        * @return TInt: KErrNone or KErrNotFound
        */
        IMPORT_C TInt TlvByTagValue( CTlv* aTlv, TUint8 aTlvTag, 
            TInt aItemNbr = 0 );
            
        /**
        * Find several TLVs by tag value
        * @param aSubBlock: retruns TLVs
        * @param aSubBlockId: TlvTag
        * @return TInt: KErrNone or KErrNotFound
        */
        IMPORT_C TInt TlvByTagValueMulti( CTlv* aTlv, TUint8 aTlvTag );	
    };


#endif // CBERTLV_H

// End of File
