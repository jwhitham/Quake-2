#include "xh.h"


extern cl_t cl;
void test (void);

int main (void)
{
    uint32_t * x;
    cl.lerpfrac = 0.479999989;
    cl.predicted_origin[0] = -29.625;
    cl.predicted_origin[1] = -168.375;
    cl.predicted_origin[2] = 24.125;
    cl.oldplayerstate[0].viewoffset[0] = 0;
    cl.oldplayerstate[0].viewoffset[1] = 0;
    cl.oldplayerstate[0].viewoffset[2] = 23.25;
    cl.playerstate.viewoffset[0] = 0;
    cl.playerstate.viewoffset[1] = 0;
    cl.playerstate.viewoffset[2] = 22.5;
    cl.prediction_error[0] = -22;
    cl.prediction_error[1] = 18;
    cl.prediction_error[2] = 0;
    cl.flag = 1;
    cl.n = 0;
    test ();
    printf ("%1.7f\n", (double) cl.vieworg[0]);
    x = (uint32_t *) (&cl.vieworg[0]);
    printf ("0x%08x\n", x[0]);
    return 0;
}

