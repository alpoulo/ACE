#include "metadatamodel.h"
#include "metadata.h"
#include "exception.h"






QVariant Ace::MetadataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   // make sure it is display role and horizontal orientation
   if ( role != Qt::DisplayRole && orientation != Qt::Vertical )
   {
      return QVariant();
   }

   // return name of column
   switch (section)
   {
   case 0:
      return QVariant(QString(tr("Name")));
   case 1:
      return QVariant(QString(tr("Type")));
   case 2:
      return QVariant(QString(tr("Value")));
   default:
      // if invalid column return nothing
      return QVariant();
   }
}






QModelIndex Ace::MetadataModel::index(int row, int column, const QModelIndex& parent) const
{
   // get pointer of parent index
   EMetadata* metaParent;
   if ( parent.isValid() )
   {
      metaParent = reinterpret_cast<EMetadata*>(parent.internalPointer());
   }
   else
   {
      metaParent = _root;
   }

   // get pointer of child
   EMetadata* child;
   if ( metaParent->isArray() )
   {
      child = metaParent->toArray()->at(row);
   }
   else if ( metaParent->isObject() )
   {
      child = metaParent->toObject()->value(metaParent->toObject()->keys().at(row));
   }
   else
   {
      // parent is a type it should not be so report error
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Metadata Model"));
      e.setDetails(tr("Discovered metadata parent that is not array or object."));
      throw e;
   }

   // return new index
   return createIndex(row,column,child);
}






QModelIndex Ace::MetadataModel::parent(const QModelIndex& child) const
{
   // get pointer of child and parent
   EMetadata* metaChild = reinterpret_cast<EMetadata*>(child.internalPointer());
   EMetadata* parent = metaChild->getParent();

   // make sure parent and grandparent exists
   if ( !parent || !parent->getParent() )
   {
      return QModelIndex();
   }

   // get pointer to grandparent and return parent index
   EMetadata* grandparent = parent->getParent();
   return createIndex(grandparent->getChildIndex(parent),child.column(),parent);
}






int Ace::MetadataModel::rowCount(const QModelIndex& parent) const
{
   // make sure there is a root metadata object
   if ( !_root )
   {
      return 0;
   }

   // get pointer of parent index
   EMetadata* metaParent;
   if ( parent.isValid() )
   {
      metaParent = reinterpret_cast<EMetadata*>(parent.internalPointer());
   }
   else
   {
      metaParent = _root;
   }

   // return array size if parent is array
   if ( metaParent->isArray() )
   {
      return metaParent->toArray()->size();
   }

   // return object size if parent is object
   else if ( metaParent->isObject() )
   {
      return metaParent->toObject()->size();
   }

   // return zero if parent is not array/object
   else
   {
      return 0;
   }
}






int Ace::MetadataModel::columnCount(const QModelIndex& parent) const
{
   Q_UNUSED(parent);
   return 3;
}






QVariant Ace::MetadataModel::data(const QModelIndex& index, int role) const
{
   // return nothing if role is not for display
   if ( role != Qt::DisplayRole )
   {
      return QVariant();
   }

   // get pointer of index and return data depending on column
   EMetadata* meta = reinterpret_cast<EMetadata*>(index.internalPointer());
   switch (index.column())
   {
   case 0:
   {
      // get parent and return number or key depending on parent type
      EMetadata* parent = meta->getParent();
      if ( parent->isArray() )
      {
         return QVariant(index.row());
      }
      else if ( parent->isObject() )
      {
         return QVariant(parent->toObject()->keys().at(index.row()));
      }
      else
      {
         // parent is of type it should not be so report error
         E_MAKE_EXCEPTION(e);
         e.setTitle(tr("Metadata Model"));
         e.setDetails(tr("Discovered metadata parent that is not array or object."));
         throw e;
      }
   }
   case 1:
      return QVariant(meta->getTypeName());
   case 2:
      return meta->toVariant();
   default:
      return QVariant();
   }
}






Qt::ItemFlags Ace::MetadataModel::flags(const QModelIndex& index) const
{
   // setup basic flags that always exist
   Qt::ItemFlags ret {Qt::ItemIsSelectable|Qt::ItemIsEnabled};

   // get pointer to metadata of index
   EMetadata* meta;
   if ( index.isValid() )
   {
      meta = reinterpret_cast<EMetadata*>(index.internalPointer());
   }
   else
   {
      meta = _root;
   }

   // if metadata type is not array, object, or null and is row 2 then it is editable
   if ( !meta->isArray() && !meta->isObject() && !meta->isNull() && index.row() == 2 )
   {
      ret |= Qt::ItemIsEditable;
   }

   // return flags
   return ret;
}






bool Ace::MetadataModel::removeRows(int row, int count, const QModelIndex& parent)
{
   // get pointer to metadata of parent
   EMetadata* metaParent;
   if ( parent.isValid() )
   {
      metaParent = reinterpret_cast<EMetadata*>(parent.internalPointer());
   }
   else
   {
      metaParent = _root;
   }

   // check if metadata type is array
   if ( metaParent->isArray() )
   {
      // get list and begin remove row opertation
      EMetadata::List* list {metaParent->toArray()};
      beginRemoveRows(parent,row,row+count-1);

      // remove rows then end remove row operation
      for (int i = 0; i < count ;++i)
      {
         list->removeAt(row);
      }
      endRemoveRows();
   }

   // check if metadata type is object
   else if ( metaParent->isObject() )
   {
      // get list and begin remove row opertation
      EMetadata::Map* map {metaParent->toObject()};
      beginRemoveRows(parent,row,row+count-1);

      // remove rows then end remove row operation
      auto keys {map->keys()};
      for (int i = 0; i < count ;++i)
      {
         map->remove(keys.at(row+i));
      }
      endRemoveRows();
   }

   // if metadata type is neither array or object an error has occured
   else
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Metadata Model"));
      e.setDetails(tr("Attempting to remove rows of metadata that is not array or object."));
      throw e;
   }

   // return success of operation
   return true;
}





// BROKEN.. setting what row??!?
bool Ace::MetadataModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
   if ( index.row() != 2 )
   {
      ;//ERROPR!!!
   }
   // make sure this is the edit role
   if ( role != Qt::EditRole )
   {
      return false;
   }

   // get pointer to metadata of index
   EMetadata* meta = reinterpret_cast<EMetadata*>(index.internalPointer());

   // set new value for metadata depending on what type it is
   if ( meta->isBool() )
   {
      *(meta->toBool()) = value.toBool();
   }
   else if ( meta->isDouble() )
   {
      bool ok {false};
      *(meta->toDouble()) = value.toDouble(&ok);

      // make sure floating point value was given correctly
      if ( !ok )
      {
         E_MAKE_EXCEPTION(e);
         e.setTitle(tr("Metadata Model"));
         e.setDetails(tr("Could not get floating point value from qt variant class."));
         throw e;
      }
   }
   else if ( meta->isString() )
   {
      *(meta->toString()) = value.toString();
   }
   else if ( meta->isBytes() )
   {
      *(meta->toBytes()) = value.toByteArray();
   }

   // if this metadata is of a type that does not hold data report error
   else
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Metadata Model"));
      e.setDetails(tr("Attempting to edit value of metadata that is an array or object."));
      throw e;
   }

   // return success of operation
   return true;
}






void Ace::MetadataModel::setRoot(EMetadata* root) noexcept
{
   // if root is not null make sure it is correct object type
   if ( root && !root->isObject() )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Metadata Model"));
      e.setDetails(tr("Attempting to root of metadata model that is not an object type."));
      throw e;
   }

   // set new root
   beginResetModel();
   _root = root;
   endResetModel();
}
