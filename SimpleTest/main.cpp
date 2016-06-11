/**
* Shapefileを読み込むテスト
* http://shapelib.maptools.org/shp_api.html
*
* @author	Gen Nishida
* @date	6/10/2016
*/

#include <shapefil.h>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

class Point {
public:
	float x;
	float y;
	float z;

public:
	Point() {}
	Point(float x, float y, float z) : x(x), y(y), z(z) {}
};

void printShapeType(int shapeType) {
	switch (shapeType) {
	case SHPT_POINT:
		std::cout << "SHPT_POINT" << std::endl;
		break;
	case SHPT_ARC:
		std::cout << "SHPT_ARC" << std::endl;
		break;
	case SHPT_POLYGON:
		std::cout << "SHPT_POLYGON" << std::endl;
		break;
	case SHPT_MULTIPOINT:
		std::cout << "SHPT_MULTIPOINT" << std::endl;
		break;
	case SHPT_POINTZ:
		std::cout << "SHPT_POINTZ" << std::endl;
		break;
	case SHPT_ARCZ:
		std::cout << "SHPT_ARCZ" << std::endl;
		break;
	case SHPT_POLYGONZ:
		std::cout << "SHPT_POLYGONZ" << std::endl;
		break;
	case SHPT_MULTIPOINTZ:
		std::cout << "SHPT_MULTIPOINTZ" << std::endl;
		break;
	case SHPT_POINTM:
		std::cout << "SHPT_POINTM" << std::endl;
		break;
	case SHPT_ARCM:
		std::cout << "SHPT_ARCM" << std::endl;
		break;
	case SHPT_POLYGONM:
		std::cout << "SHPT_POLYGONM" << std::endl;
		break;
	case SHPT_MULTIPOINTM:
		std::cout << "SHPT_MULTIPOINTM" << std::endl;
		break;
	case SHPT_MULTIPATCH:
		std::cout << "SHPT_MULTIPATCH" << std::endl;
		break;
	default:
		std::cout << "?????" << std::endl;
		break;
	}
}

int main() {
	DBFHandle hDBF = DBFOpen("..\\3D\\Building.dbf", "r");
	//DBFHandle hDBF = DBFOpen("..\\3D\\Parcels.dbf", "r");
	//DBFHandle hDBF = DBFOpen("..\\3D\\zone1.dbf", "r");
	int nFields = DBFGetFieldCount(hDBF);
	int nRecords = DBFGetRecordCount(hDBF);

	std::vector<std::string> fieldNames(nFields);
	std::vector<int> fieldTypes(nFields);
	for (int i = 0; i < nFields; ++i) {
		char fieldName[12];
		int width;
		int decimals;
		DBFFieldType fieldType = DBFGetFieldInfo(hDBF, i, fieldName, &width, &decimals);
		fieldNames[i] = std::string(fieldName);
		fieldTypes[i] = fieldType;
	}

	for (int j = 0; j < nFields; ++j) {
		if (j > 0) std::cout << "\t";
		std::cout << fieldNames[j].c_str();
	}
	std::cout << std::endl;
	for (int i = 0; i < std::min(nRecords, 20); ++i) {
		for (int j = 0; j < nFields; ++j) {
			if (j > 0) std::cout << "\t";

			if (fieldTypes[j] == FTString) {
				const char* data = DBFReadStringAttribute(hDBF, i, j);
				std::cout << data;
			}
			else if (fieldTypes[j] == FTInteger) {
				int data = DBFReadIntegerAttribute(hDBF, i, j);
				std::cout << data;
			}
			else if (fieldTypes[j] == FTDouble) {
				double data = DBFReadDoubleAttribute(hDBF, i, j);
				std::cout << data;
			}
			else if (fieldTypes[j] == FTLogical) {
				double data = DBFReadIntegerAttribute(hDBF, i, j);
				std::cout << data;
			}
			else {
				std::cout << "Invalid";
			}
		}

		std::cout << std::endl;
	}

	SHPHandle hSHP = SHPOpen("..\\3D\\Building", "r");
	//SHPHandle hSHP = SHPOpen("..\\3D\\Parcels", "r");
	//SHPHandle hSHP = SHPOpen("..\\3D\\zone1", "r");

	int nEntities;
	int shapeType;
	double minBound[4];
	double maxBound[4];
	SHPGetInfo(hSHP, &nEntities, &shapeType, minBound, maxBound);

	std::cout << "#entites: " << nEntities << std::endl;
	printShapeType(shapeType);
	std::cout << "Bound: (" << minBound[0] << "," << minBound[1] << ") - (" << maxBound[0] << "," << maxBound[1] << ")" << std::endl;

	std::vector<SHPObject*> shapeObjects;
	for (int i = 0; i < nEntities; ++i) {
		SHPObject* obj = SHPReadObject(hSHP, i);
		shapeObjects.push_back(obj);
	}

	std::vector<std::vector<std::vector<Point>>> points(shapeObjects.size());
	for (int i = 0; i < shapeObjects.size(); ++i) {
		points[i].resize(shapeObjects[i]->nParts);

		for (int j = 0; j < shapeObjects[i]->nParts; ++j) {
			int nVerticesPerPart;
			if (j < shapeObjects[i]->nParts - 1) {
				nVerticesPerPart = shapeObjects[i]->panPartStart[j + 1] - shapeObjects[i]->panPartStart[j];
			}
			else {
				nVerticesPerPart = shapeObjects[i]->nVertices - shapeObjects[i]->panPartStart[j];
			}
			points[i][j].resize(nVerticesPerPart);

			for (int k = 0; k < nVerticesPerPart; ++k) {
				if (shapeType <= 8) {
					points[i][j][k] = Point(shapeObjects[i]->padfX[shapeObjects[i]->panPartStart[j] + k], shapeObjects[i]->padfY[shapeObjects[i]->panPartStart[j] + k], 0);
				}
				else {
					points[i][j][k] = Point(shapeObjects[i]->padfX[shapeObjects[i]->panPartStart[j] + k], shapeObjects[i]->padfY[shapeObjects[i]->panPartStart[j] + k], shapeObjects[i]->padfZ[shapeObjects[i]->panPartStart[j] + k]);
				}
			}
		}
	}

	for (int i = 0; i < std::min((int)points.size(), 20); ++i) {
		std::cout << "#" << i << std::endl;
		for (int j = 0; j < points[i].size(); ++j) {
			std::cout << "  #" << j << std::endl;
			for (int k = 0; k < points[i][j].size(); ++k) {
				std::cout << "    #" << k << ": " << points[i][j][k].x << "," << points[i][j][k].y << "," << points[i][j][k].z << std::endl;
			}
		}
	}

	return 0;
}