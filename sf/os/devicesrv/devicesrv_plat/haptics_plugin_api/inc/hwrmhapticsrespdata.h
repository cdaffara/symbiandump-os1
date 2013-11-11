/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics plugin API response data header file.
*
*/


#ifndef C_HWRMHAPTICSRESPDATA_H
#define C_HWRMHAPTICSRESPDATA_H

#include <e32base.h>

class RWriteStream;
class RReadStream;

/**
 * Streamable class representing the response data from haptics
 * plugins.
 *
 * @since S60 5.1
 */
class CHWRMHapticsRespData : public CBase
    {
public:

    /**
     * 2-phase construction
     *
     * @param aErrorCode Error code (always present in response).
     * @param aData      Const reference to a descriptor that contains
     *                   the 8-bit data from which to construct the 
     *                   CHWRMHapticsRespData object.
     */
    IMPORT_C static CHWRMHapticsRespData* NewLC( TInt aErrorCode,
                                                 const TDesC8& aData );

    /**
     * Destructor
     */
    IMPORT_C virtual ~CHWRMHapticsRespData();

public:
        
    /**
     * Method for externalizing (writing) data to a stream
     *
     * @param aStream Reference to the write stream into which
     *                the data is to be externalized (written).
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
        
    /**
     * Method for internalizing (loading) data from a stream
     *
     * @param aStream Reference to the read stream from which the 
     *                data is to be internalized (loaded).
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );
        
    /**
     * Getter for error code
     *
     * @return TInt The error code.
     */
    IMPORT_C TInt ErrorCode();
        
    /**
     * Getter for the contained data
     * 
     * @return TDesC8 Constant reference to a descriptor containing 
     *                the 8-bit data.
     */
    IMPORT_C const TDesC8& Data();

private:
    /**
     * Constructor
     */     
    CHWRMHapticsRespData();
        
    /**
     * ConstructL for 2-phase construction
     */     
    void ConstructL( TInt aErrorCode, const TDesC8& aData );

private: // data
    /**
     * Error code
     */
    TInt iErrorCode;
  
    /**
     * Pointer to the 8-bit data (of this response)
     */
    HBufC8* iData;
    };
    
#endif // C_HWRMHAPTICSRESPDATA_H
