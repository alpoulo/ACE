#include <memory>
#include <QJsonDocument>

#include "Application.h"
#include "exception.h"
#include "abstractanalyticfactory.h"
#include "abstractanalytic.h"
#include "datamanager.h"
#include "datareference.h"
#include "dataobject.h"
#include "metadata.h"



using namespace std;






EApplication::EApplication(int& argc, char** argv, const QString& title
                                    , const QString& commandName):
   QCoreApplication(argc,argv),
   _argc(argc),
   _argv(argv)
{
   Q_UNUSED(title);
   Q_UNUSED(commandName);
   for (int i = 0; i < argc ;++i)
   {
      if ( i )
      {
         _command.append(' ');
      }
      _command.append(argv[i]);
   }
}






bool EApplication::notify(QObject* receiver, QEvent* event)
{
   try
   {
      return QCoreApplication::notify(receiver,event);
   }
   catch (EException e)
   {
      showException(e);
   }
   catch (std::exception e)
   {
      qDebug() << tr("STD exception %1 caught!\n").arg(e.what());
   }
   catch (...)
   {
      qDebug() << tr("Unknown exception caught!\n");
   }
   return false;
}






int EApplication::exec()
{
   try
   {
      if ( _argc > 1 )
      {
         QString which(_argv[1]);
         if ( which == QString("run") )
         {
            return run(_argc-2,&_argv[2]);
         }
         else if ( which == QString("dump"))
         {
            return dump(_argc-2,&_argv[2]);
         }
      }
      return -1;
   }
   catch (EException e)
   {
      showException(e);
      ::exit(-1);
   }
   catch (std::exception e)
   {
      qDebug() << tr("STD exception %1 caught!\n").arg(e.what());
      ::exit(-1);
   }
   catch (...)
   {
      qDebug() << tr("Unknown exception caught!\n");
      ::exit(-1);
   }
}






void EApplication::exceptionThrown(QString file, int line, QString function, QString title
                                   , QString details)
{
   EException e;
   e.setFile(file);
   e.setLine(line);
   e.setFunction(function);
   e.setTitle(title);
   e.setDetails(details);
   showException(e);
   quit();
}






void EApplication::showException(const EException& e)
{
   QTextStream stream(stdout);
   stream << tr("CRITICAL ERROR\n\n");
   stream << e.getTitle() << tr("\n\n");
   stream << e.getDetails() << tr("\n\n");
   stream << tr("File: %1\nLine: %2\nFunction: %3\n").arg(e.getFile()).arg(e.getLine())
             .arg(e.getFunction());
}






int EApplication::run(int argc, char** argv)
{
   using Role = EAbstractAnalytic::Role;
   using Type = EAbstractAnalytic::ArgumentType;
   QTextStream stream(stdout);
   if ( argc <= 0 )
   {
      stream << tr("No arguments given for run command, exiting.\n");
      return -1;
   }
   QString analyticName(argv[0]);
   --argc;
   argv = &argv[1];
   unique_ptr<EAbstractAnalytic> analytic;
   EAbstractAnalyticFactory& factory {EAbstractAnalyticFactory::getInstance()};
   for (int i = 0; i < factory.getCount() ;++i)
   {
      if ( factory.getCommandName(i) == analyticName )
      {
         analytic = factory.make(i);
         break;
      }
   }
   if ( !analytic )
   {
      stream << tr("Cannot find analytic by the name \"%1\", exiting.\n").arg(analyticName);
      return -1;
   }
   if ( argc%2 )
   {
      stream << tr("Invalid number of analytic arguments, exiting.\n");
      return -1;
   }
   QMap<QString,int> argumentMap;
   for (int i = 0; i < analytic->getArgumentCount() ;++i)
   {
      argumentMap.insert(analytic->getArgumentData(i,Role::CommandLineName).toString(),i);
   }
   for (int i = 0; i < argc ;i+=2)
   {
      QString name(argv[i]);
      if ( name.at(0) != QChar('-') || name.at(1) != QChar('-') )
      {
         stream << tr("Invalid analytic argument \"%1\", exiting.\n").arg(name);
         return -1;
      }
      QString value(argv[i+1]);
      name.remove(0,2);
      auto j = argumentMap.constFind(name);
      if ( j == argumentMap.constEnd() )
      {
         stream << tr("Cannot find analytic argument \"--%1\", exiting.\n").arg(name);
         return -1;
      }
      switch (analytic->getArgumentData(*j))
      {
      case Type::Bool:
      {
         bool val {false};
         if ( value == QString("true") )
         {
            val = true;
         }
         analytic->setArgument(*j,val);
         break;
      }
      case Type::Integer:
      {
         bool ok {false};
         int val;
         val = value.toInt(&ok);
         if ( !ok )
         {
            stream << tr("Expected argument \"%1\" to be an integer, exiting.").arg(name);
            return -1;
         }
         if ( val < analytic->getArgumentData(*j,Role::Minimum).toInt()
              || val > analytic->getArgumentData(*j,Role::Maximum).toInt() )
         {
            stream << tr("Integer argument \"%1\" is out of bounds, exiting.").arg(name);
            return -1;
         }
         analytic->setArgument(*j,val);
         break;
      }
      case Type::Double:
      {
         bool ok {false};
         double val;
         val = value.toDouble(&ok);
         if ( !ok )
         {
            stream << tr("Expected argument \"%1\" to be a real number, exiting.").arg(name);
            return -1;
         }
         if ( val < analytic->getArgumentData(*j,Role::Minimum).toDouble()
              || val > analytic->getArgumentData(*j,Role::Maximum).toDouble() )
         {
            stream << tr("Real number argument \"%1\" is out of bounds, exiting.").arg(name);
            return -1;
         }
         analytic->setArgument(*j,val);
         break;
      }
      case Type::String:
         analytic->setArgument(*j,value);
         break;
      case Type::Combo:
         if ( !analytic->getArgumentData(*j,Role::ComboValues).toStringList().contains(value) )
         {
            stream << tr("Combo argument \"%1\" contains illegal value, exiting.").arg(name);
            return -1;
         }
         analytic->setArgument(*j,value);
         break;
      case Type::FileIn:
         analytic->addFileIn(*j,value);
         break;
      case Type::FileOut:
         analytic->addFileOut(*j,value);
         break;
      case Type::DataIn:
         analytic->addDataIn(*j,value,analytic->getArgumentData(i,Role::DataType).toUInt());
         break;
      case Type::DataOut:
         analytic->addDataOut(*j,value,analytic->getArgumentData(i,Role::DataType).toUInt());
         break;
      }
   }
   analytic->setCommand(_command);
   analytic->start();
   connect(analytic.get(),SIGNAL(exceptionThrown(QString,int,QString,QString,QString)),this
           ,SLOT(exceptionThrown(QString,int,QString,QString,QString)));
   connect(analytic.get(),SIGNAL(finished()),this,SLOT(quit()));
   return QCoreApplication::exec();
}






int EApplication::dump(int argc, char** argv)
{
   QTextStream stream(stdout);
   if ( argc <= 0 )
   {
      stream << tr("No arguments given for dump command, exiting.\n");
      return -1;
   }
   Ace::DataReference* ref = Ace::DataManager::getInstance().open(argv[0]);
   (*ref)->open();
   QJsonValue root = grabMetaValues((*ref)->getMeta());
   QJsonDocument doc(root.toObject());
   stream << doc.toJson();
   return 0;
}






QJsonValue EApplication::grabMetaValues(const EMetadata &meta)
{
   switch (meta.getType())
   {
   case EMetadata::Bool:
      return QJsonValue(*meta.toBool());
   case EMetadata::Double:
      return QJsonValue(*meta.toDouble());
   case EMetadata::String:
      return QJsonValue(*meta.toString());
   case EMetadata::Array:
   {
      QJsonArray ret;
      const EMetadata::List* list = meta.toArray();
      for (auto i = list->constBegin(); i != list->constEnd() ;++i)
      {
         ret.append(grabMetaValues(**i));
      }
      return QJsonValue(ret);
   }
   case EMetadata::Object:
   {
      QJsonObject ret;
      const EMetadata::Map* map = meta.toObject();
      for (auto i = map->constBegin(); i != map->constEnd() ;++i)
      {
         ret.insert(i.key(),grabMetaValues(**i));
      }
      return QJsonValue(ret);
   }
   default:
      return QJsonValue();
   }
}
