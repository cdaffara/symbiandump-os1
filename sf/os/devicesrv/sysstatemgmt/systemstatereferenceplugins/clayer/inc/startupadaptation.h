/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface   : Domain, Startup Adaptation API
* This header file defines the Startup Adaptation API command
* interface used to issue commands towards adaptation.
*
*/



#ifndef __STARTUPADAPTATION_H__
#define __STARTUPADAPTATION_H__

#include <e32base.h>
#include <ssm/startupadaptationcommands.h>

/*
* @publishedPartner
* @released
*/
class MStartupAdaptationObserver;

/*****************************************************
*       Series 60 Customer
*       Needs customer implementation
*****************************************************/

/**
*  An ECOM plug-in interface class to be implemented by startup adaptation.
*
*  @since S60 3.2
*/
class CStartupAdaptation : public CBase
    {

public:  // Constructors and destructor

    /**
    * Create new startup adaptation plug-in object.
    *
    * @since S60 3.2
    *
    * @param anObserver An observer object for receiving command responses
    * and adaptation events.
    */
    inline static CStartupAdaptation* NewL(
        MStartupAdaptationObserver& anObserver );

    /**
    * Destructor.
    *
    * @since S60 3.2
    */
    inline virtual ~CStartupAdaptation();

public: // New functions

    /**
    * Issue a command to startup adaptation.
    *
    * @since S60 3.2
    *
    * @param aCommandId A command identifier.
    * @param aData      Data associated with the command.
    */
    virtual void CommandL(
        const StartupAdaptation::TCommand aCommandId,
        TDesC8& aData ) = 0;

    /**
    * Cancel a command.
    *
    * @since S60 3.2
    *
    * @param aCommandId A command identifier.
    */
    virtual void CancelCommandL(
        const StartupAdaptation::TCommand aCommandId ) = 0;

    /**
    * Get plug-in implementation version. Major and minor version numbers
    * are relevant.
    *
    * @since S60 3.2
    *
    * @return The plug-in implementation version.
    */
    virtual TVersion Version() const = 0;


private:    // Data

    /**
    * Plug-in identifier. Used to inform ECom after the object has been
    * destroyed.
    */
    TUid iDtorIDKey;

    };

#include <ssm/startupadaptation.inl>

#endif // __STARTUPADAPTATION_H__
