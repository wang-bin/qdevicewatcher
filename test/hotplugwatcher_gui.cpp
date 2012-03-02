/******************************************************************************
	hotplugwatcher_gui.cpp: description
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


#include "hotplugwatcher_gui.h"

#include <QtCore/QThread>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTextBrowser>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QMessageBox>

#include "qdevicewatcher.h"

HotplugWatcher_GUI::HotplugWatcher_GUI(QWidget *parent) :
	QWidget(parent)
{
	QVBoxLayout *vbl = new QVBoxLayout(this);
	setLayout(vbl);

	state = new QLabel(this);
	button = new QPushButton(tr("Start"), this);
	msg_view = new QTextBrowser(this);
	msg_view->setLineWrapMode(QTextEdit::NoWrap);

	vbl->addWidget(state);
	vbl->addWidget(button);
	vbl->addWidget(msg_view);

	tray = new QSystemTrayIcon(this);
	tray->show();

	watcher = new QDeviceWatcher;
	watcher->appendEventReceiver(this);
	connect(watcher, SIGNAL(deviceAdded(QString)), this, SLOT(slotDeviceAdded(QString)), Qt::DirectConnection);
	connect(watcher, SIGNAL(deviceChanged(QString)), this, SLOT(slotDeviceChanged(QString)), Qt::DirectConnection);
	connect(watcher, SIGNAL(deviceRemoved(QString)), this, SLOT(slotDeviceRemoved(QString)), Qt::DirectConnection);

	connect(button, SIGNAL(clicked()), SLOT(toggleWatch()));
}

HotplugWatcher_GUI::~HotplugWatcher_GUI()
{

}

void HotplugWatcher_GUI::appendMessage(const QString &msg)
{
	msg_view->append(msg);
}

void HotplugWatcher_GUI::toggleWatch()
{
	if (watcher->isRunning()) {
		if (!watcher->stop()) {
			QMessageBox::warning(0, tr("WARNGING"), tr("Failed to stop"));
		}
		button->setText(tr("Start"));
	} else {
		if (!watcher->start()) {
			QMessageBox::warning(0, tr("WARNGING"), tr("Failed to start"));
		}
		button->setText(tr("Stop"));
	}
}

void HotplugWatcher_GUI::slotDeviceAdded(const QString &dev)
{
	qDebug("tid=%#x: add %s", (unsigned int) QThread::currentThreadId(), qPrintable(dev));

	state->setText("Add: " + dev);
	tray->showMessage(tr("New device"), dev);
}

void HotplugWatcher_GUI::slotDeviceChanged(const QString &dev)
{
	qDebug("tid=%#x: change %s", (unsigned int) QThread::currentThreadId(), qPrintable(dev));

	state->setText("Change: " + dev);
	tray->showMessage(tr("Change device"), dev);
}

void HotplugWatcher_GUI::slotDeviceRemoved(const QString &dev)
{
	qDebug("tid=%#x: remove %s", (unsigned int) QThread::currentThreadId(), qPrintable(dev));

	state->setText("Remove: " + dev);
	tray->showMessage(tr("Remove device"), dev);
}
