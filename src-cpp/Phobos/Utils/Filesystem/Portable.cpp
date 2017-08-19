#include "Utils/Filesystem/Portable.h"

/*
 * Adopted from Boost https://code.woboq.org/boost/boost/libs/filesystem/src/portability.cpp.html
 */

namespace phobos { namespace utils { namespace fs {
namespace
{
  const char invalid_chars[] =
    "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
    "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
    "<>:\"/\\|";
  // note that the terminating '\0' is part of the string - thus the size below
  // is sizeof(invalid_chars) rather than sizeof(invalid_chars)-1.  I
  const std::string windows_invalid_chars(invalid_chars, sizeof(invalid_chars));

  const std::string valid_posix(
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._-");

bool portable_posix_name(const std::string & name)
{
	return name.size() != 0
		&& name.find_first_not_of(valid_posix) == std::string::npos;
}

bool windows_name(const std::string & name)
{
	return name.size() != 0
		&& name[0] != ' '
		&& name.find_first_of(windows_invalid_chars) == std::string::npos
		&& *(name.end()-1) != ' '
		&& (*(name.end()-1) != '.'
			|| name.length() == 1 || name == "..");
}

bool portable_name(const std::string & name)
{
	return
		name.size() != 0
		&& (name == "."
			|| name == ".."
			|| (windows_name(name)
				&& portable_posix_name(name)
				&& name[0] != '.' && name[0] != '-'));
}
} // unnamed namespace

bool portableDirectoryName(const std::string & name)
{
	return
		name == "."
		|| name == ".."
		|| (portable_name(name)
			&& name.find('.') == std::string::npos);
}

bool portableFileName(const std::string & name)
{
	std::string::size_type pos;
	return
		 portable_name(name)
		 && name != "."
		 && name != ".."
		 && ((pos = name.find('.')) == std::string::npos
				 || (name.find('.', pos+1) == std::string::npos
					 && (pos + 5) > name.length()))
		;
}

}}} // namespace phobos::utils::fs
