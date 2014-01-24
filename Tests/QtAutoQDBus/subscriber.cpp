
#include "subscriber.h"

#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QApplication>
#include <QLineEdit>

#include "qdbus_publisherinterface.h"

Subscriber::Subscriber()
{

  QDBusConnection dbus = QDBusConnection::sessionBus();

  m_publisher = new org::cmake::Test::Publisher("org.cmake.Test.Publisher", "/foo", dbus);
  connect(m_publisher, SIGNAL(statusChanged()), this, SLOT(statusChanged()));

  QVBoxLayout *layout = new QVBoxLayout(this);

  m_status = new QLabel;
  statusChanged();
  layout->addWidget(m_status);

  listenButton = new QPushButton("Start listening");
  connect(listenButton, SIGNAL(clicked()), m_publisher, SLOT(startListening()));
  layout->addWidget(listenButton);

  lineEdit = new QLineEdit;
  layout->addWidget(lineEdit);
  connect(lineEdit, SIGNAL(textChanged(QString)), m_publisher, SLOT(setStatus(QString)));

}

void Subscriber::statusChanged()
{
  m_status->setText(m_publisher->status());
}

int main(int argc, char**argv)
{
  QApplication app(argc, argv);

  Subscriber bar;
  bar.show();

  return app.exec();
}
