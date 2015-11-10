#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QAtomicPointer>
#include <QRegExp>

class Settings
        : public QObject
{

    Q_OBJECT

    explicit
    Settings(QSettings & settings, QString const & group);

    static QAtomicPointer< Settings > p;
    mutable QMutex mutex;

    QString const group;
    QSettings & settings;

public :

    ~Settings() Q_DECL_OVERRIDE = default;

    static
    void
    initSettings(QSettings & settings, QString const & group);

    static
    QSettings &
    get();

    static
    QMutex &
    getMutex();

    static
    QString const &
    getGroup();

    struct persistentSettings
    {

        QStringList reservedSymbols;
        QRegExp symbolExpression;
        QRegExp floatingPointExpression;
        QRegExp dummySymbolExpression;
        QRegExp escapeSequenceExpression;

    };

    static
    persistentSettings const &
    getPersistentSettings();

private :

    persistentSettings constSettings;

};

#endif // SETTINGS_H
