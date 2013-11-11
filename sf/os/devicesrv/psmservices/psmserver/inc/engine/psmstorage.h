/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM storage base class.
*
*/


#ifndef PSMSTORAGE_H
#define PSMSTORAGE_H

#include <e32base.h>
#include <xml/dom/xmlengdom.h>
#include <psmtypes.h>

/**
 *  PSM Storage base class
 *
 *  Base class for PSM storages
 *
 *  @since S60 5.0
 */
class CPsmStorage : public CBase
                    
    {
public:
    
    /**
     * Destructor.
     */
    virtual ~CPsmStorage();

    /**
	 * Starts parsing config file. This function blocks until file is parsed
 	 */
	virtual void InitStorageL( const TUint32 aStorageUid ) = 0;

    /**
     * Backsup configuration to backup storage
     */
    virtual void BackupConfigurationL( const RConfigInfoArray& aConfigArray );

    /**
	 * Closes storage file.
 	 */
	virtual void CloseStorageL();

    /**
     * Reads configuration from config storage
     */
    void GetConfigurationL( RConfigInfoArray& aConfigArray );

    /**
     * Sets new mode
     */
    void SetMode( TPsmsrvMode aMode ) { iMode = aMode; }; 

protected:

    /**
     * C++ constructor.
     */
    CPsmStorage( TPsmsrvMode iMode, RFs& aFile );

    /**
     * Blocks until XML parser / composer has done its job
     */
    void WaitStorageActionComplete();

    /**
     * Initializes root elements when storage file has been parsed
     */
    virtual void InitRootElementsL() = 0;

    /**
     * Helper function to get correct configuration set for current mode
     *
     * @param aMode Identifies the configuration set to get
     * @return XML element if set is found, otherwise leaves with KErrNotFound
     */
    TXmlEngElement FindConfigurationSetL( const TInt aMode );    

    /**
     * Lists set items of a specific passive configuration
     */
    void CreateConfigArrayL( RConfigInfoArray& aPsmConfigArray,
                             const TInt aMode );

    /** 
     * Helper functions for handling storage data
     * 
     * Getters
     */
    TInt GetAttributeIntValueL( const TXmlEngElement& aElement, const TDesC8& aAttrib ) const;
    void GetAttributeStrValueL( const TXmlEngElement& aElement, const TDesC8& aAttrib, TDes8& aTarget ) const;
    TReal GetAttributeRealValueL( const TXmlEngElement& aElement, const TDesC8& aAttrib ) const;
    TUint GetAttributeHexValueL( const TXmlEngElement& aElement, const TDesC8& aAttrib ) const;

    /**
     * Setters
     */
    void SetAttributeValueL( TXmlEngElement& aElement, const TDesC8& aAttrib, const TInt aValue ) const;
    void SetAttributeValueL( TXmlEngElement& aElement, const TDesC8& aAttrib, const TDesC8& aValue ) const;
    void SetAttributeValueL( TXmlEngElement& aElement, const TDesC8& aAttrib, const TReal aValue ) const;
    void SetAttributeHexValueL( TXmlEngElement& aElement, const TDesC8& aAttrib, const TUint32 aValue ) const;        

private:

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

protected:    // Data

    /** 
     * Reference to file server session
     */
    RFs& iFileSession;

    /**
     * Active scheduler wait for blocking construction until config file is parsed
     */
    CActiveSchedulerWait iSchedulerWait;

    /**
     * Pointer of the config xml-file parser
     */
    RXmlEngDOMParser iStorageParser;   

    /**
     * Elements of the storage file. These needs to be stored
     */
    RXmlEngDocument iStorageFile;        // Storage file
    TXmlEngElement  iStorageRoot;        // Root of the storage
    RXmlEngDOMImplementation iDOMImpl;

    /**
     * Storage UID
     */
    TUint32 iStorageUid;

    /**
     * Mode where to switch to
     */
    TPsmsrvMode iMode;

    };

#endif // PSMSTORAGE_H 
