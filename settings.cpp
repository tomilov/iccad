#include "settings.h"

#include <insituc/parser/parser.hpp>

#include <QDebug>

using namespace insituc;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
QAtomicPointer< Settings > Settings::p = nullptr;
#pragma clang diagnostic pop

Settings::Settings(QSettings & _settings, QString const & _group)
    : QObject(&_settings)
    , group(_group)
    , settings(_settings)
{
    for (string_type const & reserved_symbol_ : parser::get_constants()) {
        constSettings.reservedSymbols << QString::fromStdString(reserved_symbol_);
    }
    for (string_type const & reserved_symbol_ : parser::get_intrinsics()) {
        constSettings.reservedSymbols << QString::fromStdString(reserved_symbol_);
    }
    for (string_type const & reserved_symbol_ : parser::get_keywords()) {
        constSettings.reservedSymbols << QString::fromStdString(reserved_symbol_);
    }
    constSettings.symbolExpression = QRegExp(R"(\b[A-Za-z_][A-Za-z0-9_]*\b)");
    constSettings.floatingPointExpression = QRegExp(R"([-]?(([0-9]+(\.[0-9]*)?)|(\.[0-9]+))([Ee][+-]?[0-9]+)?)");
    constSettings.dummySymbolExpression = QRegExp(R"(\b_\b)");
    constSettings.escapeSequenceExpression = QRegExp(R"(\x1B\[([\d;]+)m)");
}

void
Settings::initSettings(QSettings & settings, QString const & group)
{
    Settings * q = new Settings(settings, group);
    if (!p.testAndSetOrdered(nullptr, q)) {
        delete q;
    }
}

QSettings &
Settings::get()
{
    Q_ASSERT(p != nullptr);
    return (*p).settings;
}

QMutex &
Settings::getMutex()
{
    Q_ASSERT(p != nullptr);
    return (*p).mutex;
}

QString const &
Settings::getGroup()
{
    Q_ASSERT(p != nullptr);
    return (*p).group;
}

auto
Settings::getPersistentSettings()
-> persistentSettings const &
{
    Q_ASSERT(p != nullptr);
    return (*p).constSettings;
}
