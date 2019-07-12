#pragma once
#include "imageview.hpp"
#include <QFrame>

//=================================================================================
class ImageViewFrame : public QFrame {
  Q_OBJECT
public:
  ImageViewFrame(ImageView *parent) : QFrame{parent}, parent_{parent} {}

protected:
  void paintEvent(QPaintEvent *) override;
  void drawWidget(QPainter &, QPaintEvent *);

private:
  ImageView *parent_;
};
