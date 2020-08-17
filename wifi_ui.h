#include <Wire.h>           // For I2C comm, but needed for not getting compile error
#include <TerminalVT100.h>
#include <stdlib.h>
#include <WiFiNINA.h>

void get_params(String arr[]);  // pre-declare
byte get_num_params();
String get_label(int i);
String get_selected_param();
void put_param(int);

uint8_t select_wifi_screen = 0;
int numSsid = 0;
byte wifi_num_params = 0;

void wifi_connect(void);
void wifi_connected(void);

void print_wifi_password() {
  int pos,too;
  pos = max(0,param_num - 9);
  too = min(pos+10, wifi_password.length());
  ui.printLine(wifi_password.get().substring(pos,too),lines[1],2);
}

uint16_t wifi_get_param(int p_num) {
  return wifi_password.charAt(p_num);
}

void wifi_inc_param_num_by(int val) {
  param_num+= val;
  if(param_num>wifi_num_params-1) param_num = 0;
  if(param_num<0) param_num = wifi_num_params;
  //print_wifi_password();
  //the_param = wifi_get_param(param_num);
  Serial.println("Increment Param #: " + String(param_num) + " Num params: " + String(wifi_num_params) + " The param: " + char(the_param));
}

void wifi_adjust_param(int e) {
    if(select_wifi_screen==2) {
      the_param += e;
      the_param = max(0x21,the_param);
      the_param = min(0x7f,the_param);
      wifi_password.putCharAt(char(the_param), param_num);
      print_wifi_password();
      Serial.println("Param: "+String(the_param)+ " Param# "+ String(param_num));
    }
}

void wifi_display_networks() {
  String temps;
  if(numSsid==0) {
    // wifi_password.put("a");
    ui.clearDisplay();
    ui.printText("WiFi:",0,0,2);
    ui.printLine("Scanning networks... Please wait.",lines[0],1);

    numSsid = numNetworks();
    ui.clearDisplay();

    wifi_num_params = numSsid;
    if (numSsid == -1)
    {
      ui.printText("WiFi:",0,0,2);
      ui.printLine("Couldn't get a WiFi connection",lines[0],1);
      //Serial.println("Couldn't get a WiFi connection");
    } else {
      // print the list of networks seen:
      ui.printText("Select:",0,0,2);
      Serial.print("number of available networks: ");
      Serial.println(numSsid);
      Serial.println();
    }
  }
  for (int thisNet = 0; thisNet < numSsid; thisNet++)
  {
      Serial.print(thisNet + 1);
      Serial.print(") ");
      Serial.print("Signal: ");
      Serial.print(WiFi.RSSI(thisNet));
      Serial.print(" dBm");
      Serial.print("\tChannel: ");
      Serial.print(WiFi.channel(thisNet));
      byte bssid[6];
      Serial.print("\t\tBSSID: ");
      printMacAddress(WiFi.BSSID(thisNet, bssid));
      Serial.print("\tEncryption: ");
      printEncryptionType(WiFi.encryptionType(thisNet));
      Serial.print("\t\tSSID: ");
      Serial.println(WiFi.SSID(thisNet));
      Serial.flush();
  }
  // print the network number and name for each network found:
  Serial.println("Num params: "+String(wifi_num_params)+ "  Param# "+ String(param_num));
  ui.fill(BLACK, lines[0]);
  // ui.hiliteLine(lines[min(2,param_num)]);
  ui.hiliteLine(min(2,param_num));
  int start = max(0,param_num-2);
  int end = max(2,param_num);
  int lineNum = 0;
  for (int i = start; i <= end; i++) {
    //int thisNet = i + param_num/3;
    temps = getSsid(i);
    Serial.println(String(i) + ". " + temps);
    ui.printLine(temps,lines[lineNum++],1);
  }
  // ui.hiliteLine(lines[param_num%3]);
  // Serial.println("Param num: "+String(lines[param_num%3]));
}

void wifi_set_ssid() {
  Serial.println("Set SSID: "+String(param_num));
  wifi_ssid.put(getSsid(param_num,false));
  String enc = getEnc(param_num);
  if(enc=="None") {
    select_wifi_screen++;
    select_wifi_screen++;
    wifi_connect();
  } else {
    ui.clearDisplay();
    wifi_password.put("a");
    ui.printText("Password",0,0,2);  

    param_num=0;
    wifi_num_params = 64;  // max num chars for wpa2 password
    the_param = 'a';

    ui.printLine("For: " + wifi_ssid.get(),lines[0],1);
    ui.printLine("-",lines[2],2);
    print_wifi_password();
    //ui.hiliteLine(lines[1]);
    select_wifi_screen++;
  }
}

void enter_wifi_password() {
  Serial.println("Entering password for: "+wifi_ssid.get());
  String prefix = "";
  if(param_num>wifi_password.length()) param_num = wifi_password.length();
  if(wifi_password.length()<=param_num) wifi_password.append("a");

  // backspace removes chars
  if(wifi_password.length()-1>param_num) {
    wifi_password.removeCharAt(param_num+1);
    ui.printLine("          ",lines[1],2);
  }
  print_wifi_password();
  for(int i=0;i<min(9,param_num);i++) {
    prefix += " ";
  }
  ui.fill(BLACK, lines[2]);
  ui.printLine(prefix + "-",lines[2],2);
}

String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}

void wifi_connect() {
  wifi_active.reset();
  pass = wifi_password.get();
  ssid = wifi_ssid.get();
  //Serial.println("Connecting to: "+wifi_ssid.get() + " Password: " + wifi_password.get());
  Serial.println("Connecting to: "+wifi_ssid.get());
  ui.clearDisplay();
  ui.printText(wifi_ssid.get(),0,0,2);  
  ui.printLine("Status: ",lines[0],1);
  wifi_status = WL_IDLE_STATUS;
  do {
    if(server.status()==0 && millis() > wifi_chk_time) WIFI_setup();
    ui.printLine("Status: " + getConnectionStatus(wifi_status),lines[0],1);
  } while(wifi_status != WL_CONNECTED && !keypress);
  if(wifi_status == WL_CONNECTED) {
    WIFI_setup();
    select_wifi_screen = 4;
    wifi_connected();
  } else {
    ui.fill(BLACK, lines[0]);
    ui.printLine("Cancelled",lines[0],1);
    select_wifi_screen = 0;
  }
}

void wifi_connected() {
  wifi_active.set();
  Serial.println("Connected to: "+wifi_ssid.get());
  ui.clearDisplay();
  ui.printLine("Status: " + getConnectionStatus(wifi_status),lines[0],1);
  ui.printText(wifi_ssid.get(),0,0,2);  
  ui.printLine("IP: " + IpAddress2String(WiFi.localIP()),lines[1],1);
  //ui.printLine(WiFi.localIP(),lines[1],1);
  ui.printLine("Signal: " + String(WiFi.RSSI()) + " dBm",lines[2],1);
}

// trigger action 
void select_wifi_ok() {
  select_wifi_screen++;
  if(select_wifi_screen>4) select_wifi_screen=0;
  // exe_fxn();
}

void select_wifi_fxn() {
  disable_trigger(true);
  switch(select_wifi_screen) {
    case 0:
      wifi_display_networks();
      break;
    case 1:
      wifi_set_ssid();
      break;
    case 2:
      enter_wifi_password();
      break;
    case 3:
      wifi_connect();
      break;
    case 4:
      wifi_connected();
      break;
  }
}

void wifi_begin() {
  wifi_password.begin(false);
  wifi_ssid.begin(false);
  wifi_active.begin(false);
  wifi_password.xfer();
  wifi_ssid.xfer();
  wifi_active.xfer();
}

String params_toJSON() {
  switch (fxn.get())
  {
  case LFO_FXN:
    return lfo_params_toJSON();
    break;
  case USER_FXN:
    // Serial.println("User params to JSON!");
    return user_params_toJSON();
    break;
  default:
    return the_spanker->params_toJSON();
  }
}

void do_server() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            //client.println("Content-type:application/json");
            client.println("Access-Control-Allow-Origin: *");
            client.println();
            
            client.print("{");
            client.print(toJSON("fxn",fxn_name()));
            client.print(",");
            client.print(toJSON("repeat_on",repeat_on.get()?"ON":"OFF"));
            client.print(",");
            client.print(toJSON("triggered",triggered?"ON":"OFF"));            
            client.print(",");
            // client.print(toJSON("selected",get_selected_param()));
            // client.print(",");            

            if(fxn_name()=="User") {
              Serial.println("Sending: "+fxn_name());
              client.print("\"sequence\" : {");            
              client.print(toJSON("label","Sequence"));            
              client.print(",");
              client.print(toJSON("type","sequence"));            
              client.print(",");
              client.print(toJSON("value",user_string.get()));            
              client.print(",");
              client.print(toJSON("legal_values",user_ops));            
              client.print(",");
              client.print(toJSON("dig_num",String(user_dig_num)));            
              client.print(",");
              client.print(toJSON("selected","true"));            
              client.print("},");
            }

            client.print("\"params\" : [");            
            // fix this for other fxns
            client.print(params_toJSON());
            client.print("]");
            client.print("}");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            if(strstr(currentLine.c_str(),"POST")) {
              char cmd = currentLine[6];
              Serial.println("Command: "+String(cmd));
              //int c = cmd;
              String in_str="";
              int i=0;
              char inchar;
              do {
                inchar = currentLine[i+6];
                in_str+=inchar;
                i++;
              } while (String(inchar)!=" " && i<20);
              Serial.println("Receiving: "+in_str);

              if(cmd==112) {
                put_param_num(in_str.substring(1).toInt());
                //exe_fxn();
              }

              if(fxn.get()==USER_FXN && !esc_mode) {
                user_update_user_string(cmd, in_str);
                // if(cmd>64 && cmd<91) {
                //   user_string.put(in_str);
                //   user_display();
                // }
              }

              if(cmd>47 && cmd<58) {
                //int c = in_str.toInt();
                Serial.println("Number received: "+in_str);
                put_param(in_str.toInt());
              }

              if(String(cmd) != "\n" && cmd != 112) {
                if(String(cmd)=="%") {
                  keypress = 27;
                } else {
                  keypress = cmd;
                }
                
                process_keypress();
              }

            }
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
  if(!triggered && fxn.get() != LFO_FXN && wifi_status==WL_CONNECTED) ui.printWiFiBars(WiFi.RSSI());
}
