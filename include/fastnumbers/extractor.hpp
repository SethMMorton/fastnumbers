#pragma once

#include <variant>

#include <Python.h>

#include "fastnumbers/buffer.hpp"
#include "fastnumbers/parser.hpp"
#include "fastnumbers/user_options.hpp"

/// Can store any possible parser
using AnyParser = std::variant<CharacterParser, UnicodeParser, NumericParser>;

/**
 * \brief Return the appropriate parser for the conained data
 * \param obj The Python object from which to extract data
 * \param buffer The buffer into which to potentially store data
 * \param options A UserOptions instance containing the options
 *                specified by the user.
 * \return std::variant of CharacterParser, UnicodeParser, or NumericParser
 */
AnyParser
extract_parser(PyObject* obj, Buffer& buffer, const UserOptions& options) noexcept(false
);
