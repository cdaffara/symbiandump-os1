/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef __CSSYChannelBase_H_
#define __CSSYChannelBase_H_

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <sensrvtypes.h>
#include <ssychanneldataprovider.h>
#include <sensrvchannelinfo.h>


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MSsyCallback;
class CStubTimer;
class MSsyPropertyProvider;

// CLASS DECLARATION

/**
*  Base class for channels
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CSSYChannelBase : public CActive, 
                        public MSsyChannelDataProvider
    {
    public:  // Constructors and destructor
        
        
        /**
        * Destructor.
        */
        virtual ~CSSYChannelBase();
                
    public: //From CActive
    
        void DoCancel();
        void RunL();
    
    public: // From MSsyChanelDataProvider
	    /**
        * Starts asynchronous data listening
        *
        * @since S60 5.0
        * @param aBuffer Pointer to a data buffer 
        * @param aCount Indicates data buffer size as a count of the data objects 
        * @return void
        */	
		void StartChannelDataL( const TSensrvChannelId aChannelId, 
                                    TUint8* aBuffer, TInt aCount);	

    	/**
        * Stops asynchronous data listening. The data buffer is not valid after call of this function.
        *
        * @since S60 5.0
        * @return void
        */	
		void StopChannelDataL( const TSensrvChannelId aChannelId );	
		
    	/**
        * Forces SSY to call WriteCompleted() even if all requested data is not written yet.
        *
        * @since S60 5.0
        * @return void
        */	
		void ForceBufferFilledL( const TSensrvChannelId aChannelId );			

    	/** 
    	* Returns a pointer to a specified interface extension - to allow future extension
    	* of this class without breaking binary compatibility
        *
        * @since S60 5.0
    	* @param aInterfaceUid Identifier of the interface to be retrieved
    	* @param aInterface A reference to a pointer that retrieves the specified interface.
    	*/
    	void GetChannelDataProviderInterfaceL( TUid aInterfaceUid, 
    	                                               TAny*& aInterface );

    public: 
    
        /**
        * Return channelId
        */
        TSensrvChannelId ChannelId();
        
        /**
        * Set channelId
        */
        void SetChannelId( const TSensrvChannelId aChannelId );
        
        /**
        * Return channel info
        */
        TSensrvChannelInfo& ChannelInfo();
        
    	/**
        * Request from sensor server to open a sensor channel to client side. 
        *
        * @since S60 5.0
        * @return void Symbian OS error code
        */	
		TInt OpenChannel();

    	/**
        * Request to close a sensor channel
        *
        * @since S60 5.0
        * @return void Symbian OS error code
        */	
		TInt CloseChannel();
		
		/**
		* Callback function for CPeriodic
		*
		*/
		static TInt PeriodicTick( TAny* aObject );

	protected:
	    
	    enum TChannelState
	        {
	        EChannelIdle,
	        EChannelOpening,
	        EChannelOpen,
	        EChannelClosing,
	        EChannelListening,
	        EChannelStopListening,
	        EChannelForceBufferFilled,
	        EChannelBufferFilled
	        
	        };
	
	    CSSYChannelBase( MSsyCallback* aSsyCallback, MSsyPropertyProvider* aPropertyProvider );
	    
	    void IssueRequest( TInt aError = KErrNone );
	    
	    TInt DoPeriodicTick();
	    	    
	    virtual void DoAppendData() = 0;
	    
	    

    protected:    // Data
    
        MSsyCallback*       iSsyCallback; 
        
        TChannelState       iChannelState;
        
        TSensrvChannelInfo  iChannelInfo; 
        
        TUint8*             iDataBuffer;
        TInt                iMaxDataCount;
        
        TInt                iCounter; 
        
        CStubTimer*         iPeriodic;
        
                
        MSsyPropertyProvider* iPropertyProvider;
    	
   	public:     // Friend classes
    	
    };

#endif // __CSSYChannelBase_H_
