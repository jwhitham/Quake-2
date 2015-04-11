#include "opt.h"


extern cl_t cl;
void test0 (void);
void test1 (void);
void test2 (void);
void testf (void);

void setup (void)
{
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
}

int main (void)
{
    uint32_t * x;

    x = (uint32_t *) (&cl.vieworg[0]);
    setup ();
    test0 ();
    printf ("test0: %1.9f %08x\n", (double) cl.vieworg[0], (* x));

    setup ();
    test1 ();
    printf ("test1: %1.9f %08x\n", (double) cl.vieworg[0], (* x));

    setup ();
    test2 ();
    printf ("test2: %1.9f %08x\n", (double) cl.vieworg[0], (* x));

    setup ();
    testf ();
    printf ("testf: %1.9f %08x\n", (double) cl.vieworg[0], (* x));
    return 0;
}

