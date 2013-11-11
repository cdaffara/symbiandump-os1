// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This class implements the CDialog class.
// 
//

/**
 @file
 @internalComponent.
*/

#if !defined(AFX_SERIALPORTDLG_H__48881567_F2B4_11D4_8077_DC3329DB2D48__INCLUDED_)
#define AFX_SERIALPORTDLG_H__48881567_F2B4_11D4_8077_DC3329DB2D48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SerialConnection.h"
//
// CSerialPortDlg dialog

class CSerialPortDlg : public CDialog
{
// Construction
public:
	CSerialPortDlg(CWnd* pParent = NULL);	// standard constructor
	CSerialConnection serialconnect;
// Dialog Data
	//{{AFX_DATA(CSerialPortDlg)
	enum { IDD = IDD_SERIALPORT_DIALOG };
	CStatic	m_LabelText;
	CString	m_StrComPortNo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialPortDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSerialPortDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALPORTDLG_H__48881567_F2B4_11D4_8077_DC3329DB2D48__INCLUDED_)

