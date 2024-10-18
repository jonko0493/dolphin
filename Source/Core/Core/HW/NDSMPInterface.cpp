// Copyright 2024 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later
// Code borrowed from melonDS

#include "NDSMPInterface.h"
#include "NDSLan.h"

namespace NDS
{
class DummyMP : public MPInterface
{
public:
  void Process() override {}

  void Begin(int inst) override {}
  void End(int inst) override {}

  int SendPacket(int inst, u8* data, int len, u64 timestamp) override { return 0; }
  int RecvPacket(int inst, u8* data, u64* timestamp) override { return 0; }
  int SendCmd(int inst, u8* data, int len, u64 timestamp) override { return 0; }
  int SendReply(int inst, u8* data, int len, u64 timestamp, u16 aid) override { return 0; }
  int SendAck(int inst, u8* data, int len, u64 timestamp) override { return 0; }
  int RecvHostPacket(int inst, u8* data, u64* timestamp) override { return 0; }
  u16 RecvReplies(int inst, u8* data, u64 timestamp, u16 aidmask) override { return 0; }
};

std::unique_ptr<MPInterface> MPInterface::Current(std::make_unique<DummyMP>());
MPInterfaceType MPInterface::CurrentType = MPInterface_Dummy;

void MPInterface::Set(MPInterfaceType type)
{
  switch (type)
  {
  //case MPInterface_Local:
  //  Current = std::make_unique<LocalMP>();
  //  break;

  case MPInterface_LAN:
    Current = std::make_unique<LAN>();
    break;

  default:
    Current = std::make_unique<DummyMP>();
    break;
  }

  CurrentType = type;
}
}  // namespace NDS
