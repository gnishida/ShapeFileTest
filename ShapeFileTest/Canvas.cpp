#include "Canvas.h"
#include <QPainter>
#include <QResizeEvent>
#include <QMouseEvent>
#include <iostream>
#include "gisutils.h"

Canvas::Canvas(QWidget *parent) : QWidget(parent) {
	ui.setupUi(this);
}

Canvas::~Canvas() {
}

void Canvas::resizeEvent(QResizeEvent* e) {
	constrainCameraCenter();
}

void Canvas::paintEvent(QPaintEvent* e) {
	QPainter painter(this);
	painter.drawImage(QPoint(0, 0), shapeImage, QRect(cameraCenter.x - width() * 0.5, cameraCenter.y - height() * 0.5, width(), height()));
}

void Canvas::mousePressEvent(QMouseEvent* e) {
	prevMousePoint = glm::vec2(e->x(), e->y());

	glm::vec2 pt(minBound.x + cameraCenter.x + prevMousePoint.x - width() * 0.5, minBound.y + shapeImage.height() - (cameraCenter.y + prevMousePoint.y - height() * 0.5));
	selectShape(pt);

	update();
}

void Canvas::mouseMoveEvent(QMouseEvent* e) {
	if (e->buttons() & Qt::LeftButton) {
		cameraCenter -= glm::vec2(e->x(), e->y()) - prevMousePoint;
		constrainCameraCenter();

		prevMousePoint = glm::vec2(e->x(), e->y());
	}

	update();
}

void Canvas::mouseReleaseEvent(QMouseEvent* e) {
}

void Canvas::loadShapfile(const std::string& filename) {
	shapes.resize(shapes.size() + 1);
	shapes.back().load(filename);

	updateShapeImage();
	cameraCenter = glm::vec2(width() * 0.5, height() * 0.5);
	constrainCameraCenter();
}

void Canvas::constrainCameraCenter() {
	if (shapeImage.width() > width()) {
		if (cameraCenter.x < width() * 0.5) cameraCenter.x = width() * 0.5;
		if (cameraCenter.x > shapeImage.width() - width() * 0.5) cameraCenter.x = shapeImage.width() - width() * 0.5;
	}
	else {
		cameraCenter.x = shapeImage.width() * 0.5;
	}

	if (shapeImage.height() > height()) {
		if (cameraCenter.y < height() * 0.5) cameraCenter.y = height() * 0.5;
		if (cameraCenter.y > shapeImage.height() - height() * 0.5) cameraCenter.y = shapeImage.height() - height() * 0.5;
	}
	else {
		cameraCenter.y = shapeImage.height() * 0.5;
	}
}

void Canvas::updateShapeImage() {
	minBound = shapes.back().minBound;
	maxBound = shapes.back().maxBound;

	shapeImage = QImage(maxBound.x - minBound.x, maxBound.y - minBound.y, QImage::Format_RGB888);
	shapeImage.fill(QColor(255, 255, 255));

	const std::vector<QColor> COLOR_TABLES = {
		QColor(0xFF, 0, 0), QColor(0, 0xFF, 0), QColor(0, 0, 0xFF), QColor(0xFF, 0xFF, 0), QColor(0xFF, 0, 0xFF), QColor(0, 0xFF, 0xFF), QColor(0, 0, 0),
		QColor(0x80, 0, 0), QColor(0, 0x80, 0), QColor(0, 0, 0x80), QColor(0x80, 0x80, 0), QColor(0x80, 0, 0x80), QColor(0, 0x80, 0x80), QColor(0x80, 0x80, 0x80),
		QColor(0xC0, 0, 0), QColor(0, 0xC0, 0), QColor(0, 0, 0xC0), QColor(0xC0, 0xC0, 0), QColor(0xC0, 0, 0xC0), QColor(0, 0xC0, 0xC0), QColor(0xC0, 0xC0, 0xC0),
		QColor(0x40, 0, 0), QColor(0, 0x40, 0), QColor(0, 0, 0x40), QColor(0x40, 0x40, 0), QColor(0x40, 0, 0x40), QColor(0, 0x40, 0x40), QColor(0x40, 0x40, 0x40),
		QColor(0x20, 0, 0), QColor(0, 0x20, 0), QColor(0, 0, 0x20), QColor(0x20, 0x20, 0), QColor(0x20, 0, 0x20), QColor(0, 0x20, 0x20), QColor(0x20, 0x20, 0x20),
		QColor(0x60, 0, 0), QColor(0, 0x60, 0), QColor(0, 0, 0x60), QColor(0x60, 0x60, 0), QColor(0x60, 0, 0x60), QColor(0, 0x60, 0x60), QColor(0x60, 0x60, 0x60),
		QColor(0xA0, 0, 0), QColor(0, 0xA0, 0), QColor(0, 0, 0xA0), QColor(0xA0, 0xA0, 0), QColor(0xA0, 0, 0xA0), QColor(0, 0xA0, 0xA0), QColor(0xA0, 0xA0, 0xA0),
		QColor(0xE0, 0, 0), QColor(0, 0xE0, 0), QColor(0, 0, 0xE0), QColor(0xE0, 0xE0, 0), QColor(0xE0, 0, 0xE0), QColor(0, 0xE0, 0xE0), QColor(0xE0, 0xE0, 0xE0)
	};
	
	QPainter painter(&shapeImage);
	int penWidth = 1;
	for (int i = 0; i < shapes.size(); ++i) {
		//painter.setPen(COLOR_TABLES[i]);

		for (int j = 0; j < shapes[i].shapeObjects.size(); ++j) {
			if (shapes[i].shapeObjects[j].selected) {
				penWidth = 3;
			}
			else {
				penWidth = 1;
			}
			painter.setPen(QPen(QBrush(COLOR_TABLES[i]), penWidth));

			for (int k = 0; k < shapes[i].shapeObjects[j].parts.size(); ++k) {
				for (int l = 0; l < shapes[i].shapeObjects[j].parts[k].points.size(); ++l) {
					gs::ShapePart& part = shapes[i].shapeObjects[j].parts[k];

					int x1 = part.points[l].x - minBound.x;
					int y1 = maxBound.y - part.points[l].y;
					int x2 = part.points[(l + 1) % part.points.size()].x - minBound.x;
					int y2 = maxBound.y - part.points[(l + 1) % part.points.size()].y;

					painter.drawLine(x1, y1, x2, y2);
				}
			}
		}
	}
}

void Canvas::selectShape(const glm::vec2& pt) {
	float min_dist = std::numeric_limits<float>::max();
	gs::ShapeObject* min_obj = NULL;

	for (int i = 0; i < shapes.size(); ++i) {
		for (int j = 0; j < shapes[i].shapeObjects.size(); ++j) {
			shapes[i].shapeObjects[j].selected = false;

			for (int k = 0; k < shapes[i].shapeObjects[j].parts.size(); ++k) {
				std::vector<glm::vec2> pts;
				for (int l = 0; l < shapes[i].shapeObjects[j].parts[k].points.size(); ++l) {
					pts.push_back(glm::vec2(shapes[i].shapeObjects[j].parts[k].points[l]));
				}
				if (gisutils::within(pts, pt)) {
					min_dist = 0;
					min_obj = &shapes[i].shapeObjects[j];
				}

				/*
				for (int l = 0; l < shapes[i].shapeObjects[j].parts[k].points.size(); ++l) {
					gs::ShapePart& part = shapes[i].shapeObjects[j].parts[k];

					float dist = glm::length(glm::vec2(part.points[l]) - pt);
					if (dist < min_dist) {
						min_dist = dist;
						min_obj = &shapes[i].shapeObjects[j];
					}
				}
				*/
			}
		}
	}

	if (min_obj != NULL) {
		min_obj->selected = true;

		///////////////// DEBUG ///////////////////////////////////////////////
		for (auto it = min_obj->attributes.begin(); it != min_obj->attributes.end(); ++it) {
			std::cout << it->first << ": " << it->second << ", ";
		}
		std::cout << std::endl;

		if (min_obj->parts.size() > 1) {
			std::cout << "!!!!!!!!!!!! Multiple parts: " << min_obj->parts.size() << std::endl;
		}
		///////////////// DEBUG ///////////////////////////////////////////////
	}

	updateShapeImage();

}