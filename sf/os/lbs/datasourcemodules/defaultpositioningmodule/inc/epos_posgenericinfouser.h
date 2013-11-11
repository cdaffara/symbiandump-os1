/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef POSGENERICINFOUSER_H
#define POSGENERICINFOUSER_H

//  INCLUDES
#include <e32base.h>
#include <lbs.h>

// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  This class is helper class to handle HPositionGenericInfo.
*/
class PosGenericInfoUser
    {
    public:
        /**
        * Copy HPositionGenericInfo
        * @param aSrc The source HPositionGenericInfo
        * @param aDes The destination of HPositionGenericInfo
        */
        static TInt CopyHGenericInfo( 
            const HPositionGenericInfo& aSrc,
            HPositionGenericInfo& aDes );
            
    private:
        /**
        * Copy field value from one HPositionGenericInfo to another
        */
        static TInt CopyField( 
            const HPositionGenericInfo& aSrc,
            HPositionGenericInfo& aDes,
            TInt aFieldId );

        /**
        * Get field data type
        */
        static TInt FieldDataType( 
            TInt aFieldId, 
            TInt& aFieldDataType, 
            TBool& aIsList );

        /**
        * Copy data by type
        */
        static TInt CopyDataByType( 
            const HPositionGenericInfo& aSrc,
            HPositionGenericInfo& aDes,
            TInt aFieldId,
            TInt aDataType );
            
        /**
        * Copy data template function
        */
        template < class T > static TInt CopyData( 
            const HPositionGenericInfo& aSrc,
            HPositionGenericInfo& aDes,
            TInt aFieldId );

    };

#endif      // POSGENERICINFOUSER_H   
            
// End of File
