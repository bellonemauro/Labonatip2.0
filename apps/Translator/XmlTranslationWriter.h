/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#ifndef XML_WRITER_H
#define XML_WRITER_H

#include <QXmlStreamWriter>

QT_BEGIN_NAMESPACE
  class QTreeWidget;
  class QTreeWidgetItem;
QT_END_NAMESPACE


class XmlTranslationWriter
{

public:
    explicit XmlTranslationWriter(const QTreeWidget *treeWidget);

    bool writeFile(QIODevice *device);

	void setSourceLanguage(QString _source_language) { source_language = _source_language; }
	void setTranslationLanguage(QString _translation_language) { translation_language = _translation_language; }

private:

 	static inline QString message() { return QStringLiteral("Message"); }
	static inline QString name() { return QStringLiteral("name"); }
	static inline QString source() { return QStringLiteral("source"); }
	static inline QString translation() { return QStringLiteral("translation"); }
	static inline QString versionAttribute() { return QStringLiteral("version"); }
	static inline QString languageAttribute() { return QStringLiteral("language"); }
	static inline QString sourceLanguageAttribute() { return QStringLiteral("sourcelanguage"); }


    
	void writeItem(const QTreeWidgetItem *item);
	void writeMessageItem(const QTreeWidgetItem *item);
	
	QString source_language;
	QString translation_language;

	QXmlStreamWriter xml;
    const QTreeWidget *treeWidget;
};
 #endif