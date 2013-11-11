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

inline CRfcommFrame::CRfcommFrame(CRfcommMuxer& aMux, CRfcommSAP* aSAP)
	: iMuxer(aMux),
	  iSAP(aSAP)
	{
	}

inline CRfcommFrame::CRfcommFrame(CRfcommMuxer& aMux)
	: iMuxer(aMux)
	{
	}

inline void CRfcommFrame::SetAddress(TUint8 aChan)
	{
	iAddr=aChan;
	}

inline TUint8 CRfcommFrame::Address() const
	{
	return iAddr;
	}

inline void CRfcommFrame::SetControl(TUint8 aCtrl)
	{
	iCtrl=aCtrl;
	}

inline TUint8 CRfcommFrame::Ctrl() const
	{
	return iCtrl;
	}

inline TUint8 CRfcommFrame::Credit() const
	{
	return 0;
	}

inline void CRfcommFrame::SetCredit(TUint8 /*aCredit*/)
	{
	return;
	}

inline TUint16 CRfcommFrame::DataLength() const
	{
	return 0;
	}

inline CRfcommSAP* CRfcommFrame::SAP() const
	{
	return iSAP;
	}

inline void CRfcommMuxCtrlFrame::SetDLCI(TUint8 aDLCI)
	{
	iDLCI=aDLCI;
	}

inline TUint8 CRfcommMuxCtrlFrame::DLCI() const
	{
	return iDLCI;
	}
