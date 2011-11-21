/******************************************************************************
	qdevicewatcher_global.h: description
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

#ifndef QDEVICEWATCHER_GLOBAL_H
#define QDEVICEWATCHER_GLOBAL_H

#if defined(Q_DLL_LIBRARY)
#  undef Q_DLL_EXPORT
#  define Q_DLL_EXPORT Q_DECL_EXPORT
#else
#  undef Q_DLL_EXPORT
#  define Q_DLL_EXPORT //Q_DECL_IMPORT //only for vc?
#endif

#endif // QDEVICEWATCHER_GLOBAL_H
