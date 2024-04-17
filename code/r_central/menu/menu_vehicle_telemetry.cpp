/*
    Ruby Licence
    Copyright (c) 2024 Petru Soroaga petrusoroaga@yahoo.com
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Copyright info and developer info must be preserved as is in the user
        interface, additions could be made to that info.
        * Neither the name of the organization nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.
        * Military use is not permited.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL Julien Verneuil BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "menu.h"
#include "menu_vehicle_telemetry.h"
#include "menu_item_select.h"
#include "menu_item_slider.h"
#include "menu_item_section.h"
#include "menu_item_text.h"
#include "menu_item_legend.h"
#include "menu_confirmation.h"


MenuVehicleTelemetry::MenuVehicleTelemetry(void)
:Menu(MENU_ID_VEHICLE_TELEMETRY, "Telemetry Settings", NULL)
{
   m_Width = 0.28;
   m_xPos = menu_get_XStartPos(m_Width); m_yPos = 0.24;

   char szBuff[32];
   
   m_pItemsSelect[0] = new MenuItemSelect("FC Telemetry Type", "The telemetry type that is sent/received from the Flight controller. This should match the telemetry type generated by the flight controller.");
   m_pItemsSelect[0]->addSelection("None");
   m_pItemsSelect[0]->addSelection("MAVLink");
   m_pItemsSelect[0]->addSelection("LTM");
   m_pItemsSelect[0]->setIsEditable();
   m_IndexVTelemetryType = addMenuItem(m_pItemsSelect[0]);

   m_pItemsSelect[7] = new MenuItemSelect("Allow any FC SysId", "Receive and use telemetry messages from any system id on the flight controller, or just from the main pilot system (sysid=1).");
   m_pItemsSelect[7]->addSelection("No");
   m_pItemsSelect[7]->addSelection("Yes");
   m_pItemsSelect[7]->setIsEditable();
   m_IndexTelemetryAnySystem = addMenuItem(m_pItemsSelect[7]);

   m_pItemsSelect[10] = new MenuItemSelect("Ignore FC Messages", "Ignore text messages from FC, do not display them in the UI.");
   m_pItemsSelect[10]->addSelection("No");
   m_pItemsSelect[10]->addSelection("Yes");
   m_pItemsSelect[10]->setIsEditable();
   m_IndexTelemetryNoFCMessages = addMenuItem(m_pItemsSelect[10]);

   m_pItemsSelect[9] = new MenuItemSelect("Remove duplicate Msgs", "Removes duplicate messages that are generated by the flight controller, if any.");
   m_pItemsSelect[9]->addSelection("No");
   m_pItemsSelect[9]->addSelection("Yes");
   m_pItemsSelect[9]->setIsEditable();
   m_IndexRemoveDuplicateMsg = addMenuItem(m_pItemsSelect[9]);
   
   m_pItemsSelect[11] = new MenuItemSelect("Force Always Armed", "Force the Armed state, permanently, for flight controllers that do not have an arm/disarm state or for custom MAVLink streams.");
   m_pItemsSelect[11]->addSelection("No");
   m_pItemsSelect[11]->addSelection("Yes");
   m_pItemsSelect[11]->setIsEditable();
   m_IndexAlwaysArmed = addMenuItem(m_pItemsSelect[11]);

   addSeparator();

   m_pItemsSelect[1] = new MenuItemSelect("Vehicle Serial Port", "The Ruby vehicle port at which the flight controller connects to.");
   m_pItemsSelect[1]->addSelection("None");
   for( int i=0; i<g_pCurrentModel->hardwareInterfacesInfo.serial_bus_count; i++ )
      m_pItemsSelect[1]->addSelection(g_pCurrentModel->hardwareInterfacesInfo.serial_bus_names[i]);
   m_pItemsSelect[1]->setIsEditable();
   m_IndexVSerialPort = addMenuItem(m_pItemsSelect[1]);

   m_pItemsSelect[2] = new MenuItemSelect("Vehicle Serial Baudrate", "Sets the baud rate between flight controller and Ruby on the vehicle side. Should match the serial speed that the flight controller generates. Higher is better.");
   for( int i=0; i<hardware_get_serial_baud_rates_count(); i++ )
   {
      sprintf(szBuff, "%d bps", hardware_get_serial_baud_rates()[i]);
      m_pItemsSelect[2]->addSelection(szBuff);
   }
   m_pItemsSelect[2]->setIsEditable();
   m_IndexVBaudRate = addMenuItem(m_pItemsSelect[2]);

   m_pItemsSelect[8] = new MenuItemSelect("Has GPS", "Sets how many GPS units are configured on the vehicle. Ruby uses GPS data for OSD display.");  
   m_pItemsSelect[8]->addSelection("No GPS");
   m_pItemsSelect[8]->addSelection("1 GPS unit");
   m_pItemsSelect[8]->addSelection("2 GPS units");
   m_pItemsSelect[8]->addSelection("3 GPS units");
   m_pItemsSelect[8]->setIsEditable();
   m_IndexGPS = addMenuItem(m_pItemsSelect[8]);

   m_pItemsSlider[0] = new MenuItemSlider("Refresh rate (Hz)", "How many times per second the flight controller telemetry gets sent from vehicle to controller.", 1,10,4, 0.1);
   m_IndexRUpdateRate = addMenuItem(m_pItemsSlider[0]);

   m_pItemsSelect[5] = new MenuItemSelect("Allow spectators", "Allows spectators to read the telemetry stream and display it in OSD.");
   m_pItemsSelect[5]->addSelection("No");
   m_pItemsSelect[5]->addSelection("Yes");
   m_pItemsSelect[5]->setIsEditable();
   m_IndexSpectator = addMenuItem(m_pItemsSelect[5]);

   m_pItemsSelect[3] = new MenuItemSelect("Telemetry Options", "Enables or disables the telemetry upload from controller to vehicle.");
   m_pItemsSelect[3]->addSelection("Bidirectional");
   m_pItemsSelect[3]->addSelection("Read Only");
   m_pItemsSelect[3]->setIsEditable();
   m_IndexTelemetryFlags = addMenuItem(m_pItemsSelect[3]);

   m_pItemsSelect[4] = new MenuItemSelect("Request data streams", "Enables the request of the telemetry data streams from vehicle.");
   m_pItemsSelect[4]->addSelection("No");
   m_pItemsSelect[4]->addSelection("Yes");
   m_pItemsSelect[4]->setIsEditable();
   m_IndexTelemetryRequestStreams = addMenuItem(m_pItemsSelect[4]);

   m_pItemsRange[0] = new MenuItemRange("Controller MAVLink SysId", "Sets the MAVLink SysId for Ruby controller as it will be seen by the vehicle flight controller.", 1, 255, g_pCurrentModel->telemetry_params.controller_mavlink_id, 1 );  
   m_pItemsRange[0]->setSufix("");
   m_IndexTelemetryControllerSysId = addMenuItem(m_pItemsRange[0]);

   m_IndexInfoSysId = addMenuItem( new MenuItemText("Note: The controller MAVLink SysId must match the parameter SYSID_MYGCS set on flight controller using Mission Planner or QGC."));

   //m_pItemsSelect[6] = new MenuItemSelect("Low Datarate", "Vehicle will send the telemetry using the lowest possible radio datarate.");
   //m_pItemsSelect[6]->addSelection("No");
   //m_pItemsSelect[6]->addSelection("Yes");
   //m_pItemsSelect[6]->setIsEditable();
   //m_IndexDataRate = addMenuItem(m_pItemsSelect[6]);
   m_IndexDataRate = -1;

   //addMenuItem( new MenuItemText("When Low Datarate is enabled, the vehicle will use the lowest possible radio datarate to send telemetry to controller for best possible range. Adaptive video flags and adaptive radio flags are ignored when sending telemetry packets when this option is enabled."));
}

MenuVehicleTelemetry::~MenuVehicleTelemetry()
{
}

void MenuVehicleTelemetry::valuesToUI()
{
   char szBuff[128];

   m_pItemsSlider[0]->setCurrentValue(g_pCurrentModel->telemetry_params.update_rate);

   m_pItemsSelect[0]->setSelection(0);
   if ( g_pCurrentModel->telemetry_params.fc_telemetry_type == TELEMETRY_TYPE_NONE )
      m_pItemsSelect[0]->setSelection(0);
   if ( g_pCurrentModel->telemetry_params.fc_telemetry_type == TELEMETRY_TYPE_MAVLINK )
      m_pItemsSelect[0]->setSelection(1);
   if ( g_pCurrentModel->telemetry_params.fc_telemetry_type == TELEMETRY_TYPE_LTM )
      m_pItemsSelect[0]->setSelection(2);
   
   m_pItemsSelect[5]->setSelection((g_pCurrentModel->telemetry_params.flags & TELEMETRY_FLAGS_SPECTATOR_ENABLE)?1:0);
   m_pItemsSelect[8]->setSelection(g_pCurrentModel->iGPSCount);

   m_pItemsSelect[11]->setSelection((g_pCurrentModel->telemetry_params.flags & TELEMETRY_FLAGS_FORCE_ARMED)?1:0);

   m_pItemsRange[0]->setCurrentValue(g_pCurrentModel->telemetry_params.controller_mavlink_id);
   if ( g_pCurrentModel->telemetry_params.fc_telemetry_type == TELEMETRY_TYPE_NONE )
   {
      m_pItemsRange[0]->setEnabled(false);
      m_pItemsSelect[2]->setEnabled(false);
      m_pItemsSelect[8]->setEnabled(false);
      m_pMenuItems[m_IndexVSerialPort]->setEnabled(false);
      m_pMenuItems[m_IndexVBaudRate]->setEnabled(false);
      m_pMenuItems[m_IndexRUpdateRate]->setEnabled(false);
      m_pMenuItems[m_IndexSpectator]->setEnabled(false);
      m_pMenuItems[m_IndexTelemetryFlags]->setEnabled(false);
      m_pMenuItems[m_IndexTelemetryRequestStreams]->setEnabled(false);
      m_pMenuItems[m_IndexInfoSysId]->setEnabled(false);
      if ( -1 != m_IndexDataRate )
         m_pMenuItems[m_IndexDataRate]->setEnabled(false);
   }
   else
   {
      m_pItemsRange[0]->setEnabled(true);
      m_pItemsSelect[8]->setEnabled(true);
      m_pMenuItems[m_IndexVSerialPort]->setEnabled(true);
      m_pMenuItems[m_IndexVBaudRate]->setEnabled(true);
      m_pMenuItems[m_IndexRUpdateRate]->setEnabled(true);
      m_pMenuItems[m_IndexSpectator]->setEnabled(true);
      m_pMenuItems[m_IndexTelemetryFlags]->setEnabled(true);
      m_pMenuItems[m_IndexTelemetryRequestStreams]->setEnabled(true);
      m_pMenuItems[m_IndexInfoSysId]->setEnabled(true);
      if ( -1 != m_IndexDataRate )
         m_pMenuItems[m_IndexDataRate]->setEnabled(true);
   }


   int iCurrentSerialPortIndex = -1;
   u32 uCurrentSerialPortSpeed = 0;
   for( int i=0; i<g_pCurrentModel->hardwareInterfacesInfo.serial_bus_count; i++ )
   {
       
       if ( g_pCurrentModel->hardwareInterfacesInfo.serial_bus_supported_and_usage[i] & ((1<<5)<<8) )
       if ( (g_pCurrentModel->hardwareInterfacesInfo.serial_bus_supported_and_usage[i] & 0xFF) == SERIAL_PORT_USAGE_TELEMETRY )
       {
          iCurrentSerialPortIndex = i;
          uCurrentSerialPortSpeed = g_pCurrentModel->hardwareInterfacesInfo.serial_bus_speed[i];
          break;
       }
   }

   if ( -1 == iCurrentSerialPortIndex || 0 == uCurrentSerialPortSpeed )
   {
      m_pItemsSelect[1]->setSelectedIndex(0);
      m_pItemsSelect[2]->setEnabled(false);
   }
   else if ( g_pCurrentModel->telemetry_params.fc_telemetry_type != TELEMETRY_TYPE_NONE )
   {
      m_pItemsSelect[1]->setSelectedIndex( 1 + iCurrentSerialPortIndex );
      m_pItemsSelect[2]->setEnabled(true);
      bool bSpeedFound = false;
      for(int i=0; i<m_pItemsSelect[2]->getSelectionsCount(); i++ )
         if ( hardware_get_serial_baud_rates()[i] == (int)uCurrentSerialPortSpeed )
         {
            m_pItemsSelect[2]->setSelection(i);
            bSpeedFound = true;
            break;
         }

      if ( ! bSpeedFound )
      {
         sprintf(szBuff, "Info: You are using a custom telemetry baud rate (%d) on this %s.", uCurrentSerialPortSpeed, g_pCurrentModel->getVehicleTypeString());
         addTopLine(szBuff);
      }
   }

   if ( g_pCurrentModel->telemetry_params.flags & TELEMETRY_FLAGS_RXTX )
      m_pItemsSelect[3]->setSelection(0);
   if ( g_pCurrentModel->telemetry_params.flags & TELEMETRY_FLAGS_RXONLY )
      m_pItemsSelect[3]->setSelection(1);

   m_pItemsSelect[4]->setSelection(0);
   if ( g_pCurrentModel->telemetry_params.flags & TELEMETRY_FLAGS_REQUEST_DATA_STREAMS )
      m_pItemsSelect[4]->setSelection(1);

   if ( -1 != m_IndexDataRate )
   {
      //m_pItemsSelect[6]->setSelection(0);
      //if ( g_pCurrentModel->telemetry_params.flags & TELEMETRY_FLAGS_SEND_ON_LOWEST_DATARATE )
      //   m_pItemsSelect[6]->setSelection(1);
   }

   m_pItemsSelect[7]->setSelection(0);
   if ( g_pCurrentModel->telemetry_params.flags & TELEMETRY_FLAGS_ALLOW_ANY_VEHICLE_SYSID )
      m_pItemsSelect[7]->setSelection(1);

   m_pItemsSelect[9]->setEnabled(true);
   m_pItemsSelect[9]->setSelection(0);
   if ( g_pCurrentModel->telemetry_params.flags & TELEMETRY_FLAGS_REMOVE_DUPLICATE_FC_MESSAGES )
      m_pItemsSelect[9]->setSelection(1);

   m_pItemsSelect[10]->setSelection(0);
   if ( g_pCurrentModel->telemetry_params.flags & TELEMETRY_FLAGS_DONT_SHOW_FC_MESSAGES )
   {
      m_pItemsSelect[10]->setSelection(1);
      m_pItemsSelect[9]->setEnabled(false);
   }
}

void MenuVehicleTelemetry::Render()
{
   RenderPrepare();
   float yTop = RenderFrameAndTitle();
   float y = yTop;
   for( int i=0; i<m_ItemsCount; i++ )
      y += RenderItem(i,y);
   RenderEnd(yTop);
}


void MenuVehicleTelemetry::onSelectItem()
{
   Menu::onSelectItem();
   if ( handle_commands_is_command_in_progress() )
   {
      handle_commands_show_popup_progress();
      return;
   }
   if ( m_pMenuItems[m_SelectedIndex]->isEditing() )
      return;

   int iCurrentSerialPortIndex = -1;
   for( int i=0; i<g_pCurrentModel->hardwareInterfacesInfo.serial_bus_count; i++ )
   {
       
       if ( g_pCurrentModel->hardwareInterfacesInfo.serial_bus_supported_and_usage[i] & ((1<<5)<<8) )
       if ( (g_pCurrentModel->hardwareInterfacesInfo.serial_bus_supported_and_usage[i] & 0xFF) == SERIAL_PORT_USAGE_TELEMETRY )
       {
          iCurrentSerialPortIndex = i;
          break;
       }
   }


   if ( m_IndexGPS == m_SelectedIndex )
   {
      int iGPSCount = m_pItemsSelect[8]->getSelectedIndex();
      if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_GPS_INFO, iGPSCount, NULL, 0) )
         valuesToUI();
      return;
   }


   if ( m_IndexSpectator == m_SelectedIndex )
   {
      telemetry_parameters_t params;
      memcpy(&params, &g_pCurrentModel->telemetry_params, sizeof(telemetry_parameters_t));
   
      if ( 0 == m_pItemsSelect[5]->getSelectedIndex() )
         params.flags &= ~TELEMETRY_FLAGS_SPECTATOR_ENABLE;
      else
         params.flags |= TELEMETRY_FLAGS_SPECTATOR_ENABLE;
  
      if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_TELEMETRY_PARAMETERS, 0, (u8*)&params, sizeof(telemetry_parameters_t)) )
         valuesToUI();
   } 

   if ( m_IndexTelemetryFlags == m_SelectedIndex )
   {
      telemetry_parameters_t params;
      memcpy(&params, &g_pCurrentModel->telemetry_params, sizeof(telemetry_parameters_t));
   
      if ( 0 == m_pItemsSelect[3]->getSelectedIndex() )
      {
         params.flags |= TELEMETRY_FLAGS_RXTX;
         params.flags &= (~TELEMETRY_FLAGS_RXONLY);
      }
      else
      {
         params.flags |= TELEMETRY_FLAGS_RXONLY;
         params.flags &= (~TELEMETRY_FLAGS_RXTX);
      }
  
      if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_TELEMETRY_PARAMETERS, 0, (u8*)&params, sizeof(telemetry_parameters_t)) )
         valuesToUI();
   } 

   if ( m_IndexTelemetryAnySystem == m_SelectedIndex )
   {
      telemetry_parameters_t params;
      memcpy(&params, &g_pCurrentModel->telemetry_params, sizeof(telemetry_parameters_t));
   
      if ( 0 == m_pItemsSelect[7]->getSelectedIndex() )
         params.flags &= (~TELEMETRY_FLAGS_ALLOW_ANY_VEHICLE_SYSID);
      else
         params.flags |= TELEMETRY_FLAGS_ALLOW_ANY_VEHICLE_SYSID;
  
      if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_TELEMETRY_PARAMETERS, 0, (u8*)&params, sizeof(telemetry_parameters_t)) )
         valuesToUI();
   }

   if ( m_IndexRemoveDuplicateMsg == m_SelectedIndex )
   {
      telemetry_parameters_t params;
      memcpy(&params, &g_pCurrentModel->telemetry_params, sizeof(telemetry_parameters_t));
   
      if ( 0 == m_pItemsSelect[9]->getSelectedIndex() )
         params.flags &= (~TELEMETRY_FLAGS_REMOVE_DUPLICATE_FC_MESSAGES);
      else
         params.flags |= TELEMETRY_FLAGS_REMOVE_DUPLICATE_FC_MESSAGES;
  
      if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_TELEMETRY_PARAMETERS, 0, (u8*)&params, sizeof(telemetry_parameters_t)) )
         valuesToUI();
   }

   if ( m_IndexAlwaysArmed == m_SelectedIndex )
   {
      telemetry_parameters_t params;
      memcpy(&params, &g_pCurrentModel->telemetry_params, sizeof(telemetry_parameters_t));
   
      if ( 0 == m_pItemsSelect[11]->getSelectedIndex() )
         params.flags &= (~TELEMETRY_FLAGS_FORCE_ARMED);
      else
         params.flags |= TELEMETRY_FLAGS_FORCE_ARMED;
  
      if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_TELEMETRY_PARAMETERS, 0, (u8*)&params, sizeof(telemetry_parameters_t)) )
         valuesToUI();    
   }

   if ( m_IndexTelemetryNoFCMessages == m_SelectedIndex )
   {
      telemetry_parameters_t params;
      memcpy(&params, &g_pCurrentModel->telemetry_params, sizeof(telemetry_parameters_t));
   
      if ( 0 == m_pItemsSelect[10]->getSelectedIndex() )
         params.flags &= (~TELEMETRY_FLAGS_DONT_SHOW_FC_MESSAGES);
      else
         params.flags |= TELEMETRY_FLAGS_DONT_SHOW_FC_MESSAGES;
  
      if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_TELEMETRY_PARAMETERS, 0, (u8*)&params, sizeof(telemetry_parameters_t)) )
         valuesToUI();
   }

   if ( m_IndexTelemetryRequestStreams == m_SelectedIndex )
   {
      telemetry_parameters_t params;
      memcpy(&params, &g_pCurrentModel->telemetry_params, sizeof(telemetry_parameters_t));
   
      if ( 0 == m_pItemsSelect[4]->getSelectedIndex() )
         params.flags &= (~TELEMETRY_FLAGS_REQUEST_DATA_STREAMS);
      else
         params.flags |= TELEMETRY_FLAGS_REQUEST_DATA_STREAMS;
  
      if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_TELEMETRY_PARAMETERS, 0, (u8*)&params, sizeof(telemetry_parameters_t)) )
         valuesToUI();
   } 

   if ( m_IndexRUpdateRate == m_SelectedIndex )
   {
      telemetry_parameters_t params;
      memcpy(&params, &g_pCurrentModel->telemetry_params, sizeof(telemetry_parameters_t));
      
      params.update_rate = m_pItemsSlider[0]->getCurrentValue();
      if ( params.update_rate == g_pCurrentModel->telemetry_params.update_rate )
         return;
  
      if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_TELEMETRY_PARAMETERS, 0, (u8*)&params, sizeof(telemetry_parameters_t)) )
         valuesToUI();
   } 

   if ( m_IndexVTelemetryType == m_SelectedIndex )
   {
      telemetry_parameters_t params;
      memcpy(&params, &g_pCurrentModel->telemetry_params, sizeof(telemetry_parameters_t));
      
      params.fc_telemetry_type = TELEMETRY_TYPE_NONE;
      if ( 1 == m_pItemsSelect[0]->getSelectedIndex() )
         params.fc_telemetry_type = TELEMETRY_TYPE_MAVLINK;
      if ( 2 == m_pItemsSelect[0]->getSelectedIndex() )
         params.fc_telemetry_type = TELEMETRY_TYPE_LTM;

      if ( params.fc_telemetry_type == g_pCurrentModel->telemetry_params.fc_telemetry_type )
         return;
  
      if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_TELEMETRY_PARAMETERS, 0, (u8*)&params, sizeof(telemetry_parameters_t)) )
         valuesToUI();
   }

   if ( m_IndexVSerialPort == m_SelectedIndex )
   {
      int iSerialPort = m_pItemsSelect[1]->getSelectedIndex();
      if ( iSerialPort != 0 )
      if ( (g_pCurrentModel->hardwareInterfacesInfo.serial_bus_supported_and_usage[iSerialPort-1] & 0xFF) == SERIAL_PORT_USAGE_SIK_RADIO )
      {
         MenuConfirmation* pMC = new MenuConfirmation("Can't use serial port", "The serial port you selected can't be used for telemetry connection to flight controller as it's used for a SiK radio interface.",1, true);
         pMC->m_yPos = 0.3;
         add_menu_to_stack(pMC);
         valuesToUI();
         return;
      }

      if ( 0 == iSerialPort )
      {
         if ( iCurrentSerialPortIndex != -1 )
         {
            type_vehicle_hardware_interfaces_info new_info;
            memcpy((u8*)&new_info, (u8*)&(g_pCurrentModel->hardwareInterfacesInfo), sizeof(type_vehicle_hardware_interfaces_info));
            new_info.serial_bus_supported_and_usage[iCurrentSerialPortIndex] &= 0xFFFFFF00;
            new_info.serial_bus_supported_and_usage[iCurrentSerialPortIndex] |= SERIAL_PORT_USAGE_NONE;
            
            log_line("Sending disabling telemetry serial port selection to vehicle.");
            if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_SERIAL_PORTS_INFO, 0, (u8*)&new_info, sizeof(type_vehicle_hardware_interfaces_info)) )
               valuesToUI();

            return;
         }
      }
      else
      {
         type_vehicle_hardware_interfaces_info new_info;
         memcpy((u8*)&new_info, (u8*)&(g_pCurrentModel->hardwareInterfacesInfo), sizeof(type_vehicle_hardware_interfaces_info));

         u8 uCurrentUsage = new_info.serial_bus_supported_and_usage[iSerialPort-1] & 0xFF;
         if ( uCurrentUsage != SERIAL_PORT_USAGE_TELEMETRY )
         {
            if ( uCurrentUsage == SERIAL_PORT_USAGE_DATA_LINK )
            {
               MenuConfirmation* pMC = new MenuConfirmation("User Data Link Disabled", "The serial port was used by your custom data link. It was reasigned to the telemetry link.",1, true);
               pMC->m_yPos = 0.3;
               add_menu_to_stack(pMC);      
            }
         }

         if ( iCurrentSerialPortIndex != -1 )
         {
            new_info.serial_bus_supported_and_usage[iCurrentSerialPortIndex] &= 0xFFFFFF00;
            new_info.serial_bus_supported_and_usage[iCurrentSerialPortIndex] |= SERIAL_PORT_USAGE_NONE;
         }
         new_info.serial_bus_supported_and_usage[iSerialPort-1] &= 0xFFFFFF00;
         new_info.serial_bus_supported_and_usage[iSerialPort-1] |= SERIAL_PORT_USAGE_TELEMETRY;
         
         log_line("Sending new serial port to be used for telemetry to vehicle.");
         if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_SERIAL_PORTS_INFO, 0, (u8*)&new_info, sizeof(type_vehicle_hardware_interfaces_info)) )
            valuesToUI();
         return;
      }
   }

   if ( m_IndexVBaudRate == m_SelectedIndex )
   {
      if ( -1 == iCurrentSerialPortIndex )
         return;

      long val = hardware_get_serial_baud_rates()[m_pItemsSelect[2]->getSelectedIndex()];
      if ( val == g_pCurrentModel->hardwareInterfacesInfo.serial_bus_speed[iCurrentSerialPortIndex] )
         return;

      type_vehicle_hardware_interfaces_info new_info;
      memcpy((u8*)&new_info, (u8*)&(g_pCurrentModel->hardwareInterfacesInfo), sizeof(type_vehicle_hardware_interfaces_info));
      new_info.serial_bus_speed[iCurrentSerialPortIndex] = (u32)val;
      log_line("Sending new serial port speed for telemetry to vehicle: %u", val);
      if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_SERIAL_PORTS_INFO, 0, (u8*)&new_info, sizeof(type_vehicle_hardware_interfaces_info)) )
         valuesToUI();
      return;
   }


   if ( m_IndexTelemetryControllerSysId == m_SelectedIndex )
   {
      int sysId = m_pItemsRange[0]->getCurrentValue();
      telemetry_parameters_t params;
      memcpy(&params, &g_pCurrentModel->telemetry_params, sizeof(telemetry_parameters_t));
      
      params.controller_mavlink_id = sysId;
      if ( params.controller_mavlink_id == g_pCurrentModel->telemetry_params.controller_mavlink_id )
         return;
  
      if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_TELEMETRY_PARAMETERS, 0, (u8*)&params, sizeof(telemetry_parameters_t)) )
         valuesToUI();
   }

   if (  m_IndexDataRate == m_SelectedIndex )
   {
      telemetry_parameters_t params;
      memcpy(&params, &g_pCurrentModel->telemetry_params, sizeof(telemetry_parameters_t));
   
      //if ( 0 == m_pItemsSelect[6]->getSelectedIndex() )
      //   params.flags &= ~TELEMETRY_FLAGS_SEND_ON_LOWEST_DATARATE;
      //else
      //   params.flags |= TELEMETRY_FLAGS_SEND_ON_LOWEST_DATARATE;
  
      if ( ! handle_commands_send_to_vehicle(COMMAND_ID_SET_TELEMETRY_PARAMETERS, 0, (u8*)&params, sizeof(telemetry_parameters_t)) )
         valuesToUI();
   } 

}
