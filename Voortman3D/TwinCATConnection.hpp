#pragma once
#include <Windows.h>
#include "TcAdsDef.h"
#include "TcAdsAPI.h"

#include "unordered_dense.h"

#include <iostream>

namespace Voortman3D {
  class TwinCATConnection {
  public:

    void ConnectToTwinCAT();
    void DisconnectFromTwinCAT();

    template <typename T>
    void ReadValue(uint32_t key, T* data) {
      if (!variableHandles.contains(key)) _UNLIKELY return;

      unsigned long variableHandle = this->variableHandles[key];

      AdsSyncReadReq(&Addr, ADSIGRP_SYM_VALBYHND, variableHandle, sizeof(T), data);
    };

    void CreateVariableHandle(const uint32_t key, char szVar[]);

    ~TwinCATConnection();

  private:
    AmsAddr Addr{};

    // Doesn't really matter in this example but some hashmaps are significantly faster than others for large quantities
    ankerl::unordered_dense::map<uint32_t, unsigned long> variableHandles;
  };
}