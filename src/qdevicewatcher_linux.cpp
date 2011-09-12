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

#include "qdevicewatcher_p.h"
//#ifdef Q_OS_LINUX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <unistd.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
#endif


#include <QtCore/QRegExp>
#if CONFIG_SOCKETNOTIFIER
#include <QtCore/QSocketNotifier>
#elif CONFIG_TCPSOCKET
#include <QtNetwork/QTcpSocket>
#endif


#define UEVENT_BUFFER_SIZE      2048

const QByteArray add_str = "add@/devices/pci0000:00/";
const QByteArray remove_str = "remove@/devices/pci0000:00/";
const QByteArray change_str = "change@/devices/pci0000:00/";

QDeviceWatcherPrivate::~QDeviceWatcherPrivate()
{
	stop();
	close(netlink_socket);
	netlink_socket = -1;
}

bool QDeviceWatcherPrivate::start()
{
#if CONFIG_SOCKETNOTIFIER
	socket_notifier->setEnabled(true);
#elif CONFIG_TCPSOCKET
	connect(socket, SIGNAL(readyRead()), SLOT(parseDeviceInfo()));
#else
	this->QThread::start();
#endif
}

bool QDeviceWatcherPrivate::stop()
{
#if CONFIG_SOCKETNOTIFIER
	socket_notifier->setEnabled(false);
#elif CONFIG_TCPSOCKET
	disconnect(socket, SIGNAL(readyRead()), SLOT(parseDeviceInfo()));
#else
	this->QThread::stop();
#endif
	return true;
}


void QDeviceWatcherPrivate::parseDeviceInfo()
{//zDebug("%s active", qPrintable(QTime::currentTime().toString()));
#if CONFIG_SOCKETNOTIFIER
	//socket_notifier->setEnabled(false); //for win
	QByteArray line;
	line.resize(UEVENT_BUFFER_SIZE*2);
	read(socket_notifier->socket(), line.data(), UEVENT_BUFFER_SIZE*2);
	parseLine(line);
	//socket_notifier->setEnabled(true); //for win
#elif CONFIG_TCPSOCKET
	QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(sender());
	QByteArray line = socket->readAll();
	parseLine(line);
#else
#endif
}

#if CONFIG_THREAD
//another thread
void QDeviceWatcherPrivate::run()
{
	QByteArray line;
	line.resize(UEVENT_BUFFER_SIZE*2);
	//eventloop
	//loop only when event happens. because of recv() block the function?
	while (1) {
		//char buf[UEVENT_BUFFER_SIZE*2] = {0};
		//recv(d->netlink_socket, &buf, sizeof(buf), 0);
		line.fill(0);
		recv(netlink_socket, line.data(), line.size(), 0);
		parseLine(line);
	}
}
#endif //CONFIG_THREAD

bool QDeviceWatcherPrivate::init()
{
	struct sockaddr_nl snl;
	const int buffersize = 16 * 1024 * 1024;
	int retval;

	memset(&snl, 0x00, sizeof(struct sockaddr_nl));
	snl.nl_family = AF_NETLINK;
	snl.nl_pid = getpid();
	snl.nl_groups = 1;

	netlink_socket = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
	if (netlink_socket == -1) {
		qWarning("error getting socket: %s", strerror(errno));
		return false;
	}

	/* set receive buffersize */
	setsockopt(netlink_socket, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));
	retval = bind(netlink_socket, (struct sockaddr*) &snl, sizeof(struct sockaddr_nl));
	if (retval < 0) {
		qWarning("bind failed: %s", strerror(errno));
		close(netlink_socket);
		netlink_socket = -1;
		return false;
	}

#if CONFIG_SOCKETNOTIFIER
	socket_notifier = new QSocketNotifier(netlink_socket, QSocketNotifier::Read, this);
	connect(socket_notifier, SIGNAL(activated(int)), SLOT(parseDeviceInfo())); //will always active
	socket_notifier->setEnabled(false);
#elif CONFIG_TCPSOCKET
	//QAbstractSocket *socket = new QAbstractSocket(QAbstractSocket::UnknownSocketType, this); //will not detect "remove", why?
	QTcpSocket *socket = new QTcpSocket(this); //works too
	if (!socket->setSocketDescriptor(netlink_socket, QAbstractSocket::ConnectedState)) {
		qWarning("Failed to assign native socket to QAbstractSocket: %s", qPrintable(socket->errorString()));
		delete socket;
		start();
		return false;
	}
#endif
	return true;
}

void QDeviceWatcherPrivate::parseLine(const QByteArray &line)
{
	//(add)(?:.*/block/)(.*)
	static QRegExp uDisk("sd[a-z][0-9]*$");
	static QRegExp sdCard("mmcblk[0-9][b-z][0-9]*$");
	char* action_str;

	bus_name = line.right(line.length()-line.lastIndexOf('/')-1);
	if (line.startsWith(add_str)) {
		action_str = "Add";
		if (uDisk.indexIn(line)!=-1) { //not exactMatch()?
			emit deviceAdded(bus_name = uDisk.cap(0));
		} else if (sdCard.indexIn(line)!=-1) {
			emit deviceAdded(bus_name = sdCard.cap(1));
		}
	} else if (line.startsWith(remove_str)) {
		action_str = "Remove";
		if (uDisk.indexIn(line)!=-1) {
			emit deviceRemoved(bus_name = uDisk.cap(0));
		} else if (sdCard.indexIn(line)!=-1) {
			emit deviceRemoved(bus_name = sdCard.cap(1));
		}
	} else if (line.startsWith(change_str)) {
		action_str = "Change";
		if (uDisk.indexIn(line)!=-1) {
			emit deviceChanged(bus_name = uDisk.cap(0));
		} else if (sdCard.indexIn(line)!=-1) {
			emit deviceChanged(bus_name = sdCard.cap(1));
		}
	}
	zDebug("%s: %s", action_str, qPrintable(bus_name));
}

//#endif //Q_OS_LINUX
