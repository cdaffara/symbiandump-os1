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



/**
 @file
 @publishedAll
*/

#ifndef DATAMONITORMESSAGES_INL
#define DATAMONITORMESSAGES_INL

/*
	CRequestDataSentNotificationReqMsg
*/
TRIVIAL_ACCESSOR(TRequestDataSentNotification, TUint32, Delta)
TRIVIAL_ACCESSOR(TRequestDataSentNotification, TUint32, SentBytes)

/*
	CRequestDataReceivedNotificationReqMsg
*/
TRIVIAL_ACCESSOR(TRequestDataReceivedNotification, TUint32, Delta)
TRIVIAL_ACCESSOR(TRequestDataReceivedNotification, TUint32, ReceivedBytes)

#endif   // DATAMONITORMESSAGES_INL

