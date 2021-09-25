#include "tinyRenderer.h"

void partOneHeadGrid()
{
	const int32_t height = 800;
	const int32_t width = 800;

	tga_image image = {0};
	makeTGAImage(&image, width, height, RGB);

	model m = {0};
	makeModel("../objfiles/african_head.obj", &m);

	for (uint32_t i = 0; i < m.numFaces; i++) {
		for (int32_t j = 0; j < 3; j++) {
			vec3f32 v0 = m.vertex[m.faceIndex[i][j]];
			vec3f32 v1 = m.vertex[m.faceIndex[i][(j+1)%3]];

			point2d p0 = {
				.x = ((v0.x+1.0f) * (image.width * 0.5f)),
				.y = ((v0.y+1.0f) * (image.height * 0.5))
			};

			point2d p1 = {
				.x = ((v1.x+1.0f) * (image.width * 0.5f)),
				.y = ((v1.y+1.0f) * (image.height * 0.5f))
			};

			line2d(p0, p1, &image, white);
		}
	}
	flipTGAImageVerticaly(&image);
	writeTGAImage(&image, "../tga/outputp1_2.tga", false);
}

void partTwoFillTriangles()
{
	tga_image image = {0};
	const int32_t height = 200;
	const int32_t width = 200;

	makeTGAImage(&image, width, height, RGB);

	triangle2d tri1 = {
		(struct point2d) {10, 70},
		(struct point2d) {50, 160},
		(struct point2d) {70, 80},
	};

	triangle2d tri2 = {
		(struct point2d) {180, 50},
		(struct point2d) {150, 1},
		(struct point2d) {70, 180},
	};

	triangle2d tri3 = {
		(struct point2d) {180, 150},
		(struct point2d) {120, 160},
		(struct point2d) {130, 180},
	};
	drawTriangle(tri1, &image, red);
	drawTriangle(tri2, &image, white);
	drawTriangle(tri3, &image, green);

	flipTGAImageVerticaly(&image);
	writeTGAImage(&image, "../tga/outputp2_1.tga", false);

}

void partTwoFillHead()
{
	const int32_t height = 800;
	const int32_t width = 800;

	tga_image image = {0};
	makeTGAImage(&image, width, height, RGB);

	model m = {0};
	makeModel("../objfiles/african_head.obj", &m);

	srand(time(NULL));

	for (uint32_t i = 0; i < m.numFaces; i++) {
		point2d screenCoords[3];
		for (int32_t j = 0; j < 3; j++) {
			vec3f32 worldIndex = m.vertex[m.faceIndex[i][j]];

			screenCoords[j] = (struct point2d) {
					.x = ((worldIndex.x+1.0f)*(image.width*0.5f)),
					.y = ((worldIndex.y+1.0f)*(image.height*0.5))};
		}

		triangle2d t = (struct triangle2d) {
				screenCoords[0],
				screenCoords[1],
				screenCoords[2]};

		tga_color color = {{
				.r =rand()%255,
				.g =rand()%255,
				.b =rand()%255,
				.a =rand()%255
		}};

		drawTriangle(t, &image, color);
	}

	flipTGAImageVerticaly(&image);
	writeTGAImage(&image, "../tga/outputp2_2.tga", false);
}

void partTwoShadeHead()
{
	const int32_t height = 800;
	const int32_t width = 800;

	tga_image image = {0};
	makeTGAImage(&image, width, height, RGB);

	model m = {0};
	makeModel("../objfiles/african_head.obj", &m);

#if 1
	v4sf lightDirectionV4sf = (v4sf){0.0f, 0.0f, -1.0f, 0.0f};
#else
	vec3f32 lightDirection = {0.0f, 0.0f, -1.0f};
#endif

	for (uint32_t i = 0; i < m.numFaces; i++) {
		point2d screenCoords[3];
		vec3f32 worldCoordinate[3];
		for (int32_t j = 0; j < 3; j++) {
			vec3f32 v = m.vertex[m.faceIndex[i][j]];
			screenCoords[j] = (struct point2d) {
					.x = ((v.x+1.0f)*(image.width*0.5f)),
					.y = ((v.y+1.0f)*(image.height*0.5f))};
			worldCoordinate[j] = v;
		}
#if 1
		v4sf wc0 = (v4sf){
			worldCoordinate[0].x,
			worldCoordinate[0].y,
			worldCoordinate[0].z,
			0
		};
		v4sf wc1 = (v4sf){
			worldCoordinate[1].x,
			worldCoordinate[1].y,
			worldCoordinate[1].z,
			0
		};
		v4sf wc2 = (v4sf){
			worldCoordinate[2].x,
			worldCoordinate[2].y,
			worldCoordinate[2].z,
			0
		};

		v4sf vA = wc2 - wc0;
		v4sf vB = wc1 - wc0;

		v4sf crossProduct = crossV4si(vA, vB);

		crossProduct = normalizeV4sf(crossProduct);

		float intensity = dotV4sf(crossProduct, lightDirectionV4sf);
#else

		vec3f32 vA = subtractVec3f32(worldCoordinate[2], worldCoordinate[0]);
		vec3f32 vB = subtractVec3f32(worldCoordinate[1], worldCoordinate[0]);

		vec3f32 crossProduct = crossProductF32(vA, vB);

		normalizeF32(&crossProduct);

		float intensity = dotProductF32(crossProduct, lightDirection);
#endif
		triangle2d t = {screenCoords[0], screenCoords[1], screenCoords[2]};

		tga_color color = {{
				.r = intensity * 255,
				.g = intensity * 255,
				.b = intensity * 255,
				.a = 255
		}};

		if (intensity > 0) {
			drawTriangle(t, &image, color);
		}
	}

	flipTGAImageVerticaly(&image);
	writeTGAImage(&image, "../tga/outputp2_3.tga", false);
}

void partThreeZbuffer()
{
	const int32_t height = 800;
	const int32_t width = 800;

	tga_image image = {0};
	makeTGAImage(&image, width, height, RGB);

	float * zBuffer = malloc(image.height * image.width * sizeof(*zBuffer));
	if (zBuffer) {
		for (int32_t i = 0; i < (image.height * image.width); i++) {
			zBuffer[i] = -1 * (FLT_MAX);
		}
	} else {
		fprintf(stderr, "zbuffer alloc failed\n");
		exit(EXIT_FAILURE);
	}

	model m = {0};
	makeModel("../objfiles/african_head.obj", &m);

	srand(time(NULL));

	for (uint32_t i = 0; i < m.numFaces; i++) {
		vec3f32 trianglePoints[3];
		vec3f32 triangleColor[3];
		for (uint32_t j = 0; j < 3; j++) {
			trianglePoints[j] = scaleWorldToScreenCoordinates(image,
						m.vertex[m.faceIndex[i][j]]);
			triangleColor[j] = m.texture[m.textureIndex[i][j]];
		}
		tga_color color = {{
			.r =  triangleColor[0].x * (image.width * image.height),
			.g =  triangleColor[1].x * (image.width * image.height),
			.b =  triangleColor[2].x * (image.width * image.height),
			.a = 255
		}};

		triangle3d tri = {
			.A = trianglePoints[0],
			.B = trianglePoints[1],
			.C = trianglePoints[2]
		};
		drawTriangleWithZaxis(tri, zBuffer, &image, color);
	}

	flipTGAImageVerticaly(&image);
	writeTGAImage(&image, "../tga/outputp3_1.tga", false);

}

inline int32_t roundFloatToInt32(float x)
{
	int32_t result = x >= 0 ? (int64_t)(x+0.5f) : (int64_t)(x-0.5f);
	return result;
}

inline v4si v4sfTOv4si(v4sf v)
{
	v4si result = {
		(int32_t)roundFloatToInt32(v[0]),
		(int32_t)roundFloatToInt32(v[1]),
		(int32_t)roundFloatToInt32(v[2]),
		(int32_t)roundFloatToInt32(v[3])
	};
	return result;
}

inline v4sf v4siTOv4sf(v4si v)
{
	v4sf result = {
		(float)v[0],
		(float)v[1],
		(float)v[2],
		(float)v[3]
	};
	return result;
}

void makeViewPort(int32_t x, int32_t y, int32_t height,
		int32_t width, int32_t depth, matrixF32 * m)
{
	m->data[0][3] = x + 0.5f * width;
	m->data[1][3] = y + 0.5f * height;
	m->data[2][3] = 0.5f * depth;

	m->data[0][0] = 0.5 * width;
	m->data[1][1] = 0.5 * height;
	m->data[2][2] = 0.5 * depth;
}

matrixF32 * vectorToMatrix(const vec3f32 v)
{
	matrixF32 * result = newMatrixF32(4,1);
	result->data[0][0] = v.x;
	result->data[1][0] = v.y;
	result->data[2][0] = v.z;
	result->data[3][0] = 1.0f;
	return result;
}

v4si matrixToVectorV4si(const matrixF32 * m)
{
	v4si result = {
		(int32_t)roundFloatToInt32(m->data[0][0] / m->data[3][0]),
		(int32_t)roundFloatToInt32(m->data[1][0] / m->data[3][0]),
		(int32_t)roundFloatToInt32(m->data[2][0] / m->data[3][0]),
		0
	};
	return result;
}

void printf4x4matrix(matrixF32 * m) {
	for (uint32_t i = 0; i < m->rows; i++) {
		for (uint32_t j = 0; j < m->cols; j++) {
			printf("%f ", m->data[i][j]);
		}
		puts("");
	}
}

typedef struct paintable_triangle
{
	v4si triangle[3];
	v4si texture[3];
	float intensity;
} paintable_triangle;


paintable_triangle makePaintableTriangle(const vec3f32 v0, const vec3f32 v1, const vec3f32 v2,
		const matrixF32 * restrict viewPort,
		const matrixF32 * restrict projection,
		const v4sf lightDirection)
{
	paintable_triangle result = {{{0},{0},{0}},{{0},{0},{0}},0};
	//sc = Screen coordinate
	v4si sc1 = matrixToVectorV4si(matrixProduct(viewPort, matrixProduct(projection, vectorToMatrix(v0))));
	v4si sc2 = matrixToVectorV4si(matrixProduct(viewPort, matrixProduct(projection, vectorToMatrix(v1))));
	v4si sc3 = matrixToVectorV4si(matrixProduct(viewPort, matrixProduct(projection, vectorToMatrix(v2))));

	result.triangle[0] = sc1;
	result.triangle[1] = sc2;
	result.triangle[2] = sc3;

	//wc = World coordinate
	v4sf wc0 = {v0.x, v0.y, v0.z, 0};
	v4sf wc1 = {v1.x, v1.y, v1.z, 0};
	v4sf wc2 = {v2.x, v2.y, v2.z, 0};

	//get vectors parallel to plane
	v4sf vA = wc2 - wc0;
	v4sf vB = wc1 - wc0;

	//calc normal vector orthogonal to plane
	v4sf normalVector = crossV4si(vA, vB);

	//convert to unit length
	normalVector = normalizeV4sf(normalVector);

	//Light vectors projection onto plane:
	result.intensity = dotV4sf(normalVector, lightDirection);

	return result;
}


void partFourPerspectiveAttemptOne()
{
	const int32_t height = 800;
	const int32_t width = 800;
	const int32_t depth = 255;
	const v4sf lightDirectionV4sf = (v4sf){0.0f, 0.0f, -1.0f, 0.0f};
	const vec3f32 camera = {0, 0, 3};

	matrixF32 * projection = identityMatrixF32(4);
	matrixF32 * viewPort = identityMatrixF32(4);

	projection->data[3][2] = -1.0f / camera.z;
	makeViewPort(width/8, height/8, width*0.75, height*0.75, depth, viewPort);

	tga_image image = {0};
	makeTGAImage(&image, width, height, RGB);

	model m = {0};
	makeModel("../objfiles/african_head.obj", &m);

	float  * zBuffer = malloc((image.height * image.width) * sizeof(*zBuffer));
	if (!zBuffer) {
		fprintf(stderr, "out of mem\n");
		exit(EXIT_FAILURE);
	}

	memset(zBuffer, FLT_MIN, (image.width*image.height) * sizeof(int32_t));

	paintable_triangle * triangles = malloc(m.numFaces * sizeof(paintable_triangle));
	if (!triangles) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

#pragma omp parallel for
	for (uint32_t i = 0; i < m.numFaces; i++) {
		triangles[i] = makePaintableTriangle(
				m.vertex[m.faceIndex[i][0]],
				m.vertex[m.faceIndex[i][1]],
				m.vertex[m.faceIndex[i][2]],
				viewPort, projection, lightDirectionV4sf);
	}

#pragma omp parallel for
	for (uint32_t i = 0; i < m.numFaces; i++) {
		triangles[i].texture[0] = getTextureCoordinatesV4si(i, 0, m);
		triangles[i].texture[1] = getTextureCoordinatesV4si(i, 1, m);
		triangles[i].texture[2] = getTextureCoordinatesV4si(i, 2, m);
	}

	//TODO: Hva er sannsyneligheten for race paa pixler?
#pragma omp parallel for
	for (uint32_t i = 0; i < m.numFaces; i++) {
		if (triangles[i].intensity > 0) {
			drawTriangleTexture(triangles[i].triangle,
					triangles[i].texture,
					triangles[i].intensity,
					zBuffer, &image, &m);
		}
	}

	flipTGAImageVerticaly(&image);
	writeTGAImage(&image, "../tga/outputp4_0.tga", false);
#if 1
	dumpZbufferToTGA(width, height, zBuffer, "../tga/outputp4_0zBuffer.tga");
#endif
}

int main(void)
{
	clock_t tic  = clock();
	//partTwoFillTriangles();
	//partOneHeadGrid();
	//partTwoFillHead();
	//partTwoShadeHead();
	//partThreeZbuffer();
	partFourPerspectiveAttemptOne();
	printf("Cycles: %lu\n",(tic - clock()));
	return EXIT_SUCCESS;
}

void dumpZbufferToTGA(uint32_t width, uint32_t height, float * zBuffer, const char * filename)
{
	tga_image result = {0};
	makeTGAImage(&result, width, height, GRAYSCALE);
	for (int32_t i = 0; i < result.height; i++) {
		for (int32_t j = 0; j < result.width; j++) {
			setTGAImagePixel(i, j, &result,
					(tga_color){
					.value = zBuffer[i + j * width]+0.5
					});
		}
	}
	flipTGAImageVerticaly(&result);
	writeTGAImage(&result, filename, false);
}

vec3f32 barycentricCoordinate(triangle3d t, vec3f32 p)
{
	vec3f32 p1 = {
		.x = t.C.y - t.A.y,
		.y = t.B.y - t.A.y,
		.z = t.A.y - p.y
	};

	vec3f32 p2 = {
		.x = t.C.x - t.A.x,
		.y = t.B.x - t.A.x,
		.z = t.A.x - p.x
	};

	vec3f32 u = crossProductF32(p1, p2);

	return (Abs(u.z) > 1e-2)
		? (struct vec3f32){(1.0f-(u.x+u.y)/u.z), (u.y/u.z), (u.x/u.z)}
		: (struct vec3f32){-1.0f, 1.0f, 1.0f};
}

vec3f32 scaleWorldToScreenCoordinates(tga_image image, vec3f32 world)
{
	vec3f32 result = {
		.x = (int32_t)((world.x + 1.0f) * (0.5f * image.width) + 0.5f),
		.y = (int32_t)((world.y + 1.0f) * (0.5f * image.height) + 0.5f),
		.z = world.z
	};
	return result;
}

void drawTriangleTexture(v4si * triangle, v4si * texture, float intensity,
		float * zBuffer, tga_image *image, model * m)
{
	if (triangle[VERT_A][Y_COOR] == triangle[VERT_B][Y_COOR] &&
			triangle[VERT_A][Y_COOR] == triangle[VERT_C][Y_COOR]) {
		return;
	}

	if (triangle[VERT_A][Y_COOR] > triangle[VERT_B][Y_COOR]) {
		swapV4si(&triangle[VERT_A], &triangle[VERT_B]);
		swapV4si(&texture[VERT_A], &texture[VERT_B]);
	}

	if (triangle[VERT_A][Y_COOR] > triangle[VERT_C][Y_COOR]) {
		swapV4si(&triangle[VERT_A], &triangle[VERT_C]);
		swapV4si(&texture[VERT_A], &texture[VERT_C]);
	}

	if (triangle[VERT_B][Y_COOR] > triangle[VERT_C][Y_COOR]) {
		swapV4si(&triangle[VERT_B], &triangle[VERT_C]);
		swapV4si(&texture[VERT_B], &texture[VERT_C]);
	}

	drawAndFillPartOfTriangleTexture(triangle[VERT_A], triangle[VERT_B],
			texture[VERT_A], texture[VERT_B],
			triangle, texture, intensity, zBuffer, image, m);

	drawAndFillPartOfTriangleTexture(triangle[VERT_B], triangle[VERT_C],
			texture[VERT_B], texture[VERT_C],
			triangle, texture, intensity, zBuffer, image, m);

}



void drawAndFillPartOfTriangleTexture(v4si X, v4si Y, v4si uvX, v4si uvY,
		v4si * tri, v4si * texture, float intensity,
		float * zBuffer, tga_image * image, model * m)
{
	int32_t totalHeight = tri[VERT_C][Y_COOR] - tri[VERT_A][Y_COOR];

	for (int32_t y = X[Y_COOR]; y <= Y[Y_COOR]; y++) {

		int segmentHeight = Y[Y_COOR] - X[Y_COOR] + 1;

		float alpha = (float)(y - tri[VERT_A][Y_COOR]) / totalHeight;
		float beta = (float)(y - X[Y_COOR]) / segmentHeight;

		v4si newA = tri[VERT_A] + v4sfTOv4si((alpha * v4siTOv4sf((tri[VERT_C]-tri[VERT_A]))));
		v4si newB = X + v4sfTOv4si((beta * v4siTOv4sf((Y - X))));

		v4si uvA = texture[X_COOR]
			+ v4sfTOv4si((alpha * v4siTOv4sf((texture[Z_COOR]-texture[X_COOR]))));

		v4si uvB = uvX + v4sfTOv4si((beta * v4siTOv4sf((uvY-uvX))));

		if (newA[X_COOR] > newB[X_COOR]) {
			swapV4si(&newA, &newB);
			swapV4si(&uvA, &uvB);
		}

		for (int32_t i = newA[X_COOR]; i <= newB[X_COOR]; i++) {

			float phi = newA[X_COOR] == newB[X_COOR]
				? 1.0f
				: (((float)(i-newA[X_COOR]))
				/ ((float)(newB[X_COOR] - newA[X_COOR])));

			v4si p = v4sfTOv4si(v4siTOv4sf(newA) + (phi * v4siTOv4sf(newB-newA)));
			v4si uvP = v4sfTOv4si((v4siTOv4sf(uvA) + (phi * v4siTOv4sf((uvB-uvA)))));

			int index = p[X_COOR] + p[Y_COOR] * image->width;
			if (zBuffer[index] < p[Z_COOR]) {
				zBuffer[index] = p[Z_COOR];

				tga_color color = getTGAImagePixel(m->diffusionMap,
						uvP[X_COOR], uvP[Y_COOR]);

				color = (tga_color){{
						.r = color.r*intensity,
						.g = color.g*intensity,
						.b = color.b*intensity,
						.a = color.a*intensity
						}};

				setTGAImagePixel(p[X_COOR], p[Y_COOR], image, color);
			}
		}
	}
}

void drawTriangleWithZaxis(triangle3d t, float * zBuffer, tga_image * image, tga_color color)
{
	vec2f32 boundryBoxMinPoint = {FLT_MAX, FLT_MAX};
	vec2f32 boundryBoxMaxPoint = {(-1 * (FLT_MAX)), (-1 * (FLT_MAX))};
	vec2f32 imageClamp = {(image->width-1), (image->height-1)};

	vec2f32 tmp = boundryBoxMinPoint;
	boundryBoxMinPoint.x = Max(0.0f, Min(Min(tmp.x, t.A.x),
				Min(Min(tmp.x, t.B.x), Min(tmp.x, t.C.x))));

	boundryBoxMinPoint.y = Max(0.0f, Min(Min(tmp.y, t.A.y),
				Min(Min(tmp.y, t.B.y), Min(tmp.y, t.C.y))));

	tmp = boundryBoxMaxPoint;

	boundryBoxMaxPoint.x = Min(imageClamp.x, Max(Max(tmp.x, t.A.x),
				Max(Max(tmp.x, t.B.x), Max(tmp.x, t.C.x))));

	boundryBoxMaxPoint.y = Min(imageClamp.y, Max(Max(tmp.y, t.A.y),
				Max(Max(tmp.y, t.B.y), Max(tmp.y, t.C.y))));

	vec3f32 p = {0, 0, 0};

	for (p.x = boundryBoxMinPoint.x;
			p.x <= boundryBoxMaxPoint.x;
			p.x++)
	{
		for (p.y = boundryBoxMinPoint.y;
				p.y <= boundryBoxMaxPoint.y;
				p.y++)
		{
			vec3f32 bc_screen = barycentricCoordinate(t, p);
			if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) {
				continue;
			}
			p.z = 0;
			p.z += ((t.A.z * bc_screen.x)
					+ (t.B.z * bc_screen.y)
					+ (t.C.z * bc_screen.z));

			int32_t currentCoord = ((int32_t)(p.x+p.y*image->width));
			if (zBuffer[currentCoord] < p.z) {
				zBuffer[currentCoord] = p.z;
				setTGAImagePixel(p.x, p.y, image, color);
			}
		}
	}

}

void drawTriangle(triangle2d t, tga_image * image, tga_color color)
{
	if (t.A.y == t.B.y && t.A.y == t.C.y) {
		return;
	}

	if (t.A.y > t.B.y) {
		swapPoint2d(&t.A, &t.B);
	}

	if (t.A.y > t.C.y) {
		swapPoint2d(&t.A, &t.C);
	}

	if (t.B.y > t.C.y) {
		swapPoint2d(&t.B, &t.C);
	}
	drawAndFillPartOfTriangle(t.A, t.B, t, image, color);
	drawAndFillPartOfTriangle(t.B, t.C, t, image, color);
}

void drawAndFillPartOfTriangle(point2d X, point2d Y, triangle2d t, tga_image * image, tga_color color)
{
	// Tegn opp til punktet hvor den horisontalen mellom (b,c) og linjen
	// A-C krysser (Lager en ny triangel)
	int32_t totalHeight = t.C.y - t.A.y;

	for (int32_t y = X.y; y <= Y.y; y++) {

		int segmentHeight = Y.y - X.y + 1;
		float alpha = (float)(y - t.A.y) / (float)totalHeight;
		float beta = (float)(y - X.y) / (float)segmentHeight;

		point2d newA = {
			.x = (t.A.x + ((t.C.x - t.A.x) * alpha)),
			.y = (t.A.y + ((t.C.y - t.A.y) * alpha))
		};

		point2d newB = {
			.x = (X.x + ((Y.x - X.x) * beta)),
			.y = (X.y + ((Y.y - X.y) * beta))
		};

		if (newA.x > newB.x) {
			swapPoint2d(&newA, &newB);
		}

		for (int32_t i = newA.x; i <= newB.x; i++) {
			setTGAImagePixel(i, y, image, color);
		}
	}
}

void line2d(point2d start, point2d end, tga_image * image, tga_color color)
{
	bool32 steep = false;

	// Hvis hoyden til linjestykket er lengre enn bredden
	// transponeres linjen (speiles) om 45 gradern.
	if (Abs(start.x - end.x) < Abs(start.y - end.y)) {
		swapCoordinates2d(&start);
		swapCoordinates2d(&end);
		steep = true;
	}

	// hvis ikke i foerste kvadrant:
	if (start.x > end.x) {
		swapPoint2d(&start, &end);
	}

#if 0
	int32_t dx = end.x - start.x;
	int32_t dy = end.y - start.y;

	int32_t derror = Abs(dy) * 2;
	int32_t error = 0;

	int32_t y = start.y;
	int32_t unitStep = (end.y > start.y) ? 1 : -1;
	if (steep) {
		for (int32_t x = start.x; x <= end.x; x++) {
			setTGAImagePixel(y, x, image, color);
			error += derror;
			if (error > dx) {
				y += unitStep;
				error -= dx * 2;
			}
		}
	} else {
		for (int32_t x = start.x; x <= end.x; x++) {
			setTGAImagePixel(x, y, image, color);
			error += derror;
			if (error > dx) {
				y += unitStep;
				error -= dx * 2;
			}
		}
	}
#else
	if (steep) {
		for (int32_t x = start.x; x <= end.x; x++) {
			float t = (x-start.x) / (float)(end.x - start.x);
			int y = start.y * (1.0f - t) + end.y * t;
			setTGAImagePixel(y, x, image, color);
		}
	} else {
		for (int32_t x = start.x; x <= end.x; x++) {
			float t = (x-start.x) / (float)(end.x - start.x);
			int y = start.y * (1.0f - t) + end.y * t;
			setTGAImagePixel(x, y, image, color);
		}
	}
#endif
}
