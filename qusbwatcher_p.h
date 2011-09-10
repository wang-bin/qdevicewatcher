/******************************************************************************
	Name: description
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

#ifndef QUSBWATCHER_P_H
#define QUSBWATCHER_P_H

#define CONFIG_SOCKETNOTIFIER 1
#define CONFIG_TCPSOCKET 0  //QtNetwork
#define CONFIG_THREAD (!CONFIG_SOCKETNOTIFIER && !CONFIG_TCPSOCKET)


#if CONFIG_THREAD
#include <QtCore/QThread>
#else
#include <QtCore/QObject>
#endif //CONFIG_THREAD

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif //Q_OS_WIN

class QUsbWatcherPrivate
#if CONFIG_THREAD
		: public QThread
#else
		: public QObject
#endif //CONFIG_THREAD
{
	Q_OBJECT
public:
	QUsbWatcherPrivate(QObject *parent = 0)
#if CONFIG_THREAD
	: QThread(parent)
#else
	: QObject(parent)
#endif //CONFIG_THREAD
	{
		init();
	}

	~QUsbWatcherPrivate();

	inline void emitDeviceAdded(const QString& dev) {emit deviceAdded(dev);}
	inline void emitDeviceChanged(const QString& dev) {emit deviceChanged(dev);}
	inline void emitDeviceRemoved(const QString& dev) {emit deviceRemoved(dev);}

signals:
	void deviceAdded(const QString& dev);
	void deviceChanged(const QString& dev); //when umounting the device
	void deviceRemoved(const QString& dev);

protected slots:
	void parseDeviceInfo();

protected:
	bool init();
#if CONFIG_THREAD
	virtual void run();
#endif
	void parseLine(const QByteArray& line);

#if defined(Q_OS_LINUX)
	QString bus_name;
	int hotplug_sock;
#elif defined(Q_OS_WIN)
	HWND hwnd;
#endif
};


#endif // QUSBWATCHER_P_H
