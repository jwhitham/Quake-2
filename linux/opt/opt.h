
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef float vec_t;
typedef vec_t vec3_t[3];

typedef struct ps_t1 {
    vec3_t  viewoffset;
} ps_t;

typedef struct cl_t1 {
    float   lerpfrac;
    vec3_t  predicted_origin;
    ps_t    playerstate;
    ps_t    oldplayerstate[5];
    vec3_t  prediction_error;
    int     flag, n; // flag = 1, n = 0
    vec3_t  vieworg;
} cl_t;
