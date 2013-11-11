// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
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

Purpose
This corehci allows you to run tproxy whilst the associated serial port is still connected to a controller 
(for example a Casira). 
It obviates the need to replace the controller with a null modem connection to a second serial port
on which you would have hcemeulator running. It therefore obviates the need to configure the hci to use the 
h4 (uart original) host controller transport layer (hctl).

What does it do?
This is the standard Symbian Core HCI branched (Nov 2008) and modified to intercept Vendor specific commands. 
Instead of passing vendor specific commands down to the HCTL, it sends back a corresponding command complete 
event with an event payload identical to the payload in the vendor specific command. Further more, prior to 
sending back the corresponding command complete event it sends several vendor specific debug events.
This is an advantage over hcemeulator which requires user input to send vendor debug events.

How do you set it up
Just compile this core hci. The config files are automatically exported, though, to be
sure, do an "abld test reallyclean" first .... or simply use the supplied bldcorehci.bat file

Constraints
It only works with HCI_V2.

Warning
If debugging in Carbide make sure you do not debug from a project containing the HCI source tree.
If you do, Carbide may revert your config to using the standard CoreHCI implementation.

What tproxy expects
tproxy expects the (vendor specific) command complete event. TProxy will handle the vendor debug events. 
It does not, however, require them.

Contact
Nick Frankau