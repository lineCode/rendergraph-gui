#pragma once
#include <QDialog>
#include <QPlainTextEdit>

namespace util {
class LogWindow : public QWidget {
  Q_OBJECT
public:
  LogWindow(QWidget *parent = nullptr);
  virtual ~LogWindow();

  void append(const QString &text);

private:
  QPlainTextEdit *logText_;
};
} // namespace util