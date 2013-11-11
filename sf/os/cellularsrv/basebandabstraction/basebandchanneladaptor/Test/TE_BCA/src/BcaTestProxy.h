// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of BCA Test Proxy
// The proxy encapsulates test logging and error checking.
// 
//

/**
 @file
*/

#ifndef BCATESTPROXY_H
#define BCATESTPROXY_H

#include <networking\bca.h>
#include <networking\bcaFactory.h>
using namespace BasebandChannelAdaptation;

#include <E32Base.h>
#include <E32std.h>
#include <test/TestExecuteStepBase.h>


/** Encapsulates all xxBca tests */
namespace te_xxBcaUnitTests
	{

	
	// BCA factory function 
	typedef MBcaFactory* (*TNewBcaFactoryProcL)(void);
	typedef TAutoClose<RLibrary> TDllAutoClose;
	
		/**
	Async request handler */
	class CExecutor: public CActive
		{
		public:
			CExecutor(MBca& iBca);
			virtual ~CExecutor();
			void Exec();
			void RunL();
			void ForceCompletion(TInt aErr);
			TInt CompletionError();			
			virtual void DoCancel();
		
		protected:
			MBca& iBca;
			TInt iCompletionError;
		};
		
	class CReader: public CExecutor
		{
		public:
			CReader(MBca& iBca);
			virtual ~CReader();
			void Read(TDes8& aPkt);
			virtual void DoCancel();
		};
		
	class CWriter: public CExecutor
		{
		public:
			CWriter(MBca& iBca);
			virtual ~CWriter();
			void Write(const TDesC8& aPkt);
			virtual void DoCancel();
		};
	
	/**
	Interface for BCA proxy 
	Test proxy inserts additional logic before dispatching the
	call to the actual BCA 
	
	@internalComponent
	*/
	class MBcaProxy
		{
		public:
			/** Releases all resources & memory associated with the BCA
			used in lieu of destructor */
			virtual void Release() = 0;
			/** 
			Requests BCA Open 
			@param aChannelId channelId */
			virtual void Open(const TDesC& aChannelId) = 0;
			
			/**
			Waits for BCA Open to complete. 
			
			@param aExpectedErr expected error code for Open 
			@leave if Open completes with error other than aExpectedErr 	*/
			virtual void WaitForOpenL(TInt aExpectedErr) = 0;
			
			/**
			Requests immediate BCA Close */
			virtual void Close() = 0;
			
			/** Requests graceful BCA shutdown */
			virtual void Shutdown() = 0;
			
			/**
			Waits for BCA Close to complete: returns once Shutdown completed.
			
			@param aExpectedErr expected error code for Shutdown 
			@leave if Shutdown completes with error other than aExpectedErr */
			virtual void WaitForShutdownL(TInt aExpectedErr) = 0;
			
			/**
			Requests BCA Read
			
			@param aBuf buffer to read into 	*/
			virtual void Read( TDes8& aBuf) = 0;
			
			/**
			Waits for BCA Read to complete: returns once Read completed. 
			
			@param aExpectedErr expected error code for Open 
			@leave if Read completes with error other than aExpectedErr 	*/
			virtual void WaitForReadL(TInt aExpectedErr) = 0;
			
			/**
			Requests BCA Write
			
			@param aBuf the buffer to write. */
			virtual void Write(const TDesC8& aBuf) = 0;
			
			/**
			Waits for BCA Write to complete: returns once Write completed. 
			
			@param aExpectedErr expected error code for Write 
			@leave if Write completes with error other than aExpectedErr
			*/
			virtual void WaitForWriteL(TInt aExpectedErr) = 0;
			
			/**
			Cancel BCA Read */
			virtual void CancelRead() = 0;
			
			/**
			Cancel BCA Write */
			virtual void CancelWrite() = 0;
			
						
			/**
			Request BCA Ioctl 
			
			@param aOptLevel option level
			@param aOptName option name
			@param aOpt packaged buffer with option value	*/
			virtual void Ioctl(TInt aOptLevel, TInt aOptName, TDes8& aOpt) = 0;
			
			/**
			Waits for BCA Ioctl to complete: returns once Ioctl completed. 
			
			@param aExpectedErr expected error code for Ioctl 
			@leave if Ioctl completes with error other than aExpectedErr
			*/
			virtual void WaitForIoctlL(TInt aExpectedErr) = 0;
			
			/** Cancels outstanding Ioctl */
			virtual void CancelIoctl() = 0 ;
		
		};
		

	/**
	BCA Test proxy:
	Routes requests to the BCA, performing test error checking
	Encapsulates common functionality for all BCA proxies */
	class CBcaTestProxy: public CBase, public MBcaProxy
		{
		public:
			CBcaTestProxy(CTestStep& aLogProvider, const TDesC& aProxyName);
			
		protected:
			CTestExecuteLogger& Logger();
			void VerifyErrCodeL(TInt aCodeToVerify, TInt aExpectedErrCode);
			
			void Release();
			
			void Open(const TDesC& aChannelId);
			void WaitForOpenL(TInt expectedErr);
			
			void Close();
			
			void Shutdown();
			void WaitForShutdownL(TInt aExpectedErr);
				
			void Read( TDes8& aBuf);
			void WaitForReadL(TInt expectedErr);
			
			void Write(const TDesC8& aBuf);
			void WaitForWriteL(TInt aExpectedErrCode);
				
			void CancelRead();
			void CancelWrite();
				
			void Ioctl( TInt aOptLevel, TInt aOptName, TDes8& aOpt);
			void WaitForIoctlL(TInt aExpectedErr);
	
	
			
			virtual void CancelIoctl();
			
			/**
			Test Step object owning the test logger */		
			CTestStep& iLogProvider;
			
			CReader* iReader;
			CWriter* iWriter;
			/** Status of request to Open */
			TRequestStatus iOpenRequest;
			/** Status of request to Close */
			TRequestStatus iShutdownRequest;
			/** Status of request to Read */
			//TRequestStatus iReadRequest;
			/** Status of request to Write */
			//TRequestStatus iWriteRequest;
			/** Status of request to Ioctl */
			TRequestStatus iIoctlRequest;
			
			
			/** Proxy name, for logging */
			TName iProxyName;
			
			/** BCA Dll owner */
			TDllAutoClose iBcaDll;
			
			/** The BCA */
			MBca* iBca;
				
		};	

	/** 
	xxBca Test Proxy
	Encapsulates xxBca specific functionality */	
	class CxxBcaProxy: 
		public CBcaTestProxy
		{
		public:
			static CxxBcaProxy* NewL(CTestStep& aLogger, const TDesC& aProxyName);
			void Release();
			
		protected:
			CxxBcaProxy(CTestStep& aLogger, const TDesC& aProxyName);
			void ConstructL();
		};
	} // namespace te_xxBcaUnitTests
#endif
