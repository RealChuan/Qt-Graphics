// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include "utils_global.h"

#include "osspecificaspects.h"

#include <optional>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

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

    enum HostArchitecture {
        HostArchitectureX86,
        HostArchitectureAMD64,
        HostArchitectureItanium,
        HostArchitectureArm,
        HostArchitectureArm64,
        HostArchitectureUnknown
    };
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

    static auto isRunningUnderRosetta() -> bool;

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

    static constexpr auto pathListSeparator() -> QChar
    {
        return OsSpecificAspects::pathListSeparator(hostOs());
    }

    static constexpr auto controlModifier() -> Qt::KeyboardModifier
    {
        return OsSpecificAspects::controlModifier(hostOs());
    }

    static auto canCreateOpenGLContext(QString *errorMessage) -> bool;

    static auto totalMemoryInstalledInBytes() -> std::optional<quint64>;

private:
    static Qt::CaseSensitivity m_overrideFileNameCaseSensitivity;
    static bool m_useOverrideFileNameCaseSensitivity;
};

} // namespace Utils
