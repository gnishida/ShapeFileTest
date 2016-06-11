#include "gisutils.h"

namespace gisutils {

	bool within(const std::vector<glm::vec2>& polygon, const glm::vec2& pt) {
		typedef boost::geometry::model::d2::point_xy<double> point_type;
		typedef boost::geometry::model::polygon<point_type> polygon_type;

		polygon_type poly;
		for (int i = 0; i < polygon.size(); ++i) {
			boost::geometry::append(poly, point_type(polygon[i].x, polygon[i].y));
		}

		point_type p(pt.x, pt.y);

		return boost::geometry::within(p, poly);
	}

}