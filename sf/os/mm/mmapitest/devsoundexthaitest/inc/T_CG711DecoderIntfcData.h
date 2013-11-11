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



#ifndef G711HWDDEVICE
#define G711HWDDEVICE

//User includes
#include "datawrapperbase.h"

//Epoc includes
#include <g711decoderintfc.h>//CG711DecoderIntfc
#include <g711encoderintfc.h>//CG711EncoderIntfc



class CT_CG711DecoderIntfcData : public CDataWrapperBase
    {
    protected:
    	void DestroyData();
    	void DoCmdNewL();
    	void DoCmdDestructor();
        void DoCmdSet(const TTEFSectionName& aSection);

    public: 
     	~CT_CG711DecoderIntfcData();
	     static	CT_CG711DecoderIntfcData* NewL();
         virtual TAny* GetObject();
	     virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	     CT_CG711DecoderIntfcData();

        void DoCmdSetDecoderMode(const TTEFSectionName& aSection);
		void DoCmdSetCng(const TTEFSectionName& aSection);
            
    private: 
	    /**
	     * Decoder for G711
	     */
	    CG711DecoderIntfc* iG711Decoder;   
        /**
         * TBool for SetCng
         */
        TBool iCng;                           
    };


#endif /*G711HWDDEVICE*/
