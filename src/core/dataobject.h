#ifndef ACE_DATAOBJECT_H
#define ACE_DATAOBJECT_H
#include <QtCore>

#include "abstractdata.h"
#include "datastream.h"
#include "metadata.h"
#include "metadatamodel.h"



namespace Ace
{
   class DataObject : public QObject
   {
      Q_OBJECT
   public:
      explicit DataObject(const QString& path);
      ~DataObject();
      void open();
      bool seek(qint64 offset) const;
      bool allocate(qint64 size);
      void clear(quint16 newType);
      bool isNew() const;
      qint64 size() const;
      EAbstractData& data();
      EMetadata& getMeta() { return _metaRoot; }
      void writeMeta();
      void reloadMeta();
      quint16 getType() { return _type; }
      QString getPath() { return _path; }
      MetadataModel* getModel() { return &_metaModel; }
      operator bool() const;
   signals:
      void cleared();
   private:
      constexpr static int _mininumFileSize {12};
      constexpr static quint64 _specialValue {584};
      std::unique_ptr<QFile> _file {nullptr};
      std::unique_ptr<EDataStream> _stream {nullptr};
      std::unique_ptr<EAbstractData> _data {nullptr};
      qint64 _headerOffset {0};
      bool _isNew {true};
      bool _invalid {false};
      EMetadata _metaRoot;
      MetadataModel _metaModel;
      quint16 _type {0};
      QString _path;
   };
}



#endif
// 64special 16inttype qstring