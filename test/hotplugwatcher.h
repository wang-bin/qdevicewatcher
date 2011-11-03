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

#ifndef HOTPLUGWATCHER_H
#define HOTPLUGWATCHER_H

#include <QtCore/QObject>
#include <QtCore/QThread>
#include "qdevicewatcher.h"
#include "qdevicechangeevent.h"

#ifndef __GNUC__
#define __PRETTY_FUNCTION__  __FUNCTION__
#endif

class HotplugWatcher : public QThread
{
	Q_OBJECT
public:
	HotplugWatcher(QObject *parent = 0):QThread(parent) {
		qDebug("tid=%#x %s", (unsigned int)QThread::currentThreadId(), __PRETTY_FUNCTION__);
		start();

		moveToThread(this); //Let bool event(QEvent *e) be in another thread
		watcher = new QDeviceWatcher;
		watcher->moveToThread(this);
		watcher->appendEventReceiver(this);
		connect(watcher, SIGNAL(deviceAdded(QString)), this, SLOT(slotDeviceAdded(QString)), Qt::DirectConnection);
		connect(watcher, SIGNAL(deviceChanged(QString)), this, SLOT(slotDeviceChanfed(QString)), Qt::DirectConnection);
		connect(watcher, SIGNAL(deviceRemoved(QString)), this, SLOT(slotDeviceRemoved(QString)), Qt::DirectConnection);
		watcher->start();
	}

public slots:
	void slotDeviceAdded(const QString& dev) { qDebug("tid=%#x %s: add %s", (unsigned int) QThread::currentThreadId(), __PRETTY_FUNCTION__, qPrintable(dev));}
	void slotDeviceRemoved(const QString& dev) { qDebug("tid=%#x %s: remove %s", (unsigned int)QThread::currentThreadId(), __PRETTY_FUNCTION__, qPrintable(dev));}
	void slotDeviceChanfed(const QString& dev) { qDebug("tid=%#x %s: change %s", (unsigned int)QThread::currentThreadId(), __PRETTY_FUNCTION__, qPrintable(dev));}
protected:
	virtual bool event(QEvent *e) {
		if (e->type() == QDeviceChangeEvent::registeredType()) {
			QDeviceChangeEvent *event = (QDeviceChangeEvent*)e;
			QString action("Change");
			if (event->action() == QDeviceChangeEvent::Add)
				action = "Add";
			else if (event->action() == QDeviceChangeEvent::Remove)
				action = "Remove";

			qDebug("tid=%#x event=%d %s: %s %s", (unsigned int)QThread::currentThreadId(), e->type(), __PRETTY_FUNCTION__, qPrintable(action), qPrintable(event->device()));
			event->accept();
			return true;
		}
		return QObject::event(e);
	}

private:
	QDeviceWatcher *watcher;
};

#endif // HOTPLUGWATCHER_H
