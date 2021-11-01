"""Supporting definitions for the Python regression tests."""

import platform
import unittest
from typing import Any, Callable, Dict, Optional, Tuple

__all__ = [
    "run_with_locale",
    "cpython_only",
]


# =======================================================================
# Decorator for running a function in a different locale, correctly resetting
# it afterwards.


def run_with_locale(catstr: str, *locales: str) -> Callable[..., Any]:
    def decorator(func: Callable[..., Any]) -> Callable[..., Any]:
        def inner(*args: Any, **kwds: Any) -> Any:
            try:
                import locale

                category = getattr(locale, catstr)
                orig_locale = locale.setlocale(category)
            except AttributeError:
                # if the test author gives us an invalid category string
                raise
            except:  # noqa
                # cannot retrieve original locale, so do nothing
                locale = orig_locale = None  # type: ignore
            else:
                for loc in locales:
                    try:
                        locale.setlocale(category, loc)
                        break
                    except:  # noqa
                        pass

            # now run the function, resetting the locale on exceptions
            try:
                return func(*args, **kwds)
            finally:
                if locale and orig_locale:
                    locale.setlocale(category, orig_locale)

        inner.__name__ = func.__name__
        inner.__doc__ = func.__doc__
        return inner

    return decorator


# =======================================================================
# unittest integration.


def _id(obj: Any) -> Any:
    return obj


def cpython_only(test: Any) -> Any:
    """
    Decorator for tests only applicable on CPython.
    """
    return impl_detail(cpython=True)(test)


def impl_detail(msg: Optional[str] = None, **guards: bool) -> Callable[[Any], Any]:
    if check_impl_detail(**guards):
        return _id
    if msg is None:
        guardnames, default = _parse_guards(guards)
        if default:
            msg = "implementation detail not available on {0}"
        else:
            msg = "implementation detail specific to {0}"
        msg = msg.format(" or ".join(sorted(guardnames.keys())))
    return unittest.skip(msg)


def _parse_guards(guards: Dict[str, bool]) -> Tuple[Dict[str, bool], bool]:
    # Returns a tuple ({platform_name: run_me}, default_value)
    if not guards:
        return ({"cpython": True}, False)
    is_true = list(guards.values())[0]
    assert list(guards.values()) == [is_true] * len(guards)  # all True or all False
    return (guards, not is_true)


# Use the following check to guard CPython's implementation-specific tests --
# or to run them only on the implementation(s) guarded by the arguments.
def check_impl_detail(**guards: bool) -> bool:
    """This function returns True or False depending on the host platform.
    Examples:
       if check_impl_detail():               # only on CPython (default)
       if check_impl_detail(jython=True):    # only on Jython
       if check_impl_detail(cpython=False):  # everywhere except on CPython
    """
    guards, default = _parse_guards(guards)
    return guards.get(platform.python_implementation().lower(), default)
