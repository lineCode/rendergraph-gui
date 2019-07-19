#include "imageview.h"
#include "imageviewprivate.h"
#include <QAction>
#include <QMenu>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollBar>

const int MIN_HEIGHT = 20;

ImageView::ImageView(QWidget *parent) : QAbstractScrollArea{parent} {
  setMinimumWidth(200);
  setMinimumHeight(MIN_HEIGHT);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this,
          SLOT(openContextMenu(const QPoint &)));
}

ImageView::~ImageView() {}

const QImage &ImageView::image() { return image_; }

void ImageView::setImage(const QImage &image) {
  image_ = image;
  viewport()->repaint();
  updateScrollBars();
}

void ImageView::setScale(qreal s) {
  setScaleWithViewportAnchor(s, QPoint{0, 0});
}

QPointF ImageView::topLeftViewOrigin() const {
  auto off = QPointF{(qreal)horizontalScrollBar()->value(),
                     (qreal)verticalScrollBar()->value()};
  return off / scale_;
}

void ImageView::setTopLeftViewOrigin(QPointF pos) {
  auto scaledPos = QPointF{pos} * scale_;
  setTopLeftViewportPos(scaledPos.toPoint());
}

void ImageView::setTopLeftViewportPos(QPoint scaledPos) {
  horizontalScrollBar()->setValue(scaledPos.x());
  verticalScrollBar()->setValue(scaledPos.y());
  viewport()->update();
}

void ImageView::updateScrollBars() {
  // update scroll bars
  QSize areaSize = viewport()->size();
  QSizeF scaledSize = QSizeF{image_.size()} * scale_;
  horizontalScrollBar()->setRange(0,
                                  (int)scaledSize.width() - areaSize.width());
  verticalScrollBar()->setRange(0,
                                (int)scaledSize.height() - areaSize.height());
  horizontalScrollBar()->setPageStep(areaSize.width());
  verticalScrollBar()->setPageStep(areaSize.height());
}

void ImageView::setScaleWithViewportAnchor(qreal s, const QPoint &anchor) {
  auto tl = topLeftViewOrigin();

  auto newtl = QPointF{anchor} * (s - scale_) / (s * scale_) + tl;
  scale_ = s;
  updateScrollBars();
  setTopLeftViewOrigin(newtl); // will call viewport()->update()
}

void ImageView::openContextMenu(const QPoint &pos) {
  QMenu contextMenu(tr("Context menu"), this);
  contextMenuPos = pos;

  QAction a("Zoom here", this);
  connect(&a, SIGNAL(triggered()), this, SLOT(contextMenuZoom()));
  contextMenu.addAction(&a);

  contextMenu.exec(viewport()->mapToGlobal(pos));
}

void ImageView::contextMenuZoom() {
  setScaleWithViewportAnchor(scale() * 2.0, contextMenuPos);
}

//=========================================================================
void ImageView::paintEvent(QPaintEvent *pe) {
  // QAbstractScrollArea::paintEvent(pe);
  QPainter painter{viewport()};
  drawWidget(painter, pe);
}

void ImageView::resizeEvent(QResizeEvent *) { updateScrollBars(); }

void ImageView::mousePressEvent(QMouseEvent *e) {
  mouseAnchor = e->pos();
  dragAnchor = topLeftViewOrigin();
}

void ImageView::mouseMoveEvent(QMouseEvent *e) {
  auto offset = QPointF{mouseAnchor - e->pos()} / scale_;
  setTopLeftViewOrigin(dragAnchor + offset);
}

void ImageView::scrollContentsBy(int dx, int dy) { viewport()->update(); }

void ImageView::drawWidget(QPainter &painter, QPaintEvent *pe) {
  QRectF rect{pe->rect()};
  int x = horizontalScrollBar()->value();
  int y = verticalScrollBar()->value();
  auto viewRectUpperLeft = QPointF{(qreal)x, (qreal)y};
  painter.drawText(rect, "Test");
  painter.drawImage(rect, image_,
                    QRectF{viewRectUpperLeft / scale_, rect.size() / scale_});
}
