
#include "publisher.h"

#include "qdbus_publisheradaptor.h"

#include <QHBoxLayout>
#include <QApplication>
#include <QLabel>

Publisher::Publisher()
{
  new PublisherAdaptor(this);

  QHBoxLayout *layout = new QHBoxLayout(this);
  m_label = new QLabel;
  layout->addWidget(m_label);

  m_label->setText("idle");
}

QString Publisher::status() const
{
  return m_label->text();
}

void Publisher::setStatus(const QString &newStatus)
{
  m_label->setText(newStatus);
  emit statusChanged();
}

void Publisher::startListening()
{
  m_label->setText("listening");
  emit statusChanged();
}

int main(int argc,char** argv)
{
  QApplication app(argc, argv);

  QDBusConnection dbus = QDBusConnection::sessionBus();
  dbus.registerService(QLatin1String("org.cmake.Test.Publisher"));

  Publisher foo;
  dbus.registerObject(QLatin1String("/foo"), &foo);
  foo.show();

  return app.exec();
}
