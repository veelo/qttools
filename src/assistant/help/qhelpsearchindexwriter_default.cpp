/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Assistant of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qhelpsearchindexwriter_default_p.h"
#include "qhelp_global.h"
#include "qhelpenginecore.h"

#include <QtCore/QDir>
#include <QtCore/QSet>
#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QVariant>
#include <QtCore/QFileInfo>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>

QT_BEGIN_NAMESPACE

namespace fulltextsearch {
namespace qt {

Writer::Writer(const QString &path)
    : indexPath(path)
    , indexFile(QString())
    , documentFile(QString())
{
    // nothing todo
}

Writer::~Writer()
{
    reset();
}

void Writer::reset()
{
    for (auto it = index.cbegin(), end = index.cend(); it != end; ++it)
        delete it.value();

    index.clear();
    documentList.clear();
}

bool Writer::writeIndex() const
{
    bool status;
    QFile idxFile(indexFile);
    if (!(status = idxFile.open(QFile::WriteOnly)))
        return status;

    QDataStream indexStream(&idxFile);
    for (auto it = index.cbegin(), end = index.cend(); it != end; ++it) {
        indexStream << it.key();
        indexStream << it.value()->documents.count();
        indexStream << it.value()->documents;
    }
    idxFile.close();

    QFile docFile(documentFile);
    if (!(status = docFile.open(QFile::WriteOnly)))
        return status;

    QDataStream docStream(&docFile);
    for (const QStringList &list : qAsConst(documentList))
        docStream << list.at(0) << list.at(1);

    docFile.close();

    return status;
}

void Writer::removeIndex() const
{
    QFile idxFile(indexFile);
    if (idxFile.exists())
        idxFile.remove();

    QFile docFile(documentFile);
    if (docFile.exists())
        docFile.remove();
}

void Writer::setIndexFile(const QString &namespaceName, const QString &attributes)
{
    const QString extension = namespaceName + QLatin1Char('@') + attributes;
    indexFile = indexPath + QLatin1String("/indexdb40.") + extension;
    documentFile = indexPath + QLatin1String("/indexdoc40.") + extension;
}

void Writer::insertInIndex(const QString &string, int docNum)
{
    if (string == QLatin1String("amp") || string == QLatin1String("nbsp"))
        return;

    Entry *entry = 0;
    if (index.count())
        entry = index[string];

    if (entry) {
        if (entry->documents.last().docNumber != docNum)
            entry->documents.append(Document(docNum, 1));
        else
            entry->documents.last().frequency++;
    } else {
        index.insert(string, new Entry(docNum));
    }
}

void Writer::insertInDocumentList(const QString &title, const QString &url)
{
    documentList.append(QStringList(title) << url);
}


QHelpSearchIndexWriter::QHelpSearchIndexWriter()
    : QThread()
    , m_cancel(false)
{
    // nothing todo
}

QHelpSearchIndexWriter::~QHelpSearchIndexWriter()
{
    mutex.lock();
    this->m_cancel = true;
    waitCondition.wakeOne();
    mutex.unlock();

    wait();
}

void QHelpSearchIndexWriter::cancelIndexing()
{
    mutex.lock();
    this->m_cancel = true;
    mutex.unlock();
}

void QHelpSearchIndexWriter::updateIndex(const QString &collectionFile,
                                         const QString &indexFilesFolder,
                                         bool reindex)
{
    wait();
    QMutexLocker lock(&mutex);

    this->m_cancel = false;
    this->m_reindex = reindex;
    this->m_collectionFile = collectionFile;
    this->m_indexFilesFolder = indexFilesFolder;

    start(QThread::LowestPriority);
}

void QHelpSearchIndexWriter::run()
{
    mutex.lock();

    if (m_cancel) {
        mutex.unlock();
        return;
    }

    const bool reindex(this->m_reindex);
    const QLatin1String key("DefaultSearchNamespaces");
    const QString collectionFile(this->m_collectionFile);
    const QString indexPath = m_indexFilesFolder;

    mutex.unlock();

    QHelpEngineCore engine(collectionFile, 0);
    if (!engine.setupData())
        return;

    if (reindex)
        engine.setCustomValue(key, QString());

    const QStringList registeredDocs = engine.registeredDocumentations();
    const QStringList indexedNamespaces = engine.customValue(key).toString().
        split(QLatin1Char('|'), QString::SkipEmptyParts);

    emit indexingStarted();

    QStringList namespaces;
    Writer writer(indexPath);
    for (const QString &namespaceName : registeredDocs) {
        mutex.lock();
        if (m_cancel) {
            mutex.unlock();
            return;
        }
        mutex.unlock();

        // if indexed, continue
        namespaces.append(namespaceName);
        if (indexedNamespaces.contains(namespaceName))
            continue;

        const QList<QStringList> attributeSets =
            engine.filterAttributeSets(namespaceName);

        for (const QStringList &attributes : attributeSets) {
            // cleanup maybe old or unfinished files
            writer.setIndexFile(namespaceName, attributes.join(QLatin1Char('@')));
            writer.removeIndex();

            QSet<QString> documentsSet;
            const QList<QUrl> docFiles = engine.files(namespaceName, attributes);
            for (QUrl url : docFiles) {
                if (m_cancel)
                    return;

                // get rid of duplicated files
                if (url.hasFragment())
                    url.setFragment(QString());

                const QString s = url.toString();
                if (s.endsWith(QLatin1String(".html"))
                    || s.endsWith(QLatin1String(".htm"))
                    || s.endsWith(QLatin1String(".txt")))
                    documentsSet.insert(s);
            }

            int docNum = 0;
            const QStringList documentsList(documentsSet.toList());
            for (const QString &url : documentsList) {
                if (m_cancel)
                    return;

                QByteArray data(engine.fileData(url));
                if (data.isEmpty())
                    continue;

                QTextStream s(data);
                QString en = QHelpGlobal::codecFromData(data);
                s.setCodec(QTextCodec::codecForName(en.toLatin1().constData()));

                QString text = s.readAll();
                if (text.isEmpty())
                    continue;

                QString title = QHelpGlobal::documentTitle(text);

                int j = 0;
                int i = 0;
                bool valid = true;
                const QChar *buf = text.unicode();
                QChar str[64];
                QChar c = buf[0];

                while ( j < text.length() ) {
                    if (m_cancel)
                        return;

                    if ( c == QLatin1Char('<') || c == QLatin1Char('&') ) {
                        valid = false;
                        if ( i > 1 )
                            writer.insertInIndex(QString(str,i), docNum);
                        i = 0;
                        c = buf[++j];
                        continue;
                    }
                    if ( ( c == QLatin1Char('>') || c == QLatin1Char(';') ) && !valid ) {
                        valid = true;
                        c = buf[++j];
                        continue;
                    }
                    if ( !valid ) {
                        c = buf[++j];
                        continue;
                    }
                    if ( ( c.isLetterOrNumber() || c == QLatin1Char('_') ) && i < 63 ) {
                        str[i] = c.toLower();
                        ++i;
                    } else {
                        if ( i > 1 )
                            writer.insertInIndex(QString(str,i), docNum);
                        i = 0;
                    }
                    c = buf[++j];
                }
                if ( i > 1 )
                    writer.insertInIndex(QString(str,i), docNum);

                docNum++;
                writer.insertInDocumentList(title, url);
            }

            if (writer.writeIndex()) {
                engine.setCustomValue(key, addNamespace(
                    engine.customValue(key).toString(), namespaceName));
            }

            writer.reset();
        }
    }

    for (const QString &namespaceName : indexedNamespaces) {
        if (namespaces.contains(namespaceName))
            continue;

        const QList<QStringList> attributeSets =
            engine.filterAttributeSets(namespaceName);

        for (const QStringList &attributes : attributeSets) {
            writer.setIndexFile(namespaceName, attributes.join(QLatin1Char('@')));
            writer.removeIndex();
        }

        engine.setCustomValue(key, removeNamespace(
            engine.customValue(key).toString(), namespaceName));
    }

    emit indexingFinished();
}

QString QHelpSearchIndexWriter::addNamespace(const QString namespaces,
                                             const QString &namespaceName)
{
    QString value = namespaces;
    if (!value.contains(namespaceName))
        value.append(namespaceName).append(QLatin1Char('|'));

    return value;
}

QString QHelpSearchIndexWriter::removeNamespace(const QString namespaces,
                                                const QString &namespaceName)
{
    QString value = namespaces;
    if (value.contains(namespaceName))
        value.remove(namespaceName + QLatin1Char('|'));

    return value;
}

}   // namespace std
}   // namespace fulltextsearch

QT_END_NAMESPACE
