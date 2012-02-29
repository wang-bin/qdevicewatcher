/******************************************************************************
	QDeviceWatcherPrivate: watching depends on platform
	Copyright (C) 2011 Wang Bin <wbsecg1@gmail.com>

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

#ifndef QDEVICEWATCHER_P_H
#define QDEVICEWATCHER_P_H

/*!
  Usually we use CONFIG_SOCKETNOTIFIER. CONFIG_TCPSOCKET and CONFIG_THREAD are test case
*/
#include <QtCore/qglobal.h>
#define CONFIG_SOCKETNOTIFIER 1
#define CONFIG_TCPSOCKET 0  //QtNetwork

#if defined(Q_OS_WINCE)
#define CONFIG_THREAD 1
#elif defined(Q_OS_LINUX)
#define CONFIG_THREAD (!CONFIG_SOCKETNOTIFIER && !CONFIG_TCPSOCKET)
#else
#define CONFIG_THREAD 0
#endif

//#define QT_NO_DEBUG_OUTPUT 0
#define CONFIG_DEBUG 1
#if CONFIG_DEBUG
#define zDebug(fmt, ...) qDebug("%s: "#fmt, __FUNCTION__, ##__VA_ARGS__)
#else
#define zDebug(fmt, ...)
#endif //CONFIG_DEBUG

#include <QtCore/QList>
#if CONFIG_THREAD
#include <QtCore/QThread>
#else
#include <QtCore/QObject>
#endif //CONFIG_THREAD

#include "qdevicewatcher.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#else
#include <QtCore/QBuffer>
#endif //Q_OS_WIN

class QDeviceWatcher;
class QDeviceWatcherPrivate
#if CONFIG_THREAD
		: public QThread
#else
		: public QObject
#endif //CONFIG_THREAD
{
	Q_OBJECT
public:
	QDeviceWatcherPrivate(QObject *parent = 0) :
#if CONFIG_THREAD
	QThread(parent)
#else
	QObject(parent)
#endif //CONFIG_THREAD
	{
		//init();
	}
	~QDeviceWatcherPrivate();

	void setWatcher(QDeviceWatcher *w) {watcher=w;}
	bool start();
	bool stop();

	//Do not use Qt::DirectConnection. this thread is not watcher's thread!
	inline void emitDeviceAdded(const QString& dev) {
		if (!QMetaObject::invokeMethod(watcher, "deviceAdded", Q_ARG(QString, dev)))
			qWarning("invoke deviceAdded failed");
	}
	//Linux: when umounting the device
	inline void emitDeviceChanged(const QString& dev) {
		if (!QMetaObject::invokeMethod(watcher, "deviceChanged", Q_ARG(QString, dev)))
			qWarning("invoke deviceChanged failed");
	}
	inline void emitDeviceRemoved(const QString& dev) {
		if (!QMetaObject::invokeMethod(watcher, "deviceRemoved", Q_ARG(QString, dev)))
			qWarning("invoke deviceRemoved failed");
	}

	QList<QObject*> event_receivers;

private slots:
	void parseDeviceInfo();

private:
	QDeviceWatcher *watcher;

	bool init();
#if CONFIG_THREAD
    virtual void run();
#endif //CONFIG_THREAD
#if defined(Q_OS_LINUX)
	QBuffer buffer;
	void parseLine(const QByteArray& line);
# if CONFIG_TCPSOCKET
	class QTcpSocket *tcp_socket;
# elif CONFIG_SOCKETNOTIFIER
	class QSocketNotifier *socket_notifier;
# endif

	QString bus_name;
	int netlink_socket;
#elif defined(Q_OS_WIN32)
	HWND hwnd;
#elif defined(Q_OS_WINCE)
    HANDLE qStore;
    HANDLE hNotify;
#endif
};


#endif // QDEVICEWATCHER_P_H
