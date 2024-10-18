// Copyright 2024 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later
// Code borrowed from melonDS

#pragma once
#include <memory>
#include "Common/CommonTypes.h"

namespace NDS
{
  enum MPInterfaceType
  {
    MPInterface_Dummy = -1,
    MPInterface_Local,
    MPInterface_LAN,
    MPInterface_Netplay,
  };

  struct MPPacketHeader
  {
    u32 Magic;
    u32 SenderID;
    u32 Type;  // 0=regular 1=CMD 2=reply 3=ack
    u32 Length;
    u64 Timestamp;
  };

  class MPInterface
  {
  public:
    virtual ~MPInterface() = default;

    static MPInterface& Get() { return *Current; }
    static MPInterfaceType GetType() { return CurrentType; }
    static void Set(MPInterfaceType type);

    [[nodiscard]] int GetRecvTimeout() const noexcept { return RecvTimeout; }
    void SetRecvTimeout(int timeout) noexcept { RecvTimeout = timeout; }

    // function called every video frame
    virtual void Process() = 0;

    virtual void Begin(int inst) = 0;
    virtual void End(int inst) = 0;

    virtual int SendPacket(int inst, u8* data, int len, u64 timestamp) = 0;
    virtual int RecvPacket(int inst, u8* data, u64* timestamp) = 0;
    virtual int SendCmd(int inst, u8* data, int len, u64 timestamp) = 0;
    virtual int SendReply(int inst, u8* data, int len, u64 timestamp, u16 aid) = 0;
    virtual int SendAck(int inst, u8* data, int len, u64 timestamp) = 0;
    virtual int RecvHostPacket(int inst, u8* data, u64* timestamp) = 0;
    virtual u16 RecvReplies(int inst, u8* data, u64 timestamp, u16 aidmask) = 0;

  protected:
    int RecvTimeout = 25;

  private:
    static MPInterfaceType CurrentType;
    static std::unique_ptr<MPInterface> Current;
  };
}
