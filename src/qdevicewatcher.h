/******************************************************************************
	QDeviceWatcher: Device watcher class
    Copyright (C) 2011-2015 Wang Bin <wbsecg1@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/

#ifndef QDEVICEWATCHER_H
#define QDEVICEWATCHER_H

#include <QtCore/QObject>
#include <QtCore/QEvent>

#ifdef BUILD_QDEVICEWATCHER_STATIC
#define Q_DW_EXPORT
#else
#if defined(BUILD_QDEVICEWATCHER_LIB)
#  undef Q_DW_EXPORT
#  define Q_DW_EXPORT Q_DECL_EXPORT
#else
#  undef Q_DW_EXPORT
#  define Q_DW_EXPORT //Q_DECL_IMPORT //only for vc?
#endif
#endif //BUILD_QDEVICEWATCHER_STATIC


class QDeviceWatcherPrivate;

class Q_DW_EXPORT QDeviceWatcher : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(QDeviceWatcher)
public:
	explicit QDeviceWatcher(QObject *parent = 0);
	~QDeviceWatcher();

	bool start();
	bool stop();
	bool isRunning() const;

	void appendEventReceiver(QObject* receiver);

signals:
	void deviceAdded(const QString& dev);
	void deviceChanged(const QString& dev); //when umounting the device
	void deviceRemoved(const QString& dev);

protected:
	bool running;
	QDeviceWatcherPrivate *d_ptr;
};


class Q_DW_EXPORT QDeviceChangeEvent : public QEvent
{
public:
    enum Action { Add, Remove, Change};
    //static const Type EventType; //VC link error

    explicit QDeviceChangeEvent(Action action, const QString& device);

    Action action() const {return m_action;}
    QString device() const {return m_device;}
    static Type registeredType()
    {
        static Type EventType = static_cast<Type>(registerEventType());
        return EventType;
    }

private:
    Action m_action;
    QString m_device;
};

#endif // QDEVICEWATCHER_H
