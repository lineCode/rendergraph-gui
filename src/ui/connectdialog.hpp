#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDialogButtonBox>

namespace ui {

	class ConnectDialog : public QDialog {
		Q_OBJECT
	public:
		ConnectDialog(QWidget* parent = nullptr) : QDialog{ parent }
		{
			transport_ = new QComboBox;
			transport_->addItem("TCP");
			transport_->addItem("IPC");

			address_ = new QLineEdit;

			buttons_ = new QDialogButtonBox{ QDialogButtonBox::Ok | QDialogButtonBox::Cancel };
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

	private:
		QComboBox* transport_;
		QLineEdit* address_;
		QDialogButtonBox* buttons_;
	};

}