#include "PlexFile.h"
#include "Client/PlexServerManager.h"
#include "utils/log.h"
#include "settings/GUISettings.h"
#include "boost/lexical_cast.hpp"

using namespace XFILE;

CPlexFile::CPlexFile(void) : CCurlFile()
{
  SetRequestHeader("X-Plex-Version", PLEX_VERSION);
  SetRequestHeader("X-Plex-Client-Platform", PlexUtils::GetMachinePlatform());
  SetRequestHeader("X-Plex-Client-Identifier", g_guiSettings.GetString("system.uuid"));
  SetRequestHeader("X-Plex-Platform", PlexUtils::GetMachinePlatform());
  SetRequestHeader("X-Plex-Provides", "player");
  SetRequestHeader("X-Plex-Product", "Plex for Home Theater");

  if (g_myplexManager.IsSignedIn())
  {
    SetRequestHeader("X-Plex-Account", boost::lexical_cast<std::string>(g_myplexManager.GetCurrentUserInfo().id));
    SetRequestHeader("X-Plex-Username", g_myplexManager.GetCurrentUserInfo().username);
  }
}

bool
CPlexFile::BuildHTTPURL(CURL& url)
{
  CURL newUrl;

  /* Resolve the correct URL */
  CStdString uuid = url.GetHostName();
  CPlexServerPtr server = g_plexServerManager.FindByUUID(uuid);
  if (!server)
  {
    /* Ouch, this should not happen! */
    CLog::Log(LOGWARNING, "CPlexFile::BuildHTTPURL tried to lookup server %s but it was not found!", uuid.c_str());
    return false;
  }

  CLog::Log(LOGDEBUG, "CPlexFile::BuildHTTURL Passing %s to BuildURL", url.GetFileName().c_str());

  newUrl = server->BuildURL(url.GetFileName(), url.GetOptions());

  if (!url.GetUserName().empty())
    newUrl.SetUserName(url.GetUserName());
  if (!url.GetPassWord().empty())
    newUrl.SetPassword(url.GetPassWord());

  CLog::Log(LOGDEBUG, "CPlexFile::BuildHTTPURL translated '%s' to '%s'", url.Get().c_str(), newUrl.Get().c_str());
  url = newUrl;

  return true;
}

bool
CPlexFile::Open(const CURL &url)
{
  CURL newUrl(url);
  if (BuildHTTPURL(newUrl))
    return CCurlFile::Open(newUrl);
  return false;
}

int
CPlexFile::Stat(const CURL &url, struct stat *buffer)
{
  CURL newUrl(url);
  if (BuildHTTPURL(newUrl))
    return CCurlFile::Stat(newUrl, buffer);
  return false;
}

bool
CPlexFile::Exists(const CURL &url)
{
  CURL newUrl(url);
  if (BuildHTTPURL(newUrl))
    return CCurlFile::Exists(newUrl);
  return false;
}
