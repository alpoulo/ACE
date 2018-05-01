#ifndef ACE_ANALYTIC_MPIMASTER_H
#define ACE_ANALYTIC_MPIMASTER_H
#include "ace_analytic_abstractmanager.h"
#include "ace_analytic_abstractinput.h"
//



namespace Ace
{
   namespace Analytic
   {
      /*!
       * This is the MPI master manager. This manager is used for an MPI run with the 
       * process that has rank 0 in other words master. The main function of this class 
       * is to create new work blocks and assign their execution to other slave node 
       * processes within the MPI run, along with getting results back and saving them to 
       * the manager's underlying analytic with the indexes being in order. Because this 
       * manager does not process blocks itself it has no abstract run object of its own. 
       * Also this manager does not start giving out work blocks to slave nodes until 
       * they signal they are ready to process them. 
       */
      class MPIMaster : public AbstractManager, public AbstractInput
      {
         Q_OBJECT
      public:
         /*!
          * Defines special codes passed between the master node and slave nodes. 
          */
         enum Code
         {
            /*!
             * Defines the terminate code which is sent to a slave node to signal it is done 
             * and should exit. 
             */
            Terminate = -1
            /*!
             * Defines the ready as serial code which is sent to the master node by a slave 
             * node to signal it is ready to execute work blocks using a serial run object. 
             */
            ,ReadyAsSerial = -2
            /*!
             * Defines the ready as ACU code which is sent to the master node by a slave node 
             * to signal it is ready to execute work blocks using an OpenCL run object. 
             */
            ,ReadyAsACU = -3
         };
         MPIMaster(quint16 type);
         virtual ~MPIMaster() override final;
         virtual bool isFinished() const override final;
      protected:
         virtual int index() const override final;
         virtual void writeResult(std::unique_ptr<EAbstractAnalytic::Block>&& result) override final;
      private slots:
         void dataReceived(const QByteArray& data, int fromRank);
      private:
         void processCode(int code, int fromRank);
         void process(const QByteArray& data, int fromRank);
         void terminate(int rank);
         /*!
          * Reference to the singleton MPI object. 
          */
         QMPI& _mpi;
         /*!
          * Next work block index to be processed to maintain order. 
          */
         int _nextWork {0};
         /*!
          * Next result block index to be processed to maintain order. 
          */
         int _nextResult {0};
      };
   }
}



#endif