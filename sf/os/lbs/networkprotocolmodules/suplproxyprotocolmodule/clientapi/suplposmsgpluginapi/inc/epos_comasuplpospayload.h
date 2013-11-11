/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines the data types related to OMA SUPL
*				 POS Payload
*
*/


#ifndef C_COMASUPLPOSPAYLOAD_H
#define C_COMASUPLPOSPAYLOAD_H



// INCLUDES
#include <e32std.h>
#include <e32def.h>
#include <lbs/epos_comasuplinforequestlist.h>



/**
* COMASuplPosPayload holds SUPL POS Payload
*
*  @since Series 60 3.1u
*/
class COMASuplPosPayload: public COMASuplInfoRequest 
    {
    
    public:  // Constructors and destructor
        
		/**
		* @Description Enumeration for POS Payload Type. Depending on the underlying
		* control plane positioning protocol, POS Payload can be one of TIA 801, RRLP or
		* RRC
		*
		*/
        enum TOMASuplPosPayloadType
		    {
			    ETIA801 = 0,
			    ERRLP   = 1,
			    ERRC    = 2
			    
		    };
			
    	/**
        * @Description Creates a new instance of a COMASuplPosPayload and sets the
        * type in COMASuplInfoRequest to EOMASuplPosPayload
        *
        * @return A pointer to COMASuplPosPayload
        * It leaves with KErrNoMemory if object creation fails        
        */
        IMPORT_C static COMASuplPosPayload* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C ~COMASuplPosPayload();  
	
	public:
	//Enumeration for type of pos payload


    public: // New functions
        
	   /**
	    * @Description SuplPayloadType retrieves type of SUPL POS Payload
	    * @return One of the POS Payload types defined by TOMASuplPosPayloadType
	    */     

        IMPORT_C TOMASuplPosPayloadType SuplPayloadType()const;
        
	   /**
	    * @Description SetSuplPayloadType Sets type of SUPL POS Payload
	    * @param  aPayLoadType One of the POS Payload types defined by 
	    * TOMASuplPosPayloadType
	    */     
        IMPORT_C void SetSuplPayloadType(TOMASuplPosPayloadType aPayLoadType);
        
	   /**
	    * @Description GetPosPayload is invoked to retrieve the TIA-801 or RRLP or 
	    * RRC payload. 
	    * @param aPayload A reference parameter of type HBufC8, which holds the
	    * SUPL POS Payload
	    * @param aPayloadType A refernce parameter of type TOMASuplPosPayloadType
	    * which holds type of POS Payload. The type of the payload depends on the
	    * underlying control plane positioning protocol
	    */     

        
        IMPORT_C void GetPosPayload(HBufC8*& aPayload,TOMASuplPosPayloadType& aPayloadType) const;
        
	   /**
	    * @Description SetPosPayload is invoked to set the type and value for POS Payload.
	    * @param aPayload Parameter of type HBufC8, which holds the SUPL POS Payload
	    * @param aPayloadType Parameter of type TOMASuplPosPayloadType which
	    * determines the type of the POS Payload. The type of the POS Payload depends on
	    * the underlying control plane positioning protocol
	    */            
        IMPORT_C void SetPosPayload(HBufC8* aPayload,TOMASuplPosPayloadType aPayloadType);
        
     	/**
        * @Description CloneL method clones the POS payload object and creates a
        * new POS payload object with the same values. This is a utility method
        * provided by the POS Message plug-in API. When SUPL Framework forwards a
        * POS Payload from the network, POS Message plug-in can optionally use this 
        * method to create a new copy of the POS Payload.
        *
        * @return A pointer of type, COMASuplPosPayload. This pointer points to
        * a new object, which holds the same values as the original POS Payload.
        * This method leaves with KErrNoMemory in case of non-availability
        * of memory for creation of new object.
        * 
        */        
        IMPORT_C COMASuplInfoRequest* CloneL()const;
        
        private:

        /**
        * C++ default constructor.
        */
        COMASuplPosPayload();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

       
    	private:    // Data
        
        //Consists of the PayLoadType
         TOMASuplPosPayloadType iPayloadType;
         
         //Contains the PayLoad itself
         HBufC8* iPayload;
         
     };

#endif      //SUPLPOSPAYLOAD_H   
            
// End of File
