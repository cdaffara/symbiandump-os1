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
* Description:  This class provides the base from which the Session object
*   			 in the protocol handler has to be derived.This is an
*   			 abstract class and has to be implemented by the POS Message plug-in
*
*/


#ifndef C_COMASUPLPOSHANDLERBASE_H
#define C_COMASUPLPOSHANDLERBASE_H

// INCLUDES
#include <e32def.h>
#include <e32std.h>
#include <e32base.h>

const TUid KPosHandlerInterfaceDefinitionUid = {0x102073D4};

class MOMASuplObserver;
class COMASuplPosSessionBase;


/**
*  Base class for OMA SUPL POS Handler. This is an abstract class that has to be implemented
*  by the POS Message plug-in. POS Message plug-in provides the licensee implementation
*  that provides POS messages to be transferred over the secure IP connection established 
*  by SUPL Framework
*
*  @since Series 60 3.1u
*/
class COMASuplPosHandlerBase: public CBase
    {
    	
	public:  // Constructors and destructor
    
		/**
		* @Description Creates a new instance of a COMASuplPosHandlerBase implementation.
		*
		* @param aImplementationUid Implementation UID of the COMASuplPosHandlerBase.
		* @return A pointer to POS Handler implementation.
		* It leaves with KErrNoMemory if object creation fails        
		*/
		IMPORT_C static COMASuplPosHandlerBase* NewL(
		/* IN */ 		const TUid &aImplementationUid);

	 /**
		* Destructor.
		*/
		IMPORT_C virtual ~COMASuplPosHandlerBase();
                		
        
    public: // New functions
        
        /**
        * @Description Asynchronous method to Initialize the instance of a 
        *								COMASuplPosHandlerBase implementation.
        * When the SUPL Framework loads the POS Handler, the SUPL Framework invokes 
        * the InitializeL method. The POS Handler can perform any 
        * initialization operation when this method is invoked.
        *
        * @param aStatus TRequestStatus for completion of asynchronous request
        * @return If the POS Message plug-in is unable to initialize its resources 
        * during this method call, it can leave with an appropriate Symbian error code. 
        * If InitializeL leaves with an error code, SUPL Framework initialization 
        * fails. SUPL Framework does not reinvoke InitializeL in the event of
        * POS Handler initialization failure.
        * 
        */
        virtual void InitializeL(TRequestStatus &aStatus) = 0;

		/**
		* @Description Cancellation method for InitializeL of POS message plugin .
		* If request cancellation is successful, POS Message plug-in completes the original request 
		* with KErrCancel. If there is no outstanding request, no action is taken.
		*/
        virtual void CancelInitialize() = 0;

		/**
        * @Description Creates a new instance of a COMASuplPosSessionBase implementation. 
        * CreateNewSessionL is a synchronous factory method invoked by the SUPL 
        * framework for POS session creation. 
        *
        * The implementation of CreateNewSessionL should create an object of the 
        * subclass of COMASuplPosSessionBase. 
        * SUPL POS Message plug-in has to allocate memory for the 
        * COMASuplPosSessionBase subclass object. Destroying this object is the 
        * responsibility of the SUPL Framework.
        *
        * CreateNewSessionL takes the MOMASuplObserver as an argument. 
	    *
        *
        * @param aSuplObserver is a mandatory parameter for construction of 
        * COMASuplPosSessionBase. The default constructor of COMASuplPosSessionBase 
        * is private and hence the subclass of COMASuplPosSessionBase has to 
        * define the overloaded constructor that takes MOMASuplObserver as the parameter. 
        * The subclass of COMASuplPosHandlerBase should implement CreateSessionL in 
        * such a way that COMASuplPosSessionBase::iStatusIndicator is set to the MOMASuplObserver 
        * parameter passed to CreateNewSessionL.
        *
        * @return The POS Session implementation. The implementation of CreateNewSessionL 
        * should return the pointer provided by NewL of the subclass of COMASuplPosSessionBase.If 
        * there is insufficient memory to create the POS session, this method leaves with KErrNoMemory
        */
        virtual COMASuplPosSessionBase *CreateNewSessionL(
        /* IN */		  MOMASuplObserver *aSuplObserver) = 0;
        
        
        /**
        * @Description Provides the Version of COMASuplPosHandlerBase implementation.
        *
        * @return Version of the Implementation of the POS Handler. If POS Message plug-in
        * cannot provide version information, this method leaves with KErrGeneral
        */
        IMPORT_C   TInt VersionL() const;
        
        /**
        * @Description Provides the Implementation UID of COMASuplPosHandlerBase implementation.
        *
        * @return Implementation UID of the Implementation of the POS Handler
        */
        IMPORT_C   TUid ImplementationUID() const;
        
        
    public: 	// Functions from base classes
        
    protected:  // New functions
                    
    protected:  // Functions from base classes
    
    /**
		* Constructor
		*/
		IMPORT_C COMASuplPosHandlerBase();

    private:    // Functions
		
		/**
        * Sets the Implementation UID of COMASuplPosHandlerBase implementation.
        *
        */
        void SetImplementationUID(const TUid &aImplementationUid);
        
		
	public:     // Data
    
    protected:  // Data

    private:    // Data
    
	    TUid iDtor_ID_Key;
	    
	    TUid iImplementationUID;
       
    };

#endif      // EPOS_CSUPLSESSIONBASE_H
            
// End of File
