// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.ant;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.StringReader;

import org.apache.tools.ant.BuildEvent;
import org.apache.tools.ant.DefaultLogger;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.util.StringUtils;

public class ScanLogger extends DefaultLogger {

    /**
     * Logs a message, if the priority is suitable.
     * In non-emacs mode, task level messages are prefixed by the
     * task name which is right-justified.
     *
     * @param event A BuildEvent containing message information.
     *              Must not be <code>null</code>.
     */
    public void messageLogged(BuildEvent event) {
        int priority = event.getPriority();
        // Filter out messages based on priority
        if (priority <= msgOutputLevel) {

            StringBuffer message = new StringBuffer();
            if (event.getTask() != null && !emacsMode) {
                // Print out the name of the task if we're in one
                String name = event.getTask().getTaskName();
                String label = "[" + name + "] ";
                int size = LEFT_COLUMN_SIZE - label.length();
                StringBuffer tmp = new StringBuffer();
                for (int i = 0; i < size; i++) {
                    tmp.append(" ");
                }
                tmp.append(label);
                label = tmp.toString();

                try {
                    BufferedReader r =
                        new BufferedReader(
                            new StringReader(event.getMessage()));
                    String line = r.readLine();
                    boolean first = true;
                    while (line != null) {
                        if (!first) {
                            message.append(StringUtils.LINE_SEP);
                        }
                        first = false;
                        message.append(label).append(line);
                        line = r.readLine();
                    }
                } catch (IOException e) {
                    // shouldn't be possible
                    message.append("ERROR:").append(label).append(event.getMessage());
                }
            } else {
                message.append(event.getMessage());
            }

            String msg = message.toString();
            if (priority != Project.MSG_ERR) {
                printMessage(msg, out, priority);
            } else {
                printMessage("ERROR:" + msg, err, priority);
            }
            log(msg);
        }
    }
}
