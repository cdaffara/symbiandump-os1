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



#ifndef CMMONSTORAGEEXTINTERFACE_H
#define CMMONSTORAGEEXTINTERFACE_H

//  INCLUDES
#include <e32def.h>
#include <et_tsy.h>
#include <ctsy/pluginapi/cmmdatapackage.h>

//  FORWARD DECLARATIONS
class TDataPackage;

// CLASS DECLARATION

/**
*  CMmONStoreExtInterface is used to provide an extension interface
*  for system specific phone object extensions.
*/
class CMmONStoreExtInterface : public CBase
    {
    public:     // Constructors and destructor
    
        /**
        * Destructor
        */
	    virtual ~CMmONStoreExtInterface(){}

    public: // New functions

        /**
        * Reads an ONStore entry from ONStore
        *         
        *
        * @param aIpc Transaction id
        * @param aIndex Location index
        * @return TInt KErrNone or error
        */
        virtual TInt ReadL( TInt /*aIpc*/, TInt /*aIndex*/ )
            { return KErrNotSupported; }

        /**
        * Writes an entry to ONStore
        *         
        *
        * @param aIpc Transaction id
        * @param aEntry Entry
        * @return TInt KErrNone or error value
        */
        virtual TInt WriteL( TInt /*aIpc*/, TDesC8* /*aEntry*/ )
            { return KErrNotSupported; }

	 	/**
        * Writes an entry in write all sequence
        *         
        *
        * @param aIpc IPC
		* @param aEntry Data package containing an ON Store entry
        * @return TInt success/failure
        */
        virtual TInt WriteAllL( TInt, 
        	const CMmDataPackage* /*aEntry*/ )
            	{ return KErrNotSupported; }
            
        /**
        * Gets info from ONStore
        *         
        *
        * @param aIpc 
        * @param aServiceType
        * @return TInt KErrNone or error value
        */
        virtual TInt GetInfoL( 
        	TInt /*aIpc*/, 
        	TUint8 /*aServiceType*/ )
            	{ return KErrNotSupported; }

        /**
        * Deletes all entries from the specified PBStore
        *         
        *
        * @param aIpc
        * @return KErrNone or error
        */
        virtual TInt DeleteAllL(
        	TInt /*aIpc*/ )
        		{ return KErrNotSupported; }

        /**
        * Deletes an entry by the location index
        *         
        *
        * @param aIpc Transaction id
        * @param aIndex Location index
        * @return TInt KErrNone or error
        */
        virtual TInt DeleteL(
        	TInt /*aIpc*/,
        	TInt /*aIndex*/ )
        		{ return KErrNotSupported; }

        /**
        * Counts ON entries
        *         
        *
        * @param aIpc Transaction id
        * @return None
        */
        virtual TInt CountONEntries( 
        	TInt /*aIpc*/ )
        		{ return KErrNotSupported; }


    protected:  // New functions
    
        /**
        * C++ default constructor.
        */
	    CMmONStoreExtInterface(){}

    private:
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL(){}
};

#endif // CMMONSTORAGEEXTINTERFACE_H

// End of file
