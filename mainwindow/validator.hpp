#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <QIntValidator>

class IntValidator : public QIntValidator
{
public:
    using QIntValidator::QIntValidator;

    auto validate(QString &input, int &pos) const -> State override;
};

#endif // VALIDATOR_HPP
