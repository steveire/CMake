
#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <QWidget>

class QPushButton;
class QLabel;
class QLineEdit;

class OrgCmakeTestPublisherInterface;
namespace org {
namespace cmake {
namespace Test {
      typedef ::OrgCmakeTestPublisherInterface Publisher;
}
}
}

struct Subscriber : public QWidget
{
  Q_OBJECT
public:
  Subscriber();

private slots:
  void statusChanged();

private:
  QPushButton *listenButton;
  QLabel *m_status;
  QLineEdit *lineEdit;
  org::cmake::Test::Publisher *m_publisher;
};

#endif
