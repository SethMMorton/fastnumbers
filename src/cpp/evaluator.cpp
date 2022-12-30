#include "fastnumbers/evaluator.hpp"
#include "fastnumbers/parser.hpp"

template <>
bool Evaluator<NumericParser>::allow_inf() const
{
    return options().allow_inf_num();
}

template <>
bool Evaluator<NumericParser>::allow_nan() const
{
    return options().allow_nan_num();
}
