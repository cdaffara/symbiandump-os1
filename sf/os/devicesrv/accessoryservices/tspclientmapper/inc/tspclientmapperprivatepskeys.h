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
* Description:  This API is used for creating mappings between remote
*                 control clients.
*
*/


#ifndef TSPCLIENTMAPPERPRIVATEPSKEYS_H
#define TSPCLIENTMAPPERPRIVATEPSKEYS_H

// INCLUDES

/**
* A P&S key for containing information about registered remote control clients.
*/
const TUid KTspClientMapperProperty = { 0x10200C70 }; // Category

/**
* KTspClientMapperKeyPlaying
* Contains a list of PIDs of all remote control clients that have registered to
* this api as playing.
*/
const TUint32 KTspClientMapperKeyPlaying = 0x00000001;

/**
* KTspClientMapperKeyRegistered
* Contains a list of PIDs of all remote control clients that have registered to
* this api as registered.
*/
const TUint32 KTspClientMapperKeyRegistered = 0x00000002;

/**
* KTspClientMapperKeyStopped
* Contains a list of PIDs of all remote control clients that have registered to
* this api as stopped.
*/
const TUint32 KTspClientMapperKeyStopped = 0x00000003;


#endif // TSPCLIENTMAPPERPRIVATEPSKEYS_H

// End of File
