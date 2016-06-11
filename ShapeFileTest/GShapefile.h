#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <shapefil.h>
#include <iostream>

namespace gs {

	class Variant {
	private:
		int _intValue;
		double _doubleValue;
		std::string _stringValue;

	public:
		int type;

	public:
		Variant() : type(FTInvalid) {}
		Variant(int value) : _intValue(value), type(FTInteger) {}
		Variant(double value) : _doubleValue(value), type(FTDouble) {}
		Variant(const std::string& value) : _stringValue(value), type(FTString) {}

		int intValue() { return _intValue; }
		double doubleValue() { return _doubleValue; }
		std::string stringValue() { return _stringValue; }

		friend std::ostream& operator<<(std::ostream& os, const Variant& v);
	};

	std::ostream& operator<<(std::ostream& os, const Variant& v);

	class ShapePart {
	public:
		bool selected;
		std::vector<glm::vec3> points;

	public:
		ShapePart() : selected(false) {}
	};

	class ShapeObject {
	public:
		bool selected;
		std::vector<ShapePart> parts;
		std::map<std::string, Variant> attributes;

	public:
		ShapeObject() : selected(false) {}
	};

	class Shape {
	public:
		int shapeType;
		glm::vec3 minBound;
		glm::vec3 maxBound;
		std::vector<ShapeObject> shapeObjects;

	public:
		Shape();
		~Shape();

		bool load(const std::string& filename);
	};

}
