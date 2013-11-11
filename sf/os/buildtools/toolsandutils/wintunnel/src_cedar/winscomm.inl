// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins/specific/winscomm.inl
// inline void DComm::SetCurrent(TInt aCurrent)
// { iLdd->iPowerHandler->SetCurrentConsumption(aCurrent); }
// 
//




inline TBool DChannelComm::AreAnyPending() const
	{ return((DComm*)iPdd)->AreAnyPending(); }

inline TInt DChannelComm::RxCount()
	{ return ((DComm*)iPdd)->RxCount(); }

inline void DChannelComm::ReadCancel()
	{ ((DComm*)iPdd)->ReadCancel(); }

inline void DChannelComm::WriteCancel()
	{ ((DComm*)iPdd)->WriteCancel(); }

inline void DChannelComm::SignalChangeCancel()
	{ ((DComm*)iPdd)->SignalChangeCancel(); }

inline void DChannelComm::ResetBuffers(TBool aTx)
	{ ((DComm*)iPdd)->ResetBuffers(aTx); }

inline TInt DChannelComm::SetRxBufferSize(TInt aSize)
	{ return ((DComm*)iPdd)->SetRxBufferSize(aSize); }

inline TDes8* DChannelComm::RxBuffer()
	{ return ((DComm*)iPdd)->RxBuffer(); }

inline	TInt DChannelComm::RxBufferSize()
	{ return ((DComm*)iPdd)->RxBufferSize(); }

inline void DChannelComm::SetStatus(TState aStatus)
	{ iStatus=aStatus; }

inline TInt DChannelComm::PddStart()
	{ return ((DComm*)iPdd)->Start(); }

inline void DChannelComm::Stop(TStopMode aMode)
	{ ((DComm*)iPdd)->Stop(aMode); }

inline void DChannelComm::PddBreak(TBool aState)
	{ ((DComm*)iPdd)->Break(aState); }

inline TUint DChannelComm::Signals() const
	{ return ((DComm*)iPdd)->Signals(); }

inline void DChannelComm::SetSignals(TUint aSetMask,TUint aClearMask)
	{ ((DComm*)iPdd)->SetSignals(aSetMask,aClearMask); }

inline TInt DChannelComm::ValidateConfig(const TCommConfigV01 &aConfig) const
	{ return ((DComm*)iPdd)->ValidateConfig(aConfig); }

inline void DChannelComm::PddConfigure(TCommConfigV01 &aConfig)
	{ ((DComm*)iPdd)->Configure(aConfig); }

inline void DChannelComm::PddCaps(TDes8 &aCaps) const
	{ ((DComm*)iPdd)->Caps(aCaps); }

inline void DChannelComm::PddCheckConfig(TCommConfigV01& aConfig)
	{ ((DComm*)iPdd)->CheckConfig(aConfig); }


