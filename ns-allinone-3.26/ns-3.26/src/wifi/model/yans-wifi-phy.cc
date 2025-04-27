/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 *          Ghada Badawy <gbadawy@gmail.com>
 *          Sébastien Deronne <sebastien.deronne@gmail.com>
 */

#include "yans-wifi-phy.h"
#include "yans-wifi-channel.h"
#include "wifi-phy-state-helper.h"
//ab
#include "wifi-mac-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/internet-module.h" 
#include "ns3/header.h"
#include "ns3/ipv4-address.h"
//#include "../../../scratch/MyHeader.h"
//k
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ampdu-tag.h"
#include "ns3/node.h"
#include "constantes.h"
#include <cmath>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("YansWifiPhy");

NS_OBJECT_ENSURE_REGISTERED (YansWifiPhy);

TypeId
YansWifiPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::YansWifiPhy")
    .SetParent<WifiPhy> ()
    .SetGroupName ("Wifi")
    .AddConstructor<YansWifiPhy> ()
  ;
  return tid;
}

YansWifiPhy::YansWifiPhy ()
{
  NS_LOG_FUNCTION (this);
}

YansWifiPhy::~YansWifiPhy ()
{
  NS_LOG_FUNCTION (this);
}

void
YansWifiPhy::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_channel = 0;
}

bool
YansWifiPhy::DoChannelSwitch (uint16_t nch)
{
  if (!IsInitialized ())
    {
      //this is not channel switch, this is initialization
      //NS_LOG_DEBUG ("initialize to channel " << nch);
      return true;
    }

  NS_ASSERT (!IsStateSwitching ());
  switch (m_state->GetState ())
    {
    case YansWifiPhy::RX:
      //NS_LOG_DEBUG ("drop packet because of channel switching while reception");
      m_endPlcpRxEvent.Cancel ();
      m_endRxEvent.Cancel ();
      goto switchChannel;
      break;
    case YansWifiPhy::TX:
      //NS_LOG_DEBUG ("channel switching postponed until end of current transmission");
      Simulator::Schedule (GetDelayUntilIdle (), &WifiPhy::SetChannelNumber, this, nch);
      break;
    case YansWifiPhy::CCA_BUSY:
    case YansWifiPhy::IDLE:
      goto switchChannel;
      break;
    case YansWifiPhy::SLEEP:
      //NS_LOG_DEBUG ("channel switching ignored in sleep mode");
      break;
    default:
      NS_ASSERT (false);
      break;
    }

  return false;

switchChannel:

  //NS_LOG_DEBUG ("switching channel " << GetChannelNumber () << " -> " << nch);
  m_state->SwitchToChannelSwitching (GetChannelSwitchDelay ());
  m_interference.EraseEvents ();
  /*
   * Needed here to be able to correctly sensed the medium for the first
   * time after the switching. The actual switching is not performed until
   * after m_channelSwitchDelay. Packets received during the switching
   * state are added to the event list and are employed later to figure
   * out the state of the medium after the switching.
   */
  return true;
}

bool
YansWifiPhy::DoFrequencySwitch (uint32_t frequency)
{
  if (!IsInitialized ())
    {
      //this is not channel switch, this is initialization
      NS_LOG_DEBUG ("start at frequency " << frequency);
      return true;
    }

  NS_ASSERT (!IsStateSwitching ());
  switch (m_state->GetState ())
    {
    case YansWifiPhy::RX:
      //NS_LOG_DEBUG ("drop packet because of channel/frequency switching while reception");
      m_endPlcpRxEvent.Cancel ();
      m_endRxEvent.Cancel ();
      goto switchFrequency;
      break;
    case YansWifiPhy::TX:
      //NS_LOG_DEBUG ("channel/frequency switching postponed until end of current transmission");
      Simulator::Schedule (GetDelayUntilIdle (), &WifiPhy::SetFrequency, this, frequency);
      break;
    case YansWifiPhy::CCA_BUSY:
    case YansWifiPhy::IDLE:
      goto switchFrequency;
      break;
    case YansWifiPhy::SLEEP:
      //NS_LOG_DEBUG ("frequency switching ignored in sleep mode");
      break;
    default:
      NS_ASSERT (false);
      break;
    }

  return false;

switchFrequency:

  //NS_LOG_DEBUG ("switching frequency " << GetFrequency () << " -> " << frequency);
  m_state->SwitchToChannelSwitching (GetChannelSwitchDelay ());
  m_interference.EraseEvents ();
  /*
   * Needed here to be able to correctly sensed the medium for the first
   * time after the switching. The actual switching is not performed until
   * after m_channelSwitchDelay. Packets received during the switching
   * state are added to the event list and are employed later to figure
   * out the state of the medium after the switching.
   */
  return true;
}

Ptr<WifiChannel>
YansWifiPhy::GetChannel (void) const
{
  return m_channel;
}

void
YansWifiPhy::SetChannel (Ptr<YansWifiChannel> channel)
{
  m_channel = channel;
  m_channel->Add (this);
}

void
YansWifiPhy::SetSleepMode (void)
{
  NS_LOG_FUNCTION (this);
  switch (m_state->GetState ())
    {
    case YansWifiPhy::TX:
      //NS_LOG_DEBUG ("setting sleep mode postponed until end of current transmission");
      Simulator::Schedule (GetDelayUntilIdle (), &YansWifiPhy::SetSleepMode, this);
      break;
    case YansWifiPhy::RX:
      //NS_LOG_DEBUG ("setting sleep mode postponed until end of current reception");
      Simulator::Schedule (GetDelayUntilIdle (), &YansWifiPhy::SetSleepMode, this);
      break;
    case YansWifiPhy::SWITCHING:
      //NS_LOG_DEBUG ("setting sleep mode postponed until end of channel switching");
      Simulator::Schedule (GetDelayUntilIdle (), &YansWifiPhy::SetSleepMode, this);
      break;
    case YansWifiPhy::CCA_BUSY:
    case YansWifiPhy::IDLE:
      //NS_LOG_DEBUG ("setting sleep mode");
      m_state->SwitchToSleep ();
      break;
    case YansWifiPhy::SLEEP:
      //NS_LOG_DEBUG ("already in sleep mode");
      break;
    default:
      NS_ASSERT (false);
      break;
    }
}

void
YansWifiPhy::ResumeFromSleep (void)
{
  NS_LOG_FUNCTION (this);
  switch (m_state->GetState ())
    {
    case YansWifiPhy::TX:
    case YansWifiPhy::RX:
    case YansWifiPhy::IDLE:
    case YansWifiPhy::CCA_BUSY:
    case YansWifiPhy::SWITCHING:
      {
        //NS_LOG_DEBUG ("not in sleep mode, there is nothing to resume");
        break;
      }
    case YansWifiPhy::SLEEP:
      {
        //NS_LOG_DEBUG ("resuming from sleep mode");
        Time delayUntilCcaEnd = m_interference.GetEnergyDuration (DbmToW (GetCcaMode1Threshold ()));
        m_state->SwitchFromSleep (delayUntilCcaEnd);
        break;
      }
    default:
      {
        NS_ASSERT (false);
        break;
      }
    }
}

void
YansWifiPhy::SetReceiveOkCallback (RxOkCallback callback)
{
  m_state->SetReceiveOkCallback (callback);
}

void
YansWifiPhy::SetReceiveErrorCallback (RxErrorCallback callback)
{
  m_state->SetReceiveErrorCallback (callback);
}

void
YansWifiPhy::StartReceivePreambleAndHeader (Ptr<Packet> packet,
                                            double rxPowerDbm,
                                            WifiTxVector txVector,
                                            enum WifiPreamble preamble,
                                            enum mpduType mpdutype,
                                            Time rxDuration)
{
  //This function should be later split to check separately whether plcp preamble and plcp header can be successfully received.
  //Note: plcp preamble reception is not yet modeled.
  NS_LOG_FUNCTION (this << packet << rxPowerDbm << txVector.GetMode () << preamble << (uint32_t)mpdutype);
  AmpduTag ampduTag;
  rxPowerDbm += GetRxGain ();
  double rxPowerW = DbmToW (rxPowerDbm);
  Time endRx = Simulator::Now () + rxDuration;
  Time preambleAndHeaderDuration = CalculatePlcpPreambleAndHeaderDuration (txVector, preamble);

  int nodeID = -500;
  //Ptr<NetDevice> thisDevice = this->GetDevice();
  Ptr<NetDevice> thisDevice= DynamicCast<NetDevice>(this->GetDevice());
  Ptr<Node> thisNode = DynamicCast<Node>(thisDevice->GetNode());
  if(thisNode != NULL) 
  {
   nodeID= thisNode->GetId ();
  }
  Time tempsActuel = Simulator::Now ();
  Ptr<InterferenceHelper::Event> event;
  event = m_interference.Add (packet->GetSize (),
                              txVector,
                              preamble,
                              rxDuration,
                              rxPowerW);
  WifiMacHeader macHdr; 
  Ptr<Packet> kacket = packet->Copy();
  kacket->RemoveHeader(macHdr);
  std::string pcktDATA = "DATA";
	
     int value;
     Mac48Address senderMac= macHdr.GetAddr2();
     if(senderMac.IsBroadcast() == false)
     {
       std::ostringstream stream;
       stream  << senderMac;
       std::string nodeDest = stream.str();
       nodeDest.erase(std::remove(nodeDest.begin(), nodeDest.end(), ':'), nodeDest.end());
       std::stringstream strHex;
       strHex << nodeDest;
       strHex >> std::hex >> value;
     }
     else
     {
        value=-9;
     }
     int senderNodeID = value-1;
 
  if((tempsActuel > Seconds(tempsBascule))&&(pcktDATA.compare(macHdr.GetTypeString())==0)&&(showCca==1))
  {
    NS_LOG_DEBUG(Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] receive signal of power: "<<rxPowerDbm<<" dBm ccaThreshold: "<<GetCcaMode1Threshold()<<" dBm "<<macHdr.GetTypeString()<<" from Node["+std::to_string(senderNodeID)+"] arpEnd");
  }

  switch (m_state->GetState ())
    {
    case YansWifiPhy::SWITCHING:
      //NS_LOG_DEBUG ("drop packet because of channel switching");
      NotifyRxDrop (packet);
      m_plcpSuccess = false;
      /*
       * Packets received on the upcoming channel are added to the event list
       * during the switching state. This way the medium can be correctly sensed
       * when the device listens to the channel for the first time after the
       * switching e.g. after channel switching, the channel may be sensed as
       * busy due to other devices' tramissions started before the end of
       * the switching.
       */
      if (endRx > Simulator::Now () + m_state->GetDelayUntilIdle ())
        {
          //that packet will be noise _after_ the completion of the
          //channel switching.
          goto maybeCcaBusy;
        }
      break;
    case YansWifiPhy::RX:
      if ((nodeID == 0)&&(tempsActuel > Seconds(tempsBascule))){ NS_LOG_DEBUG (Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] from Node["+std::to_string(senderNodeID)+"] STATS drop packet because already in Rx (power=" <<rxPowerW << "W) ");}
      NotifyRxDrop (packet);
      if (endRx > Simulator::Now () + m_state->GetDelayUntilIdle ())
        {
          //that packet will be noise _after_ the reception of the
          //currently-received packet.
          goto maybeCcaBusy;
        }
      break;
    case YansWifiPhy::TX:
      if ((nodeID == 0)&&(tempsActuel > Seconds(tempsBascule))){ NS_LOG_DEBUG (Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] from Node["+std::to_string(senderNodeID)+"] STATS drop packet because already in Tx (power=" <<rxPowerW << "W) ");}
      NotifyRxDrop (packet);
      if (endRx > Simulator::Now () + m_state->GetDelayUntilIdle ())
        {
          //that packet will be noise _after_ the transmission of the
          //currently-transmitted packet.
          goto maybeCcaBusy;
        }
      break;
    case YansWifiPhy::CCA_BUSY:
    case YansWifiPhy::IDLE:
      if (rxPowerW > GetEdThresholdW ()) //checked here, no need to check in the payload reception (current implementation assumes constant rx power over the packet duration)
        {
		  if ((nodeID == 0)&&(tempsActuel > Seconds(tempsBascule))){ NS_LOG_DEBUG (Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] from Node["+std::to_string(senderNodeID)+"] STATS receiving in IDLE state ");}
          if (preamble == WIFI_PREAMBLE_NONE && (m_mpdusNum == 0 || m_plcpSuccess == false))
            {
              m_plcpSuccess = false;
              m_mpdusNum = 0;
              if ((nodeID == 0)&&(tempsActuel > Seconds(tempsBascule))){ NS_LOG_DEBUG (Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] from Node["+std::to_string(senderNodeID)+"] STATS drop packet because no PLCP preamble/header has been received");}
              NotifyRxDrop (packet);
              goto maybeCcaBusy;
            }
          else if (preamble != WIFI_PREAMBLE_NONE && packet->PeekPacketTag (ampduTag) && m_mpdusNum == 0)
            {
              //received the first MPDU in an MPDU
              m_mpdusNum = ampduTag.GetRemainingNbOfMpdus ();
              m_rxMpduReferenceNumber++;
            }
          else if (preamble == WIFI_PREAMBLE_NONE && packet->PeekPacketTag (ampduTag) && m_mpdusNum > 0)
            {
              //received the other MPDUs that are part of the A-MPDU
              if (ampduTag.GetRemainingNbOfMpdus () < (m_mpdusNum - 1))
                {
                  //NS_LOG_DEBUG ("Missing MPDU from the A-MPDU " << m_mpdusNum - ampduTag.GetRemainingNbOfMpdus ());
                  m_mpdusNum = ampduTag.GetRemainingNbOfMpdus ();
                }
              else
                {
                  m_mpdusNum--;
                }
            }
          else if (preamble != WIFI_PREAMBLE_NONE && packet->PeekPacketTag (ampduTag) && m_mpdusNum > 0)
            {
              //NS_LOG_DEBUG ("New A-MPDU started while " << m_mpdusNum << " MPDUs from previous are lost");
              m_mpdusNum = ampduTag.GetRemainingNbOfMpdus ();
            }
          else if (preamble != WIFI_PREAMBLE_NONE && m_mpdusNum > 0 )
            {
              //NS_LOG_DEBUG ("Didn't receive the last MPDUs from an A-MPDU " << m_mpdusNum);
              m_mpdusNum = 0;
            }

          //NS_LOG_DEBUG (Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] sync to signal (power=" << rxPowerW << "W)");
          //sync to signal
          m_state->SwitchToRx (rxDuration);
          NS_ASSERT (m_endPlcpRxEvent.IsExpired ());
          NotifyRxBegin (packet);
          m_interference.NotifyRxStart ();

          if (preamble != WIFI_PREAMBLE_NONE)
            {
              NS_ASSERT (m_endPlcpRxEvent.IsExpired ());
              m_endPlcpRxEvent = Simulator::Schedule (preambleAndHeaderDuration, &YansWifiPhy::StartReceivePacket, this,
                                                      packet, txVector, preamble, mpdutype, event);
            }

          NS_ASSERT (m_endRxEvent.IsExpired ());
          m_endRxEvent = Simulator::Schedule (rxDuration, &YansWifiPhy::EndReceive, this,
                                              packet, preamble, mpdutype, event);
        }
      else
        {
          if ((nodeID == 0)&&(tempsActuel > Seconds(tempsBascule))){ NS_LOG_DEBUG (Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] from Node["+std::to_string(senderNodeID)+"] STATS drop packet because signal power too Small (" <<rxPowerW << "<" << GetEdThresholdW () << ")");}
          NotifyRxDrop (packet);
          m_plcpSuccess = false;
          goto maybeCcaBusy;
        }
      break;
    case YansWifiPhy::SLEEP:
      //NS_LOG_DEBUG (Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] drop packet because in sleep mode");
      NotifyRxDrop (packet);
      m_plcpSuccess = false;
      break;
    }

  return;

maybeCcaBusy:
  //We are here because we have received the first bit of a packet and we are
  //not going to be able to synchronize on it
  //In this model, CCA becomes busy when the aggregation of all signals as
  //tracked by the InterferenceHelper class is higher than the CcaBusyThreshold

  Time delayUntilCcaEnd = m_interference.GetEnergyDuration (DbmToW (GetCcaMode1Threshold ()));
  if (!delayUntilCcaEnd.IsZero ())
    {
      m_state->SwitchMaybeToCcaBusy (delayUntilCcaEnd);
    }
}

void
YansWifiPhy::StartReceivePacket (Ptr<Packet> packet,
                                 WifiTxVector txVector,
                                 enum WifiPreamble preamble,
                                 enum mpduType mpdutype,
                                 Ptr<InterferenceHelper::Event> event)
{
  NS_LOG_FUNCTION (this << packet << txVector.GetMode () << preamble << (uint32_t)mpdutype);
  NS_ASSERT (IsStateRx ());
  NS_ASSERT (m_endPlcpRxEvent.IsExpired ());
  WifiMode txMode = txVector.GetMode ();

  //int nodeID = -500;
  //Ptr<NetDevice> thisDevice = this->GetDevice();
  Ptr<NetDevice> thisDevice= DynamicCast<NetDevice>(this->GetDevice());
  Ptr<Node> thisNode = DynamicCast<Node>(thisDevice->GetNode());
  /*if(thisNode != NULL) 
  {
   nodeID= thisNode->GetId ();
  }*/

  struct InterferenceHelper::SnrPer snrPer;
  snrPer = m_interference.CalculatePlcpHeaderSnrPer (event);

  //NS_LOG_DEBUG ("snr(dB)=" << RatioToDb (snrPer.snr) << ", per=" << snrPer.per);

  if (m_random->GetValue () > snrPer.per) //plcp reception succeeded
    {
      if (IsModeSupported (txMode) || IsMcsSupported (txMode))
        {
          //NS_LOG_DEBUG (Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] receiving plcp payload"); //endReceive is already scheduled
          m_plcpSuccess = true;
        }
      else //mode is not allowed
        {
          //NS_LOG_DEBUG (Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] drop packet because it was sent using an unsupported mode (" << txMode << ")");
          NotifyRxDrop (packet);
          m_plcpSuccess = false;
        }
    }
  else //plcp reception failed
    {
      //NS_LOG_DEBUG (Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] drop packet because plcp preamble/header reception failed");
      NotifyRxDrop (packet);
      m_plcpSuccess = false;
    }
}

void
YansWifiPhy::SendPacket (Ptr<const Packet> packet, WifiTxVector txVector, WifiPreamble preamble)
{ 
  SendPacket (packet, txVector, preamble, NORMAL_MPDU);
  //std::cout<<Simulator::Now ()<<" YansWifiPhy::SendPacket"<<std::endl;
}

void
YansWifiPhy::SendPacket (Ptr<const Packet> packet, WifiTxVector txVector, WifiPreamble preamble, enum mpduType mpdutype)
{
  int nodeID = -500;
 
  Ptr<NetDevice> thisDevice= DynamicCast<NetDevice>(this->GetDevice());
  Ptr<Node> thisNode = DynamicCast<Node>(thisDevice->GetNode());
 
  WifiMacHeader macHdr; 

  packet->PeekHeader(macHdr);
  //Ipv4Address addrIpSource= Ipv4Address::ConvertFrom(thisDevice->GetAddress());
  Mac48Address addrMacSource= Mac48Address::ConvertFrom(thisDevice->GetAddress());

 
  
  if(thisNode != NULL) 
  {
   nodeID= thisNode->GetId ();
  }
  
 
//partie pour mettre la puissance maximale au début pour l'échange ARP 

  //Time tempsBascule = NanoSeconds(+1500000000000.0);//cette valeur change en fonction du nombre de noeuds, faut penser à retrouver le nbrNodes à ce niveau (pas prioritaire!) 
  Time tempsActuel = Simulator::Now ();
  
  if(nodeID == 0)
  {
    txVector.SetTxPowerLevel(125);
  }
  else if (tempsActuel < Seconds(tempsBascule))
  {
     txVector.SetTxPowerLevel(125); 
  }
  else
  {
     txVector.SetTxPowerLevel(0); 
  }


//end

  NS_LOG_FUNCTION (this << packet << txVector.GetMode () 
    << txVector.GetMode ().GetDataRate (txVector)
    << preamble << (uint32_t)txVector.GetTxPowerLevel () << (uint32_t)mpdutype);
  /* Transmission can happen if:
   *  - we are syncing on a packet. It is the responsability of the
   *    MAC layer to avoid doing this but the PHY does nothing to
   *    prevent it.
   *  - we are idle
   */
  //NS_ASSERT (!m_state->IsStateTx () && !m_state->IsStateSwitching ());

  if (m_state->IsStateSleep ())
    {
      //NS_LOG_DEBUG ("Dropping packet because in sleep mode");
      NotifyTxDrop (packet);
      return;
    }

  Time txDuration = CalculateTxDuration (packet->GetSize (), txVector, preamble, GetFrequency (), mpdutype, 1);
  NS_ASSERT (txDuration > NanoSeconds (0));

  if (m_state->IsStateRx ())
    {
      m_endPlcpRxEvent.Cancel ();
      m_endRxEvent.Cancel ();
      m_interference.NotifyRxEnd ();
    }
    
     int value;
     Mac48Address senderMacDest = macHdr.GetAddr1();
     if(senderMacDest.IsBroadcast() == false)
     {
       std::ostringstream stream;
       stream  << senderMacDest;
       std::string nodeDest = stream.str();
       nodeDest.erase(std::remove(nodeDest.begin(), nodeDest.end(), ':'), nodeDest.end());
       std::stringstream strHex;
       strHex << nodeDest;
       strHex >> std::hex >> value;
     }
     else
     {
        value=-9;
     }
     int destNodeID = value-1;

  std::string arpEnd = "";
  std::string wakeUp = "";
  if(tempsActuel > Seconds(tempsBascule))
  {
    arpEnd = " arpEnd";
  }
  if((macHdr.IsRetry() == false) && (tempsActuel > Seconds(tempsBascule)))
  {
    wakeUp = " WakingUp";
  }
 /* std::string pcktType = macHdr.GetTypeString();
  std::string pcktCTL = "CTL_ACK";
  if((pcktType.compare(pcktCTL)==0) && (Simulator::Now () > Seconds(tempsBascule)))
  {
    NS_LOG_DEBUG(Simulator::Now ()<<" block ack!!!");
  }
  else*/
  {
    NotifyTxBegin (packet);
    NS_LOG_DEBUG(Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] ["<<addrMacSource<<"] Transmitting "<< macHdr.GetTypeString() <<" frame PHY layer to Node["<<destNodeID<<"] ["<<macHdr.GetAddr1()<<"] | TxPower: "<<GetPowerDbm(txVector.GetTxPowerLevel())<<" Simu: "<<arpEnd<<" Retry: "<<macHdr.IsRetry()<<wakeUp);//<<"  "<<addrIpSource);
   // NS_LOG_DEBUG("                SENDER MAC HEADER | Type: "<<macHdr.GetTypeString()<<" MAC_1: "<<macHdr.GetAddr1()<<" MAC_2: "<<macHdr.GetAddr2()<<" MAC_3: "<<macHdr.GetAddr3()<<" MAC_4: "<<macHdr.GetAddr4());
     //NS_LOG_DEBUG("               txDuration:"<<txDuration);      

    //NS_LOG_DEBUG("NEW_MAC_1: "<<newMac.GetAddr1()<<" NEW_MAC_2: "<<newMac.GetAddr2()<<" NEW_MAC_3: "<<newMac.GetAddr3()<<" NEW_MAC_4: "<<newMac.GetAddr4());
    uint32_t dataRate500KbpsUnits;
    if (txVector.GetMode ().GetModulationClass () == WIFI_MOD_CLASS_HT || txVector.GetMode ().GetModulationClass () == WIFI_MOD_CLASS_VHT)
      {
        dataRate500KbpsUnits = 128 + txVector.GetMode ().GetMcsValue ();
      }
    else
      {
        dataRate500KbpsUnits = txVector.GetMode ().GetDataRate (txVector.GetChannelWidth (), txVector.IsShortGuardInterval (), 1) * txVector.GetNss () / 500000;
      }
    if (mpdutype == MPDU_IN_AGGREGATE && preamble != WIFI_PREAMBLE_NONE)
      {
        //send the first MPDU in an MPDU
        m_txMpduReferenceNumber++;
      }



    struct mpduInfo aMpdu;
    aMpdu.type = mpdutype;
    aMpdu.mpduRefNumber = m_txMpduReferenceNumber;
    NotifyMonitorSniffTx (packet, (uint16_t)GetFrequency (), GetChannelNumber (), dataRate500KbpsUnits, preamble, txVector, aMpdu);
    m_state->SwitchToTx (txDuration, packet, GetPowerDbm (txVector.GetTxPowerLevel ()), txVector, preamble);
    m_channel->Send (this, packet, GetPowerDbm (txVector.GetTxPowerLevel ()) + GetTxGain (), txVector, preamble, mpdutype, txDuration);
    //std::cout<<Simulator::Now ()<<" node["<<nodeID<<"] YansWifiPhy::SendPacket"<<std::endl;
  }
}

void
YansWifiPhy::RegisterListener (WifiPhyListener *listener)
{
  m_state->RegisterListener (listener);
}

void
YansWifiPhy::UnregisterListener (WifiPhyListener *listener)
{
  m_state->UnregisterListener (listener);
}
                                                    
void
YansWifiPhy::EndReceive (Ptr<Packet> packet, enum WifiPreamble preamble, enum mpduType mpdutype, Ptr<InterferenceHelper::Event> event)
{
  NS_LOG_FUNCTION (this << packet << event);
  NS_ASSERT (IsStateRx ());
  NS_ASSERT (event->GetEndTime () == Simulator::Now ());

  //std::string pcktDATA = "DATA";                                                                                                                                

  int nodeID = -500;
  //Ptr<NetDevice> thisDevice = this->GetDevice();
  Ptr<NetDevice> thisDevice= DynamicCast<NetDevice>(this->GetDevice());
  Ptr<Node> thisNode = DynamicCast<Node>(thisDevice->GetNode());
  Mac48Address myMac = Mac48Address::ConvertFrom(thisDevice->GetAddress());

  if(thisNode != NULL) 
  {
   nodeID= thisNode->GetId ();
  }

  struct InterferenceHelper::SnrPer snrPer;
  snrPer = m_interference.CalculatePlcpPayloadSnrPer (event);
  m_interference.NotifyRxEnd ();

  if (m_plcpSuccess == true)
    {
      //NS_LOG_DEBUG ("mode=" << (event->GetPayloadMode ().GetDataRate (event->GetTxVector ())) <<
                 //   ", snr(dB)=" << RatioToDb (snrPer.snr) << ", per=" << snrPer.per << ", size=" << packet->GetSize ());

      if (m_random->GetValue () > snrPer.per)
        { 
          //ab
          WifiMacHeader macHdr; 
          //Ipv4Header ipHdr; 

          Ptr<Packet> kacket = packet->Copy();

          Ptr<Packet> copy = packet->Copy();

          /*std::filebuf fb;
          fb.open ("test.txt",std::ios::out);
          std::ostream os(&fb);
          packet->Print(os);
          fb.close();*/
          kacket->RemoveHeader(macHdr);
          Mac48Address addrMacDest=macHdr.GetAddr1();
          Mac48Address addrMacSource2=macHdr.GetAddr2();
          //Mac48Address addrMacSource3=macHdr.GetAddr3();
          //Mac48Address addrMacSource4=macHdr.GetAddr4();
          std::ostringstream stream;
          stream  << addrMacSource2;
          std::string nodeSource = stream.str();
          nodeSource.erase(std::remove(nodeSource.begin(), nodeSource.end(), ':'), nodeSource.end());
          std::stringstream strHex;
          strHex << nodeSource;
          int value;
          strHex >> std::hex >> value;
          int senderNodeID = value-1;

          //Time tempsBascule = NanoSeconds(+1500000000000.0);//cette valeur change en fonction du nombre de noeuds, faut penser à retrouver le nbrNodes à ce niveau (pas prioritaire!) 
          Time tempsActuel = Simulator::Now ();
          std::string arpEnd = "";
          if(tempsActuel > Seconds(tempsBascule))
          {
            arpEnd = " arpEnd";
          }
          //k
          if(addrMacDest == myMac)
          { 
            struct signalNoiseDbm signalNoise;
            signalNoise.signal = RatioToDb (event->GetRxPowerW ()) + 30;            
            NS_LOG_DEBUG(Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] ["<<myMac<<"] Receive OK FOR ME from Node["<<senderNodeID<<"] ["<<addrMacSource2<<"] Frame type: "<<macHdr.GetTypeString()<<" Power: "<<signalNoise.signal<<" dBm"<<" ccaThreshold: "<<GetCcaMode1Threshold()<<" dBm"<<arpEnd);//<<addrIpSource);

           // std::cout<< "                 RECEIVER MAC HEADER | Type: "<<macHdr.GetTypeString()<<" mac1: "<< addrMacDest<<" mac2: "<< addrMacSource2<<" mac3: "<<addrMacSource3<<"  mac4: "<< addrMacSource4  << std::endl<<std::endl; 
         }
          /*else if (tempsActuel > Seconds(tempsBascule))//&&(pcktDATA.compare(macHdr.GetTypeString())==0))
          { 
            struct signalNoiseDbm signalNoise;
            signalNoise.signal = RatioToDb (event->GetRxPowerW ()) + 30;
            NS_LOG_DEBUG(Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] ["<<myMac<<"] Receive OK from Node["<<senderNodeID<<"] ["<<addrMacSource2<<"] Frame type: "<<macHdr.GetTypeString()<<" Power: "<<signalNoise.signal<<" dBm"<<" ccaThreshold: "<<GetCcaMode1Threshold()<<" dBm"<<arpEnd);//<<addrIpSource);

            //std::cout<< "                 RECEIVER MAC HEADER | Type: "<<macHdr.GetTypeString()<<" mac1: "<< addrMacDest<<" mac2: "<< addrMacSource2<<" mac3: "<<addrMacSource3<<"  mac4: "<< addrMacSource4  << std::endl<<std::endl; 
         }*/
          NotifyRxEnd (packet);
          uint32_t dataRate500KbpsUnits;
          if ((event->GetPayloadMode ().GetModulationClass () == WIFI_MOD_CLASS_HT) || (event->GetPayloadMode ().GetModulationClass () == WIFI_MOD_CLASS_VHT))
            {
              dataRate500KbpsUnits = 128 + event->GetPayloadMode ().GetMcsValue ();
            }
          else
            {
              dataRate500KbpsUnits = event->GetPayloadMode ().GetDataRate (event->GetTxVector ().GetChannelWidth (), event->GetTxVector ().IsShortGuardInterval (), 1) * event->GetTxVector ().GetNss () / 500000;
            }
          struct signalNoiseDbm signalNoise;
          signalNoise.signal = RatioToDb (event->GetRxPowerW ()) + 30;
          signalNoise.noise = RatioToDb (event->GetRxPowerW () / snrPer.snr) - GetRxNoiseFigure () + 30;
          struct mpduInfo aMpdu;
          aMpdu.type = mpdutype;
          aMpdu.mpduRefNumber = m_rxMpduReferenceNumber;
          NotifyMonitorSniffRx (packet, (uint16_t)GetFrequency (), GetChannelNumber (), dataRate500KbpsUnits, event->GetPreambleType (), event->GetTxVector (), aMpdu, signalNoise);
          m_state->SwitchFromRxEndOk (packet, snrPer.snr, event->GetTxVector (), event->GetPreambleType ());
        }
      else
        {
          /* failure. */
          if (nodeID==0)
          {
            NS_LOG_DEBUG(Simulator::Now ()<<" Node["+std::to_string(nodeID)+"] Recv fail");
          }
          NotifyRxDrop (packet);
          m_state->SwitchFromRxEndError (packet, snrPer.snr);
        }
    }
  else
    {
      m_state->SwitchFromRxEndError (packet, snrPer.snr);
    }

  if (preamble == WIFI_PREAMBLE_NONE && mpdutype == LAST_MPDU_IN_AGGREGATE)
    {
      m_plcpSuccess = false;
    }
}

} //namespace ns3
