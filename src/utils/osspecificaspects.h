// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include <QString>

#include <algorithm>

#define QTC_WIN_EXE_SUFFIX ".exe"

namespace Utils {

// Add more as needed.
enum OsType { OsTypeWindows, OsTypeLinux, OsTypeMac, OsTypeOtherUnix, OsTypeOther };

namespace OsSpecificAspects {

inline auto withExecutableSuffix(OsType osType, const QString &executable) -> QString
{
    QString finalName = executable;
    if (osType == OsTypeWindows && !finalName.endsWith(QTC_WIN_EXE_SUFFIX)) {
        finalName += QLatin1String(QTC_WIN_EXE_SUFFIX);
    }
    return finalName;
}

constexpr auto fileNameCaseSensitivity(OsType osType) -> Qt::CaseSensitivity
{
    return osType == OsTypeWindows || osType == OsTypeMac ? Qt::CaseInsensitive : Qt::CaseSensitive;
}

constexpr auto envVarCaseSensitivity(OsType osType) -> Qt::CaseSensitivity
{
    return fileNameCaseSensitivity(osType);
}

constexpr auto pathListSeparator(OsType osType) -> QChar
{
    return QLatin1Char(osType == OsTypeWindows ? ';' : ':');
}

constexpr auto controlModifier(OsType osType) -> Qt::KeyboardModifier
{
    return osType == OsTypeMac ? Qt::MetaModifier : Qt::ControlModifier;
}

inline auto pathWithNativeSeparators(OsType osType, const QString &pathName) -> QString
{
    if (osType == OsTypeWindows) {
        const int pos = pathName.indexOf('/');
        if (pos >= 0) {
            QString n = pathName;
            std::replace(std::begin(n) + pos, std::end(n), '/', '\\');
            return n;
        }
    } else {
        const int pos = pathName.indexOf('\\');
        if (pos >= 0) {
            QString n = pathName;
            std::replace(std::begin(n) + pos, std::end(n), '\\', '/');
            return n;
        }
    }
    return pathName;
}

} // namespace OsSpecificAspects
} // namespace Utils
