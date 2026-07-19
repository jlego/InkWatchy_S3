#include "gitDebug.h"

#if DEBUG_MENUS

void initGitDebug()
{
    gfxGeneralPageInit(6);
    gfxGeneralPageSetTitle(DEBUG_MENU_GIT);

    gfxGeneralPageAddKeyValue(DEBUG_GIT_COMMIT_HASH, GIT_COMMIT_HASH);

    String generationHash = fsGetString("resources_commit", "NULL");
    generationHash.trim();
    gfxGeneralPageAddKeyValue("Generation hash:", generationHash.c_str());

    gfxGeneralPageAddKeyValue(DEBUG_GIT_BRANCH, GIT_BRANCH);
    gfxGeneralPageAddKeyValue(DEBUG_GIT_BUILD_TIME, BUILD_TIME);

    gfxGeneralPageSetMain();    
}

void loopGitDebug()
{
    gfxGeneralPageLoop();
}

#endif