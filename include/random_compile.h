#pragma once


#define znew  ((rand_seed=36969*(rand_seed&65535)+(rand_seed>>16))<<16)
#define wnew  ((w=18000*(w&65535)+(w>>16))&65535)
#define MWC   (znew+wnew)
#define SHR3  (jsr=(jsr=(jsr=jsr^(jsr<<17))^(jsr>>13))^(jsr<<5))
#define CONG  (jcong=69069*jcong+1234567)
#define RAND  ((MWC^CONG)+SHR3)

long rand_seed = 362436069 * (int)__TIMESTAMP__, w = 521288629 * (int)__TIMESTAMP__, jsr = 123456789 * (int)__TIMESTAMP__, jcong = 380116160 * (int)__TIMESTAMP__;