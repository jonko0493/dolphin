// Copyright 2024 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later
// Code borrowed from melonDS

#pragma once

#include <map>
#include <queue>
#include <string>
#include <vector>

#include <enet/enet.h>

#ifndef socket_t
  #ifdef WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>

    #define socket_t SOCKET
    #define sockaddr_t SOCKADDR
    #define sockaddr_in_t SOCKADDR_IN
  #else
    #include <netinet/in.h>
    #include <sys/select.h>
    #include <sys/socket.h>
    #include <unistd.h>

    #define socket_t int
    #define sockaddr_t struct sockaddr
    #define sockaddr_in_t struct sockaddr_in
    #define closesocket close
  #endif
#endif

#include "Common/CommonTypes.h"
#include "Common/Timer.h"
#include "NDSMPInterface.h"

namespace NDS
{
class LAN : public MPInterface
{
public:
  LAN() noexcept;
  LAN(const LAN&) = delete;
  LAN& operator=(const LAN&) = delete;
  LAN(LAN&& other) = delete;
  LAN& operator=(LAN&& other) = delete;
  ~LAN() noexcept;

  enum PlayerStatus
  {
    Player_None = 0,      // no player in this entry
    Player_Client,        // game client
    Player_Host,          // game host
    Player_Connecting,    // player still connecting
    Player_Disconnected,  // player disconnected
  };

  struct Player
  {
    int ID;
    char Name[32];
    PlayerStatus Status;
    u32 Address;

    bool IsLocalPlayer;
    u32 Ping;
  };

  struct DiscoveryData
  {
    u32 Magic;
    u32 Version;
    u32 Tick;
    char SessionName[64];
    u8 NumPlayers;
    u8 MaxPlayers;
    u8 Status;  // 0=idle 1=playing
  };

  bool StartDiscovery();
  void EndDiscovery();
  bool StartHost(const char* player, int numplayers);
  bool StartClient(const char* player, const char* host);
  void EndSession();

  std::map<u32, DiscoveryData> GetDiscoveryList();
  std::vector<Player> GetPlayerList();
  int GetNumPlayers() { return NumPlayers; }
  int GetMaxPlayers() { return MaxPlayers; }

  void Process() override;

  void Begin(int inst) override;
  void End(int inst) override;

  int SendPacket(int inst, u8* data, int len, u64 timestamp) override;
  int RecvPacket(int inst, u8* data, u64* timestamp) override;
  int SendCmd(int inst, u8* data, int len, u64 timestamp) override;
  int SendReply(int inst, u8* data, int len, u64 timestamp, u16 aid) override;
  int SendAck(int inst, u8* data, int len, u64 timestamp) override;
  int RecvHostPacket(int inst, u8* data, u64* timestamp) override;
  u16 RecvReplies(int inst, u8* data, u64 timestamp, u16 aidmask) override;

private:
  bool Inited;
  bool Active;
  bool IsHost;

  ENetHost* Host;
  ENetPeer* RemotePeers[16];

  socket_t DiscoverySocket;
  u32 DiscoveryLastTick;
  std::map<u32, DiscoveryData> DiscoveryList;
  //Platform::Mutex* DiscoveryMutex;

  Player Players[16];
  int NumPlayers;
  int MaxPlayers;
  //Platform::Mutex* PlayersMutex;

  Player MyPlayer;
  u32 HostAddress;

  u16 ConnectedBitmask;

  int MPRecvTimeout;
  int LastHostID;
  ENetPeer* LastHostPeer;
  std::queue<ENetPacket*> RXQueue;

  u32 FrameCount;

  void ProcessDiscovery();

  void HostUpdatePlayerList();
  void ClientUpdatePlayerList();

  void ProcessHostEvent(ENetEvent& event);
  void ProcessClientEvent(ENetEvent& event);
  void ProcessEvent(ENetEvent& event);
  void ProcessLAN(int type);

  int SendPacketGeneric(u32 type, u8* packet, int len, u64 timestamp);
  int RecvPacketGeneric(u8* packet, bool block, u64* timestamp);
};
}  // namespace NDS
