static void FNAME(white_untextured) (ZBuffer *zb,
                                     ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{
#define INTERP_Z

#define EARLY_OUT() 				\
  {						\
  }

#define DRAW_INIT()                             \
  {                                             \
  }
 
#define PUT_PIXEL(_a)                                                   \
  {                                                                     \
    zz=z >> ZB_POINT_Z_FRAC_BITS;                                       \
    if (ZCMP(pz[_a], zz)) {                                             \
      STORE_PIX(pp[_a], 0xffffffffUL, 0xffffUL, 0xffffUL, 0xffffUL, 0xffffUL); \
      STORE_Z(pz[_a], zz);                                              \
    }                                                                   \
    z+=dzdx;                                                            \
  }

#include "ztriangle.h"
}

static void FNAME(flat_untextured) (ZBuffer *zb,
                                    ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{
  int color;
  int or0, og0, ob0, oa0;

#define INTERP_Z

#define EARLY_OUT() 				\
  {						\
  }

#define DRAW_INIT()                             \
  {                                             \
    if (!ACMP(zb, p2->a)) {                     \
      return;                                   \
    }                                           \
    or0 = p2->r;                                \
    og0 = p2->g;                                \
    ob0 = p2->b;                                \
    oa0 = p2->a;                                \
    color=RGBA_TO_PIXEL(or0, og0, ob0, oa0);    \
  }
 
#define PUT_PIXEL(_a)                                   \
  {                                                     \
    zz=z >> ZB_POINT_Z_FRAC_BITS;                       \
    if (ZCMP(pz[_a], zz)) {                             \
      STORE_PIX(pp[_a], color, or0, og0, ob0, oa0);     \
      STORE_Z(pz[_a], zz);                              \
    }                                                   \
    z+=dzdx;                                            \
  }

#include "ztriangle.h"
}

/*
 * Smooth filled triangle.
 * The code below is very tricky :)
 */

static void FNAME(smooth_untextured) (ZBuffer *zb,
                                      ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{
#define INTERP_Z
#define INTERP_RGB

#define EARLY_OUT()                                     \
  {                                                     \
    int c0, c1, c2;                                     \
    c0 = RGBA_TO_PIXEL(p0->r, p0->g, p0->b, p0->a);     \
    c1 = RGBA_TO_PIXEL(p1->r, p1->g, p1->b, p1->a);     \
    c2 = RGBA_TO_PIXEL(p2->r, p2->g, p2->b, p2->a);     \
    if (c0 == c1 && c0 == c2) {                         \
      /* It's really a flat-shaded triangle. */         \
      FNAME(flat_untextured)(zb, p0, p1, p2);           \
      return;                                           \
    }                                                   \
  }
  
#define DRAW_INIT() 				\
  {						\
  }

#define PUT_PIXEL(_a)                                                   \
  {                                                                     \
    zz=z >> ZB_POINT_Z_FRAC_BITS;                                       \
    if (ZCMP(pz[_a], zz)) {                                             \
      if (ACMP(zb, oa1)) {                                              \
        STORE_PIX(pp[_a], RGBA_TO_PIXEL(or1, og1, ob1, oa1), or1, og1, ob1, oa1); \
        STORE_Z(pz[_a], zz);                                            \
      }                                                                 \
    }                                                                   \
    z+=dzdx;                                                            \
    og1+=dgdx;                                                          \
    or1+=drdx;                                                          \
    ob1+=dbdx;                                                          \
    oa1+=dadx;                                                          \
  }

#include "ztriangle.h"
}

static void FNAME(white_textured) (ZBuffer *zb,
                                   ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{
  ZTextureLevel *texture_levels;

#define INTERP_Z
#define INTERP_ST

#define EARLY_OUT() 				\
  {						\
  }

#define DRAW_INIT()				\
  {						\
    texture_levels = zb->current_texture;       \
  }

#define PUT_PIXEL(_a)                                                   \
  {                                                                     \
    zz=z >> ZB_POINT_Z_FRAC_BITS;                                       \
    if (ZCMP(pz[_a], zz)) {                                             \
      tmp = ZB_LOOKUP_TEXTURE(texture_levels, s, t, mipmap_level, mipmap_dx);      \
      if (ACMP(zb, PIXEL_A(tmp))) {                                     \
        STORE_PIX(pp[_a], tmp, PIXEL_R(tmp), PIXEL_G(tmp), PIXEL_B(tmp), PIXEL_A(tmp)); \
        STORE_Z(pz[_a], zz);                                            \
      }                                                                 \
    }                                                                   \
    z+=dzdx;                                                            \
    s+=dsdx;                                                            \
    t+=dtdx;                                                            \
  }

#include "ztriangle.h"
}

static void FNAME(flat_textured) (ZBuffer *zb,
                                  ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{
  ZTextureLevel *texture_levels;
  int or0, og0, ob0, oa0;

#define INTERP_Z
#define INTERP_ST

#define EARLY_OUT() 				\
  {						\
  }

#define DRAW_INIT()				\
  {						\
    if (p2->a == 0 && !ACMP(zb, p2->a)) {       \
      /* This alpha is zero, and we'll never get other than 0. */       \
      return;                                   \
    }                                           \
    texture_levels = zb->current_texture;       \
    or0 = p2->r;                                \
    og0 = p2->g;                                \
    ob0 = p2->b;                                \
    oa0 = p2->a;                                \
  }

#define PUT_PIXEL(_a)                                                   \
  {                                                                     \
    zz=z >> ZB_POINT_Z_FRAC_BITS;                                       \
    if (ZCMP(pz[_a], zz)) {                                             \
      tmp = ZB_LOOKUP_TEXTURE(texture_levels, s, t, mipmap_level, mipmap_dx);      \
      int a = oa0 * PIXEL_A(tmp) >> 16;                                 \
      if (ACMP(zb, a)) {                                                \
        STORE_PIX(pp[_a],                                               \
                  RGBA_TO_PIXEL(or0 * PIXEL_R(tmp) >> 16,               \
                                og0 * PIXEL_G(tmp) >> 16,               \
                                ob0 * PIXEL_B(tmp) >> 16,               \
                                a),                                     \
                  or0 * PIXEL_R(tmp) >> 16,                             \
                  og0 * PIXEL_G(tmp) >> 16,                             \
                  ob0 * PIXEL_B(tmp) >> 16,                             \
                  a);                                                   \
        STORE_Z(pz[_a], zz);                                            \
      }                                                                 \
    }                                                                   \
    z+=dzdx;                                                            \
    s+=dsdx;                                                            \
    t+=dtdx;                                                            \
  }

#include "ztriangle.h"
}

static void FNAME(smooth_textured) (ZBuffer *zb,
                                    ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{
  ZTextureLevel *texture_levels;

#define INTERP_Z
#define INTERP_ST
#define INTERP_RGB

#define EARLY_OUT()                                     \
  {                                                     \
    int c0, c1, c2;                                     \
    c0 = RGBA_TO_PIXEL(p0->r, p0->g, p0->b, p0->a);     \
    c1 = RGBA_TO_PIXEL(p1->r, p1->g, p1->b, p1->a);     \
    c2 = RGBA_TO_PIXEL(p2->r, p2->g, p2->b, p2->a);     \
    if (c0 == c1 && c0 == c2) {                         \
      /* It's really a flat-shaded triangle. */         \
      if (c0 == 0xffffffff) {                           \
        /* Actually, it's a white triangle. */          \
        FNAME(white_textured)(zb, p0, p1, p2);          \
        return;                                         \
      }                                                 \
      FNAME(flat_textured)(zb, p0, p1, p2);             \
      return;                                           \
    }                                                   \
  }

#define DRAW_INIT()                             \
  {                                             \
    texture_levels = zb->current_texture;       \
  }

#define PUT_PIXEL(_a)                                                   \
  {                                                                     \
    zz=z >> ZB_POINT_Z_FRAC_BITS;                                       \
    if (ZCMP(pz[_a], zz)) {                                             \
      tmp = ZB_LOOKUP_TEXTURE(texture_levels, s, t, mipmap_level, mipmap_dx);      \
      int a = oa1 * PIXEL_A(tmp) >> 16;                                 \
      if (ACMP(zb, a)) {                                                \
        STORE_PIX(pp[_a],                                               \
                  RGBA_TO_PIXEL(or1 * PIXEL_R(tmp) >> 16,               \
                                og1 * PIXEL_G(tmp) >> 16,               \
                                ob1 * PIXEL_B(tmp) >> 16,               \
                                a),                                     \
                  or1 * PIXEL_R(tmp) >> 16,                             \
                  og1 * PIXEL_G(tmp) >> 16,                             \
                  ob1 * PIXEL_B(tmp) >> 16,                             \
                  a);                                                   \
        STORE_Z(pz[_a], zz);                                            \
      }                                                                 \
    }                                                                   \
    z+=dzdx;                                                            \
    og1+=dgdx;                                                          \
    or1+=drdx;                                                          \
    ob1+=dbdx;                                                          \
    oa1+=dadx;                                                          \
    s+=dsdx;                                                            \
    t+=dtdx;                                                            \
  }

#include "ztriangle.h"
}

/*
 * Texture mapping with perspective correction.
 * We use the gradient method to make less divisions.
 * TODO: pipeline the division
 */
static void FNAME(white_perspective) (ZBuffer *zb,
                                      ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{
  ZTextureLevel *texture_levels;
  float fdzdx,fndzdx,ndszdx,ndtzdx;

#define INTERP_Z
#define INTERP_STZ

#define NB_INTERP 8

#define EARLY_OUT() 				\
  {						\
  }

#define DRAW_INIT()				\
  {						\
    texture_levels = zb->current_texture;       \
    fdzdx=(float)dzdx;                          \
    fndzdx=NB_INTERP * fdzdx;                   \
    ndszdx=NB_INTERP * dszdx;                   \
    ndtzdx=NB_INTERP * dtzdx;                   \
  }


#define PUT_PIXEL(_a)                                                   \
  {                                                                     \
    zz=z >> ZB_POINT_Z_FRAC_BITS;                                       \
    if (ZCMP(pz[_a], zz)) {                                             \
      tmp = ZB_LOOKUP_TEXTURE(texture_levels, s, t, mipmap_level, mipmap_dx);      \
      if (ACMP(zb, PIXEL_A(tmp))) {                                     \
        STORE_PIX(pp[_a], tmp, PIXEL_R(tmp), PIXEL_G(tmp), PIXEL_B(tmp), PIXEL_A(tmp)); \
        STORE_Z(pz[_a], zz);                                            \
      }                                                                 \
    }                                                                   \
    z+=dzdx;                                                            \
    s+=dsdx;                                                            \
    t+=dtdx;                                                            \
  }

#define DRAW_LINE()                                     \
  {                                                     \
    register ZPOINT *pz;                                \
    register PIXEL *pp;                                 \
    register int s,t,z,zz;                     \
    register int n,dsdx,dtdx;                           \
    float sz,tz,fz,zinv;                                \
    n=(x2>>16)-x1;                                      \
    fz=(float)z1;                                       \
    zinv=1.0f / fz;                                     \
    pp=(PIXEL *)((char *)pp1 + x1 * PSZB);              \
    pz=pz1+x1;                                          \
    z=z1;						\
    sz=sz1;                                             \
    tz=tz1;                                             \
    while (n>=(NB_INTERP-1)) {                          \
      {                                                 \
        float ss,tt;                                    \
        ss=(sz * zinv);                                 \
        tt=(tz * zinv);                                 \
        s=(int) ss;                            \
        t=(int) tt;                            \
        dsdx= (int)( (dszdx - ss*fdzdx)*zinv );         \
        dtdx= (int)( (dtzdx - tt*fdzdx)*zinv );         \
        CALC_MIPMAP_LEVEL;                              \
        fz+=fndzdx;                                     \
        zinv=1.0f / fz;                                 \
      }                                                 \
      PUT_PIXEL(0);                                     \
      PUT_PIXEL(1);                                     \
      PUT_PIXEL(2);                                     \
      PUT_PIXEL(3);                                     \
      PUT_PIXEL(4);                                     \
      PUT_PIXEL(5);                                     \
      PUT_PIXEL(6);                                     \
      PUT_PIXEL(7);                                     \
      pz+=NB_INTERP;                                    \
      pp=(PIXEL *)((char *)pp + NB_INTERP * PSZB);      \
      n-=NB_INTERP;                                     \
      sz+=ndszdx;                                       \
      tz+=ndtzdx;                                       \
    }                                                   \
    {                                                   \
      float ss,tt;                                      \
      ss=(sz * zinv);                                   \
      tt=(tz * zinv);                                   \
      s=(int) ss;                              \
      t=(int) tt;                              \
      dsdx= (int)( (dszdx - ss*fdzdx)*zinv );           \
      dtdx= (int)( (dtzdx - tt*fdzdx)*zinv );           \
      CALC_MIPMAP_LEVEL;                                \
    }                                                   \
    while (n>=0) {                                      \
      PUT_PIXEL(0);                                     \
      pz+=1;                                            \
      pp=(PIXEL *)((char *)pp + PSZB);                  \
      n-=1;                                             \
    }                                                   \
  }
  
#include "ztriangle.h"
}

/*
 * Flat shaded triangle, with perspective-correct mapping.
 */

static void FNAME(flat_perspective) (ZBuffer *zb,
                                     ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{
  ZTextureLevel *texture_levels;
  float fdzdx,fndzdx,ndszdx,ndtzdx;
  int or0, og0, ob0, oa0;

#define INTERP_Z
#define INTERP_STZ
#define INTERP_RGB


#define EARLY_OUT() 				\
  {						\
  }

#define DRAW_INIT() 				\
  {						\
    if (p2->a == 0 && !ACMP(zb, p2->a)) {       \
      /* This alpha is zero, and we'll never get other than 0. */       \
      return;                                   \
    }                                           \
    texture_levels = zb->current_texture;       \
    fdzdx=(float)dzdx;                          \
    fndzdx=NB_INTERP * fdzdx;                   \
    ndszdx=NB_INTERP * dszdx;                   \
    ndtzdx=NB_INTERP * dtzdx;                   \
    or0 = p2->r;                                \
    og0 = p2->g;                                \
    ob0 = p2->b;                                \
    oa0 = p2->a;                                \
  }

#define PUT_PIXEL(_a)                                                   \
  {                                                                     \
    zz=z >> ZB_POINT_Z_FRAC_BITS;                                       \
    if (ZCMP(pz[_a], zz)) {                                             \
      tmp = ZB_LOOKUP_TEXTURE(texture_levels, s, t, mipmap_level, mipmap_dx);      \
      int a = oa0 * PIXEL_A(tmp) >> 16;                                 \
      if (ACMP(zb, a)) {                                                \
        STORE_PIX(pp[_a],                                               \
                  RGBA_TO_PIXEL(or0 * PIXEL_R(tmp) >> 16,               \
                                og0 * PIXEL_G(tmp) >> 16,               \
                                ob0 * PIXEL_B(tmp) >> 16,               \
                                a),                                     \
                  or0 * PIXEL_R(tmp) >> 16,                             \
                  og0 * PIXEL_G(tmp) >> 16,                             \
                  ob0 * PIXEL_B(tmp) >> 16,                             \
                  a);                                                   \
        STORE_Z(pz[_a], zz);                                            \
      }                                                                 \
    }                                                                   \
    z+=dzdx;                                                            \
    s+=dsdx;                                                            \
    t+=dtdx;                                                            \
  }

#define DRAW_LINE()                                     \
  {                                                     \
    register ZPOINT *pz;                                \
    register PIXEL *pp;                                 \
    register int s,t,z,zz;                     \
    register int n,dsdx,dtdx;                           \
    register int or1,og1,ob1,oa1;              \
    float sz,tz,fz,zinv;                                \
    n=(x2>>16)-x1;                                      \
    fz=(float)z1;                                       \
    zinv=1.0f / fz;                                     \
    pp=(PIXEL *)((char *)pp1 + x1 * PSZB);              \
    pz=pz1+x1;                                          \
    z=z1;						\
    sz=sz1;                                             \
    tz=tz1;                                             \
    or1 = r1;                                           \
    og1 = g1;                                           \
    ob1 = b1;                                           \
    oa1 = a1;                                           \
    while (n>=(NB_INTERP-1)) {                          \
      {                                                 \
        float ss,tt;                                    \
        ss=(sz * zinv);                                 \
        tt=(tz * zinv);                                 \
        s=(int) ss;                            \
        t=(int) tt;                            \
        dsdx= (int)( (dszdx - ss*fdzdx)*zinv );         \
        dtdx= (int)( (dtzdx - tt*fdzdx)*zinv );         \
        CALC_MIPMAP_LEVEL;                              \
        fz+=fndzdx;                                     \
        zinv=1.0f / fz;                                 \
      }                                                 \
      PUT_PIXEL(0);                                     \
      PUT_PIXEL(1);                                     \
      PUT_PIXEL(2);                                     \
      PUT_PIXEL(3);                                     \
      PUT_PIXEL(4);                                     \
      PUT_PIXEL(5);                                     \
      PUT_PIXEL(6);                                     \
      PUT_PIXEL(7);                                     \
      pz+=NB_INTERP;                                    \
      pp=(PIXEL *)((char *)pp + NB_INTERP * PSZB);      \
      n-=NB_INTERP;                                     \
      sz+=ndszdx;                                       \
      tz+=ndtzdx;                                       \
    }                                                   \
    {                                                   \
      float ss,tt;                                      \
      ss=(sz * zinv);                                   \
      tt=(tz * zinv);                                   \
      s=(int) ss;                              \
      t=(int) tt;                              \
      dsdx= (int)( (dszdx - ss*fdzdx)*zinv );           \
      dtdx= (int)( (dtzdx - tt*fdzdx)*zinv );           \
      CALC_MIPMAP_LEVEL;                                \
    }                                                   \
    while (n>=0) {                                      \
      PUT_PIXEL(0);                                     \
      pz+=1;                                            \
      pp=(PIXEL *)((char *)pp + PSZB);                  \
      n-=1;                                             \
    }                                                   \
  }

#include "ztriangle.h"
}

/*
 * Smooth filled triangle, with perspective-correct mapping.
 */

static void FNAME(smooth_perspective) (ZBuffer *zb,
                                       ZBufferPoint *p0,ZBufferPoint *p1,ZBufferPoint *p2)
{
  ZTextureLevel *texture_levels;
  float fdzdx,fndzdx,ndszdx,ndtzdx;

#define INTERP_Z
#define INTERP_STZ
#define INTERP_RGB

#define EARLY_OUT()                                     \
  {                                                     \
    int c0, c1, c2;                                     \
    c0 = RGBA_TO_PIXEL(p0->r, p0->g, p0->b, p0->a);     \
    c1 = RGBA_TO_PIXEL(p1->r, p1->g, p1->b, p1->a);     \
    c2 = RGBA_TO_PIXEL(p2->r, p2->g, p2->b, p2->a);     \
    if (c0 == c1 && c0 == c2) {                         \
      /* It's really a flat-shaded triangle. */         \
      if (c0 == 0xffffffff) {                           \
        /* Actually, it's a white triangle. */          \
        FNAME(white_perspective)(zb, p0, p1, p2);       \
        return;                                         \
      }                                                 \
      FNAME(flat_perspective)(zb, p0, p1, p2);          \
      return;                                           \
    }                                                   \
  }

#define DRAW_INIT() 				\
  {						\
    texture_levels = zb->current_texture;       \
    fdzdx=(float)dzdx;                          \
    fndzdx=NB_INTERP * fdzdx;                   \
    ndszdx=NB_INTERP * dszdx;                   \
    ndtzdx=NB_INTERP * dtzdx;                   \
  }

#define PUT_PIXEL(_a)                                                   \
  {                                                                     \
    zz=z >> ZB_POINT_Z_FRAC_BITS;                                       \
    if (ZCMP(pz[_a], zz)) {                                             \
      tmp = ZB_LOOKUP_TEXTURE(texture_levels, s, t, mipmap_level, mipmap_dx);      \
      int a = oa1 * PIXEL_A(tmp) >> 16;                                 \
      if (ACMP(zb, a)) {                                                \
        STORE_PIX(pp[_a],                                               \
                  RGBA_TO_PIXEL(or1 * PIXEL_R(tmp) >> 16,               \
                                og1 * PIXEL_G(tmp) >> 16,               \
                                ob1 * PIXEL_B(tmp) >> 16,               \
                                a),                                     \
                  or1 * PIXEL_R(tmp) >> 16,                             \
                  og1 * PIXEL_G(tmp) >> 16,                             \
                  ob1 * PIXEL_B(tmp) >> 16,                             \
                  a);                                                   \
        STORE_Z(pz[_a], zz);                                            \
      }                                                                 \
    }                                                                   \
    z+=dzdx;                                                            \
    og1+=dgdx;                                                          \
    or1+=drdx;                                                          \
    ob1+=dbdx;                                                          \
    oa1+=dadx;                                                          \
    s+=dsdx;                                                            \
    t+=dtdx;                                                            \
  }

#define DRAW_LINE()                                     \
  {                                                     \
    register ZPOINT *pz;                                \
    register PIXEL *pp;                                 \
    register int s,t,z,zz;                     \
    register int n,dsdx,dtdx;                           \
    register int or1,og1,ob1,oa1;              \
    float sz,tz,fz,zinv;                                \
    n=(x2>>16)-x1;                                      \
    fz=(float)z1;                                       \
    zinv=1.0f / fz;                                     \
    pp=(PIXEL *)((char *)pp1 + x1 * PSZB);              \
    pz=pz1+x1;                                          \
    z=z1;						\
    sz=sz1;                                             \
    tz=tz1;                                             \
    or1 = r1;                                           \
    og1 = g1;                                           \
    ob1 = b1;                                           \
    oa1 = a1;                                           \
    while (n>=(NB_INTERP-1)) {                          \
      {                                                 \
        float ss,tt;                                    \
        ss=(sz * zinv);                                 \
        tt=(tz * zinv);                                 \
        s=(int) ss;                            \
        t=(int) tt;                            \
        dsdx= (int)( (dszdx - ss*fdzdx)*zinv );         \
        dtdx= (int)( (dtzdx - tt*fdzdx)*zinv );         \
        CALC_MIPMAP_LEVEL;                              \
        fz+=fndzdx;                                     \
        zinv=1.0f / fz;                                 \
      }                                                 \
      PUT_PIXEL(0);                                     \
      PUT_PIXEL(1);                                     \
      PUT_PIXEL(2);                                     \
      PUT_PIXEL(3);                                     \
      PUT_PIXEL(4);                                     \
      PUT_PIXEL(5);                                     \
      PUT_PIXEL(6);                                     \
      PUT_PIXEL(7);                                     \
      pz+=NB_INTERP;                                    \
      pp=(PIXEL *)((char *)pp + NB_INTERP * PSZB);      \
      n-=NB_INTERP;                                     \
      sz+=ndszdx;                                       \
      tz+=ndtzdx;                                       \
    }                                                   \
    {                                                   \
      float ss,tt;                                      \
      ss=(sz * zinv);                                   \
      tt=(tz * zinv);                                   \
      s=(int) ss;                              \
      t=(int) tt;                              \
      dsdx= (int)( (dszdx - ss*fdzdx)*zinv );           \
      dtdx= (int)( (dtzdx - tt*fdzdx)*zinv );           \
      CALC_MIPMAP_LEVEL;                                \
    }                                                   \
    while (n>=0) {                                      \
      PUT_PIXEL(0);                                     \
      pz+=1;                                            \
      pp=(PIXEL *)((char *)pp + PSZB);                  \
      n-=1;                                             \
    }                                                   \
  }

#include "ztriangle.h"
}

#undef ACMP
#undef ZCMP
#undef STORE_PIX
#undef STORE_Z
#undef FNAME
#undef INTERP_MIPMAP
#undef CALC_MIPMAP_LEVEL
#undef ZB_LOOKUP_TEXTURE
