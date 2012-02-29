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

#include "qdevicewatcher_p.h"

#include <windows.h>
#include <msgqueue.h>
#include <pnp.h>
#include <storemgr.h>
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

	if (!StopDeviceNotifications(hNotify)) {
		//GetLastError
		return false;
	}
	if (!CloseMsgQueue(qStore)) {
		//GetLastError
		return false;
	}
	return true;
}


bool QDeviceWatcherPrivate::init()
{
	MSGQUEUEOPTIONS msgopts;

	msgopts.dwSize = sizeof(MSGQUEUEOPTIONS);
	msgopts.dwFlags = 0; //MSGQUEUE_NOPRECOMMIT, MSGQUEUE_ALLOW_BROKEN
	msgopts.dwMaxMessages = 0;
	msgopts.cbMaxMessage = sizeof(DEVDETAIL) + (MAX_PATH * sizeof(TCHAR));
	msgopts.bReadAccess = TRUE;

	qStore = CreateMsgQueue(NULL, &msgopts);
	if (qStore == NULL) {
		//GetLastError
		return false;
	}
	hNotify = RequestDeviceNotifications(&BLOCK_DRIVER_GUID, qStore, TRUE);
	if (hNotify == NULL) {
		//GetLastError
		return false;
	}
	return true;
}

void QDeviceWatcherPrivate::run()
{
	DWORD flags;
	DWORD size;
	//
	BYTE DevDetail[sizeof(DEVDETAIL) + (MAX_PATH * sizeof(TCHAR))];
	DEVDETAIL *pDetail = (DEVDETAIL*)DevDetail;
	while (true) {
		if(WaitForSingleObject(qStore, 3000) == WAIT_OBJECT_0) {
			while(ReadMsgQueue(qStore, pDetail, sizeof(DevDetail), &size, 1, &flags)) {
				QString dev = TCHAR2QString(pDetail->szName);
				QDeviceChangeEvent *event = 0;
				if (pDetail->fAttached) {
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
