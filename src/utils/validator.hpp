#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include "utils_global.h"

#include <QValidator>

namespace Utils {

class UTILS_EXPORT IntValidator : public QIntValidator
{
    Q_OBJECT
public:
    using QIntValidator::QIntValidator;

    auto validate(QString &input, int &pos) const -> State override;
};

class UTILS_EXPORT DoubleValidator : public QDoubleValidator
{
    Q_OBJECT
public:
    using QDoubleValidator::QDoubleValidator;

    auto validate(QString &input, int &pos) const -> State override;
};

} // namespace Utils

#endif // VALIDATOR_HPP
