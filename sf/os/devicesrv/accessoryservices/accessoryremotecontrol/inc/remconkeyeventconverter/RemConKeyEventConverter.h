/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CRemConKeyEventConverter -class.
*                    Defines the Accessory Key event handling API of
*                    Accessory Framework.
*
*/



#ifndef CREMCONKEYEVENTCONVERTER_H
#define CREMCONKEYEVENTCONVERTER_H

//  INCLUDES
#include <e32base.h>
#include <remcon/remconconverterplugin.h>
#include <remcon/remconconverterinterface.h>
#include "RemConKeyEventData.h"

// DATA TYPES
typedef TPckgBuf<TRemConKeyEventData> TRemConKeyEventDataBuf;

// FORWARD DECLARATIONS
class CRemConKeyEventConverterImpl;

// CLASS DECLARATION
/**
*  CRemConKeyEventConverter defines the ASY Key event handling API of Accessory
*  Framework.
* 
*  @lib 
*  @since S60 3.0
*/
class CRemConKeyEventConverter : public CRemConConverterPlugin, 
 								 public MRemConConverterInterface
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRemConKeyEventConverter* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CRemConKeyEventConverter();

    public: // New functions
        
    public: // Functions from base classes
	    
    protected:  // Functions from base classes
    
    private:	// From CRemConConverterPlugin
    
    	/**
		* Called by RemCon server to get a pointer to an object which implements
		* the converter API with UID aUid. This is a mechanism for allowing 
		* future change to the converter API without breaking BC in existing 
		* (non-updated) converter plugins.
		* @since S60 3.0
        * @param aUid aUid Identifies converter API version.
		*/
        TAny* GetInterface( TUid aUid );
    	
    private:	// From MRemConConverterInterface
    	
    	/** 
        * Called by the server to get the converter to convert from the 
        * interface See remconconverterplugin.h for function declaration 
        * details.
        * Not implemented in key event converter. Returns KErrNone.
        */
        TInt InterfaceToBearer(TUid aInterfaceUid, 
            TUint aOperationId,
            const TDesC8& aData,
            TRemConMessageType aMsgType, 
            TDes8& aBearerData) const;

        /** 
        * Tells the converter to convert from the bearer -specific format to
        * the interface (API) format.
        * The key-event data has to be passed in the descriptor as of type
        * TRemConKeyEventDataBuf.
        * @since S60 3.0
        * @param aBearerData Bearer input (of type TRemConKeyEventDataBuf).
        * @param aInterfaceUid The interface UID from Converter.
        * @param aOperationId Operation ID from Converter.
        * @param aMsgType The type of the message (not used).
        * @param aData Data associated with the operation.
        * @return KErrNone if successfull, KErrNotSupported if matching 
        * key-event was found from Accessory Policy but functionality defined 
        * for the key event is not currently supported and KErrGeneral if 
        * passed data is not valid or some other non-recoverable error has 
        * occurred.
        */
        TInt BearerToInterface(const TDesC8& aBearerData,
            TUid& aInterfaceUid, 
            TUint& aOperationId,
            TRemConMessageType& aMsgType, 
            TDes8& aData) const;

        /**
        * Called by the server to get the interface and bearer, the formats of
        *  which this converter converts between.
        * @since S60 3.0
        * @param aInterfaceUid The interface UID.
        * @param aBearerUid The bearer UID.
        * @return ETrue if the parameters match with UIDs defined in the 
        *           converter, otherwise EFalse.
        */
        TBool SupportedUids(const TUid& aInterfaceUid, 
            const TUid& aBearerUid) const;

        /**
        * Called by the server to find out if this converter converts between 
        * the formats of this bearer interface and RemCon interface.
        * @since S60 3.0
        * @param aInterfaceData The identifier of this interface in a bearer 
        *   defined format.
        * @param aBearerUid The bearer UID.
        * @return ETrue if the parameters match with corresponding values 
        *   defined in the converter, otherwise EFalse.
        */
        TBool SupportedInterface(const TDesC8& aInterfaceData, 
            const TUid& aBearerUid) const;
    	    	
    private:

        /**
        * C++ default constructor.
        */
        CRemConKeyEventConverter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:    // Data
        // Instance of CAccFWKeyEventConverterImpl implementation class.
        CRemConKeyEventConverterImpl* iImpl;

    };

#endif      // CREMCONKEYEVENTCONVERTER_H   
            
// End of File
