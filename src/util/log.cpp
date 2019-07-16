#include "log.hpp"
#include "logprivate.hpp"
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QFontDatabase>

namespace util {

LogWindow::LogWindow(QWidget *parent) : QWidget{parent} {
  logText_ = new QPlainTextEdit;
  //logText_->set

  QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  logText_->setFont(fixedFont);
  logText_->setReadOnly(true);

  auto layout = new QVBoxLayout;
  layout->addWidget(logText_);

  setLayout(layout);
  setWindowTitle("Log output");
}

LogWindow::~LogWindow() {}

void LogWindow::append(const QString &text) {
	logText_->appendPlainText(text);
}

static LogWindow* getLogWindow() {
	static LogWindow* logWindow = nullptr;
	if (!logWindow) {
		logWindow = new LogWindow;
	}
	return logWindow;
}

void util::log(const char *msg) {
	auto w = getLogWindow();
	w->append(msg);
	//w->append("\n");
	w->show();
}

} // namespace util