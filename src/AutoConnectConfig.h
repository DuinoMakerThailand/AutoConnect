/**
 *	Declaration of AutoConnectConfig class.
 *	@file	AutoConnectConfig.h
 *	@author	hieromon@gmail.com
 *	@version	1.3.0
 *	@date	2021-01-21
 *	@copyright	MIT license.
 */

#ifndef _AUTOCONNECTCONFIG_H_
#define _AUTOCONNECTCONFIG_H_

#include <Arduino.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include "AutoConnectDefs.h"
#include "AutoConnectTypes.h"

/**
 * Storage identifier for AutoConnectConfig. It is global constant
 * and reserved.
 */
#ifndef AC_CONFIGSTORE_IDENTIFIER
#define AC_CONFIGSTORE_IDENTIFIER "AC_CONFG"
#endif // !AC_CONFIGSTORE_IDENTIFIER

class AutoConnectConfig {
 public:
  /**
   *  AutoConnectConfig default constructor.
   *  SSID for the captive portal access point assumes AUTOCONNECT_APID which
   *  assigned from macro. Password is same as above too.
   */
  AutoConnectConfig() :
    apip(AUTOCONNECT_AP_IP),
    gateway(AUTOCONNECT_AP_GW),
    netmask(AUTOCONNECT_AP_NM),
    apid(String(F(AUTOCONNECT_APID))),
    psk(String(F(AUTOCONNECT_PSK))),
    channel(AUTOCONNECT_AP_CH),
    hidden(0),
    minRSSI(AUTOCONNECT_MIN_RSSI),
    autoSave(AC_SAVECREDENTIAL_AUTO),
    bootUri(AC_ONBOOTURI_ROOT),
    principle(AC_PRINCIPLE_RECENT),
    boundaryOffset(AC_IDENTIFIER_OFFSET),
    uptime(AUTOCONNECT_STARTUPTIME),
    autoRise(true),
    autoReset(true),
    autoReconnect(false),
    immediateStart(false),
    retainPortal(false),
    preserveAPMode(false),
    beginTimeout(AUTOCONNECT_TIMEOUT),
    portalTimeout(AUTOCONNECT_CAPTIVEPORTAL_TIMEOUT),
    menuItems(AC_MENUITEM_CONFIGNEW | AC_MENUITEM_OPENSSIDS | AC_MENUITEM_DISCONNECT | AC_MENUITEM_RESET | AC_MENUITEM_UPDATE | AC_MENUITEM_HOME),
    reconnectInterval(0),
    ticker(false),
    tickerPort(AUTOCONNECT_TICKER_PORT),
    tickerOn(LOW),
    ota(AC_OTA_EXTRA),
    auth(AC_AUTH_NONE),
    authScope(AC_AUTHSCOPE_AUX),
    username(String("")),
    password(String("")),
    hostName(String("")),
    homeUri(AUTOCONNECT_HOMEURI),
    title(AUTOCONNECT_MENU_TITLE),
    staip(0U),
    staGateway(0U),
    staNetmask(0U),
    dns1(0U),
    dns2(0U) {}
  /**
   *  Configure by SSID for the captive portal access point and password.
   */
  AutoConnectConfig(const char* ap, const char* password, const unsigned long portalTimeout = 0, const uint8_t channel = AUTOCONNECT_AP_CH) :
    apip(AUTOCONNECT_AP_IP),
    gateway(AUTOCONNECT_AP_GW),
    netmask(AUTOCONNECT_AP_NM),
    apid(String(ap)),
    psk(String(password)),
    channel(channel),
    hidden(0),
    minRSSI(AUTOCONNECT_MIN_RSSI),
    autoSave(AC_SAVECREDENTIAL_AUTO),
    bootUri(AC_ONBOOTURI_ROOT),
    principle(AC_PRINCIPLE_RECENT),
    boundaryOffset(AC_IDENTIFIER_OFFSET),
    uptime(AUTOCONNECT_STARTUPTIME),
    autoRise(true),
    autoReset(true),
    autoReconnect(false),
    immediateStart(false),
    retainPortal(false),
    preserveAPMode(false),
    beginTimeout(AUTOCONNECT_TIMEOUT),
    portalTimeout(portalTimeout),
    menuItems(AC_MENUITEM_CONFIGNEW | AC_MENUITEM_OPENSSIDS | AC_MENUITEM_DISCONNECT | AC_MENUITEM_RESET | AC_MENUITEM_UPDATE | AC_MENUITEM_HOME),
    reconnectInterval(0),
    ticker(false),
    tickerPort(AUTOCONNECT_TICKER_PORT),
    tickerOn(LOW),
    ota(AC_OTA_EXTRA),
    auth(AC_AUTH_NONE),
    authScope(AC_AUTHSCOPE_AUX),
    username(String("")),
    password(String("")),
    hostName(String("")),
    homeUri(AUTOCONNECT_HOMEURI),
    title(AUTOCONNECT_MENU_TITLE),
    staip(0U),
    staGateway(0U),
    staNetmask(0U),
    dns1(0U),
    dns2(0U) {}

  ~AutoConnectConfig() {}
  AutoConnectConfig& operator=(const AutoConnectConfig& o);
  bool load(const uint16_t offset = AC_CONFIGSTORE_OFFSET);
  bool save(const uint16_t offset = AC_CONFIGSTORE_OFFSET);

  IPAddress apip;               /**< SoftAP IP address */
  IPAddress gateway;            /**< SoftAP gateway address */
  IPAddress netmask;            /**< SoftAP subnet mask */
  String    apid;               /**< SoftAP SSID */
  String    psk;                /**< SoftAP password */
  uint8_t   channel;            /**< SoftAP used wifi channel */
  uint8_t   hidden;             /**< SoftAP SSID hidden */
  int16_t   minRSSI;            /**< Lowest WiFi signal strength (RSSI) that can be connected. */
  AC_SAVECREDENTIAL_t  autoSave;  /**< Auto save credential */
  AC_ONBOOTURI_t  bootUri;      /**< An uri invoking after reset */
  AC_PRINCIPLE_t  principle;    /**< WiFi connection principle */  
  uint16_t  boundaryOffset;     /**< The save storage offset of EEPROM */
  int       uptime;             /**< Length of start up time */
  bool      autoRise;           /**< Automatic starting the captive portal */
  bool      autoReset;          /**< Reset ESP8266 module automatically when WLAN disconnected. */
  bool      autoReconnect;      /**< Automatic reconnect with past SSID */
  bool      immediateStart;     /**< Skips WiFi.begin(), start portal immediately */
  bool      retainPortal;       /**< Even if the captive portal times out, it maintains the portal state. */
  bool      preserveAPMode;     /**< Keep existing AP WiFi mode if captive portal won't be started. */
  unsigned long beginTimeout;   /**< Timeout value for WiFi.begin */
  unsigned long portalTimeout;  /**< Timeout value for stay in the captive portal */
  uint16_t  menuItems;          /**< A compound value of the menu items to be attached */
  uint8_t   reconnectInterval;  /**< Auto-reconnect attempt interval uint */
  bool      ticker;             /**< Drives LED flicker according to WiFi connection status. */
  uint8_t   tickerPort;         /**< GPIO for flicker */
  uint8_t   tickerOn;           /**< A signal for flicker turn on */
  AC_OTA_t  ota;                /**< Attach built-in OTA */
  AC_AUTH_t auth;               /**< Enable authentication */
  uint16_t  authScope;          /**< Authetication scope */
  String    username;           /**< User name for authentication */
  String    password;           /**< Authentication password */
  String    hostName;           /**< host name */
  String    homeUri;            /**< A URI of user site */
  String    title;              /**< Menu title */
  IPAddress staip;              /**< Station static IP address */
  IPAddress staGateway;         /**< Station gateway address */
  IPAddress staNetmask;         /**< Station subnet mask */
  IPAddress dns1;               /**< Primary DNS server */
  IPAddress dns2;               /**< Secondary DNS server */

 private:
  template<class T, uint8_t s, uint8_t e = s>
  struct _bits {
    T ref;
    static constexpr  T mask = (T)(~( (T)(~0) << (e - s + 1))) << s;
    void  operator=(const T val) { ref = (ref & ~mask) | ((val & (mask >> s)) << s); }
    operator T() const { return (ref & mask) >> s; }
  };

  template<uint8_t s, uint8_t e = s>
  using bits8 = _bits<uint8_t, s, e>;

  // AutoConnectConfig in the archive, which is the layout structure on
  // the persistent area of the AutoConnectConfig settings.
  // Header section has ID and the size of ACConfigStore_t excluding ID.
  typedef struct {
    char      id[sizeof(AC_CONFIGSTORE_IDENTIFIER) - sizeof('\0')];
    uint16_t  size;             /**< Size of ACConfigStore_t excluding id */
  } ACConfigHeader_t;
  // Body section
  typedef struct {
    ACConfigHeader_t  header;
    union {
      uint8_t   _raw = 0;
      bits8<0>  autoRise;
      bits8<1>  autoReset;
      bits8<2>  autoReconnect;
      bits8<3>  immediateStart;
      bits8<4>  retainPortal;
      bits8<5>  preserveAPMode;
      bits8<6>  ticker;
      bits8<7>  tickerOn;
    } ctl1;
    uint8_t   ctl2 = 0; // reserved byte
    uint32_t  apip;
    uint32_t  apgw;
    uint32_t  apnm;
    uint32_t  stip;
    uint32_t  stgw;
    uint32_t  stnm;
    uint32_t  dns1;
    uint32_t  dns2;
    uint32_t  beginTimeout;
    uint32_t  portalTimeout;
    uint16_t  boundaryOffset;
    int16_t   minRSSI;
    uint16_t  menuItems;
    int16_t   uptime;
    uint16_t  authScope;
    uint8_t   auth;
    uint8_t   channel;
    uint8_t   hidden;
    uint8_t   autoSave;
    uint8_t   bootUri;
    uint8_t   principle;
    uint8_t   reconnectInterval;
    uint8_t   ota;
    uint8_t   tickerPort;
    char      stringSet[1];
  } ACConfigStore_t;

  void  _restore(const ACConfigStore_t* pconf);
  void  _archive(ACConfigStore_t* pconf, const size_t size);
};

#endif // !_AUTOCONNECTCONFIG_H_
