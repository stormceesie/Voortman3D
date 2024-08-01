#include "TwinCATConnection.hpp"

namespace Voortman3D {
  TwinCATConnection::~TwinCATConnection() {
    // Release all the variable handles
    for (auto& variableHandle : variableHandles.values()) _LIKELY {
      AdsSyncWriteReq(&Addr, ADSIGRP_SYM_RELEASEHND, 0, sizeof(variableHandle.second), (void*)&variableHandle.second);
    }

    DisconnectFromTwinCAT();
  }

  void TwinCATConnection::ConnectToTwinCAT() {
    long nErr, nPort;

    nPort = AdsPortOpen();
    nErr = AdsGetLocalAddress(&Addr);

#ifdef _DEBUG
    // Use '\n' over std::endl because std::endl will also flush
    std::cout << "AdsOpenPort: " << nPort << '\n';

    if (nErr) _UNLIKELY {
      std::cerr << "Error: AdsGetLocalAddress: " << nErr << "\n";
      return; // AdsPort will be closed at the end of the program
    }
#endif

    Addr.port = 851;

#ifdef _DEBUG
    std::cout << "Connected to TwinCAT\n";
#endif
  }

  void TwinCATConnection::DisconnectFromTwinCAT() {
    AdsPortClose();
  }

  void TwinCATConnection::CreateVariableHandle(const uint32_t key) {
    if (variableHandles.contains(key)) _UNLIKELY return; // Already contains the key -> return
    
    ULONG VariableHandle;


    char szVar[] = { "MAIN.fSawHeight" };

    long nErr = AdsSyncReadWriteReq(&Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(VariableHandle), &VariableHandle, sizeof(szVar), szVar);

    if (!nErr) _LIKELY{ // If there is no error while getting the variable handle
      variableHandles.emplace(key, VariableHandle);
#ifdef _DEBUG
      std::cout << "Variable handle for " << szVar << " at " << VariableHandle << '\n';
#endif
    }
    else _UNLIKELY{
#ifdef _DEBUG
      std::cerr << "Error creating variable Handle: " << nErr << '\n';
#endif
      return;
    }
  }
}