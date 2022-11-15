#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <QIntValidator>

class IntValidator : public QIntValidator
{
public:
    using QIntValidator::QIntValidator;

    State validate(QString &input, int &pos) const override;
};

#endif // VALIDATOR_HPP
