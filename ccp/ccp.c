#include "ccp_parser.h"

static CCP_VersionHandlers *handlers;
static int handlers_cnt;

void
CardinalCCP_SetVersionHandlers(CCP_VersionHandlers *h, int c) {
    handlers_cnt = c;
    handers = h;
}

static char **param_names[] = {
    {"path", "flags", "auth", ""},
    {"ident", "challenge", ""},
    {"code", ""},
    {"fd"}
};


void CardinalCCP_Translate(int major,
                           int minor,
                           CARDINAL_CCP_COMMAND cmd,
                           CARDINAL_CCP_Arg *args,
                           int argc) {

    CCP_VersionHandlers *hndl = NULL;

    for(int i = 0; i < handlers_cnt; i++) {
        if(handlers[i].major == major && handlers[i].minor == minor) {
            hndl = &handlers[i];
            break;
        }
    }

    if(hndl == NULL) {
        //Error
    }

    switch(cmd) {
    case CARDINAL_CCP_OPEN:

        break;
    }

}