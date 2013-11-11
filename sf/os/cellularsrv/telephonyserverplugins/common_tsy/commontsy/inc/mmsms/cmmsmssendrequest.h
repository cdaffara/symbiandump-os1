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



#ifndef CMMSMSSENDREQUEST_H
#define CMMSMSSENDREQUEST_H

// INCLUDES
#include <etelmm.h>
#include <ctsy/serviceapi/cmmsmsutility.h>

// CLASS DECLARATION

/**
*  CSmsSendRequest class, contains SMS send request to the SMS server.
*/
NONSHARABLE_CLASS( CSmsSendRequest ) : public CBase
    {
    public:
        
        /**
         * C++ default constructor.
         */
        CSmsSendRequest();
        
        /**
         * Destructor.
         */
        ~CSmsSendRequest();
        
        /**
         * Gets SMS data and attributes
         *          
         * 
         * @return TSendSmsDataAndAttributes
         */
    	TSendSmsDataAndAttributes GetSmsDataAndAttributes() const;
        
        /**
         * Sets SMS data and attributes
         *          
         * 
         * @param aSmsDataAndAttributes
         */
    	void SetSmsDataAndAttributes( 
    	    TSendSmsDataAndAttributes const& aSmsDataAndAttributes );
        
        /**
         * Increase send counter
         *          
         * 
         */
    	void IncreaseSendCounter();
    	
    	/**
         * Gets send counter
         *          
         * 
         * @return TUint8
         */
    	TUint8 GetSendCounter() const;

    private:        

        /*
         * SMS data and attributes
         */
        TSendSmsDataAndAttributes iSmsDataAndAttributes;

        /*
         * Send counter
         */
        TUint8 iSendCounter;
        
    };


#endif  // CMMSMSSENDREQUEST_H

// End of File
