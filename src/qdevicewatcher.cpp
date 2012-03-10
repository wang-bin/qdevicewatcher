/******************************************************************************
	QDeviceWatcher: Device watcher class
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

#include "qdevicewatcher.h"
#include "qdevicewatcher_p.h"

QDeviceWatcher::QDeviceWatcher(QObject* parent)
	:QObject(parent),running(false),d_ptr(new QDeviceWatcherPrivate)
{
	Q_D(QDeviceWatcher);
	d->setWatcher(this);
}

QDeviceWatcher::~QDeviceWatcher()
{
	if (d_ptr) {
		delete d_ptr;
		d_ptr = NULL;
	}
}

bool QDeviceWatcher::start()
{
	Q_D(QDeviceWatcher);
	if (!d->start()) {
		stop();
		running = false;
	}
	running = true;
	return running;
}

bool QDeviceWatcher::stop()
{
	Q_D(QDeviceWatcher);
	running = !d->stop();
	return !running;
}

bool QDeviceWatcher::isRunning() const
{
	return running;
}

void QDeviceWatcher::appendEventReceiver(QObject *receiver)
{
	Q_D(QDeviceWatcher);
	d->event_receivers.append(receiver);
}

void QDeviceWatcherPrivate::emitDeviceAdded(const QString &dev)
{
	if (!QMetaObject::invokeMethod(watcher, "deviceAdded", Q_ARG(QString, dev)))
		qWarning("invoke deviceAdded failed");
}

void QDeviceWatcherPrivate::emitDeviceChanged(const QString &dev)
{
	if (!QMetaObject::invokeMethod(watcher, "deviceChanged", Q_ARG(QString, dev)))
		qWarning("invoke deviceChanged failed");
}

void QDeviceWatcherPrivate::emitDeviceRemoved(const QString &dev)
{
	if (!QMetaObject::invokeMethod(watcher, "deviceRemoved", Q_ARG(QString, dev)))
		qWarning("invoke deviceRemoved failed");
}

void QDeviceWatcherPrivate::emitDeviceAction(const QString &dev, const QString &action)
{
	QString a(action.toLower());
	if (a == QLatin1String("add"))
		emitDeviceAdded(dev);
	else if (a == QLatin1String("remove"))
		emitDeviceRemoved(dev);
	else if (a == QLatin1String("change"))
		emitDeviceChanged(dev);
}
