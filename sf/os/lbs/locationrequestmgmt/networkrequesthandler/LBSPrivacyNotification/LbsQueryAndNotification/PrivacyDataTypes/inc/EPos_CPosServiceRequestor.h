/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPOSSERVICEREQUESTOR_H
#define CPOSSERVICEREQUESTOR_H

#include <e32base.h>
#include <lbs/epos_cposrequestor.h>


/**
This class represents a service requester.

This class inherits from CPosRequestor which contains functions for
reading identification information for the requester.

@see CPosRequestor
@publishedPartner
@deprecated
@capabilities None
*/
class CPosServiceRequestor : public CPosRequestor
    {
    public:
        IMPORT_C static CPosServiceRequestor* NewLC(TRequestorIdFormat aIdFormat,
        											const TDesC& aIdString);

        IMPORT_C static CPosServiceRequestor* NewL(TRequestorIdFormat aIdFormat,
													const TDesC& aIdString);

        IMPORT_C static CPosServiceRequestor* NewLC(TRequestorIdFormat aIdFormat,
        											const TDesC& aIdString,
        											TNetworkType aNetworkType,
        											TRequestType aRequestType);

        IMPORT_C static CPosServiceRequestor* NewL(TRequestorIdFormat aIdFormat,
													const TDesC& aIdString,
													TNetworkType aNetworkType,
													TRequestType aRequestType);

        IMPORT_C virtual ~CPosServiceRequestor();

    protected:
        CPosServiceRequestor(TRequestorIdFormat aIdFormat, 
        					 HBufC* aIdString,
        					 TNetworkType aNetworkType,
        					 TRequestType aRequestType);

    private:

        // By default, prohibit copy constructor
        CPosServiceRequestor( const CPosServiceRequestor& );
        // Prohibit assigment operator
        CPosServiceRequestor& operator= ( const CPosServiceRequestor& );
    };

#endif      // CPOSSERVICEREQUESTOR_H
