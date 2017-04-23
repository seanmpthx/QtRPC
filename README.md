# QtRPC
qt rpc ....

   RPCItemA <=======> LAN <=======> RPCItemB


1.build a.QtRPC.  b.QtRPCCo.

2.create headerfile. MyRpc.h

    #ifndef MYRCP_H
    #define MYRCP_H
    #include "QtRPCManager.h"
    class MyRpc : public QtRPCItem
    {
    public:
      explicit MyRPC(SocketType stype = SocketTcp, ConnectionType ctype = ConnectionPeer, 
                     const QString &identifier = QString()) : QtRPCItem(stype, ctype, identifier){}
      explicit MyRPC(QtRPCNetwork *network) : QtRPCItem(network){}

      void TestFunction1(int type);
      QString testFunction2();
    }
    #endif // MYRCP_H

3.command line: QtRPCCo.exe MyRpc.h
  result: MyRpc.h, myrpc_rpc_callbacks.h, myrpc_rpc_functions.cpp
  
  add files to your project.
  
  project A (MyRpc.h, myrpc_rpc_functions.cpp),  project B (myrpc_rpc_callbacks.h)
  
4.In project B, add code like this: myRpc->registerSlot(this, "TestFunction1", rpc_functoin_testfunction1);

  In project A, call myRpc->TestFunction1(111); 

