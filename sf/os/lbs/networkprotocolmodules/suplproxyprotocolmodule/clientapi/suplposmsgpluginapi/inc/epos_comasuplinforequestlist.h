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
* Description:  Provides the Data types for exchange of data between
*   			 SUPL Framework and POS Message plug-in
*
*/


#ifndef C_COMASUPLINFOREQUESTLIST_H
#define C_COMASUPLINFOREQUESTLIST_H


// INCLUDES
#include <e32std.h>
#include <e32def.h>
#include <e32base.h>

// Forward Declaration
class COMASuplInfoRequest;

/**
*  A container class that holds data exchanged between SUPL Framework and
*  SUPL POS Message plug-in. This class holds a list of COMASuplInfoRequest
*  objects. Each of the COMASuplInfoRequest objects in the list represent 
*  a data type that is exchanged between the POS Message plug-in and SUPL Framework.
*  There can be one and only one object of a particular data type in COMASuplInfoRequestList.
*  SUPL Framework allocates memory to each of the COMASuplInfoRequest objects and
*  the POS Message plug-in fills up the values depending on the data type held by
*  the COMASuplInfoRequest object.
*
*  @since Series 60 3.1u
*/
class COMASuplInfoRequestList : public CBase
    {
    
       public:  // Constructors and destructor
        
      	/**
        * @Description Creates a new instance of a COMASuplInfoRequestList.
        *
        * @return A pointer to COMASuplInfoRequestList.
        * It leaves with KErrNoMemory if object creation fails
        */
        IMPORT_C static COMASuplInfoRequestList* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C ~COMASuplInfoRequestList();
        
    	
    	 /**
        * C++ default constructor.
        */
        IMPORT_C COMASuplInfoRequestList();
   
   
    public: // New functions
        
   		/**
        * @Description Appends an object tof type COMASuplInfoRequest to
        * the end of COMASuplInfoRequestList.
 			  * @param aInfoRequest is a pointer to an object of type COMASuplInfoRequest. 
		    * The object held by this parameter should be one of the derived types.
		    * @return KErrNone if operation is successful, otherwise one of the Symbian system
		    * wide errors
        */  
        IMPORT_C TInt Append(COMASuplInfoRequest *aInfoRequest);
       
   		/**
        * @Description Provides the count of the number of COMASuplInfoRequest
        * objects held by COMASuplInfoRequestList.
        * @return An integer that provides the count
        */  
        IMPORT_C TInt GetCount();
        
		/**
        * @Description Provides a COMASuplInfoRequest element in the list given
        * the index
			  * @param aIndex Index of the element
        * @return A pointer to COMASuplInfoRequest if aIndex is well within range. NULL if aIndex is out of range
        */
        IMPORT_C COMASuplInfoRequest *GetElement(TInt aIndex);
        
   private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

 
    private:    // Data
        // Type of request info
        RPointerArray <COMASuplInfoRequest> iInfoRequestList;
        
    };

/**
*  Base class for different objects exchanged between SUPL Framework and
*  SUPL POS Message plug-in. This class holds information about the 
*  type of the data that is provided by POS Message plug-in to SUPL Framework.
*  It also provides the status of retrieval of information from POS Messahe plug-in.
*  SUPL Framework creates an object of one of the derived types and requests for 
*  data from POS Message plug-in. If POS Message plug-in is unable to provide
*  the information that SUPL Framework is requesting, it sets the status field to
*  reflect the problem.
*
*  @since Series 60 3.1u
*/
class COMASuplInfoRequest : public CBase
    {
    
       public:  // Constructors and destructor

      	/**
        * @Description Enumeration for the type of object
        */
      
       enum TOMASuplInfoType
       	{
       		EOMASuplReqAsstData = 0,
       		EOMASuplVelocity,
       		EOMASuplSETCapabilities,
       		EOMASuplPosPayload,
       		EOMASuplPosition  		
       		
       	};
   
        
      	/**
        * @Description Creates a new instance of a COMASuplInfoRequest.
        *
        * @return A pointer to COMASuplInfoRequest
        * It leaves with KErrNoMemory if object creation fails        
        */
        IMPORT_C static COMASuplInfoRequest* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C ~COMASuplInfoRequest();
        
    	
    	 /**
        * C++ default constructor.
        */
        IMPORT_C COMASuplInfoRequest();
        	
       public:
       
   
   
    public: // New functions
        
        /**
        * @Description Returns the type of COMASuplInfoRequest
        * @return TOMASuplInfoType indicating the type of information
        * held by COMASuplInfoRequest
        */
     
        IMPORT_C TOMASuplInfoType Type()const;
       
        /**
        * @Description Provides the status of data fetch operation performed
        * by SUPL Framework. The status is set by the POS Message plugin for
        * each of the data types.
        * @return 
        * - KErrNone if all the fields are successfully filled up
        * by POS Message plug-in. 
        * - KErrOMASuplPosInfo if POS Message plug-in
        * cannot provide the information that SUPL Framework is requesting
        * for. 
        * - KErrOMASuplParamNotSet if SUPL Framework is trying to fetch
        * an optional parameter that is not set by POS Message plug-in
        */
        IMPORT_C TInt Status()const;
        
		/**
        * @Description Sets the type of object. This method is used
        * during the construction of the derived type
        * @param aInfoType Type of info to be set
        */
        IMPORT_C void SetType(TOMASuplInfoType aInfoType);
        
        /**
        * @Description This method is used by the POS Message plug-in
        * to set the status of get operation on the particular object
        * @param aStatus Status to be set. Can be one of the following values 
        * - KErrNone if all the fields are successfully filled up
        * by POS Message plug-in. 
        * - KErrOMASuplPosInfo if POS Message plug-in
        * cannot provide the information that SUPL Framework is requesting
        * for. 
        * - KErrOMASuplParamNotSet if SUPL Framework is trying to fetch
        * an optional parameter that is not set by POS Message plug-in
        * @return None 
        */
        IMPORT_C void SetStatus(TInt aStatus);
        
     	/**
        * @Description CloneL method clones the info request object and creates a
        * new info request object with the same values. SUPL Framework invokes this method
        * to create a copy of the info request information.
        *
        * @return COMASuplInfoRequest object pointer pointing to the new requested assistance
        * data object created.
        * This method leaves with KErrNoMemory in case of non-availability
        * of memory
        * 
        */

     	IMPORT_C virtual COMASuplInfoRequest* CloneL()const;
     	
    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

 
    private:    // Data
        // Type of request info
        TOMASuplInfoType iType;
        
        //Status
        TInt iReqStatus;
    };

#endif //SUPLINFOREQUESTLIST_H
