
#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <QWidget>

class QLabel;

struct Publisher : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QString status READ status);
  Q_CLASSINFO("D-Bus Interface", "org.cmake.Test.Publisher")
public:
  Publisher();

  QString status() const;


public slots:
  void startListening();
  void setStatus(const QString &st);


signals:
  void statusChanged();

private:
  QLabel *m_label;
};

#endif
