/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_MULTI_PSY_H__
#define __CT_LBS_MULTI_PSY_H__

#include <f32file.h>

//  INCLUDES
#include "epos_cpositioner.h"

// CONSTANTS

// FORWARD DECLARATIONS
class TPositionInfo;
class TPositionCourseInfo;
class TPositionSatelliteInfo;
class HPositionGenericInfo;
class CT_LbsTestProxyPsyBase;


// CLASS DECLARATION

/**
*  Used to get position info from an external GPS.
*
*/
class CT_LbsMultiPsy : public CPositioner
    {
   public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_LbsMultiPsy* NewL(TAny* aConstructionParameters);

        /**
        * Two-phased constructor.
        */
        static CT_LbsMultiPsy* NewLC(TAny* aConstructionParameters);

        /**
        * Destructor.
        */
        ~CT_LbsMultiPsy();

    protected:  // Functions from base classes

		/**
        * From CPositioner. Requests position info asynchronously.
        *
        * @param aPosInfo A reference to a position info object. This object
        *                 must be in scope until the request has completed.
        * @param aStatus The request status
        */
		void NotifyPositionUpdate(
		/* IN/OUT */	TPositionInfoBase& aPosInfo,
		/* OUT	  */	TRequestStatus& aStatus
		);

		/**
        * From CPositioner. Cancels position info request.
        */
        void CancelNotifyPositionUpdate();   

    private:

         /**
        * C++ default constructor.
        */
        CT_LbsMultiPsy();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);

		void SetTPositionInfo(TPositionInfo& aInfo);
		void SetTPositionCourseInfo(TPositionCourseInfo& aCourseInfo);
		void SetTPositionSatelliteInfo(TPositionSatelliteInfo& aSatelliteInfo);
		void SetHPositionGenericInfo(HPositionGenericInfo& aGenericInfo);
        TInt OpenFileForAppend( );

        // By default, prohibit copy constructor
        CT_LbsMultiPsy( const CT_LbsMultiPsy& );
        // Prohibit assigment operator
        CT_LbsMultiPsy& operator= ( const CT_LbsMultiPsy& );

      private: 
         
        CT_LbsTestProxyPsyBase*		iRequestHandler;
        RFs                         iFs;
        RFile                       iFile;
        TFileText                   iFileText; 


    };

#endif      // __CT_LBS_MULTI_PSY_H__   
            
// End of File
