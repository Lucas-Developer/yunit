/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ApplicationScreenshotProvider.h"
#include "ApplicationManager.h"
#include "ApplicationInfo.h"

#include "paths.h"

#include <QDebug>
#include <QGuiApplication>
#include <QWindow>
#include <QQuickWindow>

ApplicationScreenshotProvider::ApplicationScreenshotProvider(ApplicationManager *appManager)
    : QQuickImageProvider(QQuickImageProvider::Image)
    , m_appManager(appManager)
{
}

QImage ApplicationScreenshotProvider::requestImage(const QString &imageId, QSize * size,
                                                     const QSize &requestedSize)
{
    // We ignore requestedSize here intentionally to avoid keeping scaled copies around
    Q_UNUSED(requestedSize)

    QString appId = imageId.split('/').first();

    ApplicationInfo* app = static_cast<ApplicationInfo*>(m_appManager->findApplication(appId));
    if (app == NULL) {
        qDebug() << "ApplicationScreenshotProvider - app not found:" << appId;
        return QImage();
    }

    QString filePath = QString("%1/Dash/graphics/phone/screenshots/%2@12.png").arg(qmlDirectory()).arg(app->icon().toString());

    QImage image;
    if (!image.load(filePath)) {
        qDebug() << "failed loading app image" << filePath;
    }

    QGuiApplication *unity = qobject_cast<QGuiApplication*>(qApp);

    if (app->stage() == ApplicationInfo::SideStage) {
        QByteArray gus = qgetenv("GRID_UNIT_PX");
        image = image.scaledToWidth(gus.toInt() * 48);
    } else {
        Q_FOREACH (QWindow *win, unity->allWindows()) {
            QQuickWindow *quickWin = qobject_cast<QQuickWindow*>(win);
            if (quickWin) {
                image = image.scaledToWidth(quickWin->width() - m_appManager->rightMargin());
            }
        }
    }

    size->setWidth(image.width());
    size->setHeight(image.height());
    qDebug() << "got image of size" << size->width() << size->height() << requestedSize;

    return image;
}
