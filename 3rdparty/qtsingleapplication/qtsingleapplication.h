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

#include <QApplication>

#include <3rdparty/thirdparty_global.hpp>

QT_FORWARD_DECLARE_CLASS(QSharedMemory)

namespace SharedTools {

class QtLocalPeer;

class THRIDPARTY_EXPORT QtSingleApplication : public QApplication
{
    Q_OBJECT

public:
    QtSingleApplication(const QString &id, int &argc, char **argv);
    ~QtSingleApplication() override;

    auto isRunning(qint64 pid = -1) -> bool;

    void setActivationWindow(QWidget* aw, bool activateOnMessage = true);
    [[nodiscard]] auto activationWindow() const -> QWidget*;
    auto event(QEvent *event) -> bool override;

    [[nodiscard]] auto applicationId() const -> QString;
    void setBlock(bool value);

    auto sendMessage(const QString &message, int timeout = 5000, qint64 pid = -1) -> bool;
    void activateWindow();

Q_SIGNALS:
    void messageReceived(const QString &message, QObject *socket);
    void fileOpenRequest(const QString &file);

private:
    auto instancesFileName(const QString &appId) -> QString;

    qint64 firstPeer;
    QSharedMemory *instances;
    QtLocalPeer *pidPeer;
    QWidget *actWin;
    QString appId;
    bool block;
};

} // namespace SharedTools
