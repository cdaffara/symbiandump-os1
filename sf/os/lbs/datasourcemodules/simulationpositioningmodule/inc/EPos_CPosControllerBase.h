/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPOSCONTROLLERBASE_H
#define CPOSCONTROLLERBASE_H

// INCLUDES
#include <e32base.h>
#include <lbspositioninfo.h>



// FORWARD DECLARATIONS
class TPositionInfoBase;
class CPosSimulationPositioner;

// CLASS DECLARATION

/**
 *  Base class for location request controllers.
 *
 *  Simulation PSY operates in different modes (NMEA and simulation mode) and
 *  the different functionality is handled by controller classes.
 *  CPosControllerBase is a base class to such controller classes.
 *
 *  @version $Revision: 1.5 $, $Date: 2005/03/17 07:07:04 $
 */
class CPosControllerBase : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        ~CPosControllerBase();

    public: // New functions

        /**
        * Requests position info asynchronously.
        *
        * @param aPosInfo A reference to a position info object. This object
        *                 must be in scope until the request has completed.
        * @param aStatus The request status
        * @param aSimPos Positioner which called this function
        */
        virtual void NotifyPositionUpdate(
        /* IN/OUT */    TPositionInfoBase& aPosInfo,
        /* OUT    */    TRequestStatus& aStatus,
        /* IN     */	CPosSimulationPositioner& aSimPos
        )=0;	
        
        /**
        * Cancels position info request.
        * @param aSimPos Pointer to positioner which cancelled position update
        */
        virtual void CancelNotifyPositionUpdate(const CPosSimulationPositioner& aSimPos);

        void CompleteRequest( const TInt aIndex, TInt aErrorCode);
    protected:

        /**
        * C++ default constructor.
        */
        CPosControllerBase(TBool aTimeRelativePlayback);
        /**
		* Requests position info asynchronously. Base implementation of NotifyPositionUpdate,
		* should be called in all implementations of NotifyPositionUpdate in derived classes.
		*
		* @param aPosInfo A reference to a position info object. This object
		*                 must be in scope until the request has completed.
		* @param aStatus The request status
		* @param aSimPos Pointer to positioner which called this function
		*/
        void NotifyPositionUpdateBase(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus, CPosSimulationPositioner& aSimPos);
        /**
         * Completes request and notifies all outstanding positioners with error code
         * @param aErrorCode Error code
         */
        void CompleteRequestBase(TInt aErrorCode);
        /**
		 * Finds position of positioner in iSimulationPositioners array. Function panics if 
		 * searched positioner is not found.
		 * @param aSimPos positioner which should be found
		 * @return Position in iSimulationPositioners array
		 */
		TInt FindPositioner(const CPosSimulationPositioner& aSimPos);
    private:

        // By default, prohibit copy constructor 
        CPosControllerBase( const CPosControllerBase& );
        // Prohibit assignment operator
        CPosControllerBase& operator= ( const CPosControllerBase& );
        /**
		* Cancels all outstanding positions
		*/
        void CancelAllNotifyPositionUpdate();
        
    protected:
    	/**
    	 * Helper class that holds reference information about registered positioners.
    	 * Class is used to store reference to positioners and to be able to check
    	 * on which positioners we have outstanding request to serve them.
    	 */
    	class TPositionerReference
    		{
    		public:
				TPositionerReference(CPosSimulationPositioner& aSimPos,
						TRequestStatus& aStatus,
						TPositionInfo& aPosInfo):iSimPos(aSimPos),iReqStatus(aStatus), iPosition(aPosInfo){};
				CPosSimulationPositioner&	iSimPos;
				TRequestStatus&				iReqStatus;
				TPositionInfo&				iPosition;
    		};
    protected:
    	/**
    	 * Array of registered positioners
    	 */
    	RArray<TPositionerReference>	iSimulationPositioners;
    	/**
    	 * Informs if time relative playback future is ON. ETrue - ON, EFalse - OFF.
    	 */
    	TBool	iTimeRelativePlayback;
    	/**
    	 * Defines time interval between reads in case of time relative playback.
    	 */
    	TTimeIntervalMicroSeconds32 iTimeBetweenReads;
    	/**
    	 * Position object, holds a local copy of position given
    	 * to positioners.
    	 */
    	TPositionInfo iPosition;

    };

#endif      // CPOSCONTROLLERBASE_H

// End of File
