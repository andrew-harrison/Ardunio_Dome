#include <Arduino.h>

#ifndef DDNS_H
#define DDNS_H

class EasyDDNSClass{
public:
  void service(String ddns_service);
  void client(String ddns_domain, String ddns_username,String ddns_password = "");
  void update(unsigned long ddns_update_interval);

private:
  unsigned long interval;
  unsigned long previousMillis;

  String new_ip;
  String old_ip;
  String update_url;
  String ddns_u;
  String ddns_p;
  String ddns_d;
  String ddns_choice;
};
extern EasyDDNSClass EasyDDNS;
#endif
