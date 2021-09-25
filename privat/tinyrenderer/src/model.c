/*
 * ============================================================================
 *
 *       Filename:  model.c
 *
 *    Description:  Rewrite https://github.com/ssloy/tinyrenderer to c99.
 *                  For fun and education. 
 *
 *        Version:  1.0
 *        Created:  05/20/2016 12:53:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  https://github.com/ssloy/tinyrenderer  
 *                  (This hobby rewrite: Christopher Storheim) 
 *   Organization:  N/A
 *
 * ============================================================================
 */
#include "model.h"

void makeModel(const char * filename, model * m)
{

	char line[LINE_MAX];
	FILE * fp = fopen(filename, "r");
	if (fp) {
		uint32_t i = 0;
		uint32_t face = 0;
		uint32_t nTexture = 0;
		uint32_t textIndex = 0;

		m->numFaces = numFaces(fp);
		m->numVerts = numVertecies(fp);
		m->numTextures = numTextures(fp);

		m->faceIndex = calloc(m->numFaces, sizeof(*m->faceIndex));
		m->textureIndex = calloc(m->numFaces, sizeof(*m->faceIndex));
		m->vertex = calloc(m->numVerts, sizeof(*m->vertex));
		m->texture = calloc(m->numTextures, sizeof(*m->texture));

		if (m->faceIndex == 0 || m->vertex == 0|| m->texture == 0) {
			fprintf(stderr, "Out of memory\n");
			fclose(fp);
			exit(EXIT_FAILURE);
		}

		while (fgets(line, LINE_MAX, fp) != NULL) {
			if (line[0] == 'f' && line[1] == ' ') {
				m->faceIndex[face] = processFaceAllocs(line);
				face++;
				m->textureIndex[textIndex] = processTextureAllocs(line);
				textIndex++;
			} else if (line[0] == 'v' && line[1] == ' ') {
				m->vertex[i] = fillVec3f32FromLine(line);
				i++;
			} else if (line[0] == 'v' && line[1] == 't') {
				m->texture[nTexture] = fillVec3f32FromLine(line);
				nTexture++;
			}
		}

		m->diffusionMap = calloc(1, sizeof(tga_image));
		if (m->diffusionMap) {
			char diffuseName[FILENAME_MAX];
			int32_t i = 0;
			char * postfix = "_diffuse.tga";
			const int32_t fileExtensionLength = 4;
			const char * tmp = filename;
			int32_t strLength = 0;

			while (*tmp++) {
				strLength++;
			}

			for (; i < strLength-fileExtensionLength; i++) {
				diffuseName[i] = filename[i];
			}
			while (*postfix) {
				diffuseName[i++] = *postfix++;
			}

			diffuseName[i] = 0;
			readTGAImage(m->diffusionMap, diffuseName);
		} else {
			fprintf(stderr, "Out of memory\n");
			fclose(fp);
			exit(EXIT_FAILURE);
		}
	} else {
		fprintf(stderr, "unable to read model %s\n", filename);
		exit(EXIT_FAILURE);
	}

}

uint32_t numVertecies(FILE * fp)
{
	uint32_t result = 0;
	char line[LINE_MAX];
	while (fgets(line, LINE_MAX, fp)) {
		if (line[0] == 'v' && line[1] == ' ') {
			result++;
		}
	}
	rewind(fp);
	return result;
}

uint32_t numFaces(FILE * fp)
{
	uint32_t result = 0;
	char line[LINE_MAX];
	while (fgets(line, LINE_MAX, fp)) {
		if (line[0] == 'f' && line[1] == ' ') {
			result++;
		}
	}
	rewind(fp);
	return result;
}

uint32_t numTextures(FILE * fp)
{
	uint32_t result = 0;
	char line[LINE_MAX];
	while (fgets(line, LINE_MAX, fp)) {
		if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
			result++;
		}
	}
	rewind(fp);
	return result;
}

int32_t * processTextureAllocs(const char * line)
{
	//format 266/*248*/266 261/*241*/261 265/*249*/265
	char buf[LINE_MAX];
	int32_t * result = calloc(3, sizeof(*result));
	if (!result) {
		fprintf(stderr, "Alloc Failed\n");
		exit(EXIT_FAILURE);
	}
	int32_t wasDigit = 0;
	int32_t slashCount = 0;

	for (int32_t i = 0, ch = 0, r = 0, j = 0; (ch = line[i]) != 0; i++) {
		if (isdigit(ch) && slashCount == 1) {
			buf[j++] = ch;
			wasDigit = 1;
		} else if (ch == '/' && slashCount == 0) {
			slashCount += 1;
		} else if (isspace(ch)) {
			wasDigit = 0;
		} else if (ch == '/' && wasDigit && slashCount == 1) {
			buf[j] = 0;
			result[r++] = (atoi(buf)-1);
			wasDigit = 0;
			slashCount = 0;
			j=0;
		}
	}
	return result;
}

int32_t * processFaceAllocs(const char * line)
{
	//format 266/248/266 261/241/261 265/249/265
	char buf[LINE_MAX];
	int32_t * result = calloc(3, sizeof(*result));
	if (!result) {
		fprintf(stderr, "Alloc Failed\n");
		exit(EXIT_FAILURE);
	}
	int32_t wasSlash = 0;
	int32_t wasDigit = 0;

	for (int32_t i = 0, ch = 0, r = 0, j = 0; (ch = line[i]) != 0; i++) {
		if (isdigit(ch) && !wasSlash) {
			buf[j++] = ch;
			wasSlash = 0;
			wasDigit = 1;
		} else if (isspace(ch)) {
			wasSlash = 0;
			wasDigit = 0;
			j=0;
		} else if (ch == '/' && wasDigit) {
			buf[j] = 0;
			result[r++] = (atoi(buf)-1);
			wasSlash = 1;
			wasDigit = 0;
			j=0;
		}
	}
	return result;
}

vec3f32 fillVec3f32FromLine(const char * line)
{
	char buf[LINE_MAX];
	float result[3];
	int32_t wasDigit = 0;
	for (int32_t i = 0, j = 0, ch = 0, p = 0; (ch = line[i]) != 0; i++) {
		if (isdigit(ch) || ch=='.' || ch=='-' || ch=='e' || ch=='E') {
			buf[j++] = ch;
			wasDigit = 1;
		} else if ((isspace(ch) && wasDigit)) {
			if (p < 3) {
				buf[j] = 0;
				result[p++] = atof(buf);
				wasDigit = 0;
				j = 0;
			} else {
				fprintf(stderr, "Wrong file format! %s", line);
				exit(EXIT_FAILURE);
			}
		} else {
			j = 0;
			wasDigit = 0;
		}
	}
	return (struct vec3f32){.x=result[0], .y=result[1], .z=result[2]};
}

v4si getTextureCoordinatesV4si(int32_t xIndex, int32_t yIndex, const model m)
{
	int32_t height = m.diffusionMap->height;
	int32_t width = m.diffusionMap->width;

	float xCoorReal = ((float)width) * m.texture[m.textureIndex[xIndex][yIndex]].x;
	float yCoorReal = ((float)height)* m.texture[m.textureIndex[xIndex][yIndex]].y;

	int32_t xCoordinate = (int32_t)xCoorReal;
	int32_t yCoordinate = (int32_t)yCoorReal;

	v4si result = { xCoordinate, yCoordinate, 0, 0};

	return result;
}
