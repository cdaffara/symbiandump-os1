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



#ifndef T_CERRORCONCEALMENTINTFCDATA_H
#define T_CERRORCONCEALMENTINTFCDATA_H

//User includes
#include "datawrapperbase.h"

//Epoc includes
#include <ErrorConcealmentIntfc.h>//CErrorConcealmentIntfc


class CT_CErrorConcealmentIntfcData : public CDataWrapperBase
    {
    public:
    	~CT_CErrorConcealmentIntfcData();
    	static	CT_CErrorConcealmentIntfcData* NewL();
        virtual TAny* GetObject();
        virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	    CT_CErrorConcealmentIntfcData();
	    
    private:
    	void DestroyData();
    	
    protected:
    	void DoCmdNewL();
    	void DoCmdDestructor();
        void DoCmdConcealErrorForNextBuffer(const TTEFSectionName& aSection);
		void DoCmdSetFrameMode(const TTEFSectionName& aSection);
		void DoCmdFrameModeRqrdForEC(const TTEFSectionName& aSection);
		
            
    private:
    /**
     * Wrapped object
     */
    	CErrorConcealmentIntfc* iErrorConcealmentIntfc;
    };

#endif /*T_CERRORCONCEALMENTINTFCDATA_H*/
