/**
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
* This file define all the connection specific constants
* 
*
*/



/**
 @file ConSpecificDefs.h
*/
#if (!defined CONSPECIFICDEFS_H)
#define CONSPECIFICDEFS_H

_LIT(KTe_ConnectionSectionName,"Connection");
_LIT(KTe_ConnectionName,"ConnName");
_LIT(KTe_ConnectionStopTypeName,"ConnStopType");
_LIT(KTe_ConnStopNormal,"EStopNormal");
_LIT(KTe_ConnStopAuthoritative,"EStopAuthoritative");
_LIT(KTe_ConnectionType,"ConnType");
_LIT(KTe_ConnectionAttachTypeName,"ConnAttachType");
_LIT(KTe_ConnAttachTypeNormal,"EAttachTypeNormal");
_LIT(KTe_ConnAttachTypeMonitor,"EAttachTypeMonitor");

_LIT(KTe_CommDbBearerCSD,"KCommDbBearerCSD");
_LIT(KTe_CommDbBearerWcdma,"KCommDbBearerWcdma");
_LIT(KTe_CommDbBearerLAN,"KCommDbBearerLAN");

//mobility api
_LIT(KTe_MobilityAPISectionName,"MobilityAPI");

//mobility notifications
_LIT(KTe_MobilityNotificationSectionName,"MobilityNotification");

_LIT(KTe_MobilityNotificationPreferredCarrierAvailable,"PreferredCarrierAvailable");
_LIT(KTe_MobilityNotificationNewCarrierActive,"NewCarrierActive");

_LIT(KTe_MobilityNotificationMigrateToPreferredCarrier,"MigrateToPreferredCarrier");
_LIT(KTe_MobilityNotificationIgnorePreferredCarrier,"IgnorePreferredCarrier");
_LIT(KTe_MobilityNotificationNewCarrierAccepted,"NewCarrierAccepted");
_LIT(KTe_MobilityNotificationNewCarrierRejected,"NewCarrierRejected");

#endif //CONSPECIFICDEFS_H

