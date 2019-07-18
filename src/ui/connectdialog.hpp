#pragma once
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QVBoxLayout>

namespace ui {

class ConnectDialog : public QDialog {
  Q_OBJECT
public:
  ConnectDialog(QWidget *parent = nullptr) : QDialog{parent} {
    transport_ = new QComboBox;
    transport_->addItem("TCP");
    transport_->addItem("IPC");

    address_ = new QLineEdit;
	address_->setText("localhost:5555");

    buttons_ =
        new QDialogButtonBox{QDialogButtonBox::Ok | QDialogButtonBox::Cancel};
    connect(buttons_, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons_, SIGNAL(rejected()), this, SLOT(reject()));

    auto formLayout = new QFormLayout;
    formLayout->addRow("&Transport:", transport_);
    formLayout->addRow("&Address:", address_);

    auto layout = new QVBoxLayout;
    layout->addLayout(formLayout);
    layout->addWidget(buttons_);

    setLayout(layout);
  }

  int transport() const { return transport_->currentIndex(); }
  QString address() const { return address_->text(); }

private:
  QComboBox *transport_;
  QLineEdit *address_;
  QDialogButtonBox *buttons_;
};

} // namespace ui