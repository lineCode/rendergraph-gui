#pragma once
#include <QWidget>
#include <QFrame>
#include <QScrollArea>
#include <QAbstractScrollArea>

enum class ScaleReference
{
	TopLeft,
	TopCenter,
	TopRight,
	CenterLeft,
	Center,
	CenterRight,
	BottomLeft,
	BottomCenter,
	BottomRight,
};

class ImageView : public QAbstractScrollArea {
	Q_OBJECT
public:
	ImageView(QWidget* parent = nullptr);
	~ImageView();

	const QImage& image();
	void setImage(const QImage& image);

	qreal scale() const { 
		return scale_; 
	}

	void setScale(qreal s);

	QPointF topLeftViewOrigin() const;
	
	// Sets the top-left position of the view window. 
	// Scaling is applied to the passed offset. 
	// E.g. if displaying an image at x2, offset to (10,10) in image coords 
	// will actually move the zoomed viewport to (20,20).
	void setTopLeftViewOrigin(QPointF offset); 

	// Sets the top-left position of the view window in pixels. 
	// Scaling is not applied.
	void setTopLeftViewportPos(QPoint scaledPos);

	void setScaleWithViewportAnchor(qreal scale, const QPoint& anchor);

protected:
	void paintEvent(QPaintEvent*) override;
	void resizeEvent(QResizeEvent*) override;
	void mousePressEvent(QMouseEvent*) override;
	void mouseMoveEvent(QMouseEvent*) override;
	void scrollContentsBy(int dx, int dy) override;
	void drawWidget(QPainter&, QPaintEvent*);

private Q_SLOTS:
	void openContextMenu(const QPoint& point);
	void contextMenuZoom();

private:
	void updateScrollBars();

	QPoint mouseAnchor;
	QPointF dragAnchor;
	QPoint contextMenuPos;

	QImage image_;
	qreal scale_ = 1.0;
};

