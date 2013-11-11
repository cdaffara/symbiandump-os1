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



#ifndef NETWORKPRIVACYPANICS_H
#define NETWORKPRIVACYPANICS_H

#include <e32def.h>

/**
Panic codes used with the Network Privacy API
@publishedPartner
@released
*/
enum TPosNetworkPrivacyPanic
    {
    /*
    If CPosGSMPrivacyRequestInfo::SetLCSClientL() or CPosGSMPrivacyRequestInfo::SetRequestorL()
    are called with a requester ID format not defined by a value from CPosGSMPrivacyRequestInfo::TIdType
    */
    EPosNetworkPrivacyInvalidGSMIdType = 0,
    /*
    A negative connection timeout was set in CPosNetworkPrivacy::SetConnectionTimeout()
    */
    EPosNetworkPrivacyInvalidConnectionTimeout,
    /*
    An invalid argument was passed in CPosNetworkPrivacy::NotifyVerificationTimeoutL()
    */
    EPosNetworkPrivacyInvalidArgument,
    /*
    Not supported
    */
    EPosNetworkPrivacyInvalidCDMAIdType,
    /*
    If CPosSUPLPrivacyRequestInfo::SetLCSClientL() or CPosSUPLPrivacyRequestInfo::SetRequestorL()
    are called with a requester ID format not defined by a value from CPosSUPLPrivacyRequestInfo::TIdType
    */
    EPosNetworkPrivacyInvalidSUPLIdType,
    /*
    If CPosNetworkPrivacyRequestInfo::SetRequestType() is called with a request type not defined 
    by a value from the Enum CPosNetworkPrivacyRequestInfo::_TRequestType
    */
    EPosNetworkPrivacyInvalidRequestType
    };


#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
GLDEF_C void Panic(TPosNetworkPrivacyPanic aPanic);
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS


#endif      // NETWORKPRIVACYPANICS_H

// End of File
