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

#include "qusbwatcher_p.h"



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

#include <QtCore/QRegExp>

#define UEVENT_BUFFER_SIZE      2048

const QByteArray add_str = "add@/devices/pci0000:00/";
const QByteArray remove_str = "remove@/devices/pci0000:00/";
const QByteArray change_str = "change@/devices/pci0000:00/";

#if CONFIG_SOCKETNOTIFIER
#include <QtCore/QSocketNotifier>
#include <QtCore/QFile>
#elif CONFIG_TCPSOCKET
#include <QtNetwork/QTcpSocket>
#endif

QUsbWatcherPrivate::~QUsbWatcherPrivate()
{

}

void QUsbWatcherPrivate::init()
{
	struct sockaddr_nl snl;
	const int buffersize = 16 * 1024 * 1024;
	int retval;

	memset(&snl, 0x00, sizeof(struct sockaddr_nl));
	snl.nl_family = AF_NETLINK;
	snl.nl_pid = getpid();
	snl.nl_groups = 1;

	hotplug_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
	if (hotplug_sock == -1) {
		qWarning("error getting socket: %s", strerror(errno));
		return;
	}

	/* set receive buffersize */
	setsockopt(hotplug_sock, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));
	retval = bind(hotplug_sock, (struct sockaddr*) &snl, sizeof(struct sockaddr_nl));
	if (retval < 0) {
		qWarning("bind failed: %s", strerror(errno));
		close(hotplug_sock);
		hotplug_sock = -1;
		return;
	}

#if CONFIG_SOCKETNOTIFIER
	QSocketNotifier *sn = new QSocketNotifier(hotplug_sock, QSocketNotifier::Read, this);
	connect(sn, SIGNAL(activated(int)), SLOT(parseDeviceInfo())); //will always active
#elif CONFIG_TCPSOCKET
	//QAbstractSocket *socket = new QAbstractSocket(QAbstractSocket::UnknownSocketType, this); //will not detect "remove", why?
	QTcpSocket *socket = new QTcpSocket(this); //works too
	if (!socket->setSocketDescriptor(hotplug_sock, QAbstractSocket::ConnectedState)) {
		qWarning("Failed to assign native socket to QAbstractSocket: %s", qPrintable(socket->errorString()));
		delete socket;
		start();
		return;
	}
	connect(socket, SIGNAL(readyRead()), SLOT(parseDeviceInfo()));
#else
	start();
#endif
}


void QUsbWatcherPrivate::parseDeviceInfo()
{//qDebug("%s active", qPrintable(QTime::currentTime().toString()));
#if CONFIG_SOCKETNOTIFIER
	QSocketNotifier *sn = qobject_cast<QSocketNotifier*>(sender());
	//sn->setEnabled(false); //for win
	QByteArray line;
	line.resize(UEVENT_BUFFER_SIZE*2);
	read(sn->socket(), line.data(), UEVENT_BUFFER_SIZE*2);
	parseLine(line);
	//sn->setEnabled(true); //for win
#elif CONFIG_TCPSOCKET
	QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(sender());
	QByteArray line = socket->readAll();
	parseLine(line);
#else
#endif
}

#if CONFIG_THREAD
//another thread
void QUsbWatcherPrivate::run()
{
	QByteArray line;

	line.resize(UEVENT_BUFFER_SIZE*2);
	//eventloop
	//loop only when event happens. because of recv() block the function?
	while (1) {
		//char buf[UEVENT_BUFFER_SIZE*2] = {0};
		//recv(d->hotplug_sock, &buf, sizeof(buf), 0);
		line.fill(0);
		recv(hotplug_sock, line.data(), line.size(), 0);
		parseLine(line);
	}
}
#endif //CONFIG_THREAD

void QUsbWatcherPrivate::parseLine(const QByteArray &line)
{
	//(add)(?:.*/block/)(.*)
	static QRegExp uDisk("sd[a-z][0-9]*$");
	static QRegExp sdCard("mmcblk[0-9][b-z][0-9]*$");

	bus_name = line.right(line.length()-line.lastIndexOf('/')-1);
	if (line.startsWith(add_str)) {
		if (uDisk.indexIn(line)!=-1) { //not exactMatch()?
			bus_name = uDisk.cap(0);
			emit deviceAdded(bus_name);
			qDebug("Add: %s", qPrintable(bus_name));
		} else if (sdCard.indexIn(line)!=-1) {
			bus_name = sdCard.cap(1);
			emit deviceAdded(bus_name);
			qDebug("Add: %s", qPrintable(bus_name));
		}
		qDebug("Add bus: %s", qPrintable(bus_name));
	} else if (line.startsWith(remove_str)) {
		if (uDisk.indexIn(line)!=-1) {
			bus_name = uDisk.cap(0); //?
			emit deviceAdded(bus_name);
			qDebug("Remove: %s", qPrintable(bus_name));
		} else if (sdCard.indexIn(line)!=-1) {
			bus_name = sdCard.cap(1);
			emit deviceAdded(bus_name);
			qDebug("Remove: %s", qPrintable(bus_name));
		}
		qDebug("Remove bus: %s", qPrintable(bus_name));
	} else if (line.startsWith(change_str)) {
		if (uDisk.indexIn(line)!=-1) {
			bus_name = uDisk.cap(0); //?
			emit deviceChanged(bus_name);
			qDebug("Change: %s", qPrintable(bus_name));
		} else if (sdCard.indexIn(line)!=-1) {
			bus_name = sdCard.cap(1);
			emit deviceChanged(bus_name);
			qDebug("Change: %s", qPrintable(bus_name));
		}
		qDebug("Change bus: %s", qPrintable(bus_name));
	}
}

//#endif //Q_OS_LINUX
