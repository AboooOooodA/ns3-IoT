/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 * Authors: Joe Kopena <tjkopena@cs.drexel.edu>
 *
 * This program conducts a simple experiment: It places two nodes at a
 * parameterized distance apart.  One node generates packets and the
 * other node receives.  The stat framework collects data on packet
 * loss.  Outside of this program, a control script uses that data to
 * produce graphs presenting performance at the varying distances.
 * This isn't a typical simulation but is a common "experiment"
 * performed in real life and serves as an accessible exemplar for the
 * stat framework.  It also gives some intuition on the behavior and
 * basic reasonability of the NS-3 WiFi models.
 *
 * Applications used by this program are in test02-apps.h and
 * test02-apps.cc, which should be in the same place as this file.
 * 
 */

#include <ctime>

#include <sstream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"

#include "ns3/stats-module.h"

#include "wifi-example-apps.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("WiFiDistanceExperiment");

void TxCallback (Ptr<CounterCalculator<uint32_t> > datac,
                 std::string path, Ptr<const Packet> packet) {
  NS_LOG_INFO ("Sent frame counted in " <<
               datac->GetKey ());
  datac->Update ();
  // end TxCallback
}




//----------------------------------------------------------------------
//-- main
//----------------------------------------------
int main (int argc, char *argv[]) {

  double distance = 50.0;
  string format ("omnet");
  int nbrNodes = 3;
  string experiment ("wifi-distance-test");
  string strategy ("wifi-default");
  string input;
  string runID;

  {
    stringstream sstr;
    sstr << "run-" << time (NULL);
    runID = sstr.str ();
  }

  // Set up command line parameters used to control the experiment.
  CommandLine cmd;
  cmd.AddValue ("distance", "Distance apart to place nodes (in meters).",
                distance);
  cmd.AddValue ("format", "Format to use for data output.",
                format);
  cmd.AddValue ("experiment", "Identifier for experiment.",
                experiment);
  cmd.AddValue ("strategy", "Identifier for strategy.",
                strategy);
  cmd.AddValue ("run", "Identifier for run.",
                runID);
  //ab
  cmd.AddValue("nbrNodes", "Total number of nodes in the network.",
              nbrNodes);
  //k

  cmd.Parse (argc, argv);

  if (format != "omnet" && format != "db") {
      NS_LOG_ERROR ("Unknown output format '" << format << "'");
      return -1;
    }

  #ifndef STATS_HAS_SQLITE3
  if (format == "db") {
      NS_LOG_ERROR ("sqlite support not compiled in.");
      return -1;
    }
  #endif

  {
    stringstream sstr ("");
    sstr << distance;
    input = sstr.str ();
  }

  //ab
  LogComponentEnable("WiFiDistanceApps",LOG_LEVEL_INFO);
  LogComponentEnable("YansWifiPhy",LOG_LEVEL_INFO);
  //LogComponentEnable("WiFiDistanceExperiment",LOG_LEVEL_INFO);
  //LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  //LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
  
  //k


  //------------------------------------------------------------
  //-- Create nodes and network stacks
  //--------------------------------------------
  NS_LOG_INFO ("Creating nodes.");
  NodeContainer nodes;
  nodes.Create (nbrNodes);

  NS_LOG_INFO ("Installing WiFi and Internet stack.");
  WifiHelper wifi;
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  NetDeviceContainer nodeDevices = wifi.Install (wifiPhy, wifiMac, nodes);

  //ab
  //Wifi Remote Manager
  Ptr<WifiNetDevice> ApD= DynamicCast<WifiNetDevice>(nodeDevices.Get(0));
  Ptr<WifiRemoteStationManager> ApManager=ApD->GetRemoteStationManager();


  //NUMBER OF RETRANSMISSION
  //ApManager->SetMaxSlrc(10);
  std::cout<<std::endl<<"Number of retransmission: "<<ApManager->GetMaxSlrc()<<std::endl;
    
  //RTS/CTS THRESHOLD
  //ApManager->SetRtsCtsThreshold(uint32_t);  
  std::cout<<"RTS/CTS Threshold: "<<ApManager->GetRtsCtsThreshold()<<std::endl;

  //CCA THRESHOLD
  Ptr<WifiPhy> wfManager=ApD->GetPhy();
  //wfManager->GetCcaMode1Threshold(double);
  std::cout<<"ccaMode1Threshold: "<<wfManager->GetCcaMode1Threshold()<<std::endl<<std::endl;
  //k

  InternetStackHelper internet;
  internet.Install (nodes);
  Ipv4AddressHelper ipAddrs;
  ipAddrs.SetBase ("192.168.0.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = ipAddrs.Assign (nodeDevices);




  //------------------------------------------------------------
  //-- Setup physical layout -- Répartition des Noeuds 
  //--------------------------------------------
  NS_LOG_INFO ("Installing static mobility; distance " << distance << " .");
  
  /*MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc =
    CreateObject<ListPositionAllocator>();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (0.0, distance, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (nodes);*/

  std::ostringstream strs;
  strs << distance;
  std::string strDistance = strs.str();

  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                    "X", DoubleValue (0.0),
                                    "Y", DoubleValue (0.0),
                                    "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max="+strDistance+"]"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  //print positions 

  for(int i=0;i<nbrNodes;i++)
  {
    Ptr<MobilityModel> mob = NodeList::GetNode(i)->GetObject<MobilityModel> ();
    //Ptr<Node> nodeI = NodeList::GetNode(i); nodeI->GetId() 
    if(i==0)
    {
      mob->SetPosition (Vector (0.0,0.0,0.0));
    }

    Vector pos = mob->GetPosition();
    double d = sqrt(pos.x*pos.x+pos.y*pos.y);
    std::cout<<"Node["<<i<<"]. Position("<<pos.x<<","<<pos.y<<","<<pos.z<<")   Distance to Node 0: "<< d <<std::endl; 
  }



  //------------------------------------------------------------
  //-- Create a custom traffic source and sink
  //--------------------------------------------
  NS_LOG_INFO ("Create traffic source & sink.");

                              /*Ptr<Node> appSource = NodeList::GetNode (1);
                              Ptr<Sender> sender = CreateObject<Sender>();
                              appSource->AddApplication (sender);
                              sender->SetStartTime (Seconds (1));

                              //ab
                              Ptr<Node> appSource_two = NodeList::GetNode (2);
                              Ptr<Sender> sender_two = CreateObject<Sender>();
                              appSource_two->AddApplication (sender_two);
                              sender_two->SetStartTime (Seconds (3));
                              //k*/
                              
  //ab
  for(int i=1;i<nbrNodes;i++)
  {
    Ptr<Node> appSource = NodeList::GetNode (i);
    Ptr<Sender> sender = CreateObject<Sender>();
    appSource->AddApplication (sender);
   // if(i<10)
    {
      sender->SetStartTime (Seconds (i));
    }
   // else 
    {
    //  sender->SetStartTime (Seconds (10));
    }
  }    
  //k

  Ptr<Node> appSink = NodeList::GetNode (0);
  Ptr<Receiver> receiver = CreateObject<Receiver>();
  appSink->AddApplication (receiver);
  receiver->SetStartTime (Seconds (0));
  
  Config::Set ("/NodeList/*/ApplicationList/*/$Sender/Destination",
               Ipv4AddressValue ("192.168.0.1"));


  //ab
  /*Ptr<Node> appSink_two = NodeList::GetNode (2);
  Ptr<Receiver> receiver_two = CreateObject<Receiver>();
  appSink_two->AddApplication (receiver_two);
  receiver_two->SetStartTime (Seconds (2)); */


  //Config::Set ("/NodeList/*/ApplicationList/*/$Sender/Destination",
             //  Ipv4AddressValue ("192.168.0.3"));

  //k


  //------------------------------------------------------------
  //-- Setup stats and data collection
  //--------------------------------------------

  // Create a DataCollector object to hold information about this run.
  DataCollector data;
  data.DescribeRun (experiment,
                    strategy,
                    input,
                    runID);

  // Add any information we wish to record about this run.
  data.AddMetadata ("author", "tjkopena");


  // Create a counter to track how many frames are generated.  Updates
  // are triggered by the trace signal generated by the WiFi MAC model
  // object.  Here we connect the counter to the signal via the simple
  // TxCallback() glue function defined above.

                              //Ptr<CounterCalculator<uint32_t> > totalTx =
                                //CreateObject<CounterCalculator<uint32_t> >();
                              //totalTx->SetKey ("wifi-tx-frames");
                             // totalTx->SetContext ("node[1]");
                              //Config::Connect ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTx",
                                        //       MakeBoundCallback (&TxCallback, totalTx));
                             // data.AddDataCalculator (totalTx);

                              //ab
                             // Ptr<CounterCalculator<uint32_t> > totalTx_two =
                              //  CreateObject<CounterCalculator<uint32_t> >();
                             // totalTx_two->SetKey ("wifi-tx-frames_TWO");
                             // totalTx_two->SetContext ("node[2]");
                              //Config::Connect ("/NodeList/2/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTx",
                                          //     MakeBoundCallback (&TxCallback, totalTx_two));
                              //data.AddDataCalculator (totalTx_two);
                              //k

  for(int i=1;i<nbrNodes;i++)
  {
    Ptr<CounterCalculator<uint32_t> > totalTx =
    CreateObject<CounterCalculator<uint32_t> >();
    totalTx->SetKey ("wifi-tx-frames");
    totalTx->SetContext ("node["+std::to_string(i)+"]");
    Config::Connect ("/NodeList/"+std::to_string(i)+"/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTx",
                   MakeBoundCallback (&TxCallback, totalTx));
    data.AddDataCalculator (totalTx);

  }

  // This is similar, but creates a counter to track how many frames
  // are received.  Instead of our own glue function, this uses a
  // method of an adapter class to connect a counter directly to the
  // trace signal generated by the WiFi MAC.
  Ptr<PacketCounterCalculator> totalRx =
    CreateObject<PacketCounterCalculator>();
  totalRx->SetKey ("wifi-rx-frames");
  totalRx->SetContext ("node[0]");
  Config::Connect ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
                   MakeCallback (&PacketCounterCalculator::PacketUpdate,
                                 totalRx));
  data.AddDataCalculator (totalRx);

  //ab
  /* Ptr<PacketCounterCalculator> totalRx_two =
    CreateObject<PacketCounterCalculator>();
  totalRx_two->SetKey ("wifi_two-rx-frames");
  totalRx_two->SetContext ("node[2]");*/
 // Config::Connect ("/NodeList/2/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
                  // MakeCallback (&PacketCounterCalculator::PacketUpdate,
                           //      totalRx_two));

  //data.AddDataCalculator (totalRx_two);

  //k


  // This counter tracks how many packets---as opposed to frames---are
  // generated.  This is connected directly to a trace signal provided
  // by our Sender class.

                              //Ptr<PacketCounterCalculator> appTx =
                                //CreateObject<PacketCounterCalculator>();
                              //appTx->SetKey ("sender-tx-packets");
                              //appTx->SetContext ("node[1]");
                              //Config::Connect ("/NodeList/1/ApplicationList/*/$Sender/Tx",
                                               //MakeCallback (&PacketCounterCalculator::PacketUpdate,
                                                          //   appTx));
                              //data.AddDataCalculator (appTx);

                              //ab
                              //Ptr<PacketCounterCalculator> appTx_two =
                              //CreateObject<PacketCounterCalculator>();
                              //appTx_two->SetKey ("sender-tx-packets_TWO");
                              //appTx_two->SetContext ("node[2]");
                              //Config::Connect ("/NodeList/2/ApplicationList/*/$Sender/Tx",
                                              // MakeCallback (&PacketCounterCalculator::PacketUpdate,
                                                          //   appTx_two));
                              //data.AddDataCalculator (appTx_two);

                              //k

  for(int i=1;i<nbrNodes;i++)
  {
    Ptr<PacketCounterCalculator> appTx =
    CreateObject<PacketCounterCalculator>();
    appTx->SetKey ("sender-tx-packets");
    appTx->SetContext ("node["+std::to_string(i)+"]");
    Config::Connect ("/NodeList/"+std::to_string(i)+"/ApplicationList/*/$Sender/Tx",
                    MakeCallback (&PacketCounterCalculator::PacketUpdate,
                                 appTx));
    data.AddDataCalculator (appTx);
  }

  // Here a counter for received packets is directly manipulated by
  // one of the custom objects in our simulation, the Receiver
  // Application.  The Receiver object is given a pointer to the
  // counter and calls its Update() method whenever a packet arrives.
  Ptr<CounterCalculator<> > appRx =
    CreateObject<CounterCalculator<> >();
  appRx->SetKey ("receiver-rx-packets");
  appRx->SetContext ("node[0]");
  receiver->SetCounter (appRx);
  data.AddDataCalculator (appRx);

  //ab
  /*Ptr<CounterCalculator<> > appRx_two =
    CreateObject<CounterCalculator<> >();
  appRx_two->SetKey ("receiver_two-rx-packets");
  appRx_two->SetContext ("node[2]");
  receiver_two->SetCounter (appRx_two);
  data.AddDataCalculator (appRx_two);*/


  //k


  /**
   * Just to show this is here...
   Ptr<MinMaxAvgTotalCalculator<uint32_t> > test = 
   CreateObject<MinMaxAvgTotalCalculator<uint32_t> >();
   test->SetKey("test-dc");
   data.AddDataCalculator(test);

   test->Update(4);
   test->Update(8);
   test->Update(24);
   test->Update(12);
  **/

  // This DataCalculator connects directly to the transmit trace
  // provided by our Sender Application.  It records some basic
  // statistics about the sizes of the packets received (min, max,
  // avg, total # bytes), although in this scenaro they're fixed.


                              //Ptr<PacketSizeMinMaxAvgTotalCalculator> appTxPkts =
                               // CreateObject<PacketSizeMinMaxAvgTotalCalculator>();
                              //appTxPkts->SetKey ("tx-pkt-size");
                              //appTxPkts->SetContext ("node[1]");
                              //Config::Connect ("/NodeList/1/ApplicationList/*/$Sender/Tx",
                                            //   MakeCallback
                                               //  (&PacketSizeMinMaxAvgTotalCalculator::PacketUpdate,
                                              //   appTxPkts));
                              //data.AddDataCalculator (appTxPkts);

                              //ab
                              //Ptr<PacketSizeMinMaxAvgTotalCalculator> appTxPkts_two =
                             //CreateObject<PacketSizeMinMaxAvgTotalCalculator>();
                              //appTxPkts_two->SetKey ("tx-pkt-size_TWO");
                             //appTxPkts_two->SetContext ("node[2]");
                              //Config::Connect ("/NodeList/2/ApplicationList/*/$Sender/Tx",
                                             //  MakeCallback
                                               //  (&PacketSizeMinMaxAvgTotalCalculator::PacketUpdate,
                                              //   appTxPkts_two));
                              //data.AddDataCalculator (appTxPkts_two);
                              //k

  for(int i=1;i<nbrNodes;i++)
  {
    Ptr<PacketSizeMinMaxAvgTotalCalculator> appTxPkts =
    CreateObject<PacketSizeMinMaxAvgTotalCalculator>();
    appTxPkts->SetKey ("tx-pkt-size");
    appTxPkts->SetContext ("node["+std::to_string(i)+"]");
    Config::Connect ("/NodeList/"+std::to_string(i)+"/ApplicationList/*/$Sender/Tx",
                    MakeCallback
                      (&PacketSizeMinMaxAvgTotalCalculator::PacketUpdate,
                      appTxPkts));
    data.AddDataCalculator (appTxPkts);

  }

  // Here we directly manipulate another DataCollector tracking min,
  // max, total, and average propagation delays.  Check out the Sender
  // and Receiver classes to see how packets are tagged with
  // timestamps to do this.
  Ptr<TimeMinMaxAvgTotalCalculator> delayStat =
    CreateObject<TimeMinMaxAvgTotalCalculator>();
  delayStat->SetKey ("delay");
  delayStat->SetContext (".");
  receiver->SetDelayTracker (delayStat);
  data.AddDataCalculator (delayStat);
  
  //ab
  for(int i=1;i<nbrNodes;i++)
  {
    //UdpEchoServerHelper echoServer (9);

   // ApplicationContainer serverApps = echoServer.Install (NodeList::GetNode (i));
   // serverApps.Start (Seconds (1.0));
    //serverApps.Stop (Seconds (10.0));

      //UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
      //echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
      //echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.)));
     // echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

     // ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
     // clientApps.Start (Seconds (1.0));
      //clientApps.Stop (Seconds (10.0));
  }




  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);
  wifiPhy.EnablePcap ("wifi-example-sim-abkDataLink", nodes);
  //k
  //------------------------------------------------------------
  //-- Run the simulation
  //--------------------------------------------
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop(Seconds(30));
  Simulator::Run ();
    



  //------------------------------------------------------------
  //-- Generate statistics output.
  //--------------------------------------------

  // Pick an output writer based in the requested format.
  Ptr<DataOutputInterface> output = 0;
  if (format == "omnet") {
      NS_LOG_INFO ("Creating omnet formatted data output.");
      output = CreateObject<OmnetDataOutput>();
    } else if (format == "db") {
    #ifdef STATS_HAS_SQLITE3
      NS_LOG_INFO ("Creating sqlite formatted data output.");
      output = CreateObject<SqliteDataOutput>();
    #endif
    } else {
      NS_LOG_ERROR ("Unknown output format " << format);
    }

  // Finally, have that writer interrogate the DataCollector and save
  // the results.
  if (output != 0)
    output->Output (data);

  // Free any memory here at the end of this example.
  Simulator::Destroy ();

  // end main
}
