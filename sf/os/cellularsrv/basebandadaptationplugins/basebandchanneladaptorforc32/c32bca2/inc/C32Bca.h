// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// C32 Baseband Channel Adaptor(BCA) APIs.
// This file contains the definition of the C32Bca interface.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef C32BCA_H
#define C32BCA_H
#include <networking/bca.h>
#include <c32comm.h>
#include <e32base.h>
#include <e32def.h>
#include <commsdattypesv1_1.h>
using namespace CommsDat;

/** This namespace includes the BCA component names.*/
namespace BasebandChannelAdaptation
{

	namespace C32Bca
	{
	const TUint16 KC32BcaMajorVersionNumber = 2;
    const TUint16 KC32BcaMinorVersionNumber = 1;
	
	_LIT(KChannelIdNotOverridden, "");
	
	_LIT8(KC32BcaLogFolder, "C32Bca");
	_LIT8(KC32BcaLogFile, "C32Bca.txt");
	
		
	
	
	/**
	User of C32 RComm - specific functionality.
	A target for upcalls from C32 async users.
	
	This interface is internal to the C32Bca.
	
	*/
    NONSHARABLE_CLASS(MC32User)
    	{
    	public:
    		/**
    		Called when a read request is completed
    		
    		@param aError the completion error */
    		virtual void CommReadComplete(TInt aError) = 0;
    		
    		/**
    		Called when a write request is completed
    		
    		@param aError the completion error */
    		virtual void CommWriteComplete(TInt aError) = 0;
    		
    		/**
    		Called when the link has gone down.
    		
    		@param aError link failure error code. */
    		virtual void CommLinkDown(TInt aError) = 0;    	
    	};
	
	
	
	
	/**
	Common functionality for C32 async users.	*/
	NONSHARABLE_CLASS(CCommBase): public CActive
		{
		public:
			CCommBase(MC32User& aUser, RComm& aPort, TInt aAoPriority);
			virtual ~CCommBase();
		
		protected:
			/**
			Target for synchronous notifications from us */
			MC32User& iUser;
			/**
			Serial port we service for the iUser */
			RComm& iPort;
		};
	
	/**
	Async reader for the serial port.	*/
	NONSHARABLE_CLASS(CCommReader): public CCommBase
		{
		public:
		 	CCommReader(MC32User& aUser, RComm& aPort);
			~CCommReader();
			void Read(TDes8& aBuf);
			void RunL();
			void DoCancel();			
		};
	
	
	/**
	Async writer for the serial port.*/	
	NONSHARABLE_CLASS(CCommWriter): public CCommBase
		{
		public:
			CCommWriter(MC32User& aUser, RComm& aPort);
			~CCommWriter();	
			void Write(const TDesC8& aBuf);
			void RunL();
			void DoCancel();
		};
	
	
	/**
	Monitors the serial port for failures, specifically control line changes.
	
	Depending on how the port is open (DTE or DCE), change in DCD or DTR may
	mean link failure.	
	*/	
	NONSHARABLE_CLASS(CCommLinkMonitor): public CCommBase
		{
		public:
			CCommLinkMonitor(MC32User& aUser, RComm& aPort);
			void Setup(TUint32 aC32ControlLinesMask);
			void NotifyLinkDown();
			void RunL();
			void DoCancel();
			
		private:
			/**
			EIA-232 signals after the most recent change. */
			TUint iSignals;
			
			/**
			State of the EIA-232 signals before the change. */
			TUint iSavedSignalState;
			
			/**
			We request to be notified when these control lines change */
			TUint iNotifyChangeSignalMask;
		};
	
	
		
	
	
	/**
	 * The interface between NIF and C32 Bearer Transport.
	 
	 */
	NONSHARABLE_CLASS(CC32Bca): public CBase, 
		public MBca,  // BCA is accessed via this interface by clients.
		public MC32User // BCA receives internal notifications on C32 events via this interface.
		{
	public:
	    CC32Bca(); 
	    virtual ~CC32Bca();
	    virtual void Release();
	    
		void ConstructL();
		
		virtual void Open(TRequestStatus& aStatus, const TDesC& aChannelId);
		virtual void Shutdown(TRequestStatus& aStatus);
		virtual void Close();
		
		virtual void Read(TRequestStatus& aStatus, TDes8& aBuf);
		virtual void Write(TRequestStatus& aStatus, const TDesC8& aBuf);
		
		virtual void CancelRead();
		virtual void CancelWrite();
		
		virtual void Ioctl(TRequestStatus& aStatus, TUint aOptLevel, TUint aOptName, TDes8& aOpt);
		virtual void CancelIoctl();
	
		// MC32User virtuals
		virtual void CommReadComplete(TInt aErr);
		virtual void CommWriteComplete(TInt aErr);
		virtual void CommLinkDown(TInt aErr);
				
	private:
		void CloseCommPort();	
	
		void ReadCommPortFromCommDbL(TDes& aPortName);
		void ReadCsyNameFromCommDbL(TDes& aCsyName);
		void ReadCommRoleFromCommDbL(TCommRole& aCommRole);
		void ConnectToCommDbBearerRecordL();
		void CloseCommDbConnection();
	    void MonitorControlLinesL(TUint32 aMask);
	    
	    TRequestStatus* iReadRequest;
	    TRequestStatus* iWriteRequest;
	    
	    CCommReader* iReader;
	    CCommWriter* iWriter;
	    CCommLinkMonitor* iLinkMonitor;
	    
	    /** Comm port name*/
		TName iCommPort;
		/** CSY name */
		TName iCsyName;
		/** Comm port role: DTE / DCE */
		TCommRole iCommRole;
		/** Serial port */
		RComm iComm;
		/** session to C32 Server */
		RCommServ iCommServ;
		/** state to keep track of if iCsyName is set by Ioctl()*/
		TBool iCsyNameOverride;
		/** Comm port role explicitly specified */
		TBool iCommRoleOverride;
		/** state to keep track of if the iComm is Opened.*/
		TBool iCommPortOpen;
		/** Iap ID for CommDB access */
		TUint32 iIapId;
		/** CommDB Connection */
		CMDBSession* iCommsDat;
		/** View on the relevant Bearer record */
		TUint32 iModemId;
		
		/** Should the next read be errored with this error  code? */
		TInt iErrorOnNextRead;
		
		/** Should the next write be errored with this error  code? */
		TInt iErrorOnNextWrite;
		
		};
	/**
	* C32BCA Panic.
	* @internalComponent.
	*/
	_LIT(KC32BcaPanic, "C32Bca.dll");

	/**
	* C32BCA Panic code.
	* @internalComponent.
	*/
	enum TC32BcaPanic
		{
		/** Comm port is open */
		ECommPortIsOpen,
		
		/** Comm port is closed */
		ECommPortClose,
		
		/** Reader already exists, we have a memory leak */
		EReaderAlreadyExists,
		
		/** Writer already exists, we have a memory leak */
		EWriterAlreadyExists,
		
		/** Monitor already existrs, we have a memory leak */
		EMonitorAlreadyExists,
		
		/** Closing the BCA, but the reader object is NULL */ 	
		ENullReaderOnClose,
		
		/** Closing the BCA, but the writer object is NULL */
		ENullWriterOnClose,
		
		/** TRequestStatus on completion of Read is NULL */ 
		ENullReadRequestStatus,
		
		/**  Read completed, but the request is not pending. */
		EReadRequestNotPending,
		
		/**
		TRequestStatus on completion of Write is NULL */
		ENullWriteRequestStatus,
		
		/** Write completed, but the request is not pending */
		EWriteRequestNotPending,
		
		/** Other internal logic error */
		EGeneralLogicError,
		};
	void Panic(TC32BcaPanic aPanic);

	}
}

#endif //C32BCA_H
