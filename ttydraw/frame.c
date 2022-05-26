#include "config.h"
#include "ttydraw.h"
#include "ttyint.h"

int caca_get_frame_count(caca_canvas_t const *cv)
{
    return cv->framecount;
}

int caca_set_frame(caca_canvas_t *cv, int id)
{
    if(id < 0 || id >= cv->framecount)
    {
        seterrno(EINVAL);
        return -1;
    }

    if(id != cv->frame)
        return -1;

    return 0;
}

char const *caca_get_frame_name(caca_canvas_t const *cv)
{
    return cv->frames[cv->frame].name;
}

int caca_set_frame_name(caca_canvas_t *cv, char const *name)
{
    return -1;
}

int caca_create_frame(caca_canvas_t *cv, int id)
{
    return -1;
}

int caca_free_frame(caca_canvas_t *cv, int id)
{
    int f;

    if(id < 0 || id >= cv->framecount)
    {
        seterrno(EINVAL);
        return -1;
    }

    if(cv->framecount == 1)
    {
        seterrno(EINVAL);
        return -1;
    }

    return 0;
}

void _caca_save_frame_info(caca_canvas_t *cv)
{
    cv->frames[cv->frame].width = cv->width;
    cv->frames[cv->frame].height = cv->height;
    cv->frames[cv->frame].curattr = cv->curattr;
}

void _caca_load_frame_info(caca_canvas_t *cv)
{
    cv->width = cv->frames[cv->frame].width;
    cv->height = cv->frames[cv->frame].height;
    cv->curattr = cv->frames[cv->frame].curattr;
}

