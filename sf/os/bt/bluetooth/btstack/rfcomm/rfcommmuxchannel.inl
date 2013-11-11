// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

inline void CRfcommMuxChannel::Open()
	{
	iState->Open(*this);
	}

inline void CRfcommMuxChannel::Close()
	{
	iState->Close(*this);
	}

inline TBool CRfcommMuxChannel::IsOpen()
	{
	return iState->IsOpen(*this);
	}

inline TBool CRfcommMuxChannel::IsErrored()
	{
	return iState->IsErrored(*this);
	}

inline TInt CRfcommMuxChannel::MaxDataSize()
	{
	return iMaxDataSize;
	}


// Rfcomm events
inline void CRfcommMuxChannel::UA()
	{
	iState->UA(*this);
	}

inline void CRfcommMuxChannel::DISC()
	{
	iState->DISC(*this);
	}

inline void CRfcommMuxChannel::DM()
	{
	iState->DM(*this);
	}

inline void CRfcommMuxChannel::PN(TBool aCommand, TRfcommPortParams& aParams)
	{
	iState->PN(*this, aCommand, aParams);
	}

inline void CRfcommMuxChannel::SABM()
	{
	iState->SABM(*this);
	}

inline void CRfcommMuxChannel::FrameTimeout(CRfcommFrame* aFrm)
	{
	iState->FrameTimeout(*this, aFrm);
	}


// L2CAP events (passed on from the mux)
inline void CRfcommMuxChannel::Disconnect()
	{
	iState->Disconnect(*this);
	}

inline void CRfcommMuxChannel::CanClose()
	{
	iState->CanClose(*this);
	}

inline void CRfcommMuxChannel::ConnectComplete()
	{
	iState->ConnectComplete(*this);
	}

inline void CRfcommMuxChannel::Error(TInt anError,TUint anOperationMask)
	{
	iState->Error(*this, anError, anOperationMask);
	}

inline TInt CRfcommMuxChannel::TransmitSABM()
	{
	return iMux.TransmitSABM(KMuxDLCI);
	}

inline TInt CRfcommMuxChannel::TransmitUA()
	{
	return iMux.TransmitUA(KMuxDLCI);
	}

inline TInt CRfcommMuxChannel::TransmitDM()
	{
	return iMux.TransmitDM(KMuxDLCI, ETrue);
	}

inline TInt CRfcommMuxChannel::TransmitPN(TBool aCommand, const TRfcommPortParams& aParams)
	{
	return iMux.TransmitPN(KMuxDLCI, aCommand, aParams);
	}
