#ifndef _GLUGG_SHAPES_
#define _GLUGG_SHAPES_

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "MicroGlut.h"
// uses framework Cocoa
// uses framework OpenGL
#include "GL_utilities.h"
#include "VectorUtils4.h"

//#ifdef __cplusplus
//extern "C" {
//#endif

void gluggCube(GLfloat size);
void gluggDoughnut(GLfloat r, GLfloat R, GLint nsides, GLint rings);
void gluggDodecahedron(float size);
void gluggOctahedron(GLfloat size);
void gluggIcosahedron(GLfloat size);
void gluggTetrahedron(GLfloat size);
void gluggEllipsoid(int aSlices, int hSlices, vec3 dimensions);
void gluggSphere(int aSlices, int hSlices, float radius);
void gluggCylinder(int aSlices, float height, float width);
void gluggCone(int aSlices, float height, float width);
void gluggCylinderAlt(int aSlices, float height, float topwidth, float bottomwidth);


//#ifdef __cplusplus
//}
//#endif

#endif



// --------------- Implementation part ----------------

#ifdef MAIN

#ifndef GLUGG_SHAPES_IMPLEMENTATION
#define GLUGG_SHAPES_IMPLEMENTATION



// GLUGG SHAPES
// This is a collection of basic procedural shapes. Note that they do
// NOT generate a model directly since they are intended for building
// combined shapes. Thus, the calls need to be brackeded by gluggBegin
// and gluggEnd!
// Why? Because you can then create very complex shapes and record them
// into a single model!


// Parts of this code is based on glut_shapes.c in the FreeGlut library.
// Original code by Mark J. Kilgard.
// Conversion to a GLUGG demo by Ingemar Ragnemalm 2021
// and later to a reusable unit.
// Version 2: Even more converted, removing internal (old-style C)
// vector operations in favor of VectorUtils3 and vec3's.
// Also added size for several shapes that didn't have it.
// Slightly edited 220113, took out some dead code.

// Copyright statement from original code:

/* Copyright (c) Mark J. Kilgard, 1994, 1997. */

/**
(c) Copyright 1993, Silicon Graphics, Inc.

ALL RIGHTS RESERVED

Permission to use, copy, modify, and distribute this software
for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that
both the copyright notice and this permission notice appear in
supporting documentation, and that the name of Silicon
Graphics, Inc. not be used in advertising or publicity
pertaining to distribution of the software without specific,
written prior permission.

THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU
"AS-IS" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR
OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  IN NO
EVENT SHALL SILICON GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE
ELSE FOR ANY DIRECT, SPECIAL, INCIDENTAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER,
INCLUDING WITHOUT LIMITATION, LOSS OF PROFIT, LOSS OF USE,
SAVINGS OR REVENUE, OR THE CLAIMS OF THIRD PARTIES, WHETHER OR
NOT SILICON GRAPHICS, INC.  HAS BEEN ADVISED OF THE POSSIBILITY
OF SUCH LOSS, HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
ARISING OUT OF OR IN CONNECTION WITH THE POSSESSION, USE OR
PERFORMANCE OF THIS SOFTWARE.

US Government Users Restricted Rights

Use, duplication, or disclosure by the Government is subject to
restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
(c)(1)(ii) of the Rights in Technical Data and Computer
Software clause at DFARS 252.227-7013 and/or in similar or
successor clauses in the FAR or the DOD or NASA FAR
Supplement.  Unpublished-- rights reserved under the copyright
laws of the United States.  Contractor/manufacturer is Silicon
Graphics, Inc., 2011 N.  Shoreline Blvd., Mountain View, CA
94039-7311.

OpenGL(TM) is a trademark of Silicon Graphics, Inc.
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// uses framework Cocoa
// uses framework OpenGL
#include "VectorUtils4.h"
#include "glugg.h"

void gluggCube(GLfloat size)
{
  static vec3 n[6] =
  {
    vec3(-1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, -1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(0.0, 0.0, -1.0)
  };
  static GLint faces[6][4] =
  {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
  };
  vec3 v[8] =
  {
  	vec3(-size/2,-size/2,-size/2),
  	vec3(-size/2,-size/2, size/2),
  	vec3(-size/2, size/2, size/2),
  	vec3(-size/2, size/2,-size/2),
  	vec3( size/2,-size/2,-size/2),
  	vec3( size/2,-size/2, size/2),
  	vec3( size/2, size/2, size/2),
  	vec3( size/2, size/2,-size/2)
  };
  GLint i;
  
  gluggMode(GLUGG_QUADS);

  for (i = 5; i >= 0; i--)
  {
    gluggNormalv(n[i]);
    gluggVertexv(v[faces[i][0]]);
    gluggVertexv(v[faces[i][1]]);
    gluggVertexv(v[faces[i][2]]);
    gluggVertexv(v[faces[i][3]]);
  }
}


void gluggDoughnut(GLfloat r, GLfloat R, GLint nsides, GLint rings)
{
  int i, j;
  GLfloat theta, phi, theta1;
  GLfloat cosTheta, sinTheta;
  GLfloat cosTheta1, sinTheta1;
  GLfloat ringDelta, sideDelta;

  ringDelta = 2.0 * M_PI / rings;
  sideDelta = 2.0 * M_PI / nsides;

  theta = 0.0;
  cosTheta = 1.0;
  sinTheta = 0.0;
  gluggMode(GLUGG_TRIANGLE_STRIP);
  for (i = rings - 1; i >= 0; i--)
  {
    // Reset for each strip.
  	gluggMode(GLUGG_TRIANGLE_STRIP);
  	
    theta1 = theta + ringDelta;
    cosTheta1 = cos(theta1);
    sinTheta1 = sin(theta1);
    
    phi = 0.0;
    for (j = nsides; j >= 0; j--)
    {
      GLfloat cosPhi, sinPhi, dist;

      phi += sideDelta;
      cosPhi = cos(phi);
      sinPhi = sin(phi);
      dist = R + r * cosPhi;

	vec3 n2 = SetVector(cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi);
	vec3 p2 = SetVector(cosTheta1 * dist, -sinTheta1 * dist, r * sinPhi);
	vec3 n3 = SetVector(cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi);
	vec3 p3 = SetVector(cosTheta * dist, -sinTheta * dist,  r * sinPhi);

	// Works with GLUGG_TRIANGLE_STRIP:
	gluggNormalv(n2);
	gluggVertexv(p2);
	gluggNormalv(n3);
	gluggVertexv(p3);
    }
    theta = theta1;
    cosTheta = cosTheta1;
    sinTheta = sinTheta1;
  }
}


static vec3 dodec[20];

static void
initDodecahedron(void)
{
  GLfloat alpha, beta;

  alpha = sqrt(2.0 / (3.0 + sqrt(5.0)));
  beta = 1.0 + sqrt(6.0 / (3.0 + sqrt(5.0)) -
    2.0 + 2.0 * sqrt(2.0 / (3.0 + sqrt(5.0))));
  /* *INDENT-OFF* */
  dodec[0].x = -alpha; dodec[0].y = 0; dodec[0].z = beta;
  dodec[1].x = alpha; dodec[1].y = 0; dodec[1].z = beta;
  dodec[2].x = -1; dodec[2].y = -1; dodec[2].z = -1;
  dodec[3].x = -1; dodec[3].y = -1; dodec[3].z = 1;
  dodec[4].x = -1; dodec[4].y = 1; dodec[4].z = -1;
  dodec[5].x = -1; dodec[5].y = 1; dodec[5].z = 1;
  dodec[6].x = 1; dodec[6].y = -1; dodec[6].z = -1;
  dodec[7].x = 1; dodec[7].y = -1; dodec[7].z = 1;
  dodec[8].x = 1; dodec[8].y = 1; dodec[8].z = -1;
  dodec[9].x = 1; dodec[9].y = 1; dodec[9].z = 1;
  dodec[10].x = beta; dodec[10].y = alpha; dodec[10].z = 0;
  dodec[11].x = beta; dodec[11].y = -alpha; dodec[11].z = 0;
  dodec[12].x = -beta; dodec[12].y = alpha; dodec[12].z = 0;
  dodec[13].x = -beta; dodec[13].y = -alpha; dodec[13].z = 0;
  dodec[14].x = -alpha; dodec[14].y = 0; dodec[14].z = -beta;
  dodec[15].x = alpha; dodec[15].y = 0; dodec[15].z = -beta;
  dodec[16].x = 0; dodec[16].y = beta; dodec[16].z = alpha;
  dodec[17].x = 0; dodec[17].y = beta; dodec[17].z = -alpha;
  dodec[18].x = 0; dodec[18].y = -beta; dodec[18].z = alpha;
  dodec[19].x = 0; dodec[19].y = -beta; dodec[19].z = -alpha;
  /* *INDENT-ON* */
}

static void
pentagon(int a, int b, int c, int d, int e, float size)
{
  vec3 n0, d1, d2;

  d1 = VectorSub(dodec[a], dodec[b]);
  d2 = VectorSub(dodec[b], dodec[c]);
  n0 = CrossProduct(d1, d2);
  n0 = normalize(n0);

  gluggNormalv(n0);
  gluggVertexv(ScalarMult(dodec[a], size));
  gluggVertexv(ScalarMult(dodec[b], size));
  gluggVertexv(ScalarMult(dodec[c], size));
  
  gluggVertexv(ScalarMult(dodec[a], size));
  gluggVertexv(ScalarMult(dodec[c], size));
  gluggVertexv(ScalarMult(dodec[d], size));

  gluggVertexv(ScalarMult(dodec[a], size));
  gluggVertexv(ScalarMult(dodec[d], size));
  gluggVertexv(ScalarMult(dodec[e], size));
}

void gluggDodecahedron(float size)
{
  static int inited = 0;

  if (inited == 0)
  {
    inited = 1;
    initDodecahedron();
  }
  gluggMode(GLUGG_TRIANGLES);
  pentagon(0, 1, 9, 16, 5, size);
  pentagon(1, 0, 3, 18, 7, size);
  pentagon(1, 7, 11, 10, 9, size);
  pentagon(11, 7, 18, 19, 6, size);
  pentagon(8, 17, 16, 9, 10, size);
  pentagon(2, 14, 15, 6, 19, size);
  pentagon(2, 13, 12, 4, 14, size);
  pentagon(2, 19, 18, 3, 13, size);
  pentagon(3, 0, 5, 12, 13, size);
  pentagon(6, 15, 8, 10, 11, size);
  pentagon(4, 17, 8, 15, 14, size);
  pentagon(4, 12, 5, 16, 17, size);
}

static void
drawtriangle(int i, vec3 data[], int ndx[][3], GLfloat size)
{
  vec3 x0, x1, x2;
  vec3 q0, q1;

  x0 = ScalarMult(data[ndx[i][0]], size);
  x1 = ScalarMult(data[ndx[i][1]], size);
  x2 = ScalarMult(data[ndx[i][2]], size);

  q0 = VectorSub(x0, x1);
  q1 = VectorSub(x1, x2);
  q1 = CrossProduct(q0, q1);
  q1 = normalize(q1);

  gluggNormalv(q1);
  gluggVertexv(x0);
  gluggVertexv(x1);
  gluggVertexv(x2);
}

/* octahedron data: The octahedron produced is centered at the
   origin and has radius 1.0 */
static vec3 odata[6] =
{
  vec3(1.0, 0.0, 0.0),
  vec3(-1.0, 0.0, 0.0),
  vec3(0.0, 1.0, 0.0),
  vec3(0.0, -1.0, 0.0),
  vec3(0.0, 0.0, 1.0),
  vec3(0.0, 0.0, -1.0)
};

static int ondex[8][3] =
{
  {0, 4, 2},
  {1, 2, 4},
  {0, 3, 4},
  {1, 4, 3},
  {0, 2, 5},
  {1, 5, 2},
  {0, 5, 3},
  {1, 3, 5}
};

void gluggOctahedron(GLfloat size)
{
  int i;

  gluggMode(GLUGG_TRIANGLES);
  for (i = 7; i >= 0; i--)
    drawtriangle(i, odata, ondex, size);
}



#define X .525731112119133606
#define Z .850650808352039932

static vec3 idata[12] =
{
  vec3(-X, 0, Z),
  vec3(X, 0, Z),
  vec3(-X, 0, -Z),
  vec3(X, 0, -Z),
  vec3(0, Z, X),
  vec3(0, Z, -X),
  vec3(0, -Z, X),
  vec3(0, -Z, -X),
  vec3(Z, X, 0),
  vec3(-Z, X, 0),
  vec3(Z, -X, 0),
  vec3(-Z, -X, 0)
};

static int iIndex[20][3] =
{
  {0, 4, 1},
  {0, 9, 4},
  {9, 5, 4},
  {4, 5, 8},
  {4, 8, 1},
  {8, 10, 1},
  {8, 3, 10},
  {5, 3, 8},
  {5, 2, 3},
  {2, 7, 3},
  {7, 10, 3},
  {7, 6, 10},
  {7, 11, 6},
  {11, 0, 6},
  {0, 1, 6},
  {6, 1, 10},
  {9, 0, 11},
  {9, 11, 2},
  {9, 2, 5},
  {7, 2, 11},
};

void gluggIcosahedron(GLfloat size)
{
  int i;

  gluggMode(GLUGG_TRIANGLES);
  for (i = 19; i >= 0; i--)
    drawtriangle(i, idata, iIndex, size);
}

/* tetrahedron data: */

#define Ts       1.73205080756887729

static GLfloat tdata[4][3] =
{
  {Ts, Ts, Ts},
  {Ts, -Ts, -Ts},
  {-Ts, Ts, -Ts},
  {-Ts, -Ts, Ts}
};

static int tndex[4][3] =
{
  {0, 1, 3},
  {2, 1, 0},
  {3, 2, 0},
  {1, 2, 3}
};

void gluggTetrahedron(GLfloat size)
{
  int i;

  gluggMode(GLUGG_TRIANGLES);
  for (i = 3; i >= 0; i--)
    drawtriangle(i, (vec3 *)tdata, tndex, size);
}



// ----------- end of code based on glut_shapes.c ---------------
// The following code is my original work.

// Make ellipsoid, using the shape of a sphere and resize.
// Note that I use 1/width, 1/height etc for normals!
void gluggEllipsoid(int aSlices, int hSlices, vec3 dimensions)
{
	float height = dimensions.y;
	float width = dimensions.x;
	float depth = dimensions.z;
	
	vec3 top = SetVector(0,height,0);
	vec3 bottom = SetVector(0,-height,0);
	vec3 bn = SetVector(0,-1,0); // Bottom normal
	vec3 tn = SetVector(0,1,0); // Top normal

// Create bands
	for (float b = -M_PI/2 + M_PI / hSlices; b < M_PI/2 - M_PI / hSlices + 0.0001; b += M_PI / hSlices)
	{
		gluggMode(GLUGG_TRIANGLE_STRIP);
		for (float a = 0.0; a < 2.0*M_PI+0.0001; a += 2.0*M_PI / aSlices)
		{
			float y1 = sin(b - M_PI / hSlices);
			float y2 = sin(b);
			//float a0 = a - 2.0*M_PI / aSlices;
			float a1 = a;
			
			float width1 = sqrt(1 - y1 * y1);
			float width2 = sqrt(1 - y2 * y2);
			
			vec3 p1 = SetVector(width1 * cos(a1), y1, width1 * sin(a1));
			vec3 p2 = SetVector(width2 * cos(a1), y2, width2 * sin(a1));
			
			p1 = SetVector(p1.x*width, p1.y*height, p1.z*depth);
			p2 = SetVector(p2.x*width, p2.y*height, p2.z*depth);

			vec3 p1n = normalize(SetVector(p1.x/width, p1.y/height, p1.z/depth));
			vec3 p2n = normalize(SetVector(p2.x/width, p2.y/height, p2.z/depth));

	// Done making points and normals. Now create polygons!
			gluggNormalv(p2n);
		    gluggVertexv(p2);
			gluggNormalv(p1n);
		    gluggVertexv(p1);
		}
	}

//	// Walk around top
	gluggMode(GLUGG_TRIANGLE_FAN); // Reset to new fan
	float y = sin(M_PI/2 - M_PI / hSlices);
	float width1 = sqrt(1 - y * y);
	gluggNormalv(tn);
	gluggVertexv(top);
	for (float a = 0.0; a < 2.0*M_PI+0.0001; a += 2.0*M_PI / aSlices)
	{
		vec3 p = SetVector(width1 * cos(a), y, width1 * sin(a));

		p = SetVector(p.x*width, p.y*height, p.z*depth);
		vec3 pn = normalize(SetVector(p.x/width, p.y/height, p.z/depth));
		
		gluggNormalv(pn);
	    gluggVertexv(p);
	}
//	// Walk around bottom
	gluggMode(GLUGG_TRIANGLE_FAN); // Reset to new fan
	y = sin(-M_PI/2 + M_PI / hSlices);
	gluggNormalv(bn);
	gluggVertexv(bottom);
	for (float a = 2.0*M_PI; a >= -0.001; a -= 2.0*M_PI / aSlices)
	{
		vec3 p = SetVector(width1 * cos(a), y, width1 * sin(a));

		p = SetVector(p.x*width, p.y*height, p.z*depth);
		vec3 pn = normalize(SetVector(p.x/width, p.y/height, p.z/depth));
		
		gluggNormalv(pn);
	    gluggVertexv(p);
	}
}

// Make a sphere using the ellipsoid
void gluggSphere(int aSlices, int hSlices, float radius)
{
	gluggEllipsoid(aSlices, hSlices, SetVector(radius, radius, radius));
}

void gluggCylinder(int aSlices, float height, float width)
{
	gluggMode(GLUGG_TRIANGLE_STRIP);

	vec3 top = SetVector(0,height,0);
	vec3 center = SetVector(0,0,0);
	vec3 bn = SetVector(0,-1,0); // Bottom normal
	vec3 tn = SetVector(0,1,0); // Top normal
	
	for (float a = 0.0; a < 2.0*M_PI+0.0001; a += 2.0*M_PI / aSlices)
	{
		float a1 = a;
		
		vec3 p1 = SetVector(width * cos(a1), height, width * sin(a1));
		vec3 p2 = SetVector(width * cos(a1), 0, width * sin(a1));
		vec3 pn = SetVector(cos(a1), 0, sin(a1));

// Done making points and normals. Now create polygons!
		gluggNormalv(pn);
	    gluggVertexv(p2);
	    gluggVertexv(p1);
	}
	
	// Then walk around the top and bottom with fans
	gluggMode(GLUGG_TRIANGLE_FAN);
	gluggNormalv(bn);
	gluggVertexv(center);
	// Walk around edge
	for (float a = 0.0; a <= 2.0*M_PI+0.001; a += 2.0*M_PI / aSlices)
	{
		vec3 p = SetVector(width * cos(a), 0, width * sin(a));
	    gluggVertexv(p);
	}
	// Walk around edge
	gluggMode(GLUGG_TRIANGLE_FAN); // Reset to new fan
	gluggNormalv(tn);
	gluggVertexv(top);
	for (float a = 2.0*M_PI; a >= -0.001; a -= 2.0*M_PI / aSlices)
	{
		vec3 p = SetVector(width * cos(a), height, width * sin(a));
	    gluggVertexv(p);
	}
}

void gluggCone(int aSlices, float height, float width)
{
	gluggMode(GLUGG_TRIANGLES);
	// GLUGG_TRIANGLE_FAN not suitable for sides since the top vertex needs different normals for different polygons.

	vec3 top = SetVector(0,height,0);
	vec3 center = SetVector(0,0,0);
	vec3 bn = SetVector(0,-1,0); // Bottom normal
	
	for (float a = 0.0; a < 2.0*M_PI; a += 2.0*M_PI / aSlices)
	{
		// Make two triangles, one to center, one to top (0,height,0).
		
		float a0 = a - 2.0*M_PI / aSlices;
		float a1 = a;
		
		vec3 p = SetVector(width * cos(a1), 0, width * sin(a1));
		vec3 q = SetVector(width * cos(a0), 0, width * sin(a0));

		vec3 n = normalize(SetVector(width, height, 0));
		vec3 pn = SetVector(n.x * cos(a1), n.y, n.x * sin(a1));
		vec3 qn = SetVector(n.x * cos(a0), n.y, n.x * sin(a0));

// Done making points and normals. Now create polygons!
		gluggNormalv(pn);
	    gluggVertexv(p);
		gluggNormalv(normalize(VectorAdd(pn, qn)));
	    gluggVertexv(top);
	    gluggVertexv(q);
	    
		gluggNormalv(bn);
	    gluggVertexv(p);
	    gluggVertexv(q);
	    gluggVertexv(center);
	}
	
	// Then walk around the bottom with a fan
	gluggMode(GLUGG_TRIANGLE_FAN);	
	gluggNormalv(bn);
	gluggVertexv(center);
	// Walk around edge
	for (float a = 0.0; a < 2.0*M_PI; a += 2.0*M_PI / aSlices)
	{
		vec3 p = SetVector(width * cos(a), 0, width * sin(a));
	    gluggVertexv(p);
	}
}


// Cylinder with configurable top and bottom width.
// Created for tree generation in TNM084.
// This code really replaces both the cylinder and the cone!
void gluggCylinderAlt(int aSlices, float height, float topwidth, float bottomwidth)
{
	gluggMode(GLUGG_TRIANGLE_STRIP);
	vec3 top = SetVector(0,height,0);
	vec3 center = SetVector(0,0,0);
	vec3 bn = SetVector(0,-1,0); // Bottom normal
	vec3 tn = SetVector(0,1,0); // Top normal

	for (float a = 0.0; a < 2.0*M_PI+0.0001; a += 2.0*M_PI / aSlices)
	{
		float a1 = a;

		vec3 p1 = SetVector(topwidth * cos(a1), height, topwidth * sin(a1));
		vec3 p2 = SetVector(bottomwidth * cos(a1), 0, bottomwidth * sin(a1));
		vec3 pn = SetVector(cos(a1), 0, sin(a1));

// Done making points and normals. Now create polygons!
		gluggNormalv(pn);
	    gluggTexCoord(height, a1/M_PI);
	    gluggVertexv(p2);
	    gluggTexCoord(0, a1/M_PI);
	    gluggVertexv(p1);
	}

	// Then walk around the top and bottom with fans
	gluggMode(GLUGG_TRIANGLE_FAN);
	gluggNormalv(bn);
	gluggVertexv(center);
	// Walk around edge
	for (float a = 0.0; a <= 2.0*M_PI+0.001; a += 2.0*M_PI / aSlices)
	{
		vec3 p = SetVector(bottomwidth * cos(a), 0, bottomwidth * sin(a));
	    gluggVertexv(p);
	}
	// Walk around edge
	gluggMode(GLUGG_TRIANGLE_FAN); // Reset to new fan
	gluggNormalv(tn);
	gluggVertexv(top);
	for (float a = 2.0*M_PI; a >= -0.001; a -= 2.0*M_PI / aSlices)
	{
		vec3 p = SetVector(topwidth * cos(a), height, topwidth * sin(a));
	    gluggVertexv(p);
	}
}

#endif
#endif
