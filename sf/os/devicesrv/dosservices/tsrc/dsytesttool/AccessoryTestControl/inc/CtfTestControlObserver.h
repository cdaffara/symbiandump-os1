/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef __CTFTESTCONTROLOBSERVER_H__
#define __CTFTESTCONTROLOBSERVER_H__


#include "CtfAccessoryTestControl.h"
#include "TfAccessoryTestControlTypes.h"

#include <AccessoryMode.h>
#include <AccessorySettings.h>
#include <AccessoryConnection.h>
#include <AccessoryControl.h>
#include <AccessoryAudioControl.h>
#include <AccessoryBTControl.h>
#include <AccessorySingleConnection.h>

#include <AccPolGenericID.h>
#include <AccPolGenericIDarray.h>
#include <AccConfigFileParser.h>
#include <AccPolCommonNameValuePairs.h>
#include <AccConfigFileParser.h>

class CTFAccessoryTestControl;

class CTFTestControlObserver : public CActive
    {
    public:  
        
        //CTFAccessoryTestControl aTestControl
		static CTFTestControlObserver* NewL(CTFAccessoryTestControl& aTestControl); 
               		
        CTFTestControlObserver(CTFAccessoryTestControl& aTestControl);                       
        	
        ~CTFTestControlObserver();                       
        
        //
        // Accessory Connction
        //
        void NotifyNewAccessoryConnected(TAccPolGenericID& aGenericId);
                
        void NotifyAccessoryDisconnected(TAccPolGenericID& aGenericId);
        
        void NotifyBooleanAccessoryValueChanged(TAccPolGenericID& aGenericId, const TUint32& aName);
        
        void NotifyIntegerAccessoryValueChanged(TAccPolGenericID& aGenericId, const TUint32& aName);

        void NotifyObjectAccessoryValueChanged(TAccPolGenericID& aGenericId, const TUint32& aName);

		void GetAccessoryConnectionStatus(TAccPolGenericIDArray& aGenericIdArray);                                
		//
		// Accessory Mode
		//                                
        void NotifyAccessoryModeChanged();   
        
        void GetAccessoryMode();
						
 		//
 		// Accessory Bluetooth Control
 		//               
        void NotifyBluetoothAudioLinkOpenReq(); 			           

		void NotifyBluetoothAudioLinkCloseReq(); 			  
		
		void ConnectBTAccessory(TBTDevAddr& btaddr); 
		
		void DisconnectBTAccessory(TBTDevAddr& btaddr);						           
		
		// 
		// Accessory Audio Control
		//		
		void AccessoryAudioLinkOpen( TAccPolGenericID& aGenericI, TUint32 aAudioType );   
		
		void NotifyAccessoryAudioLinkOpened(TAccPolGenericID& aGenericId);
				
		void NotifyAccessoryAudioLinkClosed(TAccPolGenericID& aGenericId);
		
		void AccessoryAudioLinkClose(TAccPolGenericID& aGenericId, TUint32 aAudioType );
		
		void NotifyAccessoryConnectionStatusChanged(TAccPolGenericIDArray& aGenericIdArray);
		
		// Accessory Control
		
		void ConnectAccessory(TAccPolGenericID& aGenericId, TUint64 aHWDeviceID);
		
		void DisconnectAccessory(TAccPolGenericID& aGenericId);

		// Support functions
		TBool FindRequest(TTFRequestType aRequestType);		
		
    public: // 

		static const TInt iOffset;
            
    protected:  // Functions from base classes       
        void DoCancel();
	    void RunL();
	    TInt RunError( TInt aError );

	private:
	
		void SetRequestType( TTFRequestType aRequestType) {iRequestType=aRequestType;}
	private:
      
      	friend class 					CTFAccessoryTestControl;
      	TSglQueLink iSlink;
      	TAccPolAccessoryMode 			iAccMode;	
      	CTFAccessoryTestControl&	 	iAccessoryTestControl;
        TTFRequestType			 		iRequestType;                
        TTFRequestType			 		iRequestCompleted;                
        TInt64							iValue;
        TUint32                         iCapabilityName;
        TAccValueTypeTBool	 			iBooleanValue;
        TAccValueTypeTInt 				iIntegerValue;
        TAccValueTypeObject             iObjectValue;
        TAccAudioType					iAudioType;
        TBTDevAddr                      iBTaddr;
        HBufC8*                         iObjectValueBuf;
    };

#endif //__CTFTESTCONTROLOBSERVER_H__
