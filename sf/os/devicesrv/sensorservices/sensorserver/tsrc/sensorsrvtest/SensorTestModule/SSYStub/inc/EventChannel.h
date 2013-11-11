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


#ifndef __CEventChannel_H_
#define __CEventChannel_H_

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <sensrvtypes.h>
#include <ssychanneldataprovider.h>
#include <sensrvchannelinfo.h>
#include <SensrvDataListener.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MSsyCallback;
class MSsyPropertyProvider;
class CSensrvChannel;

// CLASS DECLARATION

/**
*  Base class for channels
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CEventChannel : public CActive, 
                        public MSsyChannelDataProvider,
                        public MSensrvDataListener
    {
    public:  // Constructors and destructor
        
        static CEventChannel* NewL( MSsyCallback* aSsyCallback, 
                                    MSsyPropertyProvider* aPropertyProvider, 
                                    TSensrvChannelInfo aChannelInfo );
        /**
        * Destructor.
        */
        virtual ~CEventChannel();
                
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

	public: // From MSensrvDataListener
	
	    /**
        * Indicates data is available in the receiving buffer. A client can read 
        * the data through GetData()-function in the related channel object. Data 
        * is valid until the data received notification occurs again.
        *
        * @since S60 5.0
        * @param[in] aChannel Reference to the related channel object
        * @param[in] aCount Data object count in receiving buffer.
        * @param[in] aDataLost Number of lost data items. Data loss can occur if
        *     the client does not request new data from server fast enough. This 
        *     can happen
        *     when system is under heavy load and the client process has lower
        *     priority than sensor server process. If data loss is a problem, 
        *     consider using higher object count in data listening, which will reduce 
        *     number of expensive IPC calls used in data transmission.
        */  
        void DataReceived( CSensrvChannel& aChannel, 
                                   TInt aCount, 
                                   TInt aDataLost );
        
        /**
        * Data listening failed. 
        * If error was fatal, channel has also been closed and sensor server session 
        * terminated. If error was minor, some data has potentially been lost.
        *
        * @since S60 5.0
        * @param[in] aChannel Reference to the related channel object
        * @param[in] aError Error code.
        */
        void DataError( CSensrvChannel& aChannel, 
                                TSensrvErrorSeverity aError );
		
    	/** 
    	* Returns a pointer to a specified interface - to allow future extension
    	* of this class without breaking binary compatibility
        *
        * @since S60 5.0
    	* @param aInterfaceUid Identifier of the interface to be retrieved
    	* @param aInterface A reference to a pointer that retrieves the specified interface.
    	*/
    	void GetDataListenerInterfaceL( TUid aInterfaceUid, 
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
	
	    CEventChannel( MSsyCallback* aSsyCallback, 
                       MSsyPropertyProvider* aPropertyProvider, 
                       TSensrvChannelInfo aChannelInfo );
                       
        void ConstructL();
	    
	    void IssueRequest( TInt aError = KErrNone );

	    	 
	    
	    

    protected:    // Data
    
        MSsyCallback*       iSsyCallback; 
        
        TChannelState       iChannelState;
        
        TSensrvChannelInfo  iChannelInfo; 
        
        TUint8*             iDataBuffer;
        TInt                iMaxDataCount;
        
        TInt                iCounter; 
        
        TInt                iReceivedDataCount;
        
        MSsyPropertyProvider* iPropertyProvider;
        
        CSensrvChannel*     iChannel;
        
        TUint8* iWritePointer;
        
        TInt iPreviousEvent;
    	
   	public:     // Friend classes
    	
    };

#endif // __CEventChannel_H_
