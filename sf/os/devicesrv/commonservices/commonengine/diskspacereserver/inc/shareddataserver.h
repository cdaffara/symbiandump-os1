/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Server class.
*
*
*/


#ifndef SHAREDDATASERVER_H
#define SHAREDDATASERVER_H

// CLASS DECLARATION

/**
* SharedDataServer functionality has been moved to DosServer.
* Class left to maintain binary compatibility.
*/
class CSharedDataServer : public CServer2
    { 

    public:
        /**
        * To maintain binary compatibility.
        */
        class TSignal 
            {
            };
        /**
        * To maintain binary compatibility.
        */
        virtual ~CSharedDataServer();

        /**
        * To maintain binary compatibility.
        */  
        IMPORT_C static TInt ThreadStart( CSharedDataServer::TSignal& aSignal );

    };

#endif

// Enf of File
