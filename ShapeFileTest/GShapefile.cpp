#include "GShapefile.h"

namespace gs {

	std::ostream& operator<<(std::ostream& os, const Variant& v) {
		if (v.type == FTInteger) {
			os << v._intValue;
		}
		else if (v.type == FTDouble) {
			os << v._doubleValue;
		}
		else if (v.type == FTString) {
			os << v._stringValue;
		}
		return os;
	}

	Shape::Shape() {
	}


	Shape::~Shape() {
	}

	bool Shape::load(const std::string& filename) {
		int index = filename.rfind(".");
		if (index == std::string::npos) return false;
		
		DBFHandle hDBF = DBFOpen((filename.substr(0, index) + ".dbf").c_str(), "r");
		if (hDBF == NULL) return false;

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

		SHPHandle hSHP = SHPOpen(filename.substr(0, index).c_str(), "r");
		if (hSHP == NULL) return false;

		shapeObjects.clear();

		int nEntities;
		double minBound[4];
		double maxBound[4];
		SHPGetInfo(hSHP, &nEntities, &shapeType, minBound, maxBound);

		if (shapeType <= 8) {
			this->minBound = glm::vec3(minBound[0], minBound[1], 0);
			this->maxBound = glm::vec3(maxBound[0], maxBound[1], 0);
		}
		else {
			this->minBound = glm::vec3(minBound[0], minBound[1], minBound[2]);
			this->maxBound = glm::vec3(maxBound[0], maxBound[1], maxBound[2]);
		}

		shapeObjects.resize(nEntities);
		for (int i = 0; i < nEntities; ++i) {
			// read attributes
			for (int j = 0; j < nFields; ++j) {
				if (fieldTypes[j] == FTString) {
					const char* data = DBFReadStringAttribute(hDBF, i, j);
					shapeObjects[i].attributes[fieldNames[j]] = Variant(data);
				}
				else if (fieldTypes[j] == FTInteger) {
					int data = DBFReadIntegerAttribute(hDBF, i, j);
					shapeObjects[i].attributes[fieldNames[j]] = Variant(data);
				}
				else if (fieldTypes[j] == FTDouble) {
					double data = DBFReadDoubleAttribute(hDBF, i, j);
					shapeObjects[i].attributes[fieldNames[j]] = Variant(data);
				}
				else if (fieldTypes[j] == FTLogical) {
					double data = DBFReadIntegerAttribute(hDBF, i, j);
					shapeObjects[i].attributes[fieldNames[j]] = Variant(data);
				}
			}

			SHPObject* obj = SHPReadObject(hSHP, i);

			shapeObjects[i].parts.resize(obj->nParts);

			for (int j = 0; j < obj->nParts; ++j) {
				int offset = obj->panPartStart[j];

				int nVerticesPerPart;
				if (j < obj->nParts - 1) {
					nVerticesPerPart = obj->panPartStart[j + 1] - offset;
				}
				else {
					nVerticesPerPart = obj->nVertices - offset;
				}
				shapeObjects[i].parts[j].points.resize(nVerticesPerPart);

				for (int k = 0; k < nVerticesPerPart; ++k) {
					if (shapeType <= 8) {
						shapeObjects[i].parts[j].points[k] = glm::vec3(obj->padfX[offset + k], obj->padfY[offset + k], 0);
					}
					else {
						shapeObjects[i].parts[j].points[k] = glm::vec3(obj->padfX[offset + k], obj->padfY[offset + k], obj->padfZ[offset + k]);
					}
				}
			}

			// release the allocated memory
			SHPDestroyObject(obj);
		}

		DBFClose(hDBF);
		SHPClose(hSHP);

		return true;
	}

}