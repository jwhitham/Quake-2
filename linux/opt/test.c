
#include "xh.h"

cl_t cl;

void test (void)
{
    int         i;
    float       lerp, backlerp;
    ps_t*       ps;
    ps_t*       ops;

    ps = &cl.playerstate;
    ops = &cl.oldplayerstate[cl.n];
    lerp = cl.lerpfrac;

    // calculate the origin
    if (cl.flag)
    {   // use predicted values
        unsigned    delta;

        backlerp = 1.0 - lerp;
        for (i=0 ; i<3 ; i++)
        {
            cl.vieworg[i] = cl.predicted_origin[i] + ops->viewoffset[i]
                + cl.lerpfrac * (ps->viewoffset[i] - ops->viewoffset[i])
                - backlerp * cl.prediction_error[i];
        }
    }
}


