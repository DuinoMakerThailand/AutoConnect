/**
 *	Declaration of AutoConnect class and accompanying AutoConnectConfig class.
 *	@file	AutoConnect.h
 *	@author	hieromon@gmail.com
 *	@version	1.3.0
 *	@date	2021-01-21
 *	@copyright	MIT license.
 */

#ifndef _AUTOCONNECT_H_
#define _AUTOCONNECT_H_

#include <vector>
#include <memory>
#include <functional>
#include <DNSServer.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
extern "C" {
#include <user_interface.h>
}
using WebServerClass = ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
using WebServerClass = WebServer;
#endif
#include <EEPROM.h>
#include <PageBuilder.h>
#include "AutoConnectDefs.h"
#include "AutoConnectPage.h"
#include "AutoConnectCredential.h"
#include "AutoConnectTicker.h"
#include "AutoConnectAux.h"
#include "AutoConnectTypes.h"

// The realization of AutoConnectOTA is effective only by the explicit
#include "AutoConnectOTA.h"
class AutoConnectOTA;  // Reference to avoid circular

// The realization of AutoConnectUpdate is effective only by the explicit
// definition of AUTOCONNECT_USE_UPDATE
#include "AutoConnectUpdate.h"
class AutoConnectUpdate;  // Reference to avoid circular

#include "AutoConnectConfig.h"

typedef std::vector<std::reference_wrapper<AutoConnectAux>> AutoConnectAuxVT;

class AutoConnect {
 public:
  AutoConnect();
  AutoConnect(WebServerClass& webServer);
  virtual ~AutoConnect();
  bool  begin(void);
  bool  begin(const char* ssid, const char* passphrase = nullptr, unsigned long timeout = 0);
  bool  config(AutoConnectConfig& Config);
  bool  config(const char* ap, const char* password = nullptr);
  void  end(void);
  uint16_t getEEPROMUsedSize(void);
  void  handleClient(void);
  void  handleRequest(void);
  void  home(const String& uri);
  WebServerClass& host(void);
  String where(void) const { return _auxUri; }

  AutoConnectAux* aux(const String& uri) const;
  AutoConnectAux* append(const String& uri, const String& title);
  AutoConnectAux* append(const String& uri, const String& title, WebServerClass::THandlerFunction handler);
  bool  detach(const String& uri);
  inline void disableMenu(const uint16_t items) { _apConfig.menuItems &= (0xffff ^ items); }
  inline void enableMenu(const uint16_t items) { _apConfig.menuItems |= items; }
  void  join(AutoConnectAux& aux);
  void  join(AutoConnectAuxVT auxVector);
  bool  on(const String& uri, const AuxHandlerFunctionT handler, AutoConnectExitOrder_t order = AC_EXIT_AHEAD);

  /** For AutoConnectAux described in JSON */
#ifdef AUTOCONNECT_USE_JSON
  bool  load(PGM_P aux);
  bool  load(const __FlashStringHelper* aux);
  bool  load(const String& aux);
  bool  load(Stream& aux);
#endif // !AUTOCONNECT_USE_JSON

  typedef std::function<bool(IPAddress&)>  DetectExit_ft;
  typedef std::function<void(IPAddress&)>  ConnectExit_ft;
  typedef std::function<bool(void)>        WhileCaptivePortalExit_ft;
  void  onDetect(DetectExit_ft fn);
  void  onConnect(ConnectExit_ft fn);
  void  onNotFound(WebServerClass::THandlerFunction fn);
  void  whileCaptivePortal(WhileCaptivePortalExit_ft fn);

 protected:
  typedef enum {
    AC_RECONNECT_SET,
    AC_RECONNECT_RESET
  } AC_STARECONNECT_t;
  typedef enum {
    AC_SEEKMODE_ANY,
    AC_SEEKMODE_NEWONE,
    AC_SEEKMODE_CURRENT
  } AC_SEEKMODE_t;
  void  _authentication(bool allow);
  void  _authentication(bool allow, const HTTPAuthMethod method);
  bool  _configAP(void);
  bool  _configSTA(const IPAddress& ip, const IPAddress& gateway, const IPAddress& netmask, const IPAddress& dns1, const IPAddress& dns2);
  String _getBootUri(void);
  bool  _getConfigSTA(station_config_t* config);
  bool  _loadAvailCredential(const char* ssid, const AC_PRINCIPLE_t principle = AC_PRINCIPLE_RECENT, const bool excludeCurrent = false);
  bool  _loadCurrentCredential(char* ssid, char* password, const AC_PRINCIPLE_t principle, const bool excludeCurrent);
  bool  _seekCredential(const AC_PRINCIPLE_t principle, const AC_SEEKMODE_t mode);
  void  _startWebServer(void);
  void  _startDNSServer(void);
  void  _stopDNSServer(void);
  void  _stopPortal(void);
  bool  _classifyHandle(HTTPMethod mothod, String uri);
  void  _handleUpload(const String& requestUri, const HTTPUpload& upload);
  void  _handleNotFound(void);
  void  _purgePages(void);
  virtual PageElement*  _setupPage(String& uri);
#ifdef AUTOCONNECT_USE_JSON
  template<typename T>
  bool  _parseJson(T in);
  bool  _load(JsonVariant& aux);
#endif // !AUTOCONNECT_USE_JSON

  /** Request handlers implemented by Page Builder */
  String  _induceConnect(PageArgument& args);
  String  _induceDisconnect(PageArgument& args);
  String  _induceReset(PageArgument& args);
  String  _invokeResult(PageArgument& args);

  /** For portal control */
  bool  _captivePortal(void);
  bool  _hasTimeout(unsigned long timeout);
  bool  _isIP(String ipStr);
  void  _softAP(void);
  wl_status_t _waitForConnect(unsigned long timeout);
  void  _waitForEndTransmission(void);
  void  _disconnectWiFi(bool wifiOff);
  void  _setReconnect(const AC_STARECONNECT_t order);

  /** Utilities */
  String               _attachMenuItem(const AC_MENUITEM_t item);
  static uint32_t      _getChipId(void);
  static uint32_t      _getFlashChipRealSize(void);
  static String        _toMACAddressString(const uint8_t mac[]);
  static unsigned int  _toWiFiQuality(int32_t rssi);
  ConnectExit_ft       _onConnectExit;
  DetectExit_ft        _onDetectExit;
  WhileCaptivePortalExit_ft _whileCaptivePortal;
  WebServerClass::THandlerFunction _notFoundHandler;
  size_t               _freeHeapSize;

  /** Servers which works in concert. */
  typedef std::unique_ptr<WebServerClass, std::function<void(WebServerClass *)> > WebserverUP;
  WebserverUP _webServer = WebserverUP(nullptr, std::default_delete<WebServerClass>());
  std::unique_ptr<DNSServer>      _dnsServer;

  /**
   *  Dynamically hold one page of AutoConnect menu.
   *  Every time a GET/POST HTTP request occurs, an AutoConnect
   *  menu page corresponding to the URI is generated.
   */
  std::unique_ptr<PageBuilder> _responsePage;
  std::unique_ptr<PageElement> _currentPageElement;

  /** Extended pages made up with AutoConnectAux */
  AutoConnectAux* _aux = nullptr; /**< A top of registered AutoConnectAux */
  String        _auxUri;        /**< Last accessed AutoConnectAux */
  String        _prevUri;       /**< Previous generated page uri */
  /** Available updater, only reset by AutoConnectUpdate::attach is valid */
  std::unique_ptr<AutoConnectUpdate>  _update;
  /** OTA updater */
  std::unique_ptr<AutoConnectOTA>     _ota;

  /** Saved configurations */
  AutoConnectConfig  _apConfig;
  station_config_t   _credential;
  uint8_t       _hiddenSSIDCount;
  int16_t       _scanCount;
  uint8_t       _connectCh;
  unsigned long _portalAccessPeriod;
  unsigned long _attemptPeriod;

  /** The control indicators */
  bool  _rfAdHocBegin = false;  /**< Specified with AutoConnect::begin */
  bool  _rfConnect = false;     /**< URI /connect requested */
  bool  _rfDisconnect = false;  /**< URI /disc requested */
  bool  _rfReset = false;       /**< URI /reset requested */
  wl_status_t   _rsConnect;     /**< connection result */
#ifdef ARDUINO_ARCH_ESP32
  WiFiEventId_t _disconnectEventId = -1; /**< STA disconnection event handler registered id  */
#endif
  /** Only available with ticker enabled */
  std::unique_ptr<AutoConnectTicker>  _ticker;

  /** HTTP header information of the currently requested page. */
  IPAddress     _currentHostIP; /**< host IP address */
  String        _uri;           /**< Requested URI */
  String        _redirectURI;   /**< Redirect destination */
  String        _menuTitle;     /**< Title string of the page */

  /** PageElements of AutoConnect site. */
  static const char _CSS_BASE[] PROGMEM;
  static const char _CSS_LUXBAR[] PROGMEM;
  static const char _CSS_UL[] PROGMEM;
  static const char _CSS_ICON_LOCK[] PROGMEM;
  static const char _CSS_INPUT_BUTTON[] PROGMEM;
  static const char _CSS_INPUT_TEXT[] PROGMEM;
  static const char _CSS_TABLE[] PROGMEM;
  static const char _CSS_SPINNER[] PROGMEM;
  static const char _ELM_HTML_HEAD[] PROGMEM;
  static const char _ELM_MENU_PRE[] PROGMEM;
  static const char _ELM_MENU_AUX[] PROGMEM;
  static const char _ELM_MENU_POST[] PROGMEM;
  static const char _PAGE_STAT[] PROGMEM;
  static const char _PAGE_CONFIGNEW[] PROGMEM;
  static const char _PAGE_CONNECTING[] PROGMEM;
  static const char _PAGE_OPENCREDT[] PROGMEM;
  static const char _PAGE_SUCCESS[] PROGMEM;
  static const char _PAGE_RESETTING[] PROGMEM;
  static const char _PAGE_DISCONN[] PROGMEM;
  static const char _PAGE_FAIL[] PROGMEM;
  static const char _PAGE_404[] PROGMEM;

  /** Token handlers for PageBuilder */
  String _token_CSS_BASE(PageArgument& args);
  String _token_CSS_ICON_LOCK(PageArgument& args);
  String _token_CSS_INPUT_BUTTON(PageArgument& args);
  String _token_CSS_INPUT_TEXT(PageArgument& args);
  String _token_CSS_LUXBAR(PageArgument& args);
  String _token_CSS_SPINNER(PageArgument& args);
  String _token_CSS_TABLE(PageArgument& args);
  String _token_CSS_UL(PageArgument& args);
  String _token_MENU_AUX(PageArgument& args);
  String _token_MENU_POST(PageArgument& args);
  String _token_MENU_PRE(PageArgument& args);
  String _token_AP_MAC(PageArgument& args);
  String _token_BOOTURI(PageArgument& args);
  String _token_CHANNEL(PageArgument& args);
  String _token_CHIP_ID(PageArgument& args);
  String _token_CONFIG_STAIP(PageArgument& args);
  String _token_CPU_FREQ(PageArgument& args);
  String _token_CURRENT_SSID(PageArgument& args);
  String _token_DBM(PageArgument& args);
  String _token_ESTAB_SSID(PageArgument& args);
  String _token_FLASH_SIZE(PageArgument& args);
  String _token_FREE_HEAP(PageArgument& args);
  String _token_GATEWAY(PageArgument& args);
  String _token_HEAD(PageArgument& args);
  String _token_HIDDEN_COUNT(PageArgument& args);
  String _token_LIST_SSID(PageArgument& args);
  String _token_LOCAL_IP(PageArgument& args);
  String _token_NETMASK(PageArgument& args);
  String _token_OPEN_SSID(PageArgument& args);
  String _token_SOFTAP_IP(PageArgument& args);
  String _token_SSID_COUNT(PageArgument& args);
  String _token_STA_MAC(PageArgument& args);
  String _token_STATION_STATUS(PageArgument& args);
  String _token_UPTIME(PageArgument& args);
  String _token_WIFI_MODE(PageArgument& args);
  String _token_WIFI_STATUS(PageArgument& args);

 private:
  // The access point collation key is determined at compile time
  // according to the AUTOCONNECT_APKEY_SSID definition, which is
  inline bool _isValidAP(const station_config_t& config, const uint8_t item) const {
#if defined(AUTOCONNECT_APKEY_SSID)
    return !strcmp(reinterpret_cast<const char*>(config.ssid), WiFi.SSID(item).c_str());
#else
    return !memcmp(config.bssid, WiFi.BSSID(item), sizeof(station_config_t::bssid));
#endif
  }

  // After a reboot without WiFi.disconnect() a WiFi error Reason202
  // - AUTH_FAIL occurs with some routers: the connection was not broken
  // off correctly.
  // The _reconnectDelay makes a duration of the delay to reconnect to
  // avoid the unconnectedly. issue #292
  // This functino will be eventually pulled out since the issue will
  // be gone with the WiFi lib of the arduino-esp32 core.
  inline void _reconnectDelay(const uint32_t ms) {
#if defined(ARDUINO_ARCH_ESP32) && AUTOCONNECT_RECONNECT_DELAY > 0
    AC_DBG_DUMB(", %" PRIu32 " delay duration", AUTOCONNECT_RECONNECT_DELAY);
    WiFi.disconnect(true, false);
    delay(ms);
#else
    AC_UNUSED(ms);
#endif
  }

  static const  String  _emptyString; /**< An empty string allocation  **/

#if defined(ARDUINO_ARCH_ESP8266)
  friend ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
  friend class WebServer;
#endif

  friend class AutoConnectAux;
  friend class AutoConnectUpdate;
};

#endif  // _AUTOCONNECT_H_
