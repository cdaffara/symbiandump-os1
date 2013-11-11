/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Declaration of CSsmSystemCommandsBase class.
*
*/

#ifndef SSMSYSTEMCOMMANDSBASE_H
#define SSMSYSTEMCOMMANDSBASE_H

#include <e32base.h>
#include <ssm/ssmcustomcommand.h>

class CSsmMapperUtility;

/**
 *  Base class for system commands.
 */
NONSHARABLE_CLASS( CSsmSystemCommandsBase ) : public CBase
    {

public:

    /**
     * Destructor
     */
    virtual ~CSsmSystemCommandsBase();

protected:
    
    /**
    * First phase constructor.
    */
    CSsmSystemCommandsBase();

   
    void SetL( TUid aCategory, TUint aKey, TInt aValue );
    
    void DefineL( TUid aCategory, TUint aKey, 
                  const TSecurityPolicy& aReadPolicy,
                  const TSecurityPolicy& aWritePolicy );
    
    CSsmMapperUtility& MapperUtilityL();


private: // data

    // Mapper utility instance, owned.
    CSsmMapperUtility* iMapperUtility;

    };

#endif // SSMSYSTEMCOMMANDSBASE_H
