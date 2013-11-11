// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef MBca2Impl_H
#define MBca2Impl_H

#include <e32base.h>
#include <comms-infras/commsbufpond.h>
#include <comms-infras/commsbufchain.h>
#include <networking/bca2factory.h>

const TInt KMaxIPPacket = 1500;
const TInt KIPTagHeaderLength = 2;
const TInt KMaxIPPacketAndHeader = KMaxIPPacket + KIPTagHeaderLength;

//forward declarations
namespace BasebandChannelAdaptation
	{
	class MBca;
	namespace C32Bca
		{
		class CC32BcaFactory;
		}
	}
	
namespace BasebandChannelAdaptation2
	{
	class MBca2Factory;
	class MBca2;
	struct MBca2::TBcaParams;
	class MUpperControl;
	class MUpperDataReceiver;
	class MLowerDataSender;
	}

/** This namespace includes the BCA component names.*/
namespace BasebandChannelAdaptation2
{
	namespace C32BcaShim
	{
	/**
	 * The interface to the factory responsible for C32BCAshim creation. */
	NONSHARABLE_CLASS(MBca2FactoryImpl): public CBase, public MBca2Factory
		{
	public:
		virtual void Release();
		virtual MBca2* NewBcaL();
		};

	class CReceiver : public CActive
		{
	public:
		CReceiver(BasebandChannelAdaptation::MBca* aMBca);
		~CReceiver();
	
	public: // Inherited from CActive.
		virtual void RunL();
		virtual void DoCancel();
		void SetParams(MUpperDataReceiver* aDataReceiver, const MBca2::TBcaParams* aBcaParams);
		void StartListening();
		
	private:
		TBuf8<KMaxIPPacketAndHeader> iData;
		const MBca2::TBcaParams* iBcaParams;
		RCommsBufChain iBufChain;
		BasebandChannelAdaptation::MBca* iMBca;
		MUpperDataReceiver* iDataReceiver;
		};

	
	class CSender : public CActive
		{
	public:
		CSender(BasebandChannelAdaptation::MBca* aMBca);
		~CSender();
	
	public: // Inherited from CActive.
		virtual void RunL();
		virtual void DoCancel();	
		TInt Send(TRequestStatus& aStatus,RCommsBufChain& aPdu);
		void SendBuffer(const TDesC8& aBuffer);
	
	private:
		TRequestStatus* iSendRequest;
		TBuf8<KMaxIPPacketAndHeader> iSendBuffer;
		BasebandChannelAdaptation::MBca* iMBca;
		};	

	//forward decl
	class MLowerDataSenderImpl;
	NONSHARABLE_CLASS(MBca2Impl): public CBase, public MBca2
		{
	public:
		MBca2Impl();
		~MBca2Impl();
		void ConstructL();
		
		virtual TInt Open(MUpperControl& aControl, 
				MUpperDataReceiver& aData, 
						const TBcaParams& aBcaParams);
		
		virtual void Start();
		
		virtual MLowerDataSender& GetSender();
		
		virtual void Close();
		
		virtual void Release();
		
		virtual void SetFlowControl(TBlockOption aOption);
		
		virtual void Control(TRequestStatus& aStatus,
								TUint aOptLevel,
								TUint aOptName,
								TDes8& aOpt);
		
		virtual void CancelControl();
		

		private:
			MUpperControl* iControl; 
			const MBca2::TBcaParams* iBcaParams;
			MLowerDataSenderImpl* iLowerDataSender;					
			CReceiver* iReceiver;
			BasebandChannelAdaptation::MBca* iMBca;
			BasebandChannelAdaptation::C32Bca::CC32BcaFactory* iC32BcaFactory;
			TBool iRecvBlocked:1;
		};


	NONSHARABLE_CLASS(MLowerDataSenderImpl): public MLowerDataSender
		{
		public:
			static MLowerDataSenderImpl* NewL(BasebandChannelAdaptation::MBca* aMBca);
			MLowerDataSenderImpl();
			void ConstructL(BasebandChannelAdaptation::MBca* aMBca);
			void SetControl(MUpperControl* aControl);
			virtual ~MLowerDataSenderImpl();
			virtual	TSendResult Send(RCommsBufChain& aCommsBufChain);
		private:
			CSender* iSender;
			MUpperControl* iControl;			
			TBool iSendBlocked:1;
		};	
	}
}
#endif //MBca2Impl_H

