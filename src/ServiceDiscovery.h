#ifndef SD_H
#define SD_H

#include <IPAddress.h>

class ServiceDiscovery {
  public:
    /// @brief Initialize mDNS/SD
    /// @param clientId 
    ServiceDiscovery(const char* clientId);
    void searchMQTT();

    int getSelected();
    void setSelected(int selected);

    IPAddress getIP();

    /// @brief retrieve selected result of recent mDNS/SD query
    /// @return hostname
    String getHostname();

    uint16_t getPort();

  private:
    int found, selected;
};

#endif
