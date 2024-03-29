/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include "utils_global.h"

#include "osspecificaspects.h"

#include <QString>

#ifdef Q_OS_WIN
#define QTC_HOST_EXE_SUFFIX QTC_WIN_EXE_SUFFIX
#else
#define QTC_HOST_EXE_SUFFIX ""
#endif // Q_OS_WIN

namespace Utils {

class UTILS_EXPORT HostOsInfo
{
public:
    static constexpr auto hostOs() -> OsType
    {
#if defined(Q_OS_WIN)
        return OsTypeWindows;
#elif defined(Q_OS_LINUX)
        return OsTypeLinux;
#elif defined(Q_OS_MAC)
        return OsTypeMac;
#elif defined(Q_OS_UNIX)
        return OsTypeOtherUnix;
#else
        return OsTypeOther;
#endif
    }

    enum HostArchitecture { HostArchitectureX86, HostArchitectureAMD64, HostArchitectureItanium,
                            HostArchitectureArm, HostArchitectureUnknown };
    static auto hostArchitecture() -> HostArchitecture;

    static constexpr auto isWindowsHost() -> bool { return hostOs() == OsTypeWindows; }
    static constexpr auto isLinuxHost() -> bool { return hostOs() == OsTypeLinux; }
    static constexpr auto isMacHost() -> bool { return hostOs() == OsTypeMac; }
    static constexpr auto isAnyUnixHost() -> bool
    {
#ifdef Q_OS_UNIX
        return true;
#else
        return false;
#endif
    }

    static auto withExecutableSuffix(const QString &executable) -> QString
    {
        return OsSpecificAspects::withExecutableSuffix(hostOs(), executable);
    }

    static void setOverrideFileNameCaseSensitivity(Qt::CaseSensitivity sensitivity);
    static void unsetOverrideFileNameCaseSensitivity();

    static auto fileNameCaseSensitivity() -> Qt::CaseSensitivity
    {
        return m_useOverrideFileNameCaseSensitivity
                ? m_overrideFileNameCaseSensitivity
                : OsSpecificAspects::fileNameCaseSensitivity(hostOs());
    }

    static auto pathListSeparator() -> QChar
    {
        return OsSpecificAspects::pathListSeparator(hostOs());
    }

    static auto controlModifier() -> Qt::KeyboardModifier
    {
        return OsSpecificAspects::controlModifier(hostOs());
    }

    static auto canCreateOpenGLContext(QString *errorMessage) -> bool;

private:
    static Qt::CaseSensitivity m_overrideFileNameCaseSensitivity;
    static bool m_useOverrideFileNameCaseSensitivity;
};

} // namespace Utils
