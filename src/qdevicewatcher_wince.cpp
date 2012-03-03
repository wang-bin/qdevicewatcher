/******************************************************************************
	qdevicewatcher_wince.cpp: for wince. TO BE TESTED
	Copyright (C) 2012 Wang Bin <wbsecg1@gmail.com>
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
******************************************************************************/

/*!
 * http://msdn.microsoft.com/en-us/library/aa447466.aspx
*/
#include "qdevicewatcher_p.h"

#ifdef Q_OS_WINCE
#include <msgqueue.h>
#include <pnp.h>
#include <Storemgr.h> //BLOCK_DRIVER_GUID
#include <QtCore/QCoreApplication>
#include "qdevicechangeevent.h"

#ifdef UNICODE
#  if QT_VERSION >= 0x040000
#    define TCHAR2QString(x) QString::fromUtf16(x)
#  else
#    define TCHAR2QString(x) QString::fromUcs2(x)
#  endif
#else
#  define TCHAR2QString(x)	 QString::fromLocal8Bit((x))
#endif //UNICODE

typedef union {
	DEVDETAIL d;
	char pad[sizeof(DEVDETAIL)+MAX_DEVCLASS_NAMELEN]; //BYTE pad[sizeof(DEVDETAIL) + (MAX_PATH * sizeof(TCHAR))];
} Q_DEVDETAIL;


QDeviceWatcherPrivate::~QDeviceWatcherPrivate()
{
}

bool QDeviceWatcherPrivate::start()
{
	if (!init())
		return false;
	this->QThread::start();
	return true;
}


bool QDeviceWatcherPrivate::stop()
{
	quit();

	if (!StopDeviceNotifications(mNotificationHandle)) {
		qWarning("StopDeviceNotifications() error: %d", GetLastError());
		return false;
	}
	if (!CloseMsgQueue(mQueueHandle)) {
		qWarning("CloseMsgQueue() error: %d", GetLastError());
		return false;
	}
	return true;
}


bool QDeviceWatcherPrivate::init()
{
	MSGQUEUEOPTIONS msgopts;

	msgopts.dwSize = sizeof(MSGQUEUEOPTIONS);
	msgopts.dwFlags = 0; //MSGQUEUE_VARIABLESIZE | MSGQUEUE_MSGSIZE; /*msdn*/ //MSGQUEUE_NOPRECOMMIT | MSGQUEUE_ALLOW_BROKEN
	msgopts.dwMaxMessages = 0;
	msgopts.cbMaxMessage = sizeof(Q_DEVDETAIL);
	msgopts.bReadAccess = TRUE;
	msgopts.dwMaxMessages = 0; //?
	//msgopts.cbMaxMsgQueue = 0; //?
	//msgopts.dwDesiredAccess = GENERIC_READ;
	//msgopts.dwShareMode = 0;
	//msgopts.dwCreationDisposition = CREATE_NEW;

	mQueueHandle = CreateMsgQueue(NULL, &msgopts);
	if (mQueueHandle == NULL) {
		qWarning("CreateMsgQueue() error: %d", GetLastError());
		return false;
	}
	mNotificationHandle = RequestDeviceNotifications(&BLOCK_DRIVER_GUID, mQueueHandle, TRUE);
	if (mNotificationHandle == NULL) {
		qWarning("RequestDeviceNotifications() error: %d", GetLastError());
		return false;
	}
	return true;
}

void QDeviceWatcherPrivate::run()
{
	DWORD flags;
	DWORD size;
	Q_DEVDETAIL detail;
	SetLastError(0); //?
	while (true) {
		if(WaitForSingleObject(mQueueHandle, 3000) == WAIT_OBJECT_0) {
			while(ReadMsgQueue(mQueueHandle, &detail, sizeof(detail), &size, 1, &flags)) {
				QString dev = TCHAR2QString(detail.d.szName);
				QDeviceChangeEvent *event = 0;
				if (detail.d.fAttached) {
					emitDeviceAdded(dev);
					event = new QDeviceChangeEvent(QDeviceChangeEvent::Add, dev);
				} else {
					emitDeviceRemoved(dev);
					event = new QDeviceChangeEvent(QDeviceChangeEvent::Remove, dev);
				}
				if (event != 0 && !event_receivers.isEmpty()) {
					foreach(QObject* obj, event_receivers) {
						QCoreApplication::postEvent(obj, event, Qt::HighEventPriority);
					}
				}
			}
		}
	}
}

void QDeviceWatcherPrivate::parseDeviceInfo()
{
}

#endif //Q_OS_WINCE
