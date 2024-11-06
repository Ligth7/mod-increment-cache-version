/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */
#include "Config.h"
#include "DatabaseEnv.h"
#include "DatabaseEnvFwd.h"
#include "Log.h"
#include "WorldScript.h"

class mod_increment_cache_version_world : public WorldScript
{
public:
    mod_increment_cache_version_world() : WorldScript("mod_increment_cache_version_world") { }

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        if(!sConfigMgr->GetOption<bool>("IncrementCacheVersion.Enable", true))
            return;

        // do not increment if ClientCacheVersion is set to non-default value in worldserver.conf
        if (int32 clientCacheId = sConfigMgr->GetOption<int32>("ClientCacheVersion", 0))
        {
            if (clientCacheId != 0)
            {
                LOG_WARN("module.IncrementCacheVersion", "module.IncrementCacheVersion::OnBeforeConfigLoad: ClientCacheVersion is set to non-default value {}. Skipped", clientCacheId);
                return;
            }
        }
        // Increment cache version, assume this data exists
        QueryResult result = WorldDatabase.Query("SELECT cache_id FROM version");
        uint32 cacheId = result->Fetch()[0].Get<uint32>() + 1;
        WorldDatabase.Execute("UPDATE version SET cache_id = '{}'", cacheId);
        LOG_INFO("module.IncrementCacheVersion", "module.IncrementCacheVersion::OnBeforeConfigLoad: Cache version incremented to: {}", cacheId);
    }
};

void AddSC_increment_cache_version_script()
{
    new mod_increment_cache_version_world();
}
