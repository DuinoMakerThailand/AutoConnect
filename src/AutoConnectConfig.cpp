/**
 *	AutoConnectConfig class implementation.
 *	@file	AutoConnectConfig.h
 *	@author	hieromon@gmail.com
 *	@version	1.3.0
 *	@date	2021-01-21
 *	@copyright	MIT license.
 */

#include "AutoConnectConfig.h"

AutoConnectConfig& AutoConnectConfig::operator=(const AutoConnectConfig& o) {
  apip = o.apip;
  gateway = o.gateway;
  netmask = o.netmask;
  apid = o.apid;
  psk = o.psk;
  channel = o.channel;
  hidden = o.hidden;
  minRSSI=o.minRSSI;
  autoSave = o.autoSave;
  bootUri = o.bootUri;
  principle = o.principle;
  boundaryOffset = o.boundaryOffset;
  uptime = o.uptime;
  autoRise = o.autoRise;
  autoReset = o.autoReset;
  autoReconnect = o.autoReconnect;
  immediateStart = o.immediateStart;
  retainPortal = o.retainPortal;
  preserveAPMode = o.preserveAPMode;
  beginTimeout = o.beginTimeout;
  portalTimeout = o.portalTimeout;
  menuItems = o.menuItems;
  reconnectInterval = o.reconnectInterval;
  ticker = o.ticker;
  tickerPort = o.tickerPort;
  tickerOn = o.tickerOn;
  ota = o.ota;
  auth = o.auth;
  authScope = o.authScope;
  username = o.username;
  password = o.password;
  hostName = o.hostName;
  homeUri = o.homeUri;
  title = o.title;
  staip = o.staip;
  staGateway = o.staGateway;
  staNetmask = o.staNetmask;
  dns1 = o.dns1;
  dns2 = o.dns2;
  return *this;
}

#if defined(ARDUINO_ARCH_ESP8266)
#define NO_GLOBAL_EEPROM
#include <EEPROM.h>

/**
 * Load AutoConnectConfig settings from the persistent storage.
 * In ESP8266, the store for AutoConnectConfig settings is allocated by
 * the EEPROMClass.
 * The offset argument has the starting offset for the saving area of
 * the AutoConnectConfig settings on the EEPROM.
 * @param  offset Offset to AutoConnectConfig saving area
 * @return true   Successfully loaded
 * @return false  Failed to load
 */
bool AutoConnectConfig::load(const uint16_t offset) {
  ACConfigStore_t*  pconf;
  ACConfigHeader_t  pconfHeader;
  size_t  pconfSize;
  EEPROMClass eeprom;

  eeprom.begin(offset + sizeof(ACConfigHeader_t));
  eeprom.get(offset, pconfHeader);
  if (strncmp(pconfHeader.id, AC_CONFIGSTORE_IDENTIFIER, sizeof(pconfHeader.id))) {
    AC_DBG("Illegal AC_CONFG storage %#.4x\n", offset);
    return false;
  }
  eeprom.end();

  pconfSize = sizeof(pconfHeader.id) + pconfHeader.size;
  pconf = reinterpret_cast<ACConfigStore_t*>(malloc(pconfSize));
  if (!pconf) {
    AC_DBG("AutoConnectConfig pbuf allocation failed %u\n", pconfSize);
    return false;
  }
  eeprom.begin(offset + pconfSize);
  uint8_t*  rp = reinterpret_cast<uint8_t*>(pconf);
  size_t  bsz = pconfSize;
  int addr = offset;
  while (bsz--)
    *rp++ = eeprom.read(addr++);
  eeprom.end();
  _restore(pconf);
  free(pconf);

  AC_DBG("AutoConnectConfig loaded\n");
  return true;
}

/**
 * Save AutoConnectConfig settings from the persistent storage.
 * In ESP32, the store for AutoConnectConfig settings is allocated by
 * the EEPROMClass.
 * @param  offset Offset to AutoConnectConfig saving area
 * @return true   Successfully saved
 * @return false  Failed to save
 */
bool AutoConnectConfig::save(const uint16_t offset) {
  EEPROMClass eeprom;

  size_t  stringSetSize = apid.length() + sizeof('0') + psk.length() + sizeof('0') + username.length() + sizeof('0') + password.length() + sizeof('0') + hostName.length() + sizeof('0') + homeUri.length() + sizeof('0') + title.length() + sizeof('0');
  size_t  pconfSize = sizeof(ACConfigStore_t) - sizeof(ACConfigStore_t::stringSet) + stringSetSize;
  ACConfigStore_t*  pconf = reinterpret_cast<ACConfigStore_t*>(malloc(pconfSize));
  if (!pconf) {
    AC_DBG("AutoConnectConfig pbuf allocation failed %u\n", pconfSize);
    return false;
  }
  _archive(pconf, pconfSize);
  
  eeprom.begin(offset + pconfSize);
  eeprom.put(offset, *pconf);
  uint8_t*  wp = reinterpret_cast<uint8_t*>(pconf->stringSet);
  int addr = offset + sizeof(ACConfigStore_t) - sizeof(ACConfigStore_t::stringSet);
  while (stringSetSize--)
    eeprom.write(addr++, *wp++);
  bool  rc = eeprom.commit();
  eeprom.end();
  free(pconf);
  if (rc) {
    AC_DBG("AutoConnectConfig %u bytes saved\n", pconfSize);
  } else {
    AC_DBG("AutoConnectConfig pbuf save failed\n");
  }

  return rc;
}

#elif defined(ARDUINO_ARCH_ESP32)
#include <Preferences.h>

#define AC_CONFIGSTORE_NVSNAME  AC_IDENTIFIER
#define AC_CONFIGSTORE_NVSKEY   AC_CONFIGSTORE_IDENTIFIER

/**
 * Load AutoConnectConfig settings from the persistent storage.
 * In ESP32, the store for AutoConnectConfig settings is allocated by
 * the Preferences.
 * The Offset argument has no effect because Preferences identify the
 * assigned position by the key named AC_CONFIGSTORE_NVSKEY.
 * @param  offset
 * @return true   Successfully loaded
 * @return false  Failed to load, the nvs key not found.
 */
bool AutoConnectConfig::load(const uint16_t offset) {
  AC_UNUSED(offset);
  ACConfigStore_t*  pconf;
  size_t  pconfSize;
  Preferences pref;
  bool  rc = false;

  if (!pref.begin(AC_CONFIGSTORE_NVSNAME, true)) {
    AC_DBG("Preferences %s not found, AutoConnectConfig not loaded\n", AC_CONFIGSTORE_NVSNAME);
    return rc;
  }

  pconfSize = pref.getBytesLength(AC_CONFIGSTORE_NVSKEY);
  if (pconfSize) {
    pconf = reinterpret_cast<ACConfigStore_t*>(malloc(pconfSize));
    if (pconf) {
      pref.getBytes(AC_CONFIGSTORE_NVSKEY, pconf, pconfSize);
      _restore(pconf);
      rc = true;
      AC_DBG("AutoConnectConfig loaded\n");
    }
    else {
      AC_DBG("AutoConnectConfig pbuf allocation failed:%u\n", pconfSize);
    }
    free(pconf);
  }
  else {
    AC_DBG("Preferences %s not found\n", AC_CONFIGSTORE_NVSKEY);
  }
  pref.end();

  return rc;
}

/**
 * Save AutoConnectConfig settings from the persistent storage.
 * In ESP32, the settings are saved to NVS by Preferences based using
 * key as AC_CONFIGSTORE_NVSKEY. The Offset argument has no effect.
 * @param  offset
 * @return true   Successfully saved
 * @return false  Could not create an area with AC_CONFIGSTORE_NVSNAME on NVS.
 */
bool AutoConnectConfig::save(const uint16_t offset) {
  AC_UNUSED(offset);
  Preferences pref;
  bool  rc = false;

  size_t  stringSetSize = apid.length() + sizeof('0') + psk.length() + sizeof('0') + username.length() + sizeof('0') + password.length() + sizeof('0') + hostName.length() + sizeof('0') + homeUri.length() + sizeof('0') + title.length() + sizeof('0');
  size_t  pconfSize = sizeof(ACConfigStore_t) - sizeof(ACConfigStore_t::stringSet) + stringSetSize;
  ACConfigStore_t*  pconf = reinterpret_cast<ACConfigStore_t*>(malloc(pconfSize));
  if (!pconf) {
    AC_DBG("AutoConnectConfig pbuf allocation failed:%u", pconfSize);
    return rc;
  }
  _archive(pconf, pconfSize);

  if (pref.begin(AC_CONFIGSTORE_NVSNAME, false)) {
    pref.putBytes(AC_CONFIGSTORE_NVSKEY, pconf, pconfSize);
    pref.end();
    rc = true;
    AC_DBG("AutoConnectConfig %u bytes saved\n", pconfSize);
  }
  else {
    AC_DBG("Preferences %s not found\n", AC_CONFIGSTORE_NVSKEY);
  }
  free(pconf);

  return rc;
}

#endif

/**
 * Restore from the archive structure to an instance of AutoConnectConfig.
 * @param pconf AutoConnectConfig archive structure
 */
void AutoConnectConfig::_restore(const ACConfigStore_t* pconf) {
  autoRise = pconf->ctl1.autoRise & 0b1;
  autoReset = pconf->ctl1.autoReset & 0b1;
  autoReconnect = pconf->ctl1.autoReconnect & 0b1;
  immediateStart = pconf->ctl1.immediateStart & 0b1;
  retainPortal = pconf->ctl1.retainPortal & 0b1;
  preserveAPMode = pconf->ctl1.preserveAPMode & 0b1;
  ticker = pconf->ctl1.tickerOn & 0b1;
  apip = IPAddress(pconf->apip);
  gateway = IPAddress(pconf->apgw);
  netmask = IPAddress(pconf->apnm);
  staip = IPAddress(pconf->stip);
  staGateway = IPAddress(pconf->stgw);
  staNetmask = IPAddress(pconf->stnm);
  dns1 = IPAddress(pconf->dns1);
  dns2 = IPAddress(pconf->dns2);
  channel = pconf->channel;
  hidden = pconf->hidden;
  autoSave = static_cast<AC_SAVECREDENTIAL_t>(pconf->autoSave);
  bootUri = static_cast<AC_ONBOOTURI_t>(pconf->bootUri);
  principle = static_cast<AC_PRINCIPLE_t>(pconf->principle);
  reconnectInterval = pconf->reconnectInterval;
  ota = static_cast<AC_OTA_t>(pconf->ota);
  auth = static_cast<AC_AUTH_t>(pconf->auth);
  authScope = pconf->authScope;
  minRSSI = pconf->minRSSI;
  uptime = pconf->uptime;
  menuItems = pconf->menuItems;
  boundaryOffset = pconf->boundaryOffset;
  beginTimeout = pconf->beginTimeout;
  portalTimeout = pconf->portalTimeout;
  tickerPort = pconf->tickerPort;

  const char* sp = pconf->stringSet;
  apid = String(sp);
  sp += strlen(sp) + sizeof('\0');
  psk = String(sp);
  sp += strlen(sp) + sizeof('\0');
  username = String(sp);
  sp += strlen(sp) + sizeof('\0');
  password = String(sp);
  sp += strlen(sp) + sizeof('\0');
  hostName = String(sp);
  sp += strlen(sp) + sizeof('\0');
  homeUri = String(sp);
  sp += strlen(sp) + sizeof('\0');
  title = String(sp);
}

/**
 * Storing to the archive structure of an instance of AutoConnectConfig.
 * @param pconf AutoConnectConfig archive structure
 */
void AutoConnectConfig::_archive(ACConfigStore_t* pconf, const size_t size) {
  strncpy(pconf->header.id, AC_CONFIGSTORE_IDENTIFIER, sizeof(pconf->header.id));
  pconf->header.size = size - sizeof(pconf->header.id);
  pconf->ctl1.autoRise = autoRise;
  pconf->ctl1.autoReset = autoReset;
  pconf->ctl1.autoReconnect = autoReconnect;
  pconf->ctl1.immediateStart = immediateStart;
  pconf->ctl1.retainPortal = retainPortal;
  pconf->ctl1.preserveAPMode = preserveAPMode;
  pconf->ctl1.ticker = ticker;
  pconf->ctl1.tickerOn = tickerOn;
  pconf->apip = static_cast<uint32_t>(apip);
  pconf->apgw = static_cast<uint32_t>(gateway);
  pconf->apnm = static_cast<uint32_t>(netmask);
  pconf->stip = static_cast<uint32_t>(staip);
  pconf->stgw = static_cast<uint32_t>(staGateway);
  pconf->stnm = static_cast<uint32_t>(staNetmask);
  pconf->dns1 = static_cast<uint32_t>(dns1);
  pconf->dns2 = static_cast<uint32_t>(dns2);
  pconf->beginTimeout = beginTimeout;
  pconf->portalTimeout = portalTimeout;
  pconf->boundaryOffset = boundaryOffset;
  pconf->minRSSI = minRSSI;
  pconf->menuItems = menuItems;
  pconf->uptime = uptime;
  pconf->authScope = authScope;
  pconf->auth = static_cast<uint8_t>(auth);
  pconf->channel = channel;
  pconf->hidden = hidden;
  pconf->autoSave = static_cast<uint8_t>(autoSave);
  pconf->bootUri = static_cast<uint8_t>(bootUri);
  pconf->principle = static_cast<uint8_t>(principle);
  pconf->reconnectInterval = reconnectInterval;
  pconf->ota = static_cast<uint8_t>(ota);
  pconf->tickerPort = tickerPort;

  char* sp = pconf->stringSet;
  strcpy(sp, apid.c_str());
  sp += apid.length() + sizeof('\0');
  strcpy(sp, psk.c_str());
  sp += psk.length() + sizeof('\0');
  strcpy(sp, username.c_str());
  sp += username.length() + sizeof('\0');
  strcpy(sp, password.c_str());
  sp += password.length() + sizeof('\0');
  strcpy(sp, hostName.c_str());
  sp += hostName.length() + sizeof('\0');
  strcpy(sp, homeUri.c_str());
  sp += homeUri.length() + sizeof('\0');
  strcpy(sp, title.c_str());
}
